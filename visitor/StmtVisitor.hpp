#ifndef StmtVisitor_h
#define StmtVisitor_h

#include "../Stmt.hpp"
#include "../Environment.hpp"
#include "ExprVisitor.hpp"

class StmtVisitor {
public:
    virtual void operator()(const StmtBlock&) const = 0;
    virtual void operator()(const StmtCase&) const = 0;
    virtual void operator()(const StmtClass&) const = 0;
    virtual void operator()(const StmtExpression&) const = 0;
    virtual void operator()(const StmtFunction&) const = 0;
    virtual void operator()(const StmtIf&) const = 0;
    virtual void operator()(const StmtPrint&) const = 0;
    virtual void operator()(const StmtRepeat&) const = 0;
    virtual void operator()(const StmtReturn&) const = 0;
    virtual void operator()(const StmtVariable&) const = 0;
    virtual void operator()(const StmtWhile&) const = 0;
    virtual ~StmtVisitor() {}
};

#endif // StmtVisitor_h