#ifndef Interpreter_h
#define Interpreter_h

#include "../FwdTypes.hpp"
#include "../parser/Tokens.h"
#include "ExprVisitor.hpp"
#include "StmtVisitor.hpp"
#include "../Environment.hpp"
#include "../Call.hpp"
#include "../Class.hpp"
#include <iostream>
#include <unordered_map>

class Interpreter : public ExprVisitor, public StmtVisitor {
    std::shared_ptr<Environment> globals;
    mutable std::shared_ptr<Environment> environment;
    std::ostream& out;
    mutable Value recent;
    mutable std::unordered_map<const Expr*,int> locals;
    friend Value LoxFunction::call(const Interpreter&, const std::vector<std::shared_ptr<Expr>>&);
public:
    Interpreter(std::shared_ptr<Environment> environment, std::ostream& out = std::cout)
        : globals{ environment }, environment{ environment }, out{ out } {}
    Interpreter(const Interpreter&) = default;
    
    const Value& lastValue() const { return recent; }

    virtual Value operator()(const ExprArray& e) const override {
        Value value = std::make_shared<LoxArray>();
        for (const auto& element : e.get()) {
            get<std::shared_ptr<LoxArray>>(value)->append(evaluate(element));
        }
        return value;
    }

    virtual Value operator()(const ExprAssign& e) const override {
        Value value = evaluate(e.getExpr());
        int distance = -1;
        if (auto iter = locals.find(&e); iter != locals.end()) {
            distance = iter->second;
        }
        if (distance != -1) {
            environment->assignAt(distance, e.getName(), value);
        }
        else {
            globals->assign(e.getName(), value);
        }
        return value;
    }

    virtual Value operator()(const ExprBinary& e) const override {
        Value eval1 = evaluate(e.getLeft()), eval2 = evaluate(e.getRight());
        if (static_cast<ValueType>(eval1.index()) == ValueType::Number
            && static_cast<ValueType>(eval2.index()) == ValueType::Number) {
            switch (e.getOp()) {
                case Tokens::NOT_EQUAL:
                    return get<double>(eval1) != get<double>(eval2);
                case Tokens::EQUAL_EQUAL:
                    return get<double>(eval1) == get<double>(eval2);
                case Tokens::GREATER:
                    return get<double>(eval1) > get<double>(eval2);
                case Tokens::GREATER_EQUAL:
                    return get<double>(eval1) >= get<double>(eval2);
                case Tokens::LESS:
                    return get<double>(eval1) < get<double>(eval2);
                case Tokens::LESS_EQUAL:
                    return get<double>(eval1) <= get<double>(eval2);
                case Tokens::MINUS:
                    return get<double>(eval1) - get<double>(eval2);
                case Tokens::PLUS:
                    return get<double>(eval1) + get<double>(eval2);
                case Tokens::SLASH:
                    if (get<double>(eval2) == 0.0) {
                        throw Error("Divide by zero");
                    }
                    return get<double>(eval1) / get<double>(eval2);
                case Tokens::STAR:
                    return get<double>(eval1) * get<double>(eval2);
            }
        }
        else if (static_cast<ValueType>(eval1.index()) == ValueType::String
            && static_cast<ValueType>(eval2.index()) == ValueType::String) {
            switch (e.getOp()) {
                case Tokens::NOT_EQUAL:
                    return get<std::string>(eval1) != get<std::string>(eval2);
                case Tokens::EQUAL_EQUAL:
                    return get<std::string>(eval1) == get<std::string>(eval2);
                case Tokens::GREATER:
                    return get<std::string>(eval1) > get<std::string>(eval2);
                case Tokens::GREATER_EQUAL:
                    return get<std::string>(eval1) >= get<std::string>(eval2);
                case Tokens::LESS:
                    return get<std::string>(eval1) < get<std::string>(eval2);
                case Tokens::LESS_EQUAL:
                    return get<std::string>(eval1) <= get<std::string>(eval2);
                case Tokens::PLUS:
                    return get<std::string>(eval1) + get<std::string>(eval2);
            }
        }
        else if (static_cast<ValueType>(eval1.index()) == ValueType::Boolean
            && static_cast<ValueType>(eval2.index()) == ValueType::Boolean) {
            switch (e.getOp()) {
                case Tokens::NOT_EQUAL:
                    return get<bool>(eval1) != get<bool>(eval2);
                case Tokens::EQUAL_EQUAL:
                    return get<bool>(eval1) == get<bool>(eval2);
            }
        }
        else {
            throw Error("Bad type(s) for binary operator");
        }
        throw Error("Bad binary operator for type");
    }

