//
// Created by miro on 12/3/16.
//

#include "../../include/befa/assembly/asm_arg_parser.hpp"
#include "../../include/befa.hpp"
#include "../../include/befa/utils/range.hpp"

namespace symbol_table {
#define DECLARE_REGISTER(name, size) \
    static Register<size> reg##name(#name);

#define CAST_TO_VISITABLE(reg) \
    static_cast<VisitableBase *>(&reg)

DECLARE_REGISTER(_rax, types::QWORD)
DECLARE_REGISTER(_al, types::BYTE)
DECLARE_REGISTER(_ah, types::BYTE)
DECLARE_REGISTER(_ax, types::WORD)
DECLARE_REGISTER(_eax, types::DWORD)
DECLARE_REGISTER(_rbx, types::QWORD)
DECLARE_REGISTER(_bl, types::BYTE)
DECLARE_REGISTER(_bh, types::BYTE)
DECLARE_REGISTER(_bx, types::WORD)
DECLARE_REGISTER(_ebx, types::DWORD)
DECLARE_REGISTER(_rcx, types::QWORD)
DECLARE_REGISTER(_cl, types::BYTE)
DECLARE_REGISTER(_ch, types::BYTE)
DECLARE_REGISTER(_cx, types::WORD)
DECLARE_REGISTER(_ecx, types::DWORD)
DECLARE_REGISTER(_rdx, types::QWORD)
DECLARE_REGISTER(_dl, types::BYTE)
DECLARE_REGISTER(_dh, types::BYTE)
DECLARE_REGISTER(_dx, types::WORD)
DECLARE_REGISTER(_edx, types::DWORD)
DECLARE_REGISTER(_rdi, types::QWORD)
DECLARE_REGISTER(_di, types::WORD)
DECLARE_REGISTER(_edi, types::DWORD)
DECLARE_REGISTER(_rsi, types::QWORD)
DECLARE_REGISTER(_si, types::WORD)
DECLARE_REGISTER(_esi, types::DWORD)
DECLARE_REGISTER(_rsp, types::QWORD)
DECLARE_REGISTER(_sp, types::WORD)
DECLARE_REGISTER(_esp, types::DWORD)
DECLARE_REGISTER(_rbp, types::QWORD)
DECLARE_REGISTER(_bp, types::WORD)
DECLARE_REGISTER(_ebp, types::DWORD)
DECLARE_REGISTER(_rip, types::QWORD)
DECLARE_REGISTER(_ip, types::WORD)
DECLARE_REGISTER(_eip, types::DWORD)
DECLARE_REGISTER(_rcs, types::QWORD)
DECLARE_REGISTER(_cs, types::WORD)
DECLARE_REGISTER(_ecs, types::DWORD)
DECLARE_REGISTER(_rds, types::QWORD)
DECLARE_REGISTER(_ds, types::WORD)
DECLARE_REGISTER(_eds, types::DWORD)
DECLARE_REGISTER(_res, types::QWORD)
DECLARE_REGISTER(_es, types::WORD)
DECLARE_REGISTER(_ees, types::DWORD)
DECLARE_REGISTER(_rfs, types::QWORD)
DECLARE_REGISTER(_fs, types::WORD)
DECLARE_REGISTER(_efs, types::DWORD)
DECLARE_REGISTER(_rgs, types::QWORD)
DECLARE_REGISTER(_gs, types::WORD)
DECLARE_REGISTER(_egs, types::DWORD)
DECLARE_REGISTER(_rss, types::QWORD)
DECLARE_REGISTER(_ss, types::WORD)
DECLARE_REGISTER(_ess, types::DWORD)
DECLARE_REGISTER(_cf, types::BIT)
DECLARE_REGISTER(_pf, types::BIT)
DECLARE_REGISTER(_af, types::BIT)
DECLARE_REGISTER(_zf, types::BIT)
DECLARE_REGISTER(_sf, types::BIT)
DECLARE_REGISTER(_tf, types::BIT)
DECLARE_REGISTER(_if, types::BIT)
DECLARE_REGISTER(_df, types::BIT)
DECLARE_REGISTER(_of, types::BIT)
DECLARE_REGISTER(_iopl, types::BIT)
DECLARE_REGISTER(_nt, types::BIT)
DECLARE_REGISTER(_rf, types::BIT)
DECLARE_REGISTER(_vm, types::BIT)
DECLARE_REGISTER(_ac, types::BIT)
DECLARE_REGISTER(_vif, types::BIT)
DECLARE_REGISTER(_vip, types::BIT)
DECLARE_REGISTER(_id, types::BIT)
DECLARE_REGISTER(_r8, types::QWORD)
DECLARE_REGISTER(_r8d, types::DWORD)
DECLARE_REGISTER(_r8w, types::WORD)
DECLARE_REGISTER(_r8b, types::BYTE)
DECLARE_REGISTER(_r8h, types::BYTE)
DECLARE_REGISTER(_r9, types::QWORD)
DECLARE_REGISTER(_r9d, types::DWORD)
DECLARE_REGISTER(_r9w, types::WORD)
DECLARE_REGISTER(_r9b, types::BYTE)
DECLARE_REGISTER(_r9h, types::BYTE)
DECLARE_REGISTER(_r10, types::QWORD)
DECLARE_REGISTER(_r10d, types::DWORD)
DECLARE_REGISTER(_r10w, types::WORD)
DECLARE_REGISTER(_r10b, types::BYTE)
DECLARE_REGISTER(_r10h, types::BYTE)
DECLARE_REGISTER(_r11, types::QWORD)
DECLARE_REGISTER(_r11d, types::DWORD)
DECLARE_REGISTER(_r11w, types::WORD)
DECLARE_REGISTER(_r11b, types::BYTE)
DECLARE_REGISTER(_r11h, types::BYTE)
DECLARE_REGISTER(_r12, types::QWORD)
DECLARE_REGISTER(_r12d, types::DWORD)
DECLARE_REGISTER(_r12w, types::WORD)
DECLARE_REGISTER(_r12b, types::BYTE)
DECLARE_REGISTER(_r12h, types::BYTE)
DECLARE_REGISTER(_r13, types::QWORD)
DECLARE_REGISTER(_r13d, types::DWORD)
DECLARE_REGISTER(_r13w, types::WORD)
DECLARE_REGISTER(_r13b, types::BYTE)
DECLARE_REGISTER(_r13h, types::BYTE)
DECLARE_REGISTER(_r14, types::QWORD)
DECLARE_REGISTER(_r14d, types::DWORD)
DECLARE_REGISTER(_r14w, types::WORD)
DECLARE_REGISTER(_r14b, types::BYTE)
DECLARE_REGISTER(_r14h, types::BYTE)
DECLARE_REGISTER(_r15, types::QWORD)
DECLARE_REGISTER(_r15d, types::DWORD)
DECLARE_REGISTER(_r15w, types::WORD)
DECLARE_REGISTER(_r15b, types::BYTE)
DECLARE_REGISTER(_r15h, types::BYTE)
DECLARE_REGISTER(_xmm0, types::XMM)
DECLARE_REGISTER(_xmm1, types::XMM)
DECLARE_REGISTER(_xmm2, types::XMM)
DECLARE_REGISTER(_xmm3, types::XMM)
DECLARE_REGISTER(_xmm4, types::XMM)
DECLARE_REGISTER(_xmm5, types::XMM)
DECLARE_REGISTER(_xmm6, types::XMM)
DECLARE_REGISTER(_xmm7, types::XMM)

const std::map<std::string, VisitableBase *> registers{
    {"rax", CAST_TO_VISITABLE(reg_rax)},
    {"al", CAST_TO_VISITABLE(reg_al)},
    {"ah", CAST_TO_VISITABLE(reg_ah)},
    {"ax", CAST_TO_VISITABLE(reg_ax)},
    {"eax", CAST_TO_VISITABLE(reg_eax)},
    {"rbx", CAST_TO_VISITABLE(reg_rbx)},
    {"bl", CAST_TO_VISITABLE(reg_bl)},
    {"bh", CAST_TO_VISITABLE(reg_bh)},
    {"bx", CAST_TO_VISITABLE(reg_bx)},
    {"ebx", CAST_TO_VISITABLE(reg_ebx)},
    {"rcx", CAST_TO_VISITABLE(reg_rcx)},
    {"cl", CAST_TO_VISITABLE(reg_cl)},
    {"ch", CAST_TO_VISITABLE(reg_ch)},
    {"cx", CAST_TO_VISITABLE(reg_cx)},
    {"ecx", CAST_TO_VISITABLE(reg_ecx)},
    {"rdx", CAST_TO_VISITABLE(reg_rdx)},
    {"dl", CAST_TO_VISITABLE(reg_dl)},
    {"dh", CAST_TO_VISITABLE(reg_dh)},
    {"dx", CAST_TO_VISITABLE(reg_dx)},
    {"edx", CAST_TO_VISITABLE(reg_edx)},
    {"rdi", CAST_TO_VISITABLE(reg_rdi)},
    {"di", CAST_TO_VISITABLE(reg_di)},
    {"edi", CAST_TO_VISITABLE(reg_edi)},
    {"rsi", CAST_TO_VISITABLE(reg_rsi)},
    {"si", CAST_TO_VISITABLE(reg_si)},
    {"esi", CAST_TO_VISITABLE(reg_esi)},
    {"rsp", CAST_TO_VISITABLE(reg_rsp)},
    {"sp", CAST_TO_VISITABLE(reg_sp)},
    {"esp", CAST_TO_VISITABLE(reg_esp)},
    {"rbp", CAST_TO_VISITABLE(reg_rbp)},
    {"bp", CAST_TO_VISITABLE(reg_bp)},
    {"ebp", CAST_TO_VISITABLE(reg_ebp)},
    {"rip", CAST_TO_VISITABLE(reg_rip)},
    {"ip", CAST_TO_VISITABLE(reg_ip)},
    {"eip", CAST_TO_VISITABLE(reg_eip)},
    {"rcs", CAST_TO_VISITABLE(reg_rcs)},
    {"cs", CAST_TO_VISITABLE(reg_cs)},
    {"ecs", CAST_TO_VISITABLE(reg_ecs)},
    {"rds", CAST_TO_VISITABLE(reg_rds)},
    {"ds", CAST_TO_VISITABLE(reg_ds)},
    {"eds", CAST_TO_VISITABLE(reg_eds)},
    {"res", CAST_TO_VISITABLE(reg_res)},
    {"es", CAST_TO_VISITABLE(reg_es)},
    {"ees", CAST_TO_VISITABLE(reg_ees)},
    {"rfs", CAST_TO_VISITABLE(reg_rfs)},
    {"fs", CAST_TO_VISITABLE(reg_fs)},
    {"efs", CAST_TO_VISITABLE(reg_efs)},
    {"rgs", CAST_TO_VISITABLE(reg_rgs)},
    {"gs", CAST_TO_VISITABLE(reg_gs)},
    {"egs", CAST_TO_VISITABLE(reg_egs)},
    {"rss", CAST_TO_VISITABLE(reg_rss)},
    {"ss", CAST_TO_VISITABLE(reg_ss)},
    {"ess", CAST_TO_VISITABLE(reg_ess)},
    {"cf", CAST_TO_VISITABLE(reg_cf)},
    {"pf", CAST_TO_VISITABLE(reg_pf)},
    {"af", CAST_TO_VISITABLE(reg_af)},
    {"zf", CAST_TO_VISITABLE(reg_zf)},
    {"sf", CAST_TO_VISITABLE(reg_sf)},
    {"tf", CAST_TO_VISITABLE(reg_tf)},
    {"if", CAST_TO_VISITABLE(reg_if)},
    {"df", CAST_TO_VISITABLE(reg_df)},
    {"of", CAST_TO_VISITABLE(reg_of)},
    {"iopl", CAST_TO_VISITABLE(reg_iopl)},
    {"nt", CAST_TO_VISITABLE(reg_nt)},
    {"rf", CAST_TO_VISITABLE(reg_rf)},
    {"vm", CAST_TO_VISITABLE(reg_vm)},
    {"ac", CAST_TO_VISITABLE(reg_ac)},
    {"vif", CAST_TO_VISITABLE(reg_vif)},
    {"vip", CAST_TO_VISITABLE(reg_vip)},
    {"id", CAST_TO_VISITABLE(reg_id)},
    {"r8", CAST_TO_VISITABLE(reg_r8)},
    {"r8d", CAST_TO_VISITABLE(reg_r8d)},
    {"r8w", CAST_TO_VISITABLE(reg_r8w)},
    {"r8b", CAST_TO_VISITABLE(reg_r8b)},
    {"r8h", CAST_TO_VISITABLE(reg_r8h)},
    {"r9", CAST_TO_VISITABLE(reg_r9)},
    {"r9d", CAST_TO_VISITABLE(reg_r9d)},
    {"r9w", CAST_TO_VISITABLE(reg_r9w)},
    {"r9b", CAST_TO_VISITABLE(reg_r9b)},
    {"r9h", CAST_TO_VISITABLE(reg_r9h)},
    {"r10", CAST_TO_VISITABLE(reg_r10)},
    {"r10d", CAST_TO_VISITABLE(reg_r10d)},
    {"r10w", CAST_TO_VISITABLE(reg_r10w)},
    {"r10b", CAST_TO_VISITABLE(reg_r10b)},
    {"r10h", CAST_TO_VISITABLE(reg_r10h)},
    {"r11", CAST_TO_VISITABLE(reg_r11)},
    {"r11d", CAST_TO_VISITABLE(reg_r11d)},
    {"r11w", CAST_TO_VISITABLE(reg_r11w)},
    {"r11b", CAST_TO_VISITABLE(reg_r11b)},
    {"r11h", CAST_TO_VISITABLE(reg_r11h)},
    {"r12", CAST_TO_VISITABLE(reg_r12)},
    {"r12d", CAST_TO_VISITABLE(reg_r12d)},
    {"r12w", CAST_TO_VISITABLE(reg_r12w)},
    {"r12b", CAST_TO_VISITABLE(reg_r12b)},
    {"r12h", CAST_TO_VISITABLE(reg_r12h)},
    {"r13", CAST_TO_VISITABLE(reg_r13)},
    {"r13d", CAST_TO_VISITABLE(reg_r13d)},
    {"r13w", CAST_TO_VISITABLE(reg_r13w)},
    {"r13b", CAST_TO_VISITABLE(reg_r13b)},
    {"r13h", CAST_TO_VISITABLE(reg_r13h)},
    {"r14", CAST_TO_VISITABLE(reg_r14)},
    {"r14d", CAST_TO_VISITABLE(reg_r14d)},
    {"r14w", CAST_TO_VISITABLE(reg_r14w)},
    {"r14b", CAST_TO_VISITABLE(reg_r14b)},
    {"r14h", CAST_TO_VISITABLE(reg_r14h)},
    {"r15", CAST_TO_VISITABLE(reg_r15)},
    {"r15d", CAST_TO_VISITABLE(reg_r15d)},
    {"r15w", CAST_TO_VISITABLE(reg_r15w)},
    {"r15b", CAST_TO_VISITABLE(reg_r15b)},
    {"r15h", CAST_TO_VISITABLE(reg_r15h)},
    {"xmm0", CAST_TO_VISITABLE(reg_xmm0)},
    {"xmm1", CAST_TO_VISITABLE(reg_xmm1)},
    {"xmm2", CAST_TO_VISITABLE(reg_xmm2)},
    {"xmm3", CAST_TO_VISITABLE(reg_xmm3)},
    {"xmm4", CAST_TO_VISITABLE(reg_xmm4)},
    {"xmm5", CAST_TO_VISITABLE(reg_xmm5)},
    {"xmm6", CAST_TO_VISITABLE(reg_xmm6)},
    {"xmm7", CAST_TO_VISITABLE(reg_xmm7)},
};

/**
 * Gets name via symbol visitor
 *
 * @param sym to fetch name from
 * @return if visitable is not symbol, returns empty string
 */
std::string get_name(
    const std::shared_ptr<symbol_table::VisitableBase> &sym
) {
  return map_visitable<symbol_table::SymbolVisitorL>(
      sym, [](const symbol_table::Symbol *ptr) -> std::string
      { return ptr->getName(); });
}

std::string Temporary::fetchName(
    const std::string &op,
    const symbol_ptr &rhs,
    const symbol_ptr &lhs
) const {
  std::string
      lhs_name = get_name(lhs),
      rhs_name = get_name(rhs);

  // if it is unary () should be used
  if (lhs_name == "")
    rhs_name = "(" + rhs_name + ")";
  return lhs_name + op + rhs_name;
}
}  // namespace symbol_table

