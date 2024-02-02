#ifndef ExprVisitor_h
#define ExprVisitor_h

#include "../Expr.hpp"
#include "../Environment.hpp"

class ExprVisitor {
public:
    virtual Value operator()(const ExprArray&) const = 0;
    virtual Value operator()(const ExprAssign&) const = 0;
    virtual Value operator()(const ExprBinary&) const = 0;
    virtual Value operator()(const ExprCall&) const = 0;
    virtual Value operator()(const ExprGet&) const = 0;
    virtual Value operator()(const ExprGrouping&) const = 0;
    virtual Value operator()(const ExprLiteral&) const = 0;
    virtual Value operator()(const ExprLogical&) const = 0;
    virtual Value operator()(const ExprSet&) const = 0;
    virtual Value operator()(const ExprSetSubscript&) const = 0;
    virtual Value operator()(const ExprSubscript&) const = 0;
    virtual Value operator()(const ExprSuper&) const = 0;
    virtual Value operator()(const ExprTernary&) const = 0;
    virtual Value operator()(const ExprThis&) const = 0;
    virtual Value operator()(const ExprUnary&) const = 0;
    virtual Value operator()(const ExprVariable&) const = 0;
    virtual ~ExprVisitor() {}
};

#endif // ExprVisitor_h