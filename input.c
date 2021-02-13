#include <stdio.h>

#include <SDL2/SDL.h>

#include "input.h"

KeyStack* ks_new() {
	KeyStack* key_stack_ptr = malloc(sizeof(KeyStack));
	if (!key_stack_ptr)
		return NULL;

	key_stack_ptr->size = 0;

	return key_stack_ptr;
}

void ks_add(KeyStack* key_stack_ptr, SDL_Scancode key) {
	for (int i = 0; i < key_stack_ptr->size; i++) {
		if (key == key_stack_ptr->keys[i]) {
			return;
		}
	}
	if (key_stack_ptr->size >= KS_MAX_SIZE) {
		for(int i = 0; i < KS_MAX_SIZE - 1; i++) {
			key_stack_ptr->keys[i] = key_stack_ptr->keys[i + 1];
		}
		key_stack_ptr->size = KS_MAX_SIZE - 1;
	}

	key_stack_ptr->keys[key_stack_ptr->size] = key;
	key_stack_ptr->size++;
}

void ks_delete(KeyStack* key_stack_ptr, SDL_Scancode key) {
	int i = 0;
	for (; i < key_stack_ptr->size; i++) {
		if (key_stack_ptr->keys[i] == key) {
			break;
		}
	}

	if (i <= key_stack_ptr->size) {
		for (; i < key_stack_ptr->size; i++) {
			key_stack_ptr->keys[i] = key_stack_ptr->keys[i + 1];
		}
		key_stack_ptr->size--;
	}
}	

SDL_Scancode ks_peek(KeyStack* key_stack_ptr) {
	if (key_stack_ptr->size == 0)
		return -1;

	return key_stack_ptr->keys[key_stack_ptr->size - 1];
}

void ks_print(KeyStack* key_stack_ptr) {
	for (int i = 0; i < key_stack_ptr->size; i++) {
		printf("%i %s\n", i, SDL_GetScancodeName(key_stack_ptr->keys[i]));
	}
}


void process_input(InputContainer* input_cont) {
	SDL_Event evt;
	int quit_requested = 0, octave = input_cont->octave;
	char note = input_cont->note, mod = input_cont->mod;
	KeyStack* ks_ptr = input_cont->ks_ptr;

	while (SDL_PollEvent(&evt)) {
		if 	(evt.type == SDL_QUIT) {
			quit_requested = 1;
		} else if (evt.type == SDL_KEYUP) {
			ks_delete(ks_ptr, evt.key.keysym.scancode);
		} else if (evt.type == SDL_KEYDOWN) {
			ks_add(ks_ptr, evt.key.keysym.scancode);
		}
	}

	SDL_Scancode pressed_key = ks_peek(ks_ptr);
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
	input_cont->ks_ptr = ks_ptr;
}
