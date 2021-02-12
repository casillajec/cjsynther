#include <SDL2/SDL.h>

#ifndef _
#define _
typedef struct {
	SDL_Scancode pressed_key;
	char note;
	char mod;
	int octave;
	int quit_requested;	
} InputContainer;
#endif

void process_input(InputContainer* input_cont);
