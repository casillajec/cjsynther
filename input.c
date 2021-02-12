#include <SDL2/SDL.h>

#include "input.h"

void process_input(SDL_Scancode* pressed_key, char* note, char* mod, int* octave, int* quit_requested) {
	SDL_Event evt;
	while (SDL_PollEvent(&evt)) {
		if 	(evt.type == SDL_QUIT)
			*quit_requested = 1;
		else if (evt.type == SDL_KEYUP && evt.key.keysym.scancode == *pressed_key)
			*pressed_key = -1;
		else if (evt.type == SDL_KEYDOWN)
			*pressed_key = evt.key.keysym.scancode; 
	}

	switch (*pressed_key) {
		case SDL_SCANCODE_ESCAPE:
			*quit_requested = 1; break;
		case SDL_SCANCODE_Z:
			*note = 'A'; *mod = ' '; break;
		case SDL_SCANCODE_S:
			*note = 'A'; *mod = '#'; break;
		case SDL_SCANCODE_X:
			*note = 'B'; *mod = ' '; break;
		case SDL_SCANCODE_C:
			*note = 'C'; *mod = ' '; break;
		case SDL_SCANCODE_F:
			*note = 'C'; *mod = '#'; break;
		case SDL_SCANCODE_V:
			*note = 'D'; *mod = ' '; break;
		case SDL_SCANCODE_G:
			*note = 'D'; *mod = '#'; break;
		case SDL_SCANCODE_B:
			*note = 'E'; *mod = ' '; break;
		case SDL_SCANCODE_N:
			*note = 'F'; *mod = ' '; break;
		case SDL_SCANCODE_J:
			*note = 'F'; *mod = '#'; break;
		case SDL_SCANCODE_M:
			*note = 'G'; *mod = ' '; break;
		case SDL_SCANCODE_K:
			*note = 'G'; *mod = '#'; break;
		case SDL_SCANCODE_2:
			*octave = 2; break;
		case SDL_SCANCODE_3:
			*octave = 3; break;
		case SDL_SCANCODE_4:
			*octave = 4; break;
		case SDL_SCANCODE_5:
			*octave = 5; break;
		case SDL_SCANCODE_6:
			*octave = 6; break;
		default:
			*note = 'S'; *mod = ' '; break;
	}
}
