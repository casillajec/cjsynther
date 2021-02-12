#include <SDL2/SDL.h>

#include "input.h"

void process_input(InputContainer* input_cont) {
	SDL_Event evt;
	int quit_requested = 0, octave = input_cont->octave;
	SDL_Scancode pressed_key = input_cont->pressed_key;
	char note = input_cont->note, mod = input_cont->mod;

	while (SDL_PollEvent(&evt)) {
		if 	(evt.type == SDL_QUIT)
			quit_requested = 1;
		else if (evt.type == SDL_KEYUP && evt.key.keysym.scancode == pressed_key)
			pressed_key = -1;
		else if (evt.type == SDL_KEYDOWN)
			pressed_key = evt.key.keysym.scancode; 
	}

	switch (pressed_key) {
		case SDL_SCANCODE_ESCAPE:
			quit_requested = 1; break;
		case SDL_SCANCODE_Z:
			note = 'A'; mod = ' '; break;
		case SDL_SCANCODE_S:
			note = 'A'; mod = '#'; break;
		case SDL_SCANCODE_X:
			note = 'B'; mod = ' '; break;
		case SDL_SCANCODE_C:
			note = 'C'; mod = ' '; break;
		case SDL_SCANCODE_F:
			note = 'C'; mod = '#'; break;
		case SDL_SCANCODE_V:
			note = 'D'; mod = ' '; break;
		case SDL_SCANCODE_G:
			note = 'D'; mod = '#'; break;
		case SDL_SCANCODE_B:
			note = 'E'; mod = ' '; break;
		case SDL_SCANCODE_N:
			note = 'F'; mod = ' '; break;
		case SDL_SCANCODE_J:
			note = 'F'; mod = '#'; break;
		case SDL_SCANCODE_M:
			note = 'G'; mod = ' '; break;
		case SDL_SCANCODE_K:
			note = 'G'; mod = '#'; break;
		case SDL_SCANCODE_2:
			octave = 2; break;
		case SDL_SCANCODE_3:
			octave = 3; break;
		case SDL_SCANCODE_4:
			octave = 4; break;
		case SDL_SCANCODE_5:
			octave = 5; break;
		case SDL_SCANCODE_6:
			octave = 6; break;
		default:
			note = 'S'; mod = ' '; break;
	}

	input_cont->quit_requested = quit_requested;
	input_cont->octave = octave;
	input_cont->note = note;
	input_cont->mod = mod;
	input_cont->pressed_key = pressed_key;
}
