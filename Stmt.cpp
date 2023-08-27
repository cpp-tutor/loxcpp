#include "Stmt.hpp"
#include "visitor/StmtVisitor.hpp"

void StmtBlock::accept(const StmtVisitor& v) const {
    v(*this);
}

void StmtClass::accept(const StmtVisitor& v) const {
    v(*this);
}

void StmtExpression::accept(const StmtVisitor& v) const {
    v(*this);
}

void StmtFunction::accept(const StmtVisitor& v) const {
    v(*this);
}

void StmtIf::accept(const StmtVisitor& v) const {
    v(*this);
}

void StmtPrint::accept(const StmtVisitor& v) const {
    v(*this);
}

void StmtReturn::accept(const StmtVisitor& v) const {
    v(*this);
}

void StmtVariable::accept(const StmtVisitor& v) const {
    v(*this);
}

void StmtWhile::accept(const StmtVisitor& v) const {
    v(*this);
}
