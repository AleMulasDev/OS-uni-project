game: main.c spacecraft.o enemies.o
	gcc -o game main.c spacecraft.o enemies.o enemy.o bullet.o utils_struct.o -lncurses -std=c90 -lm

spacecraft.o: spacecraft.c spacecraft.h bullet.o utils_struct.o
	gcc -c -o spacecraft.o spacecraft.c

enemies.o: enemies.c enemies.h enemy.o utils_struct.o
	gcc -c -o enemies.o enemies.c

enemy.o: enemy.c enemy.h utils_struct.o bullet.o
	gcc -c -o enemy.o enemy.c

bullet.o: bullet.c bullet.h utils_struct.o
	gcc -c -o bullet.o bullet.c

utils_struct.o: utils_struct.c utils_struct.h
	gcc -c -o utils_struct.o utils_struct.c

clean:
	rm *.o
	rm game