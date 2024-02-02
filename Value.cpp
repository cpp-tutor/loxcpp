#include "FwdTypes.hpp"
#include "Call.hpp"
#include "Class.hpp"
#include <sstream>
#include <iomanip>
#include <stdexcept>

std::string toString(const Value& l) {
    switch (static_cast<ValueType>(l.index())) {
        case ValueType::Nil:
            return "nil";
        case ValueType::Boolean:
            return get<bool>(l) ? "true" : "false";
        case ValueType::Number: {
            std::ostringstream oss;
            oss << std::setprecision(14) << get<double>(l);
            return oss.str();
        }
        case ValueType::String:
            return get<std::string>(l);
        case ValueType::Array: {
            const auto& array = get<std::shared_ptr<LoxArray>>(l);
            if (!array->size()) {
                return "[]";
            }
            std::ostringstream oss;
            oss << "[ ";
            auto sep = "";
            for (const auto& elem : array->get()) {
                oss << sep << toString(elem);
                sep = ", ";
            }
            oss << " ]";
            return oss.str();
        }
        case ValueType::Callable:
            return get<std::shared_ptr<LoxCallable>>(l)->toString();
        case ValueType::Instance:
            return get<std::shared_ptr<LoxInstance>>(l)->toString();
    }
    return "Error: bad ValueType";
}

bool isTruthy(const Value& l) {
    if (static_cast<ValueType>(l.index()) == ValueType::Array) {
        return get<std::shared_ptr<LoxArray>>(l)->size() != 0;
    }
    else if (static_cast<ValueType>(l.index()) == ValueType::Number) {
        return get<double>(l) != 0.0;
    }
    else if (static_cast<ValueType>(l.index()) == ValueType::Nil) {
        return false;
    }
    else if (static_cast<ValueType>(l.index()) == ValueType::Boolean) {
        return get<bool>(l);
    }
    else {
        return true;
    }
}

const Value& LoxArray::operator[](std::vector<Value>::size_type idx) const {
    try {
        return array.at(idx);
    }
    catch (std::out_of_range&) {
        throw Error("Array access out of bounds with index " + std::to_string(idx) + " and size " + std::to_string(size()));
    }
}

Value& LoxArray::operator[](std::vector<Value>::size_type idx) {
    if (size() <= idx) {
        array.resize(idx + 1);
    }
    return array.at(idx);
}
