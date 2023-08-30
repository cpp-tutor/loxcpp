#ifndef Resolver_h
#define Resolver_h

#include "Interpreter.hpp"
#include <vector>
#include <unordered_map>

class Resolver : public ExprVisitor, public StmtVisitor {
    Interpreter& interpreter;
    mutable std::vector<std::unordered_map<std::string,bool>> scopes;
    enum class FunctionType {
        NONE,
        FUNCTION,
        INITIALIZER,
        METHOD
    };
    enum class ClassType {
        NONE,
        CLASS,
        SUBCLASS
    };
    mutable FunctionType currentFunction = FunctionType::NONE;
    mutable ClassType currentClass = ClassType::NONE;
public:
    Resolver(Interpreter& interpreter) : interpreter{ interpreter } {}

    virtual Value operator()(const ExprAssign& e) const override {
        resolve(e.getExpr());
        resolveLocal(&e, e.getName());
        return std::monostate{};
    }

    virtual Value operator()(const ExprBinary& e) const override {
        resolve(e.getLeft());
        resolve(e.getRight());
        return std::monostate{};
    }

    virtual Value operator()(const ExprCall& e) const override {
        resolve(e.getCallee());
        for (auto arg : e.getArgs()) {
            resolve(arg);
        }
        return std::monostate{};
    }

    virtual Value operator()(const ExprGet& e) const override {
        resolve(e.getObject());
        return std::monostate{};
    }

    virtual Value operator()(const ExprGrouping& e) const override {
        resolve(e.get());
        return std::monostate{};
    }

    virtual Value operator()(const ExprLiteral& e) const override {
        return std::monostate{};
    }

    virtual Value operator()(const ExprLogical& e) const override {
        resolve(e.getLeft());
        resolve(e.getRight());
        return std::monostate{};
    }

    virtual Value operator()(const ExprSet& e) const override {
        resolve(e.getValue());
        resolve(e.getObject());
        return std::monostate{};
    }

    virtual Value operator()(const ExprSuper& e) const override {
        if (currentClass == ClassType::NONE) {
            throw Error("Can't use 'super' outside of a class.");
        }
        else if (currentClass != ClassType::SUBCLASS) {
            throw Error("Can't use 'super' in a class with no superclass.");
        }
        resolveLocal(&e, "super");
        return std::monostate{};
    }

    virtual Value operator()(const ExprThis& e) const override {
        if (currentClass == ClassType::NONE) {
            throw Error("Can't use 'this' outside of a class.");
        }
        resolveLocal(&e, "this");
        return std::monostate{};
    }

    virtual Value operator()(const ExprUnary& e) const override {
        resolve(e.get());
        return std::monostate{};
    }

    virtual Value operator()(const ExprVariable& e) const override {
        if (!scopes.empty()) {
            if (auto iter = scopes.back().find(e.get());
                iter != scopes.back().end() && iter->second == false) {
                throw Error( "Can't read local variable in its own initializer.");
            }
        }
        resolveLocal(&e, e.get());
        return std::monostate{};
    }

    virtual void operator()(const StmtBlock& s) const override {
        beginScope();
        resolve(s.get());
        endScope();
    }

    virtual void operator()(const StmtClass& s) const override {
        ClassType enclosingClass = currentClass;
        currentClass = ClassType::CLASS;
        declare(s.getName());
        define(s.getName());
        if (s.getSuper()) {
            if (s.getSuper()->get() == s.getName()) {
                throw Error("A class can't inherit from itself.");
            }
            currentClass = ClassType::SUBCLASS;
            resolve(s.getSuper());
            beginScope();
            scopes.back().insert({ "super", true });
        }
        beginScope();
        scopes.back().insert({ "this", true });
        for (auto method : s.getMethods()) {
            auto declaration = (method->getName() == "init") ? FunctionType::INITIALIZER : FunctionType::METHOD;
            resolveFunction(*method, declaration);
        }
        endScope();
        if (s.getSuper()) {
            endScope();
        }
        currentClass = enclosingClass;
    }

    virtual void operator()(const StmtExpression& s) const override {
        resolve(s.get());
    }
    
    virtual void operator()(const StmtFunction& s) const override {
        declare(s.getName());
        define(s.getName());
        resolveFunction(s, FunctionType::FUNCTION);
    }

    virtual void operator()(const StmtIf& s) const override {
        resolve(s.getCond());
        resolve(s.getThen());
        if (s.getElse()) {
            resolve(s.getElse());
        }
    }

    virtual void operator()(const StmtPrint& s) const override {
        resolve(s.get());
    }

    virtual void operator()(const StmtReturn&s) const override {
        if (currentFunction == FunctionType::NONE) {
            throw Error("Can't return from top-level code.");
        }
        auto nil = std::dynamic_pointer_cast<ExprLiteral>(s.get());
        if (!nil || static_cast<ValueType>(nil->get().index()) != ValueType::Nil) {
            if (currentFunction == FunctionType::INITIALIZER) {
                throw Error("Can't return a value from an initializer.");
            }
            resolve(s.get());
        }
    }

    virtual void operator()(const StmtVariable& s) const override {
        declare(s.getName());
        if (s.getInit()) {
            resolve(s.getInit());
        }
        define(s.getName());
    }

    virtual void operator()(const StmtWhile& s) const override {
        resolve(s.getCond());
        resolve(s.getBody());
    }

private:
    void resolve(const std::vector<std::shared_ptr<Stmt>>& statements) const {
        for (auto statement : statements) {
            resolve(statement);
        }
    }

    void resolve(std::shared_ptr<Stmt> statement) const {
        statement->accept(*this);
    }

    void resolve(std::shared_ptr<Expr> statement) const {
        statement->accept(*this);
    }

    void beginScope() const {
        scopes.push_back({});
    }

    void endScope() const {
        scopes.pop_back();
    }

    void declare(const std::string& name) const {
        if (scopes.empty()) {
            return;
        }
        if (auto iter = scopes.back().find(name); iter == scopes.back().end()) {
            scopes.back().insert({ name, false });
        }
        else {
            throw Error("Already a variable named " + name + " in this scope.");
        }
    }

    void define(const std::string& name) const {
        if (scopes.empty()) {
            return;
        }
        if (auto iter = scopes.back().find(name); iter == scopes.back().end()) {
            scopes.back().insert({ name, true });
        }
        else {
            iter->second = true;
        }
    }

    void resolveLocal(const Expr *e, const std::string& name) const {
        auto sz = static_cast<int>(scopes.size());
        for (int i = sz - 1; i >= 0; i--) {
            if (auto iter = scopes.at(i).find(name); iter != scopes.at(i).end()) {
                interpreter.resolve(e, sz - 1 - i);
                return;
            }
        }
    }

    void resolveFunction(const StmtFunction& s, FunctionType type) const {
        FunctionType enclosingFunction = currentFunction;
        currentFunction = type;
        beginScope();
        for (const auto& param : s.getParams()) {
            declare(param);
            define(param);
        }
        resolve(s.getBody());
        endScope();
        currentFunction = enclosingFunction;
    }

};

#endif // Resolver_h