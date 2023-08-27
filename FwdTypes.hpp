#ifndef FwdTypes_h
#define FwdTypes_h

#include <memory>
#include <exception>
#include <string_view>
#include <string>
#include <variant>
#include <ostream>

class Error : public std::exception {
    std::string str;
public:
    Error(std::string_view s) : str{ s } {}
    const char *what() const noexcept override { return str.c_str(); }
};

class SType {
public:
    virtual ~SType() {}
};

class ExprVisitor;
class StmtVisitor;
class LoxCallable;
class LoxClass;
class LoxInstance;

using Value = std::variant<std::monostate,bool,double,std::string,std::shared_ptr<LoxCallable>,std::shared_ptr<LoxInstance>>;

enum class ValueType { Nil, Boolean, Number, String, Callable, Instance };

std::string toString(const Value&);
bool isTruthy(const Value& l);

class Return {
    Value value;
public:
    Return(Value&& value) : value{ value } {}
    const auto get() const { return value; }
};

#endif // FwdTypes_h