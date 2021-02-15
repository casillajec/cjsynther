#include <stdio.h>

#include <SDL2/SDL.h>

#include "input.h"
#include "beep.h"
#include "oscilators.h"

const SDL_Scancode semitones[] = {
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
	SDL_SCANCODE_8,

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

int is_semitone(SDL_Scancode key) {
	for (int i = 0; i < 24; i++) {
		if (key == semitones[i])
			return 1;
	}
	return 0;
}

int get_r_semitone_diff(SDL_Scancode key) {

	int r_semitone_diff = 0;
	while (key != semitones[r_semitone_diff] && r_semitone_diff < 24)
		r_semitone_diff++;

	return r_semitone_diff < 24 ? r_semitone_diff : -1;
}

void kps_init(KeyPitchStack* kps_ptr) {
	kps_ptr->size = 0;
}

void kps_add(KeyPitchStack* kps_ptr, SDL_Scancode key) {

	if (!is_semitone(key))
		return;

	for (int i = 0; i < kps_ptr->size; i++) {
		if (key == kps_ptr->keys[i]) {
			return;
		}
	}

	if (kps_ptr->size >= KPS_MAX_SIZE) {
		for(int i = 0; i < KPS_MAX_SIZE - 1; i++) {
			kps_ptr->keys[i] = kps_ptr->keys[i + 1];
		}
		kps_ptr->size = KPS_MAX_SIZE - 1;
	}

	kps_ptr->keys[kps_ptr->size] = key;
	kps_ptr->size++;
}

void kps_remove(KeyPitchStack* kps_ptr, SDL_Scancode key) {
	int i = 0;
	for (; i < kps_ptr->size; i++) {
		if (kps_ptr->keys[i] == key) {
			break;
		}
	}

	if (i < kps_ptr->size) {
		for (; i < kps_ptr->size; i++) {
			kps_ptr->keys[i] = kps_ptr->keys[i + 1];
		}
		kps_ptr->size--;
	}
}	

SDL_Scancode kps_peek(KeyPitchStack* kps_ptr) {
	if (kps_ptr->size == 0)
		return -1;

	return kps_ptr->keys[kps_ptr->size - 1];
}

void kps_print(KeyPitchStack* kps_ptr) {
	printf("KeyPitchStack of size %i\n", kps_ptr->size);
	for (int i = 0; i < kps_ptr->size; i++) {
		printf("%i: %s\n", i, SDL_GetScancodeName(kps_ptr->keys[i]));
	}
}

void kps_update(KeyPitchStack* kps_ptr, Uint8 octave) {
	for (int i = 0; i < kps_ptr->size; i++) {
		kps_ptr->pitches[i] = pitch_from_rdiff(get_r_semitone_diff(kps_ptr->keys[i]), octave);
	}
}

void process_input(InputContainer* input_cont) {
	SDL_Event evt;
	int quit_requested = 0, octave = input_cont->octave, oscilator = input_cont->oscilator, pitch_mod = input_cont->pitch_mod;
	char note = input_cont->note, mod = input_cont->mod;
	float mod_freq = input_cont->mod_freq, mod_amplitude = input_cont->mod_amplitude;
	KeyPitchStack* kps_ptr = input_cont->kps_ptr;

	SDL_Scancode key;
	while (SDL_PollEvent(&evt)) {
		if 	(evt.type == SDL_QUIT) {
			quit_requested = 1;
		} else if (evt.type == SDL_KEYUP) {
			key = evt.key.keysym.scancode;
			switch (key) {
				case SDL_SCANCODE_LSHIFT:
				case SDL_SCANCODE_RSHIFT:
					pitch_mod = 0; break;
			}
			kps_remove(kps_ptr, key);

		} else if (evt.type == SDL_KEYDOWN) {
		//} else if (evt.type == SDL_KEYDOWN && evt.key.repeat == 0) {
			key = evt.key.keysym.scancode;
			switch (key) {
				case SDL_SCANCODE_ESCAPE:
					quit_requested = 1; break;
				case SDL_SCANCODE_LSHIFT:
				case SDL_SCANCODE_RSHIFT:
					pitch_mod = 1; break;
				case SDL_SCANCODE_RIGHT:
					mod_freq += 0.1f; break;
				case SDL_SCANCODE_LEFT:
					mod_freq -= 0.1f;
					mod_freq = mod_freq < 0 ? 0.0f : mod_freq;
					break;
				case SDL_SCANCODE_UP:
					mod_amplitude += 0.01f; break;
				case SDL_SCANCODE_DOWN:
					mod_amplitude -= 0.01f;
					mod_amplitude = mod_amplitude < 0 ? 0.0f : mod_amplitude;
					break;
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
				case SDL_SCANCODE_F9:
					oscilator = OSC_SIN; break;
				case SDL_SCANCODE_F10:
					oscilator = OSC_SQUARE; break;
				case SDL_SCANCODE_F11:
					oscilator = OSC_TRIANGLE; break;
				case SDL_SCANCODE_F12:
					oscilator = OSC_EXP; break;
			}
			kps_add(kps_ptr, key);
		}
	}
	
	kps_update(kps_ptr, octave);
	input_cont->quit_requested = quit_requested;
	input_cont->octave = octave;
	input_cont->oscilator = oscilator;
	input_cont->kps_ptr = kps_ptr;
	input_cont->pitch_mod = pitch_mod;
	input_cont->mod_freq = mod_freq;
	input_cont->mod_amplitude = mod_amplitude;
}
