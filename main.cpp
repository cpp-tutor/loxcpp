#include "FwdTypes.hpp"
#include "lexer/Loxer.h"
#include "parser/Loxgram.h"
#include "visitor/Interpreter.hpp"
#include "visitor/Resolver.hpp"
#include "visitor/ASTPrinter.hpp"
#include "Environment.hpp"
#include <vector>
#include <functional>
#include <fstream>

enum class RuntimeOptions { None = 0, ShowAST = 1, Batch = 2, Interactive = 4, JIT = 8 };

RuntimeOptions operator | (RuntimeOptions lhs, RuntimeOptions rhs) {
    return static_cast<RuntimeOptions>(static_cast<std::underlying_type_t<RuntimeOptions>>(lhs)
        | static_cast<std::underlying_type_t<RuntimeOptions>>(rhs));
}

RuntimeOptions operator & (RuntimeOptions lhs, RuntimeOptions rhs) {
    return static_cast<RuntimeOptions>(static_cast<std::underlying_type_t<RuntimeOptions>>(lhs)
        & static_cast<std::underlying_type_t<RuntimeOptions>>(rhs));
}

int main(const int argc, const char **argv) {
    std::ifstream input{};
    RuntimeOptions opts = RuntimeOptions::None;
    int arg = 0;
    while (++arg < argc) {
        if (argv[arg][0] == '-') {
            switch(argv[arg][1]) {
                case 'a':
                    opts = opts | RuntimeOptions::ShowAST;
                    break;
                case 'b':
                    opts = opts | RuntimeOptions::Batch;
                    break;
                case 'i':
                    opts = opts | RuntimeOptions::Interactive;
                    break;
                case 'j':
                    opts = opts | RuntimeOptions::JIT;
                    std::cerr << "Error: JIT engine not yet implemented\n";
                    return 1;
                    break;
                default:
                    break;
            }
        }
        else {
            break;
        }
    }
    if ((opts & (RuntimeOptions::Batch | RuntimeOptions::Interactive))
        == (RuntimeOptions::Batch | RuntimeOptions::Interactive)) {
            std::cerr << "Error: Only specify one of -b and -i\n";
            return 1;
    }
    if ((opts & (RuntimeOptions::Batch | RuntimeOptions::Interactive))
        == RuntimeOptions::None) {
            opts = opts | RuntimeOptions::Batch;
    }
    if (arg == (argc - 1)) {
        input.open(argv[arg]);
    }
    std::vector<std::shared_ptr<Stmt>> program;
    auto environment = Environment::makeShared();
    Interpreter interpreter{ environment };
    Resolver resolver{ interpreter };
    ASTPrinter print{};
    std::function<void(std::shared_ptr<Stmt>)> build;
    if ((opts & RuntimeOptions::Batch) != RuntimeOptions::None) {
        build = [&](std::shared_ptr<Stmt> statement){
                program.push_back(statement);
        };
    }
    else {
        build = [&](std::shared_ptr<Stmt> statement){
            statement->accept(resolver);
            statement->accept(interpreter);
            if ((std::dynamic_pointer_cast<StmtExpression>(statement)
                || std::dynamic_pointer_cast<StmtVariable>(statement))) {
                if ((opts & RuntimeOptions::ShowAST) == RuntimeOptions::None) {
                    if (const Value& result = interpreter.lastValue();
                        static_cast<ValueType>(result.index()) != ValueType::Nil) {
                        std::cout << "=> " << toString(result) << '\n';
                    }
                }
                else {
                    std::shared_ptr<Expr> expression;
                    if (std::dynamic_pointer_cast<StmtExpression>(statement)) {
                        expression = std::dynamic_pointer_cast<StmtExpression>(statement)->get();
                    }
                    else {
                        expression = std::dynamic_pointer_cast<StmtVariable>(statement)->getInit();
                    }
                    std::cout << "=> " << toString(interpreter.lastValue())
                        << ' ' << toString(expression->accept(print)) << '\n';
                }
            }
        };
    }
    Loxer scanner{ input.is_open() ? input : std::cin };
    Loxgram parser{ scanner, &build };
    if (!parser.parse() && ((opts | RuntimeOptions::Batch) != RuntimeOptions::None)) {
        for (auto statement : program) {
            statement->accept(resolver);
            statement->accept(interpreter);
        }
    }
}
