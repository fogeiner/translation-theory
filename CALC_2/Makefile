#!/usr/bin/make -f

CC=g++
CPPFLAGS+= -g
main: main.o SyntaxTree.o BufferedStream.o Logger.o Tokenizer.o ArithmeticsParser.o LocatableStream.o

main.o: main.cpp ArithmeticsParser.h SyntaxTree.h

BufferedStream.o: BufferedStream.cpp BufferedStream.h

ArithmeticsParser.o: ArithmeticsParser.cpp ArithmeticsParser.h

Logger.o: Logger.cpp Logger.h

Tokenizer.o: Tokenizer.cpp Tokenizer.h

LocatableStream.o: LocatableStream.cpp LocatableStream.h

SyntaxTree.o: SyntaxTree.cpp SyntaxTree.h

SyntaxTree.h: _Nodes.h

_Nodes.h: _Node.h
	./genNodes.py

clean:
	rm -rf *.o main core _Nodes.h