    virtual Value operator()(const ExprCall& e) const override {
        if (e.getArgs().size() >= 255) {
            throw Error("Too many arguments");
        }
        Value callee = evaluate(e.getCallee());
        if (static_cast<ValueType>(callee.index()) != ValueType::Callable) {
            throw Error("Only functions and classes are callable");
        }
        if (e.getArgs().size() != get<std::shared_ptr<LoxCallable>>(callee)->arity()) {
            throw Error("Wrong number of arguments");
        }
        return get<std::shared_ptr<LoxCallable>>(callee)->call(*this, e.getArgs());
    }

    virtual Value operator()(const ExprGet& e) const override {
        Value value = evaluate(e.getObject());
        if (static_cast<ValueType>(value.index()) == ValueType::Instance) {
            return get<std::shared_ptr<LoxInstance>>(value)->get(e.getName());
        }
        else {
            throw Error("Only instances have properties");
        }
    }

    virtual Value operator()(const ExprGrouping& e) const override {
        return evaluate(e.get());
    }

    virtual Value operator()(const ExprLiteral& e) const override {
        return e.get();
    }

    virtual Value operator()(const ExprLogical& e) const override {
        Value eval = evaluate(e.getLeft());
        if (e.getOp() == Tokens::OR) {
            if (isTruthy(eval)) {
                return eval;
            }
        }
        else {
            if (!isTruthy(eval)) {
                return eval;
            }
        }
        return evaluate(e.getRight());
    }

    virtual Value operator()(const ExprSet& e) const override {
        Value object = evaluate(e.getObject());
        if (static_cast<ValueType>(object.index()) != ValueType::Instance) {
            throw Error("Only instances have fields");
        }
        Value value = evaluate(e.getValue());
        get<std::shared_ptr<LoxInstance>>(object)->set(e.getName(), value);
        return value;
    }

    virtual Value operator()(const ExprSetSubscript& e) const override {
        Value value = evaluate(e.getExpr());
        if (Value l = evaluate(e.getArray());
            static_cast<ValueType>(l.index()) == ValueType::Array) {
            if (Value idx = evaluate(e.getIndex());
                static_cast<ValueType>(idx.index()) == ValueType::Number) {
                (*get<std::shared_ptr<LoxArray>>(l))[get<double>(idx)] = value;
            }
            else {
                throw Error("Array index is not a number");
            }
        }
        else {
            throw Error("Expression is not an array");
        }
        return value;
    }

    virtual Value operator()(const ExprSubscript& e) const override {
        Value value = evaluate(e.getArray());
        if (static_cast<ValueType>(value.index()) == ValueType::Array) {
            Value idx = evaluate(e.getIndex());
            if (static_cast<ValueType>(idx.index()) == ValueType::Number) {
                return (*get<std::shared_ptr<LoxArray>>(value))[get<double>(idx)];
            }
            else {
                throw Error("Array index is not a number");
            }
        }
        else {
            throw Error("Can only use subscript with an array");
        }
    }

