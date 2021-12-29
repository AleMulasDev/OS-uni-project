game: main.c spacecraft.o
	gcc -o game main.c spacecraft.o bullet.o utils_struct.o -lncurses

spacecraft.o: spacecraft.c spacecraft.h utils_struct.o bullet.o
	gcc -c -o spacecraft.o spacecraft.c

bullet.o: bullet.c bullet.h utils_struct.o
	gcc -c -o bullet.o bullet.c

utils_struct.o: utils_struct.c utils_struct.h
	gcc -c -o utils_struct.o utils_struct.c

clean:
	rm *.o