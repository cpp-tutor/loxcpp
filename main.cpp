#include "FwdTypes.hpp"
#include "lexer/Lex.h"
#include "parser/Parse.h"
#include "visitor/Interpreter.hpp"
#include "visitor/Resolver.hpp"
#include "visitor/ASTPrinter.hpp"
#include "Environment.hpp"
#include <vector>
#include <functional>
#include <fstream>
#include <sstream>

#define VER_STR "0.01 (2024-Jan-01)"

#ifdef _MSC_VER
#define OPTION_SWITCH '/'
#define USE_STR "[/?|/h|/v] [/i [/a]|/b] [filename]"
#else
#define OPTION_SWITCH '-'
#define USE_STR "[-?|-h|-v] [-i [-a]|-b] [filename]"
#endif

enum class ReturnCode { Unset = -1, Success = 0, Informational, OptionsError, BadFile, ParseError, RuntimeError };

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
    auto returnCode = ReturnCode::Unset;
    std::ifstream input{};
    RuntimeOptions opts = RuntimeOptions::None;
    int arg = 0;
    while (++arg < argc) {
        if (argv[arg][0] == OPTION_SWITCH) {
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
                    break;
                case 'h':
                case '?':
                    std::cerr << "Usage: " << argv[0] << ' ' << USE_STR << '\n';
                    returnCode = ReturnCode::Informational;
                    break;
                case 'v':
                    std::cerr << VER_STR << '\n';
                    returnCode = ReturnCode::Informational;
                    break;
                default:
                    std::cerr << "Unrecognized option \'" << OPTION_SWITCH << argv[arg][1] << "\'\n";
                    returnCode = ReturnCode::OptionsError;
                    break;
            }
        }
        else {
            break;
        }
    }
    if (returnCode != ReturnCode::Informational) {
        if ((opts & (RuntimeOptions::Batch | RuntimeOptions::Interactive))
            == (RuntimeOptions::Batch | RuntimeOptions::Interactive)) {
                std::cerr << "Error: Only specify one of -b and -i\n";
                returnCode = ReturnCode::OptionsError;
        }
        if (((opts & RuntimeOptions::ShowAST) != RuntimeOptions::None)
            && ((opts & RuntimeOptions::Interactive) == RuntimeOptions::None)) {
                std::cerr << "Error: Use of -a needs use of -i\n";
                returnCode = ReturnCode::OptionsError;
            }
        if ((opts & RuntimeOptions::JIT) != RuntimeOptions::None) {
            std::cerr << "Error: JIT engine not yet implemented\n";
            returnCode = ReturnCode::OptionsError;
        }
    }
    if ((opts & (RuntimeOptions::Batch | RuntimeOptions::Interactive))
        == RuntimeOptions::None) {
            opts = opts | RuntimeOptions::Batch;
    }
    if (arg != argc) {
        if ((opts & RuntimeOptions::Interactive) != RuntimeOptions::Interactive) {
            if (arg == (argc - 1)) {
                input.open(argv[arg]);
                if (!input.is_open()) {
                    std::cerr << "Error: Could not open file \'" << argv[arg] << "\'\n";
                    returnCode = ReturnCode::BadFile;
                }
            }
            else {
                std::cerr << "Error: Only one input file can be specified\n";
                returnCode = ReturnCode::OptionsError;
            }
        }
        else {
            std::cerr << "Error: Cannot specify input file with " << OPTION_SWITCH << "i\n";
            returnCode = ReturnCode::OptionsError;
        }
    }
    if (returnCode != ReturnCode::Unset) {
        return static_cast<int>(returnCode);
    }
    auto environment = Environment::makeShared();
    Interpreter interpreter{ environment };
    Resolver resolver{ interpreter };
    std::function<void(std::shared_ptr<Stmt>)> execute;
    std::vector<std::shared_ptr<Stmt>> program;
    if ((opts & RuntimeOptions::Batch) != RuntimeOptions::None) {
        execute = [&](std::shared_ptr<Stmt> statement){
                program.push_back(statement);
        };
    }
    else {
        execute = [&](std::shared_ptr<Stmt> statement){
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
                        << ' ' << toString(expression->accept(ASTPrinter{})) << '\n';
                }
            }
        };
    }
    if ((opts & RuntimeOptions::Batch) == RuntimeOptions::None) {
        bool continuation = false;
        std::string inputChunk{};
        while (!std::cin.eof()) {
            std::string prompt = continuation ? "? " : "> ";
            std::string inputLine{};
            std::cout << prompt;
            std::getline(std::cin, inputLine);
            if (continuation && inputLine.empty()) {
                continuation = false;
                returnCode = ReturnCode::Success;
            }
            else {
                inputChunk = (continuation) ? inputChunk + '\n' + inputLine : inputLine;
                std::istringstream in{ inputChunk };
                Lex scanner{ in };
                Parse parser{ scanner, nullptr, false }; 
                returnCode = parser.parse() ? ReturnCode::ParseError : ReturnCode::Success;
            }
            if (returnCode == ReturnCode::Success) {
                std::istringstream in{ inputChunk };
                Lex scanner{ in };
                Parse parser{ scanner, &execute, true };
                try {
                    parser.parse();
                }
                catch (std::exception &e) {
                    std::cerr << e.what() << " near line " << scanner.lineNr() << '\n';
                    returnCode = ReturnCode::RuntimeError;
                }
                continuation = false;
            }
            else {
                continuation = true;
            }
        }
    }
    else {
        Lex scanner{ input.is_open() ? input : std::cin >> std::noskipws };
        Parse parser{ scanner, &execute }; 
        try {
            returnCode = parser.parse() ? ReturnCode::ParseError : ReturnCode::Success;
            if (returnCode != ReturnCode::ParseError) {
                for (auto statement : program) {
                    statement->accept(resolver);
                    statement->accept(interpreter);
                }
            }
        }
        catch (std::exception &e) {
            std::cerr << e.what() << " near line " << scanner.lineNr() << '\n';
            returnCode = ReturnCode::RuntimeError;
        }
    }
    return static_cast<int>(returnCode);
}