    virtual Value operator()(const ExprSuper& e) const override {
        auto iter = locals.find(&e);
        int distance = (iter != locals.end()) ? iter->second : -1;
        auto superclass = std::dynamic_pointer_cast<LoxClass>(
            get<std::shared_ptr<LoxCallable>>(environment->getAt(distance, "super")));
        auto object = get<std::shared_ptr<LoxInstance>>(environment->getAt(distance - 1, "this"));
        Value method = superclass->findMethod(e.get());
        if (static_cast<ValueType>(method.index()) == ValueType::Nil) {
            throw Error("Undefined property '" + e.get() + "'");
        }
        return std::dynamic_pointer_cast<LoxFunction>(
            std::get<std::shared_ptr<LoxCallable>>(method))->bind(object);
    }

    virtual Value operator()(const ExprTernary& e) const override {
        auto [ loIncl, hiIncl ] = e.getIncl();
        Value lo = evaluate(e.getLo());
        Value hi = evaluate(e.getHi());
        Value var = lookupVariable(e.getName(), &e);
        bool aboveLo = loIncl ? var >= lo : var > lo;
        bool belowHi = hiIncl ? var <= hi : var < hi; 
        return aboveLo && belowHi;
    }

    virtual Value operator()(const ExprThis& e) const override {
        return lookupVariable("this", &e);
    }

    virtual Value operator()(const ExprUnary& e) const override {
        Value eval = evaluate(e.get());
        switch (e.getOp()) {
            case Tokens::NOT:
                return !isTruthy(eval);
            case Tokens::MINUS:
                if (static_cast<ValueType>(eval.index()) == ValueType::Number) {
                    return -get<double>(eval);
                }
        }
        throw Error("Bad type for unary operator");
    }

    virtual Value operator()(const ExprVariable& e) const override {
        return lookupVariable(e.get(), &e);
    }

    virtual void operator()(const StmtBlock& s) const override {
        environment = Environment::makeShared(environment);
        for (auto statement : s.get()) {
            statement->accept(*this);
        }
        environment = environment->ancestor(1);
    }

    virtual void operator()(const StmtCase& s) const override {
        auto expr = evaluate(s.getCaseOf());
        for (const auto& w : s.getWhen()) {
            if (expr == evaluate(w.first)) {
                w.second->accept(*this);
                return;
            }
        }
        if (auto e = s.getElse()) {
            e->accept(*this);
        }
    }

    virtual void operator()(const StmtClass& s) const override {
        std::shared_ptr<LoxClass> superclass;
        if (s.getSuper()) {
            auto super = evaluate(s.getSuper());
            if (static_cast<ValueType>(super.index()) != ValueType::Callable
                || !(superclass = std::dynamic_pointer_cast<LoxClass>(
                    get<std::shared_ptr<LoxCallable>>(super)))) {
                throw Error("Superclass must be a class");
            }
        }
        environment->define(s.getName(), std::monostate{});
        if (superclass) {
            environment = Environment::makeShared(environment);
            environment->define("super", superclass);
        }
        std::unordered_map<std::string,std::shared_ptr<LoxFunction>> methods;
        for (auto method : s.getMethods()) {
            methods.insert({ method->getName(),
                std::make_shared<LoxFunction>(*method, environment, method->getName() == "init")});
        }
        std::shared_ptr<LoxCallable> klass = std::make_shared<LoxClass>(s.getName(), superclass, std::move(methods));
        if (superclass) {
            environment = environment->ancestor(1);
        }
        environment->assign(s.getName(), klass);
    }

    virtual void operator()(const StmtExpression& s) const override {
        recent = s.get()->accept(*this);
    }
    
    virtual void operator()(const StmtFunction& s) const override {
        environment->define(s.getName(), std::make_shared<LoxFunction>(s, environment, false));
    }

    virtual void operator()(const StmtIf& s) const override {
        bool condition = isTruthy(s.getCond()->accept(*this));
        if (condition) {
            s.getThen()->accept(*this);
        }
        else if (s.getElse()) {
            s.getElse()->accept(*this);
        }
    }

