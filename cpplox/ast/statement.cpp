#include <ast/statement.h>

namespace cpplox {

BlockStatement::BlockStatement(std::vector<Statement> s) : statements(std::move(s)) {}

ExprStatement::ExprStatement(Expr e) : expr(std::move(e)) {}

IfStatement::IfStatement(Expr c, Statement t) : condition(std::move(c)), thenBranch(std::make_unique<Statement>(std::move(t))) {}
IfStatement::IfStatement(Expr c, Statement t, Statement e) : condition(std::move(c)), thenBranch(std::make_unique<Statement>(std::move(t))), elseBranch(std::make_unique<Statement>(std::move(e))) {}

PrintStatement::PrintStatement(Expr e) : expr(std::move(e)) {}

VarStatement::VarStatement(Token token, std::optional<Expr> init) : name(std::move(token)), initializer(std::move(init)) {}

WhileStatement::WhileStatement(Expr c, Statement b) : condition(std::move(c)), body(std::make_unique<Statement>(std::move(b))) {}

} // cpplox