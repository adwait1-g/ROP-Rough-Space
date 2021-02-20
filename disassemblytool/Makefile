# Makefile for Capstone Disassembly Engine

LIBNAME = capstone

disassemble: disassemble.o
	g++ -g $< -O3 -Wall -l$(LIBNAME) -o $@

%.o: %.cpp
	g++ -g -c $< -o $@
