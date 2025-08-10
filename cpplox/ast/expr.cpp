#include <memory>

#include <ast/expr.h>

namespace cpplox {

AssignExpr::AssignExpr(Token n, Expr v) : name(std::move(n)), object(std::make_unique<Expr>(std::move(v))) {}

BinaryExpr::BinaryExpr(Expr l, Token o, Expr r) : left(std::make_unique<Expr>(std::move(l))), op(std::move(o)), right(std::make_unique<Expr>(std::move(r))) {}

CallExpr::CallExpr(Expr c, Token p, std::vector<Expr> a) : callee(std::make_unique<Expr>(std::move(c))), paren(std::move(p)), arguments(std::move(a)) {}

GetExpr::GetExpr(Expr o, Token n) : object(std::make_unique<Expr>(std::move(o))), name(std::move(n)) {}

GroupingExpr::GroupingExpr(Expr e) : expr(std::make_unique<Expr>(std::move(e))) {}

LogicalExpr::LogicalExpr(Expr l, Token o, Expr r) : left(std::make_unique<Expr>(std::move(l))), op(std::move(o)), right(std::make_unique<Expr>(std::move(r))) {}

SetExpr::SetExpr(Expr o, Token n, Expr v) : object(std::make_unique<Expr>(std::move(o))), name(std::move(n)), value(std::make_unique<Expr>(std::move(v))) {}

SuperExpr::SuperExpr(Token k, Token m) : keyword(std::move(k)), method(std::move(m)) {}

ThisExpr::ThisExpr(Token keyword) : keyword(std::move(keyword)) {}

UnaryExpr::UnaryExpr(Token o, Expr r) : op(std::move(o)), right(std::make_unique<Expr>(std::move(r))) {}

VarExpr::VarExpr(Token t) : name(std::move(t)) {}

} // cpplox