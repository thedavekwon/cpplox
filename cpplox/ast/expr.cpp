#include <memory>

#include <ast/expr.h>

namespace cpplox {

BinaryExpr::BinaryExpr(Expr l, Token o, Expr r) : left(std::make_unique<Expr>(std::move(l))), op(std::move(o)), right(std::make_unique<Expr>(std::move(r))) {}

GroupingExpr::GroupingExpr(Expr e) : expr(std::make_unique<Expr>(std::move(e))) {}

UnaryExpr::UnaryExpr(Token o, Expr r) : op(std::move(o)), right(std::make_unique<Expr>(std::move(r))) {}

} // cpplox