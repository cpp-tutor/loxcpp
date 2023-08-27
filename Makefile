# Makefile for loxcpp project

CPP = g++
CPPFLAGS = -Wall -std=c++20
FLEXCPP = flexc++
BISONCPP = bisonc++
OBJS = main.o Call.o Expr.o Stmt.o Value.o lexer/loxer.o parser/loxgram.o
BASE = loxcpp

all: $(BASE)

flexcpp: # lexer/loxer.cpp
	cd lexer && $(FLEXCPP) loxer

bisoncpp: # parser/loxgram.cpp
	cd parser && $(BISONCPP) loxgram

%.o: %.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ $<

$(BASE): $(OBJS)
	$(CPP) $(CPPFLAGS) -o $(BASE) $(OBJS)

clean:
	rm -f $(BASE) $(OBJS)
