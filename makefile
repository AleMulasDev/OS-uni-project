game: main.c spacecraft.o enemies.o hitboxes.o utils_struct.o utils_thread.o
	gcc -o game main.c hitboxes.o spacecraft.o enemies.o enemy.o bullet.o utils_struct.o utils_thread.o -lncurses -std=c90 -lm -lpthread

hitboxes.o: hitboxes.c hitboxes.h spacecraft.o enemy.o utils_struct.o utils_thread.o
	gcc -c -o hitboxes.o hitboxes.c

spacecraft.o: spacecraft.c spacecraft.h bullet.o utils_struct.o utils_thread.o
	gcc -c -o spacecraft.o spacecraft.c

enemies.o: enemies.c enemies.h enemy.o utils_struct.o bullet.o utils_thread.o
	gcc -c -o enemies.o enemies.c

enemy.o: enemy.c enemy.h utils_struct.o utils_thread.o
	gcc -c -o enemy.o enemy.c

bullet.o: bullet.c bullet.h utils_struct.o utils_thread.o
	gcc -c -o bullet.o bullet.c

utils_thread.o: utils_thread.c utils_thread.h utils_struct.o
	gcc -c -o utils_thread.o utils_thread.c

utils_struct.o: utils_struct.c utils_struct.h
	gcc -c -o utils_struct.o utils_struct.c

clean:
	rm *.o
	rm game