    virtual void operator()(const StmtPrint& s) const override {
        out << toString(s.get()->accept(*this)) << '\n';
    }

    virtual void operator()(const StmtRepeat& s) const override {
        do {
            s.getBody()->accept(*this);
        } while(!isTruthy(s.getCond()->accept(*this)));
    }

    virtual void operator()(const StmtReturn& s) const override {
        throw Return(s.get()->accept(*this));
    }

    virtual void operator()(const StmtVariable& s) const override {
        environment->define(s.getName(), recent = s.getInit()->accept(*this), s.getIsConstant());
    }

    virtual void operator()(const StmtWhile& s) const override {
        while (isTruthy(s.getCond()->accept(*this))) {
            s.getBody()->accept(*this);
        }
    }

    void resolve(const Expr *e_ptr, int depth) const {
        if (auto iter = locals.find(e_ptr); iter == locals.end()) {
            locals.insert({e_ptr, depth});
        }
        else {
            iter->second = depth;
        }
    }

private:
    Value evaluate(std::shared_ptr<Expr> e) const {
        if (auto s = std::dynamic_pointer_cast<ExprArray>(e)) {
            return operator()(static_cast<ExprArray&>(*s));
        }
        else if (auto s = std::dynamic_pointer_cast<ExprAssign>(e)) {
            return operator()(static_cast<ExprAssign&>(*s));
        }
        else if (auto s = std::dynamic_pointer_cast<ExprBinary>(e)) {
            return operator()(static_cast<ExprBinary&>(*s));
        }
        else if (auto s = std::dynamic_pointer_cast<ExprCall>(e)) {
            return operator()(static_cast<ExprCall&>(*s));
        }
        else if (auto s = std::dynamic_pointer_cast<ExprGet>(e)) {
            return operator()(static_cast<ExprGet&>(*s));
        }
        else if (auto s = std::dynamic_pointer_cast<ExprGrouping>(e)) {
            return operator()(static_cast<ExprGrouping&>(*s));
        }
        else if (auto s = std::dynamic_pointer_cast<ExprLiteral>(e)) {
            return operator()(static_cast<ExprLiteral&>(*s));
        }
        else if (auto s = std::dynamic_pointer_cast<ExprLogical>(e)) {
            return operator()(static_cast<ExprLogical&>(*s));
        }
        else if (auto s = std::dynamic_pointer_cast<ExprSet>(e)) {
            return operator()(static_cast<ExprSet&>(*s));
        }
        else if (auto s = std::dynamic_pointer_cast<ExprSetSubscript>(e)) {
            return operator()(static_cast<ExprSetSubscript&>(*s));
        }
        else if (auto s = std::dynamic_pointer_cast<ExprSubscript>(e)) {
            return operator()(static_cast<ExprSubscript&>(*s));
        }
        else if (auto s = std::dynamic_pointer_cast<ExprSuper>(e)) {
            return operator()(static_cast<ExprSuper&>(*s));
        }
        else if (auto s = std::dynamic_pointer_cast<ExprTernary>(e)) {
            return operator()(static_cast<ExprTernary&>(*s));
        }
        else if (auto s = std::dynamic_pointer_cast<ExprThis>(e)) {
            return operator()(static_cast<ExprThis&>(*s));
        }
        else if (auto s = std::dynamic_pointer_cast<ExprUnary>(e)) {
            return operator()(static_cast<ExprUnary&>(*s));
        }
        else if (auto s = std::dynamic_pointer_cast<ExprVariable>(e)) {
            return operator()(static_cast<ExprVariable&>(*s));
        }
        else {
            return std::monostate{};
        }
    }

    Value lookupVariable(const std::string& name, const Expr *e) const {
        int distance = -1;
        if (auto iter = locals.find(e); iter != locals.end()) {
            distance = iter->second;
        }
        if (distance != -1) {
            return environment->getAt(distance, name);
        }
        else {
            return globals->get(name);
        }
    }

};

#endif // Interpreter_h