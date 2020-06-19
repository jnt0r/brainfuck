prog: main.o
	gcc -o interpreter main.o

clean:
	rm -f main.o interpreter

main.o: main.c
	gcc -c main.c