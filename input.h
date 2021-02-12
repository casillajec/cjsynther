#include <SDL2/SDL.h>

#ifndef INPUT_CONTAINER
#define INPUT_CONTAINER
typedef struct {
	SDL_Scancode pressed_key;
	char note;
	char mod;
	int octave;
	int quit_requested;	
} InputContainer;
#endif

void process_input(InputContainer* input_cont);
