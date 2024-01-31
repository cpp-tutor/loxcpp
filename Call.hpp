#ifndef Call_h
#define Call_h

#include "FwdTypes.hpp"
#include "Stmt.hpp"
#include <vector>
#include <string>
#include <ctime>

class Environment;
class Interpreter;
class Expr;
class LoxInstance;

class LoxCallable {
public:
    virtual Value call(const Interpreter&, const std::vector<std::shared_ptr<Expr>>&) = 0;
    virtual const size_t arity() const = 0;
    virtual const std::string toString() const = 0;
};

class LoxFunction : public LoxCallable {
public:
    std::string name;
    std::vector<std::string> params;
    std::shared_ptr<StmtBlock> body;
    std::shared_ptr<Environment> closure;
    bool isInitializer;
public:
    LoxFunction() = default;
    LoxFunction(const StmtFunction& declaration, std::shared_ptr<Environment> closure, bool isInitializer)
        : name{ declaration.getName() }, params{ declaration.getParams() },
        body { declaration.getBody() }, closure{ closure }, isInitializer{ isInitializer } {}
    virtual Value call(const Interpreter&, const std::vector<std::shared_ptr<Expr>>&) override;
    virtual const size_t arity() const override { return params.size(); }
    virtual const std::string toString() const override { return "<fn " + name + '>'; }
    std::shared_ptr<LoxFunction> bind(std::shared_ptr<LoxInstance>);
};

class Clock : public LoxCallable {
    virtual Value call(const Interpreter&, const std::vector<std::shared_ptr<Expr>>&) override {
        return static_cast<double>(clock());
    }
    virtual const size_t arity() const override { return 0; }
    virtual const std::string toString() const override { return "<native fn>"; }
};

#endif // Call_h