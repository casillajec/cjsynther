
all: main.c beep.o beep_it.o input.o oscilators.o key_pitch_stack.o state.o
	gcc -o cjsynther main.c `sdl2-config --cflags --libs` -lm -lSDL2_ttf beep.o beep_it.o input.o oscilators.o key_pitch_stack.o state.o

queue.o: queue.h queue.c beep.o
	gcc -c queue.c

input.o: input.h input.c beep.o
	gcc -c input.c

beep_it.o: beep_it.h beep_it.c beep.o
	gcc -c beep_it.c

beep.o: beep.h beep.c
	gcc -c beep.c

oscilators.o: oscilators.h oscilators.c
	gcc -c oscilators.c

key_pitch_stack.o: key_pitch_stack.h key_pitch_stack.c
	gcc -c key_pitch_stack.c

state.o: state.h state.c
	gcc -c state.c

minmain: minmain.c
	gcc -o minmain minmain.c `sdl2-config --cflags --libs` -lm

clean:
	rm beep.o beep_it.o input.o oscilators.o key_pitch_stack.o
