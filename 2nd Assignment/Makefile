all: gates child

.PHONY: all clean

ask2.o: ask2.c
	gcc -c ask2.c

gates: ask2.o
	gcc ask2.o -o gates

child.o: child.c
	gcc -c child.c

child: child.o
	gcc child.o -o child

clean:
	rm *.o gates child