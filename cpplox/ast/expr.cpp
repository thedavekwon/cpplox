#include <memory>

#include <ast/expr.h>

namespace cpplox {

AssignExpr::AssignExpr(Token n, Expr v) : name(std::move(n)), value(std::make_unique<Expr>(std::move(v))) {}

BinaryExpr::BinaryExpr(Expr l, Token o, Expr r) : left(std::make_unique<Expr>(std::move(l))), op(std::move(o)), right(std::make_unique<Expr>(std::move(r))) {}

GroupingExpr::GroupingExpr(Expr e) : expr(std::make_unique<Expr>(std::move(e))) {}

LogicalExpr::LogicalExpr(Expr l, Token o, Expr r) : left(std::make_unique<Expr>(std::move(l))), op(std::move(o)), right(std::make_unique<Expr>(std::move(r))) {}

UnaryExpr::UnaryExpr(Token o, Expr r) : op(std::move(o)), right(std::make_unique<Expr>(std::move(r))) {}

VarExpr::VarExpr(Token t) : name(std::move(t)) {}

} // cpplox