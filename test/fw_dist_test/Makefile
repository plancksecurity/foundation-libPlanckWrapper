
LDLIBS=-lstdc++ -lpEpAdapter
LDFLAGS=-L../../src/
CXXFLAGS=-std=c++11 -g
CXXFLAGS+=-I./src
TEST_EXTRA_OBJS=../framework/utils.o

# Src
SRC=$(wildcard src/*.cc)
OBJ=$(subst .cc,.o,$(SRC))

# Tests
TEST_SRC=$(wildcard test/*.cc)
TEST_OBJ=$(subst .cc,,$(TEST_SRC))


.PHONY: all clean rmtestdata
.DEFAULT_GOAL := all

all: $(TEST_OBJ)

$(TEST_OBJ): $(OBJ) $(TEST_EXTRA_OBJS)


clean:
	rm -f $(OBJ)
	rm -f $(TEST_OBJ)
	rm -rf src/*.dSYM
	rm -rf test/*.dSYM

