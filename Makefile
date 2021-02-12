
all: main.c beep.o beep_it.o input.o
	gcc -o main main.c `sdl2-config --cflags --libs` -lm beep.o beep_it.o input.o

queue.o: queue.h queue.c beep.o
	gcc -c queue.c

beep_it.o: beep_it.h beep_it.c beep.o
	gcc -c beep_it.c

beep.o: beep.h beep.c
	gcc -c beep.c

input.o: input.h input.c
	gcc -c input.c

minmain: minmain.c
	gcc -o minmain minmain.c `sdl2-config --cflags --libs` -lm

clean:
	rm queue.o beep.o beep_it.o
