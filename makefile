main: spacecraft.o main.c
	gcc -o main main.c spacecraft.o -lncurses

spacecraft.o: spacecraft.c utils_struct.o
	gcc -c -o spacecraft.o spacecraft.c

utils_struct.o: utils_struct.h
	gcc -c -o utils_struct.o utils_struct.h

clean:
	rm *.o