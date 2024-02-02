#include "FwdTypes.hpp"
#include "Expr.hpp"
#include "visitor/ExprVisitor.hpp"

Value ExprArray::accept(const ExprVisitor& v) const {
    return v(*this);
}

Value ExprAssign::accept(const ExprVisitor& v) const {
    return v(*this);
}

Value ExprBinary::accept(const ExprVisitor& v) const {
    return v(*this);
}

Value ExprCall::accept(const ExprVisitor& v) const {
    return v(*this);
}

Value ExprGet::accept(const ExprVisitor& v) const {
    return v(*this);
}

Value ExprGrouping::accept(const ExprVisitor& v) const {
    return v(*this);
}

Value ExprLiteral::accept(const ExprVisitor& v) const {
    return v(*this);
}

Value ExprLogical::accept(const ExprVisitor& v) const {
    return v(*this);
}

Value ExprSet::accept(const ExprVisitor& v) const {
    return v(*this);
}

Value ExprSetSubscript::accept(const ExprVisitor& v) const {
    return v(*this);
}

Value ExprSubscript::accept(const ExprVisitor& v) const {
    return v(*this);
}

Value ExprSuper::accept(const ExprVisitor& v) const {
    return v(*this);
}

Value ExprTernary::accept(const ExprVisitor& v) const {
    return v(*this);
}

Value ExprThis::accept(const ExprVisitor& v) const {
    return v(*this);
}

Value ExprUnary::accept(const ExprVisitor& v) const {
    return v(*this);
}

Value ExprVariable::accept(const ExprVisitor& v) const {
    return v(*this);
}
