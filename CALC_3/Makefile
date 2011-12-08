#!/usr/bin/make -f

CC=g++
main: main.o BufferedStream.o Logger.o LocatableStream.o Tokenizer.o

main.o: main.cpp

BufferedStream.o: BufferedStream.cpp BufferedStream.h

Logger.o: Logger.cpp Logger.h

LocatableStream.o: LocatableStream.cpp LocatableStream.h


Tokenizer.o: Tokenizer.cpp Tokenizer.h

clean:
	rm -rf *.o main core
