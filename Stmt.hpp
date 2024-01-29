#ifndef Stmt_h
#define Stmt_h

#include "FwdTypes.hpp"
#include "Expr.hpp"
#include <string>
#include <vector>

class Stmt : public SType {
public:
    virtual void accept(const StmtVisitor&) const = 0;
    virtual ~Stmt() {}
};

class StmtFunction;

class StmtBlock : public Stmt {
    std::vector<std::shared_ptr<Stmt>> statements;
public:
    StmtBlock() = default;
    StmtBlock(const std::vector<std::shared_ptr<Stmt>>& statements) : statements{ statements } {}
    void append(std::shared_ptr<Stmt> statement) {
        statements.push_back(statement);
    }
    const auto& get() const { return statements; }
    void accept(const StmtVisitor&) const override;
};

class StmtClass : public Stmt {
    std::string name;
    std::shared_ptr<ExprVariable> superclass;
    std::vector<std::shared_ptr<StmtFunction>> methods;
public:
    StmtClass() = default;
    StmtClass(const std::string& name,
        std::shared_ptr<ExprVariable> superclass,
        const std::vector<std::shared_ptr<StmtFunction>>& methods)
        : name{ name }, superclass{ superclass }, methods{ methods } {}
    void setName(const std::string& n) { name = n; }
    void setSuper(std::shared_ptr<ExprVariable> s) { superclass = s; }
    void addMethod(std::shared_ptr<StmtFunction> m) { methods.push_back(m); }
    const auto& getName() const { return name; }
    const auto& getSuper() const { return superclass; }
    const auto& getMethods() const { return methods; }
    void accept(const StmtVisitor&) const override;
};

class StmtExpression : public Stmt {
    std::shared_ptr<Expr> expression;
public:
    StmtExpression(std::shared_ptr<Expr> expression) : expression{ expression } {}
    const auto get() const { return expression; }
    void accept(const StmtVisitor&) const override;
};

class StmtFunction : public Stmt {
    std::string name;
    std::vector<std::string> params;
    std::shared_ptr<StmtBlock> body;
public:
    StmtFunction() = default;
    StmtFunction(const std::string& name,
        const std::vector<std::string>& params,
        std::shared_ptr<StmtBlock> body)
        : name{ name }, params{ params }, body{ body } {}
    void setName(const std::string& n) { name = n; }
    void addParam(const std::string& p) { params.push_back(p); }
    void setBody(std::shared_ptr<StmtBlock> b) { body = b; }
    const auto& getName() const { return name; }
    const auto& getParams() const { return params; }
    const auto& getBody() const { return body; }
    void accept(const StmtVisitor&) const override;
};

class StmtIf : public Stmt {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> thenBranch;
    std::shared_ptr<Stmt> elseBranch;
public:
    StmtIf(std::shared_ptr<Expr> condition,
        std::shared_ptr<Stmt> thenBranch,
        std::shared_ptr<Stmt> elseBranch)
        : condition{ condition }, thenBranch{ thenBranch }, elseBranch{ elseBranch } {}
    const auto getCond() const { return condition; }
    const auto getThen() const { return thenBranch; }
    const auto getElse() const { return elseBranch; }
    void accept(const StmtVisitor&) const override;
};

class StmtPrint : public Stmt {
    std::shared_ptr<Expr> expression;
public:
    StmtPrint(std::shared_ptr<Expr> expression) : expression{ expression } {}
    const auto get() const { return expression; }
    void accept(const StmtVisitor&) const override;
};

class StmtReturn : public Stmt {
    std::shared_ptr<Expr> value;
public:
    StmtReturn(std::shared_ptr<Expr> value) : value{ value } {}
    const auto get() const { return value; }
    void accept(const StmtVisitor&) const override;
};

class StmtVariable : public Stmt {
    std::string name;
    std::shared_ptr<Expr> initializer;
    bool isConstant;
public:
    StmtVariable(const std::string& name,
        std::shared_ptr<Expr> initializer,
        bool isConstant = false)
        : name{ name }, initializer{ initializer }, isConstant{ isConstant } {}
    const auto& getName() const { return name; }
    const auto getInit() const { return initializer; }
    const auto getIsConstant() const { return isConstant; }
    void accept(const StmtVisitor&) const override;
};

class StmtWhile : public Stmt {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> body;
public:
    StmtWhile(std::shared_ptr<Expr> condition,
        std::shared_ptr<Stmt> body)
        : condition{ condition }, body{ body } {}
    const auto getCond() const { return condition; }
    const auto getBody() const { return body; }
    void accept(const StmtVisitor&) const override;
};

#endif // Stmt_h
