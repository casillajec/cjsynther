#include <SDL2/SDL.h>

#ifndef KEY_STACK
#define KEY_STACK
#define KS_MAX_SIZE 10

typedef struct {
	int size;
	SDL_Scancode keys[KS_MAX_SIZE];
} KeyStack;
#endif

KeyStack* ks_new();

void ks_add(KeyStack* key_stack_ptr, SDL_Scancode key);

void ks_delete(KeyStack* key_stack_ptr, SDL_Scancode key);

SDL_Scancode ks_peek(KeyStack* key_stack_ptr);

#ifndef PITCH_SET
#define PITCH_SET

#define PS_DEFAULT_MAX_SIZE 10
typedef struct {
	int max_size;
	int size;
	float pitches[PS_DEFAULT_MAX_SIZE];
} PitchSet;
#endif

void ps_init(PitchSet* ps_ptr);

void ps_add(PitchSet* ps_ptr, float pitch);

void ps_remove(PitchSet* ps_ptr, float pitch);

void ps_set_max_size(PitchSet* ps_ptr, int max_size);

void ps_print(PitchSet* ps_ptr);

float ps_last(PitchSet* ps_ptr);

#ifndef INPUT_CONTAINER
#define INPUT_CONTAINER
typedef struct {
	PitchSet* ps_ptr;
	//KeyStack* ks_ptr;
	char note;
	char mod;
	int octave;
	int quit_requested;	
} InputContainer;
#endif

void process_input(InputContainer* input_cont);

