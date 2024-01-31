#ifndef Class_h
#define Class_h

#include "Call.hpp"
#include <string>
#include <unordered_map>

class LoxInstance : public std::enable_shared_from_this<LoxInstance> {
    std::shared_ptr<LoxClass> klass;
    std::unordered_map<std::string,Value> fields;
public:
    LoxInstance(std::shared_ptr<LoxClass> klass) : klass{ klass } {}
    Value get(const std::string&);
    void set(const std::string&, const Value&);
    const std::string toString() const;
};

class LoxClass : public LoxCallable, public std::enable_shared_from_this<LoxClass> {
    std::shared_ptr<LoxClass> superclass;
    std::string name;
    std::unordered_map<std::string,std::shared_ptr<LoxFunction>> methods;
    friend const std::string LoxInstance::toString() const;
public:
    LoxClass(const std::string& name, std::shared_ptr<LoxClass> superclass,
        std::unordered_map<std::string,std::shared_ptr<LoxFunction>>&& methods)
        : superclass{ superclass }, name{ name }, methods{ methods } {}
    virtual Value call(const Interpreter&, const std::vector<std::shared_ptr<Expr>>&) override;
    virtual const size_t arity() const override;
    virtual const std::string toString() const override { return "<class " + name + '>'; }
    Value findMethod(const std::string&) const;
};

inline Value LoxInstance::get(const std::string& name) {
    if (auto iter = fields.find(name); iter != fields.end()) {
        return iter->second;
    }
    Value method = klass->findMethod(name);
    if (static_cast<ValueType>(method.index()) == ValueType::Callable) {
        return std::dynamic_pointer_cast<LoxFunction>(
            std::get<std::shared_ptr<LoxCallable>>(method))
            ->bind(shared_from_this());
    }
    throw Error("Undefined property \'" + name + "\'");
}

inline void LoxInstance::set(const std::string& name, const Value& value) {
    if (auto iter = fields.find(name); iter != fields.end()) {
        iter->second = value;
    }
    else {
        fields.insert({ name, value });
    }
}

inline const std::string LoxInstance::toString() const {
    return '<' + klass->name + " instance>";
}

inline Value LoxClass::findMethod(const std::string& name) const {
    if (auto iter = methods.find(name); iter != methods.end()) {
        return iter->second;
    }
    else if (superclass) {
        return superclass->findMethod(name);
    }
    else {
        return std::monostate{};
    }
}

inline Value LoxClass::call(const Interpreter& interpreter, const std::vector<std::shared_ptr<Expr>>& arguments) {
    auto instance = std::make_shared<LoxInstance>(shared_from_this());
    auto initializer = findMethod("init");
    if (static_cast<ValueType>(initializer.index()) == ValueType::Callable) {
        std::dynamic_pointer_cast<LoxFunction>(get<std::shared_ptr<LoxCallable>>(initializer))->bind(instance)->call(interpreter, arguments);
    }
    return instance;
}

inline const size_t LoxClass::arity() const {
    auto initializer = findMethod("init");
    if (static_cast<ValueType>(initializer.index()) == ValueType::Callable) {
        return std::dynamic_pointer_cast<LoxFunction>(get<std::shared_ptr<LoxCallable>>(initializer))->arity();
    }
    else {
        return 0;
    }
}

#endif // Class_h