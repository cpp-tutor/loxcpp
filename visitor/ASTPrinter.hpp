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
        return "?";
    }

    virtual Value operator()(const ExprBinary& e) const override {
        std::string str;
        str += '(';
        str += ops.find(e.getOp())->second;
        str += ' ';
        str += toString(e.getLeft()->accept(*this));
        str += ' ';
        str += toString(e.getRight()->accept(*this));
        str += ')';
        return str;
    }

    virtual Value operator()(const ExprCall& e) const override {
        return "?";
    }

    virtual Value operator()(const ExprGet& e) const override {
        return "?";
    }

    virtual Value operator()(const ExprGrouping& e) const override {
        std::string str{ "(group " };
        str += toString(e.get()->accept(*this));
        str += ')';
        return str;
    }

    virtual Value operator()(const ExprLiteral& e) const override {
        return toString(e.get());
    }

    virtual Value operator()(const ExprLogical& e) const override {
        std::string str;
        str += '(';
        str += ops.find(e.getOp())->second;
        str += ' ';
        str += toString(e.getLeft()->accept(*this));
        str += ' ';
        str += toString(e.getRight()->accept(*this));
        str += ')';
        return str;
    }

    virtual Value operator()(const ExprSet& e) const override {
        return "?";
    }

    virtual Value operator()(const ExprSuper& e) const override {
        return "super";
    }

    virtual Value operator()(const ExprThis& e) const override {
        return "this";
    }

    virtual Value operator()(const ExprUnary& e) const override {
        std::string str;
        str += '(';
        str += ops.find(e.getOp())->second;
        str += ' ';
        str += toString(e.get()->accept(*this));
        str += ')';
        return str;
    }

    virtual Value operator()(const ExprVariable& e) const override {
        return e.get();
    }
};

#endif // ASTPrinter_h