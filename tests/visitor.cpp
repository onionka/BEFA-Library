//
// Created by miro on 11/30/16.
//

#include <gtest/gtest.h>

#define INSTRUCTION_TEST

#include <memory>
#include <befa/utils/visitor.hpp>
#include <befa/llvm/instruction.hpp>

namespace {

struct V_42;
struct V_43;
struct V_44;

REGISTER_VISITABLES(
    visitable_impl, visitable_base, visitor_base,
    V_42, V_43, V_44
)

#define CREATE_VISITABLE_SAMPLE(num) \
  struct V_##num \
    : visitable_impl<V_##num> { \
    int return_##num() const { return num; } \
  }

CREATE_VISITABLE_SAMPLE(42);
CREATE_VISITABLE_SAMPLE(43);
CREATE_VISITABLE_SAMPLE(44);

#undef CREATE_VISITABLE_SAMPLE

struct VisitorSample : visitor_base {
  VisitorSample(int &visits) : visits(visits) {}

#define _IMPLEMENT_VISIT(num) \
  void visit(const V_##num *instr) override { \
    ASSERT_EQ(instr->return_##num(), num); ++visits; \
  }

  _IMPLEMENT_VISIT(42)
  _IMPLEMENT_VISIT(43)
  _IMPLEMENT_VISIT(44)

#undef _IMPLEMENT_VISIT
 private:
  int &visits;
};

TEST(VisitorTest, BasicVisitor) {
  std::vector<std::shared_ptr<visitable_base>> visitables{
      std::make_shared<V_42>(),
      std::make_shared<V_43>(),
      std::make_shared<V_44>(),
  };
  int visits = 0;
  VisitorSample vi(visits);
  visitables[0]->accept(vi);
  ASSERT_EQ(visits, 1);
  visitables[1]->accept(vi);
  ASSERT_EQ(visits, 2);
  visitables[2]->accept(vi);
  ASSERT_EQ(visits, 3);
}

TEST(VisitorTest, OperatorShift) {
  std::vector<std::shared_ptr<visitable_base>> visitables{
      std::make_shared<V_42>(),
      std::make_shared<V_43>(),
      std::make_shared<V_44>(),
  };
  int i = 0;
  auto V_42_visitor = [&](const V_42 *visitable) {
    ++i;
    ASSERT_TRUE(std::string(
                    typeid(decltype(visitable)).name()
                ).find(std::string("V_42")) != std::string::npos);
  };
  auto V_43_visitor = [&](const V_43 *visitable) {
    ++i;
    ASSERT_TRUE(std::string(
                    typeid(decltype(visitable)).name()
                ).find(std::string("V_43")) != std::string::npos);
  };
  *visitables[0] >> V_42_visitor;
  ASSERT_EQ(1, i);
  *visitables[1] >> V_42_visitor;
  ASSERT_EQ(1, i);
  *visitables[1] >> V_43_visitor;
  ASSERT_EQ(2, i);
  *visitables[2] >> V_43_visitor;
  ASSERT_EQ(2, i);
  *visitables[2]
      >> [&](const V_43 *visitable) {
        ++i;
        ASSERT_TRUE(
            std::string(typeid(decltype(visitable)).name())
                .find(std::string("V_43")) != std::string::npos);
      }
      >> [&](const V_44 *visitable) {
        ++i;
        ASSERT_TRUE(
            std::string(typeid(decltype(visitable)).name())
                .find(std::string("V_44")) != std::string::npos);
      }
      >> [&](const V_44 *visitable) {
        ++i;
        ASSERT_TRUE(
            std::string(typeid(decltype(visitable)).name())
                .find(std::string("V_44")) != std::string::npos);
      };
  ASSERT_EQ(4, i);
}

namespace generalized {

struct V_A;
struct V_AA;
struct V_AB;
struct V_ABA;

REGISTER_VISITABLES(
    visitable_impl, visitable_base, visitor_base,
    V_A, V_AA, V_AB,
    V_ABA
)

struct V_A : visitable_impl<V_A> {
  const char *name = "Visitable A";

  virtual const char *getName() const {
    return name;
  }
};

struct V_AA : virtual visitable_base, V_A {
  const char *name = "Visitable AA";

  virtual const char *getName() const {
    return name;
  }

  void accept(visitor_base &visitor) const override {
    visitor.visit(static_cast<const V_AA *>(this));
  }
};

struct V_AB : virtual visitable_base, V_A {
  const char *name = "Visitable AB";

  virtual const char *getName() const {
    return name;
  }

  void accept(visitor_base &visitor) const override {
    visitor.visit(this);
  }
};

struct V_ABA : virtual visitable_base, V_AB {
  const char *name = "Visitable ABA";

  virtual const char *getName() const {
    return name;
  }

  void accept(visitor_base &visitor) const override {
    visitor.visit(this);
  }
};

using Traits = visitable_traits<
    V_A, V_AA, V_AB,
    V_ABA
>;

struct GeneralizedAVisitor : Generalizer<
    Traits,
    // Base
    V_AB,
    // Derivations
    V_AB, V_ABA
> {
  void generalized_visitor(const V_AB *ptr) override {
    ASSERT_STRNE(ptr->getName(), "Visitable A");
    ASSERT_STRNE(ptr->getName(), "Visitable AA");
  }
};

}  // namespace generalized

TEST(VisitorTest, WierdStructure) {
  generalized::GeneralizedAVisitor visitor;
  std::vector<std::shared_ptr<generalized::visitable_base>> vs;
  vs.push_back(std::make_shared<generalized::V_AA>());
  vs.push_back(std::make_shared<generalized::V_A>());
  vs.push_back(std::make_shared<generalized::V_AB>());
  vs.push_back(std::make_shared<generalized::V_ABA>());

  for (auto &v : vs) {
    v->accept(visitor);
  }
}

template<typename T>
bool one_of(T &&val, std::vector<T> arr) {
  return std::find(arr.begin(), arr.end(), val) != arr.end();
}

using generalized_ab = LambdaGeneralizer<
    generalized::Traits,
    // Base
    generalized::V_AB,
    // Derivations
    generalized::V_AB, generalized::V_ABA
>;

TEST(VisitorTest, LambdaGeneralizer) {
  std::vector<std::shared_ptr<generalized::visitable_base>> vs;
  vs.push_back(std::make_shared<generalized::V_AA>());
  vs.push_back(std::make_shared<generalized::V_A>());
  vs.push_back(std::make_shared<generalized::V_AB>());
  vs.push_back(std::make_shared<generalized::V_ABA>());
  int counter = 0;
  for (auto &v : vs) {
    invoke_accept(v, generalized_ab(
        [&](const generalized::V_AB *ptr) -> void {
          ASSERT_STRNE(ptr->getName(), "Visitable A");
          ASSERT_STRNE(ptr->getName(), "Visitable AA");
          ASSERT_TRUE(one_of(ptr->getName(), {"Visitable AB", "Visitable ABA"}));
          counter++;
        })
    );
  }
  ASSERT_EQ(counter, 2);
}

}