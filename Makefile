CC = gcc
CXX = g++

CXXFLAGS = -Wall -std=c++11 -pedantic -ggdb -Wimplicit-fallthrough=0 -W

EXECUTABLE_NAME = pcc
BIN = ./bin
SRC = ./src

SRCS := $(shell find $(SRC) -name *.cpp -or -name *.c -or -name *.s)

main:
	@mkdir -p $(BIN)
	$(CXX) -o $(BIN)/$(EXECUTABLE_NAME) $(CXXFLAGS) $(SRCS)