asm_arg_parser::symbol_obserable asm_arg_parser::getArgs(
    const asm_arg_parser::symbol_map &functions
) const throw(std::runtime_error) {
  return parse().skip(1)
      .map([&](const std::string &arg) -> symbol_ptr
           { return handle_expression(arg, functions); })
#if !defined(NASSERT_EX) || NASSERT_EX == 0
      .filter([](
          std::shared_ptr<symbol_table::VisitableBase> ptr
      ) -> bool {
        assert_ex((bool) ptr, "failed to create symbol/expression");
        return true;
      })
#endif // NDEBUG
      ;
}

std::shared_ptr<symbol_table::VisitableBase> asm_arg_parser::create_dereference
    (std::string size, std::string expr) const throw(std::runtime_error) {
  using namespace symbol_table::types;

  using temp_byte = symbol_table::SizedTemporary<BYTE>;
  using temp_word = symbol_table::SizedTemporary<WORD>;
  using temp_dword = symbol_table::SizedTemporary<DWORD>;
  using temp_qword = symbol_table::SizedTemporary<QWORD>;
  using temp_xmm = symbol_table::SizedTemporary<XMM>;

#define IMPLEMENT_TYPE_HANDLER(type) do { \
  if (size == type::size_trait::name) { \
    if (auto rhs = handle_expression(expr)) { \
      return std::make_shared<type>("*", rhs); \
    } \
  } \
} while (false)

  IMPLEMENT_TYPE_HANDLER(temp_byte);
  IMPLEMENT_TYPE_HANDLER(temp_word);
  IMPLEMENT_TYPE_HANDLER(temp_dword);
  IMPLEMENT_TYPE_HANDLER(temp_qword);
  IMPLEMENT_TYPE_HANDLER(temp_xmm);

