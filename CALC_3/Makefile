#!/usr/bin/make -f

CC=g++
main: main.o ArithmeticsParser.o BufferedStream.o Logger.o LocatableStream.o SyntaxTree.o Tokenizer.o

main.o: main.cpp

ArithmeticsParser.o: ArithmeticsParser.cpp ArithmeticsParser.h

BufferedStream.o: BufferedStream.cpp BufferedStream.h

Logger.o: Logger.cpp Logger.h

LocatableStream.o: LocatableStream.cpp LocatableStream.h

SyntaxTree.o: SyntaxTree.cpp SyntaxTree.h

Tokenizer.o: Tokenizer.cpp Tokenizer.h

clean:
	rm -rf *.o main core
