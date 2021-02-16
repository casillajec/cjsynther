#include <stdio.h>

#include <SDL2/SDL.h>

#include "beep.h"
#include "oscilators.h"
#include "state.h"

#include "key_pitch_stack.h"

const SDL_Scancode semitones[] = {
	SDL_SCANCODE_Q,  // A
	SDL_SCANCODE_2,  // A#
	SDL_SCANCODE_W,  // B
	SDL_SCANCODE_E,  // C
	SDL_SCANCODE_4,  // C#
	SDL_SCANCODE_R,  // D
	SDL_SCANCODE_5,  // D#
	SDL_SCANCODE_T,  // E
	SDL_SCANCODE_Y,  // F
	SDL_SCANCODE_7,  // F#
	SDL_SCANCODE_U,  // G
	SDL_SCANCODE_8,  // G#

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
