CFLAGS = -Wall -std=c++17 -lstdc++fs -O3 -lpthread
CC = g++

all: hw4

hw4: hw4.o map.o file_process.o utils.o
	$(CC) $(CFLAGS) -o hw4 *.o -lpthread

hw4.o: hw4.cpp map.cpp file_process.cpp utils.cpp
	$(CC) $(CFLAGS) -c hw4.cpp

utils.o: utils.hpp utils.cpp
	$(CC) $(CFLAGS) -c utils.cpp

file_process.o: file_process.hpp file_process.cpp utils.hpp
	$(CC) $(CFLAGS) -c file_process.cpp

map.o: map.hpp map.cpp file_process.hpp
	$(CC) $(CFLAGS) -c map.cpp

clean:
	rm -f *.o hw4