// Generated by Bisonc++ V6.05.00 on Wed, 16 Aug 2023 12:55:33 +0100

#ifndef Loxgram_h_included
#define Loxgram_h_included

// $insert baseclass
#include "Loxgrambase.h"

#include "../lexer/Loxer.h"
#include <functional>

class Loxgram: public LoxgramBase
{
    Loxer& scanner;
    std::function<void(std::shared_ptr<Stmt>)> *action_stmt;
    public:
        Loxgram() = default;
        Loxgram(Loxer& scanner, decltype(action_stmt) action_stmt = nullptr)
            : scanner{ scanner }, action_stmt{ action_stmt } {}
        int parse();

    private:
        void error();                   // called on (syntax) errors
        int lex();                      // returns the next token from the
                                        // lexical scanner. 
        void print();                   // use, e.g., d_token, d_loc
        void exceptionHandler(std::exception const &exc);

    // support functions for parse():
        void executeAction_(int ruleNr);
        void errorRecovery_();
        void nextCycle_();
        void nextToken_();
        void print_();
};


#endif