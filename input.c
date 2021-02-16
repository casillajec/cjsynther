#include <stdio.h>

#include <SDL2/SDL.h>

#include "input.h"
#include "beep.h"
#include "oscilators.h"
#include "state.h"

void process_input(SynthState* st_ptr) {
	SDL_Event evt;
	SDL_Scancode key;

	while (SDL_PollEvent(&evt)) {
		if 	(evt.type == SDL_QUIT) {
			st_ptr->quit_requested = 1;
		} else if (evt.type == SDL_KEYUP) {
			key = evt.key.keysym.scancode;
			switch (key) {
				case SDL_SCANCODE_LSHIFT:
				case SDL_SCANCODE_RSHIFT:
					st_ptr->pitch_mod = 0; break;
			}
			kps_remove(st_ptr->kps_ptr, key);

		} else if (evt.type == SDL_KEYDOWN) {
			key = evt.key.keysym.scancode;
			switch (key) {
				case SDL_SCANCODE_ESCAPE:
					st_ptr->quit_requested = 1; break;
				case SDL_SCANCODE_LSHIFT:
				case SDL_SCANCODE_RSHIFT:
					st_ptr->pitch_mod = 1; break;
				case SDL_SCANCODE_RIGHT:
					st_ptr->mod_freq += 0.1f; break;
				case SDL_SCANCODE_LEFT:
					st_ptr->mod_freq -= 0.1f;
					st_ptr->mod_freq = st_ptr->mod_freq < 0 ? 0.0f : st_ptr->mod_freq;
					break;
				case SDL_SCANCODE_UP:
					st_ptr->mod_amplitude += 0.01f; break;
				case SDL_SCANCODE_DOWN:
					st_ptr->mod_amplitude -= 0.01f;
					st_ptr->mod_amplitude = st_ptr->mod_amplitude < 0 ? 0.0f : st_ptr->mod_amplitude;
					break;
				case SDL_SCANCODE_F2:
					st_ptr->octave = 2; break;
				case SDL_SCANCODE_F3:
					st_ptr->octave = 3; break;
				case SDL_SCANCODE_F4:
					st_ptr->octave = 4; break;
				case SDL_SCANCODE_F5:
					st_ptr->octave = 5; break;
				case SDL_SCANCODE_F6:
					st_ptr->octave = 6; break;
				case SDL_SCANCODE_F9:
					st_ptr->oscilator = OSC_SIN; break;
				case SDL_SCANCODE_F10:
					st_ptr->oscilator = OSC_SQUARE; break;
				case SDL_SCANCODE_F11:
					st_ptr->oscilator = OSC_TRIANGLE; break;
				case SDL_SCANCODE_F12:
					st_ptr->oscilator = OSC_EXP; break;
			}
			kps_add(st_ptr->kps_ptr, key);
		}
	}
	
	kps_update(st_ptr->kps_ptr, st_ptr->octave);
}
