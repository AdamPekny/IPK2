CC = g++
CFLAGS = -g -Wall -Wextra -pedantic -std=c++20
BINNAME = ipkcpd
SRC = $(wildcard *.cpp)

$(BINNAME): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm *.o
