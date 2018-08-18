.PYTHON: clean

CC := cc

all: search_in_stream kmp_test

search_in_stream: kmpalg.h search_in_stream.c
	$(CC) -o search_in_stream  search_in_stream.c 

kmp_test: kmpalg.h kmp_test.c
	$(CC) -o kmp_test kmp_test.c

clean:
	rm -rf kmp_test search_in_stream
