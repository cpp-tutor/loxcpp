#include "FwdTypes.hpp"
#include "Call.hpp"
#include "Class.hpp"
#include <sstream>
#include <iomanip>

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
        case ValueType::Callable:
            return get<std::shared_ptr<LoxCallable>>(l)->toString();
        case ValueType::Instance:
            return get<std::shared_ptr<LoxInstance>>(l)->toString();
    }
    return "Error: bad ValueType";
}

bool isTruthy(const Value& l) {
    if (static_cast<ValueType>(l.index()) == ValueType::Nil) {
        return false;
    }
    else if (static_cast<ValueType>(l.index()) == ValueType::Boolean) {
        return get<bool>(l);
    }
    else {
        return true;
    }
}
