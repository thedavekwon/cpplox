#include <ast/statement.h>

namespace cpplox {

BlockStatement::BlockStatement(std::vector<Statement> s) : statements(std::move(s)) {}

ExprStatement::ExprStatement(Expr e) : expr(std::move(e)) {}

PrintStatement::PrintStatement(Expr e) : expr(std::move(e)) {}

VarStatement::VarStatement(Token token, std::optional<Expr> init) : name(std::move(token)), initializer(std::move(init)) {}

} // cpplox