CC=g++
CFLAGS=-g -O2
EXECS=main

all: ${EXECS}

main: main.cpp PlaceAlgo.o
	${CC} ${FLAGS} $^ -o main


PlaceAlgo.o: PlaceAlgo.cpp
	${CC} -c ${FLAGS} $^ -o PlaceAlgo.o


clean:
	rm -f main PlaceAlgo.o main.o
