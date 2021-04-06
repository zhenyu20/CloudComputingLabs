CXXFLAGS+=-O2 -ggdb -DDEBUG
CXXFLAGS+=-Wall -Wextra

all: sudoku

sudoku: main.cc neighbor.cc sudoku_basic.cc 
	g++ -O2 -pthread -o $@ $^
