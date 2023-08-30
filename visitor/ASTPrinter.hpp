#ifndef ASTPrinter_h
#define ASTPrinter_h

#include "../parser/Tokens.h"
#include "ExprVisitor.hpp"
#include <ostream>
#include <unordered_map>

static const std::unordered_map<int,std::string_view> ops{
    { Tokens::BANG_EQUAL,       "NE" },
    { Tokens::EQUAL_EQUAL,      "EQ" },
    { Tokens::GREATER,          "GT" },
    { Tokens::GREATER_EQUAL,    "GE" },
    { Tokens::LESS,             "LT" },
    { Tokens::LESS_EQUAL,       "LE" },
    { Tokens::MINUS,            "-"  },
    { Tokens::PLUS,             "+"  },
    { Tokens::SLASH,            "/"  },
    { Tokens::STAR,             "*"  },
    { Tokens::BANG,             "!"  },
    { Tokens::OR,               "||" },
    { Tokens::AND,              "&&" },
};

class ASTPrinter : public ExprVisitor {
public:
    virtual Value operator()(const ExprAssign& e) const override {
        std::string str;
        str.append("(assign ");
        str.append(e.getName());
        str.append(" ");
        str.append(toString(e.getExpr()->accept(*this)));
        str.append(")");
        return str;
    }

    virtual Value operator()(const ExprBinary& e) const override {
        std::string str;
        str.append("(");
        str.append(ops.find(e.getOp())->second);
        str.append(" ");
        str.append(toString(e.getLeft()->accept(*this)));
        str.append(" ");
        str.append(toString(e.getRight()->accept(*this)));
        str.append(")");
        return str;
    }

    virtual Value operator()(const ExprCall& e) const override {
        std::string str;
        str.append("(call ");
        str.append(toString(e.getCallee()->accept(*this)));
        str.append("()");
        for (auto expr : e.getArgs()) {
            str.append(" ");
            str.append(toString(expr->accept(*this)));
        }
        str.append(")");
        return str;
    }

    virtual Value operator()(const ExprGet& e) const override {
        return "(get)";
    }

    virtual Value operator()(const ExprGrouping& e) const override {
        std::string str{ "(group " };
        str.append(toString(e.get()->accept(*this)));
        str.append(")");
        return str;
    }

    virtual Value operator()(const ExprLiteral& e) const override {
        return toString(e.get());
    }

    virtual Value operator()(const ExprLogical& e) const override {
        std::string str;
        str.append("(");
        str.append(ops.find(e.getOp())->second);
        str.append(" ");
        str.append(toString(e.getLeft()->accept(*this)));
        str.append(" ");
        str.append(toString(e.getRight()->accept(*this)));
        str.append(")");
        return str;
    }

    virtual Value operator()(const ExprSet& e) const override {
        return "(set)";
    }

    virtual Value operator()(const ExprSuper& e) const override {
        std::string str;
        str.append("(super ");
        str.append(e.get());
        str.append(")");
        return str;
    }

    virtual Value operator()(const ExprThis& e) const override {
        return "(this)";
    }

    virtual Value operator()(const ExprUnary& e) const override {
        std::string str;
        str.append("(");
        str.append(ops.find(e.getOp())->second);
        str.append(" ");
        str.append(toString(e.get()->accept(*this)));
        str.append(")");
        return str;
    }

    virtual Value operator()(const ExprVariable& e) const override {
        return e.get();
    }
};

#endif // ASTPrinter_h