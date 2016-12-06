//
// Created by miro on 11/30/16.
//

#include <gtest/gtest.h>

#define INSTRUCTION_TEST

#include <memory>
#include <befa/utils/visitor.hpp>
#include <befa/llvm/instruction.hpp>


namespace {

struct VisitableSample42;
struct VisitableSample43;
struct VisitableSample44;

REGISTER_VISITABLES(
    visitable_impl, visitable_base, visitor_base, InstructionFactory,
    VisitableSample42, VisitableSample43, VisitableSample44
)

#define CREATE_VISITABLE_SAMPLE(num) \
  struct VisitableSample##num \
    : visitable_impl<VisitableSample##num> { \
    int return_##num() const { return num; } \
  }

CREATE_VISITABLE_SAMPLE(42);
CREATE_VISITABLE_SAMPLE(43);
CREATE_VISITABLE_SAMPLE(44);

#undef CREATE_VISITABLE_SAMPLE

struct VisitorSample : visitor_base {
  VisitorSample(int &visits) : visits(visits) {}

#undef IMPLEMENT_VISIT

#define IMPLEMENT_VISIT(num) \
  void visit(const VisitableSample##num *instr) override { \
    ASSERT_EQ(instr->return_##num(), num); ++visits; \
  }

  IMPLEMENT_VISIT(42)
  IMPLEMENT_VISIT(43)
  IMPLEMENT_VISIT(44)

#undef IMPLEMENT_VISIT
 private:
  int &visits;
};

TEST(VisitorTest, BasicVisitor) {
  std::vector<std::shared_ptr<visitable_base>> visitables{
      std::make_shared<VisitableSample42>(),
      std::make_shared<VisitableSample43>(),
      std::make_shared<VisitableSample44>(),
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

}