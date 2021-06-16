include ../../Makefile.conf

TARGET=src/libPityTest.a
LDFLAGS:=-L../../src/ $(LDFLAGS)
LDLIBS=-lstdc++ -lpEpAdapter
CXXFLAGS:=-std=c++11 -g -I./src $(CXXFLAGS)
#TEST_EXTRA_OBJS=../framework/utils.o

# Src
SRC=$(wildcard src/*.cc)
OBJ=$(subst .cc,.o,$(SRC))

# Tests
TEST_SRC=$(wildcard test/*.cc)
TEST_OBJ=$(subst .cc,,$(TEST_SRC))


.PHONY: all clean rmtestdata
.DEFAULT_GOAL := all


all: $(TARGET) test

$(TARGET): $(OBJ)
	$(AR) -rc $@ $^


test : $(TEST_OBJ)

$(TEST_OBJ): $(OBJ)


clean:
	rm -f $(TARGET)
	rm -f $(OBJ)
	rm -f $(TEST_OBJ)
	rm -rf src/*.dSYM
	rm -rf test/*.dSYM

