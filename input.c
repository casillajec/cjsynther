#include <stdio.h>

#include <SDL2/SDL.h>

#include "input.h"
#include "beep.h"

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

void ps_init(PitchSet* ps_ptr) {
	ps_ptr->max_size = PS_DEFAULT_MAX_SIZE;
	ps_ptr->size = 0;
}

void ps_add(PitchSet* ps_ptr, float pitch) {
	if (ps_ptr->size >= ps_ptr->max_size || pitch == 0.0)
		return;

	for (int i = 0; i < ps_ptr->size; i++) {
		if (ps_ptr->pitches[i] == pitch)
			return;
	}

	ps_ptr->pitches[ps_ptr->size] = pitch;
	ps_ptr->size++;
}

void ps_remove(PitchSet* ps_ptr, float pitch) {
	int i;
	for (i = 0; i < ps_ptr->size; i++) {
		if (ps_ptr->pitches[i] == pitch)
			break;
	}

	if (i < ps_ptr->size) {
		ps_ptr->size--;
		for (; i < ps_ptr->size; i++) {
			ps_ptr->pitches[i] = ps_ptr->pitches[i+1];
		}
	}
}

void ps_print(PitchSet* ps_ptr) {
	printf("PitchSet of size: %i with max size: %i\n", ps_ptr->size, ps_ptr->max_size);
	for (int i = 0; i < ps_ptr->size; i++) {
		printf("%i %f\n", i, ps_ptr->pitches[i]);
	}
}

void ps_set_max_size(PitchSet* ps_ptr, int max_size) {
	ps_ptr->max_size = max_size;
	ps_ptr->size = ps_ptr->size > max_size ? max_size : ps_ptr->size;
}

float ps_last(PitchSet* ps_ptr) {
	return ps_ptr->pitches[ps_ptr->size - 1];
}

typedef struct {
	int r_semitone_diff;
	int octave;
} r_semitone_diff_ret;

r_semitone_diff_ret get_r_semitone_diff(SDL_Scancode key, int octave) {
	const SDL_Scancode main_semitones[] = {
		SDL_SCANCODE_Z,  // A
		SDL_SCANCODE_S,  // A#
		SDL_SCANCODE_X,  // B
		SDL_SCANCODE_C,  // C
		SDL_SCANCODE_F,  // C#
		SDL_SCANCODE_V,  // D
		SDL_SCANCODE_G,  // D#
		SDL_SCANCODE_B,  // E
		SDL_SCANCODE_N,  // F
		SDL_SCANCODE_J,  // F#
		SDL_SCANCODE_M,  // G
		SDL_SCANCODE_K   // G#
	};

	const SDL_Scancode secd_semitones[] = {
		SDL_SCANCODE_Q,
		SDL_SCANCODE_2,
		SDL_SCANCODE_W,
		SDL_SCANCODE_E,
		SDL_SCANCODE_4,
		SDL_SCANCODE_R,
		SDL_SCANCODE_5,
		SDL_SCANCODE_T,
		SDL_SCANCODE_Y,
		SDL_SCANCODE_7,
		SDL_SCANCODE_U,
		SDL_SCANCODE_8
	};
	int r_semitone_diff = 0;
	r_semitone_diff_ret ret;

	while (key != main_semitones[r_semitone_diff] && r_semitone_diff < 12)
		r_semitone_diff++;

	if (r_semitone_diff < 12) {
		ret.r_semitone_diff = r_semitone_diff;
		ret.octave = octave;
		return ret;
	}

	r_semitone_diff = 0;
	while (key != secd_semitones[r_semitone_diff] && r_semitone_diff < 12)
		r_semitone_diff++;

	if (r_semitone_diff < 12) {
		ret.r_semitone_diff = r_semitone_diff;
		ret.octave = octave - 1;
		return ret;
	}
	
	ret.r_semitone_diff = -1;
	ret.octave = octave;
	return ret;
}

void process_input(InputContainer* input_cont) {
	SDL_Event evt;
	int quit_requested = 0, octave = input_cont->octave;
	char note = input_cont->note, mod = input_cont->mod;
	PitchSet* ps_ptr = input_cont->ps_ptr;

	SDL_Scancode pressed_key;
	float pitch;
	r_semitone_diff_ret ret;
	int _octave, r_semitone_diff;
	while (SDL_PollEvent(&evt)) {
		if 	(evt.type == SDL_QUIT) {
			quit_requested = 1;
		} else if (evt.type == SDL_KEYUP) {
			printf("KEYUP: %s\n", SDL_GetScancodeName(evt.key.keysym.scancode));
			ret = get_r_semitone_diff(evt.key.keysym.scancode, octave);
			r_semitone_diff = ret.r_semitone_diff;
			_octave = ret.octave;
			pitch = pitch_from_rdiff(r_semitone_diff, _octave);
			ps_remove(ps_ptr, pitch);

		} else if (evt.type == SDL_KEYDOWN) {
			pressed_key = evt.key.keysym.scancode;
			printf("KEYDOWN: %s\n", SDL_GetScancodeName(pressed_key));
			switch (pressed_key) {
				case SDL_SCANCODE_ESCAPE:
					quit_requested = 1; break;
				case SDL_SCANCODE_F2:
					octave = 2; break;
				case SDL_SCANCODE_F3:
					octave = 3; break;
				case SDL_SCANCODE_F4:
					octave = 4; break;
				case SDL_SCANCODE_F5:
					octave = 5; break;
				case SDL_SCANCODE_F6:
					octave = 6; break;
			}
			ret = get_r_semitone_diff(evt.key.keysym.scancode, octave);
			r_semitone_diff = ret.r_semitone_diff;
			_octave = ret.octave;
			pitch = pitch_from_rdiff(r_semitone_diff, _octave);
			ps_add(ps_ptr, pitch);
		}
	}

	input_cont->quit_requested = quit_requested;
	input_cont->octave = octave;
	input_cont->note = note;
	input_cont->mod = mod;
	input_cont->ps_ptr = ps_ptr;
}
