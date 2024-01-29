#include "Call.hpp"
#include "Expr.hpp"
#include "Environment.hpp"
#include "visitor/Interpreter.hpp"
#include "Class.hpp"
#include <utility>

Value LoxFunction::call(const Interpreter& interpreter, const std::vector<std::shared_ptr<Expr>>& arguments) {
    auto environment = Environment::makeShared(closure);
    for (size_t i = 0; i != arguments.size(); ++i) {
        environment->define(params.at(i), arguments.at(i)->accept(interpreter));
    }
    std::swap(environment, interpreter.environment);
    try {
        body->accept(interpreter);
        std::swap(environment, interpreter.environment);
        return isInitializer ? closure->getAt(0, "this") : std::monostate{};
    }
    catch (const Return& r) {
        std::swap(environment, interpreter.environment);
        return isInitializer ? closure->getAt(0, "this") : r.get();
    }
}

std::shared_ptr<LoxFunction> LoxFunction::bind(std::shared_ptr<LoxInstance> instance) {
    auto environment = Environment::makeShared(closure);
    environment->define("this", instance);
    auto function = std::make_shared<LoxFunction>();
    function->name = name;
    function->body = body;
    function->params = params;
    function->closure = environment;
    function->isInitializer = isInitializer;
    return function;
}