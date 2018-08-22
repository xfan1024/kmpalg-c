.PYTHON: clean test

CC := cc

all: search_in_stream kmp_test
test: test.sh kmp_test
	./test.sh

search_in_stream: kmpalg.h search_in_stream.c
	$(CC) -Wall -o search_in_stream  search_in_stream.c 

kmp_test: kmpalg.h kmp_test.c
	$(CC) -Wall -o kmp_test kmp_test.c

clean:
	rm -rf kmp_test search_in_stream
