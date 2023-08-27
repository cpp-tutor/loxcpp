#ifndef Environment_h
#define Environment_h

#include "FwdTypes.hpp"
#include <unordered_map>
#include <string>

class Environment : public std::enable_shared_from_this<Environment> {
    std::shared_ptr<Environment> enclosing;
    std::unordered_map<std::string,Value> values;
    // note: constuctor is private as always accessed through std::shared_ptr<Environment>
    Environment() = default;
public:
    static std::shared_ptr<Environment> makeShared(
        std::shared_ptr<Environment> parent = nullptr, std::shared_ptr<Environment> clone = nullptr) {
        std::shared_ptr<Environment> e{ new Environment() };
        e->enclosing = parent;
        if (clone) {
            e->values = clone->values;
        }
        return e;
    }

    void define(const std::string& name, Value value) {
        if (auto iter = values.find(name); iter != values.end()) {
            iter->second = value;
        }
        else {
            values.insert({name, value});
        }
    }

    Value get(const std::string& name) {
        if (auto iter = values.find(name); iter != values.end()) {
            return iter->second;
        }
        if (enclosing) {
            return enclosing->get(name);
        }
        throw Error("Undefined variable \'" + name + "\'.");
    }

    void assign(const std::string& name, const Value& value) {
        if (auto iter = values.find(name); iter != values.end()) {
            iter->second = value;
            return;
        }
        if (enclosing) {
            enclosing->assign(name, value);
            return;
        }
        throw Error("Undefined variable \'" + name + "\'.");
    }

    std::shared_ptr<Environment> ancestor(unsigned distance) {
        if (distance == 0) {
            return shared_from_this();
        }
        else if (enclosing) {
            return enclosing->ancestor(distance - 1);
        }
        else {
            return nullptr;
        }
    }

    Value getAt(unsigned distance, const std::string& name) {
        return ancestor(distance)->get(name);
    }

    void assignAt(unsigned distance, const std::string& name, const Value& value) {
        ancestor(distance)->assign(name, value);
    }

};

#endif // Environment_h