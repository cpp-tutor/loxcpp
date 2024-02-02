#ifndef FwdTypes_h
#define FwdTypes_h

#include <memory>
#include <exception>
#include <string_view>
#include <string>
#include <vector>
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
class LoxArray;

using Value = std::variant<std::monostate,bool,double,std::string,std::shared_ptr<LoxArray>,std::shared_ptr<LoxCallable>,std::shared_ptr<LoxInstance>>;

enum class ValueType { Nil, Boolean, Number, String, Array, Callable, Instance };

std::string toString(const Value&);
bool isTruthy(const Value& l);

class Return {
    Value value;
public:
    Return(Value&& value) : value{ value } {}
    const auto get() const { return value; }
};

class LoxArray {
    std::vector<Value> array;
public:
    LoxArray() = default;
    LoxArray(std::vector<Value>&& array) : array{ std::move(array) } {}
    auto size() const { return array.size(); }
    void append(const Value& l) { array.push_back(l); }
    const auto& get() const { return array; }
    const Value& operator[](std::vector<Value>::size_type idx) const;
    Value& operator[](std::vector<Value>::size_type idx);
};

#endif // FwdTypes_h