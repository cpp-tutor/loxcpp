# Makefile for loxcpp project

CPP = g++
CPPFLAGS = -Wall -std=c++20
FLEXCPP = flexc++
BISONCPP = bisonc++
OBJS = main.o Call.o Expr.o Stmt.o Value.o lexer/lex.o parser/parse.o
BASE = abaci0

all: $(BASE)

flexcpp: # lexer/loxer.cpp
	cd lexer && $(FLEXCPP) lex.l

bisoncpp: # parser/loxgram.cpp
	cd parser && $(BISONCPP) parse.y

%.o: %.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ $<

$(BASE): $(OBJS)
	$(CPP) $(CPPFLAGS) -o $(BASE) $(OBJS)

clean:
	rm -f $(BASE) $(OBJS)
