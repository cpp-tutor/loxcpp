#ifndef Expr_h
#define Expr_h

#include "FwdTypes.hpp"
#include <vector>

class Expr : public SType {
public:
    virtual Value accept(const ExprVisitor&) const { return std::monostate{}; }
    virtual ~Expr() {}
};

class ExprAssign : public Expr {
    std::string name;
    std::shared_ptr<Expr> expr;
public:
    ExprAssign(const std::string& name, std::shared_ptr<Expr> expr)
        : name{ name }, expr{ expr } {}
    const auto& getName() const { return name; }
    const auto getExpr() const { return expr; }
    virtual Value accept(const ExprVisitor&) const override;
};

class ExprBinary : public Expr {
    int op;
    std::shared_ptr<Expr> left, right;
public:
    ExprBinary(int op, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right)
        : op{ op }, left{ left }, right { right } {}
    const auto getOp() const { return op; }
    const auto getLeft() const { return left; }
    const auto getRight() const { return right; }
    virtual Value accept(const ExprVisitor&) const override;
};

class ExprCall : public Expr {
    std::shared_ptr<Expr> callee;
    std::vector<std::shared_ptr<Expr>> arguments;
public:
    ExprCall() = default;
    ExprCall(std::shared_ptr<Expr> callee, const std::vector<std::shared_ptr<Expr>>& arguments)
        : callee{ callee }, arguments{ arguments } {}
    void appendArg(std::shared_ptr<Expr> argument) { arguments.push_back(argument); }
    void setCallee(std::shared_ptr<Expr> c) { callee = c; }
    const auto getCallee() const { return callee; }
    const auto& getArgs() const { return arguments; }
    virtual Value accept(const ExprVisitor&) const override;
};

class ExprGet : public Expr {
    std::shared_ptr<Expr> object;
    std::string name;
public:
    ExprGet(std::shared_ptr<Expr> object, std::string name)
        : object{ object }, name{ name } {}
    const auto getObject() const { return object; }
    const auto& getName() const { return name; }
    virtual Value accept(const ExprVisitor&) const override;
};

class ExprGrouping : public Expr {
    std::shared_ptr<Expr> expr;
public:
    ExprGrouping(std::shared_ptr<Expr> expr)
        : expr{ expr } {}
    const auto get() const { return expr; }
    virtual Value accept(const ExprVisitor&) const override;
};

class ExprLiteral : public Expr {
    Value value;
public:
    explicit ExprLiteral(std::monostate l) : value{ l } {}
    explicit ExprLiteral(bool l) : value{ l } {}
    explicit ExprLiteral(double l) : value{ l } {}
    explicit ExprLiteral(const std::string& l) : value{ l } {}
    const auto get() const { return value; }
    virtual Value accept(const ExprVisitor&) const override;
};

class ExprLogical : public Expr {
    int op;
    std::shared_ptr<Expr> left, right;
public:
    ExprLogical(int op, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right)
        : op{ op }, left{ left }, right { right } {}
    const auto getOp() const { return op; }
    const auto getLeft() const { return left; }
    const auto getRight() const { return right; }
    virtual Value accept(const ExprVisitor&) const override;
};

class ExprSet : public Expr {
    std::shared_ptr<Expr> object, value;
    std::string name;
public:
    ExprSet(std::shared_ptr<Expr> object, const std::string& name, std::shared_ptr<Expr> value)
        : object{ object }, value{ value }, name{ name } {}
    const auto getObject() const { return object; }
    const auto getValue() const { return value; }
    const auto& getName() const { return name; }
    virtual Value accept(const ExprVisitor&) const override;
};

class ExprSuper : public Expr {
    std::string name;
public:
    ExprSuper(const std::string& name) : name{ name } {}
    const auto& get() const { return name; }
    virtual Value accept(const ExprVisitor&) const override;
};

class ExprThis : public Expr {
public:
    ExprThis() = default;
    virtual Value accept(const ExprVisitor&) const override;
};

class ExprUnary : public Expr {
    int op;
    std::shared_ptr<Expr> expr;
public:
    ExprUnary(int op, std::shared_ptr<Expr> expr)
        : op{ op }, expr{ expr } {}
    const auto getOp() const { return op; }
    const auto get() const { return expr; }
    virtual Value accept(const ExprVisitor&) const override;
};

class ExprVariable : public Expr {
    std::string name;
public:
    ExprVariable(const std::string& name) : name{ name } {}
    const auto& get() const { return name; }
    virtual Value accept(const ExprVisitor&) const override;
};

#endif // Expr_h