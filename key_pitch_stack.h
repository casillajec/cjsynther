#include <SDL2/SDL.h>

#ifndef KEY_PITCH_STACK
#define KEY_PITCH_STACK
#define KPS_MAX_SIZE 10

typedef struct {
	int size;
	SDL_Scancode keys[KPS_MAX_SIZE];
	int r_semitone_diffs[KPS_MAX_SIZE];
	float pitches[KPS_MAX_SIZE];
} KeyPitchStack;

#endif

void kps_init(KeyPitchStack* kps_ptr);

void kps_add(KeyPitchStack* key_stack_ptr, SDL_Scancode key);

void kps_remove(KeyPitchStack* key_stack_ptr, SDL_Scancode key);

void kps_update(KeyPitchStack* kps_ptr, Uint8 octave);

SDL_Scancode kps_peek(KeyPitchStack* kps_ptr);
