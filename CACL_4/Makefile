#!/usr/bin/make -f

CC=g++
CPPFLAGS+= -g
main: main.o BufferedStream.o Logger.o LocatableStream.o Tokenizer.o Parser.o

main.o: main.cpp

BufferedStream.o: BufferedStream.cpp BufferedStream.h

Logger.o: Logger.cpp Logger.h

LocatableStream.o: LocatableStream.cpp LocatableStream.h

Parser.o: Parser.cpp Parser.h

Tokenizer.o: Tokenizer.cpp Tokenizer.h

clean:
	rm -rf *.o main core
