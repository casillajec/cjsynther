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

#ifndef INPUT_CONTAINER
#define INPUT_CONTAINER
typedef struct {
	KeyStack* ks_ptr;
	char note;
	char mod;
	int octave;
	int quit_requested;	
} InputContainer;
#endif

void process_input(InputContainer* input_cont);

