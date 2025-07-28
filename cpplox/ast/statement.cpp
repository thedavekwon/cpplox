#include <ast/statement.h>

namespace cpplox {

BlockStatement::BlockStatement(std::vector<Statement> s) : statements(std::move(s)) {}

ExprStatement::ExprStatement(Expr e) : expr(std::move(e)) {}

FunctionStatement::FunctionStatement(Token n, std::vector<Token> p, BlockStatement b) : name(std::move(n)), params(std::move(p)), body(std::make_unique<BlockStatement>(std::move(b))) {}

IfStatement::IfStatement(Expr c, Statement t) : condition(std::move(c)), thenBranch(std::make_unique<Statement>(std::move(t))) {}
IfStatement::IfStatement(Expr c, Statement t, Statement e) : condition(std::move(c)), thenBranch(std::make_unique<Statement>(std::move(t))), elseBranch(std::make_unique<Statement>(std::move(e))) {}

PrintStatement::PrintStatement(Expr e) : expr(std::move(e)) {}

ReturnStatement::ReturnStatement(Token k, std::optional<Expr> v) : keyword(std::move(k)), value(std::move(v)) {}

VarStatement::VarStatement(Token t, std::optional<Expr> i) : name(std::move(t)), initializer(std::move(i)) {}

WhileStatement::WhileStatement(Expr c, Statement b) : condition(std::move(c)), body(std::make_unique<Statement>(std::move(b))) {}

} // cpplox