#undef IMPLEMENT_TYPE_HANDLER

  return nullptr;
//  throw std::runtime_error(
//      "unknown type of parameter SIZE: " + size + " EXPR: " + expr);
}

std::shared_ptr<symbol_table::VisitableBase> asm_arg_parser::create_operation
    (std::string lhs, std::string op,
     std::string rhs) const throw(std::runtime_error) {
  if (auto lhs_expr = handle_expression(lhs))
    if (auto rhs_expr = handle_expression(rhs))
      return std::make_shared<symbol_table::Temporary>(
          lhs_expr, op, rhs_expr
      );
  return nullptr;
//  throw std::runtime_error(
//      "'" + lhs + op + rhs + "' expression not implemented"
//  );
}

std::shared_ptr<symbol_table::VisitableBase> asm_arg_parser::create_imm
    (std::string value) const throw() {
  return std::make_shared<symbol_table::Immidiate>(value);
}

static std::regex multiplication("(.+)(\\*)(.+)");
static std::regex add_or_substract("(.+)(\\+|\\-)(.+)");
static std::regex dereference(
    "(XMMWORD|BYTE|WORD|DWORD|QWORD) PTR (?:\\w+:)?\\[(.*)\\]"
);
static std::regex number("((?:0x0*)?[0-9a-fA-F]+)");
static std::regex address("(?:0x)?0*([0-9a-fA-F]+)");

std::shared_ptr<symbol_table::VisitableBase> asm_arg_parser::handle_expression(
    std::string expr,
    const asm_arg_parser::symbol_map &functions
) const throw(std::runtime_error) {
  std::smatch result;

  { // if (possible) parameter is function
    if (std::regex_match(expr, result, address)) {
      // find function by address
      auto func_symbol = functions.find(std::stoul(result.str(1), nullptr, 16));
      if (func_symbol != functions.cend()) {
        return func_symbol->second;
      }
    }
  }

  { // if (possible) parameter is register
    auto reg_symbol = symbol_table::registers.find(expr);
    if (reg_symbol != symbol_table::registers.end()) {
      return std::shared_ptr<symbol_table::VisitableBase>(
          reg_symbol->second, symbol_table::register_deleter
      );
    }
  }

  { // is it value?
    if (std::regex_match(expr, result, number)) {
      return create_imm(result.str(1));
    }
  }

  // note the predcedence of operators:
  //  1. dereference
  //  2. multiplication
  //  3. add/subtraction

  { // it is dereference?
    if (std::regex_match(expr, result, dereference)) {
      return create_dereference(
          result.str(1), result.str(2)
      );
    }
  }

  { // is it multiplication operation?
    if (std::regex_match(expr, result, multiplication)) {
      return create_operation(result.str(1), result.str(2), result.str(3));
    }
  }

  { // is it add or subtract operation?
    if (std::regex_match(expr, result, add_or_substract)) {
      return create_operation(result.str(1), result.str(2), result.str(3));
    }
  }

  // unknown symbol or expression
  return std::make_shared<symbol_table::Symbol>(expr);
}
//
