#include <math.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

#include "beep.h"
#include "beep_it.h"
#include "input.h"

typedef struct {
	BeepIt* beep_it;
	int current_samples;
	float kb_hz;
	PitchSet* ps_ptr;
} CallbackContainer;

const int AMPLITUDE = 28000;
const int SAMPLE_RATE = 48000;  // Samples per second


float attack_release_factor(int current_sample, int total_samples) {
	float attack_treshold = total_samples * 0.05;
	float release_treshold = total_samples - (total_samples * 0.5);

	int is_attack = (current_sample <= attack_treshold);
	int is_sustain = (attack_treshold < current_sample && current_sample < release_treshold);
	int is_release = (current_sample >= release_treshold);

	float attack_factor = (float)current_sample / attack_treshold;
	float release_factor = (float)(total_samples - current_sample) / release_treshold;
	
	return (is_attack * attack_factor) + (is_sustain) + (is_release * release_factor);
}

void audio_callback(void *userdata, Uint8* stream, int bytes) {
	Sint16* buffer = (Sint16*) stream;
	CallbackContainer* cb_cont = (CallbackContainer*) userdata;
	BeepIt* beep_it = cb_cont->beep_it;
	PitchSet* ps_ptr = cb_cont->ps_ptr;
	int length = bytes / sizeof(Sint16);  // Specific for AUDIO_SYS16 since buffer is Uint8

	Beep* beep_ptr = beep_it_current_ptr(beep_it);
	for (int i = 0; i < length; i++) {
		if (beep_ptr->current_sample > beep_ptr->total_samples) {
			beep_ptr = beep_it_advance_ptr(beep_it, 1);
		}
		float attack_release = attack_release_factor(beep_ptr->current_sample, beep_ptr->total_samples);
		float time = (float)cb_cont->current_samples / SAMPLE_RATE;
		float melody_osc = sin(beep_ptr->hz * 2.0 * M_PI * time);
		float kb_osc = 0.0;
		for (int j = 0; j < ps_ptr->size; j++) {
			kb_osc += sin(ps_ptr->pitches[j] * 2.0 * M_PI * time);
		}
		if (ps_ptr->size != 0)
			kb_osc = kb_osc / ps_ptr->size;

		buffer[i] = (Sint16)(
			+ (AMPLITUDE*0.5*attack_release) * melody_osc
			+ (AMPLITUDE*0.5)		 * kb_osc
		);
		beep_ptr->current_sample++;
		cb_cont->current_samples = (cb_cont->current_samples + 1) % SAMPLE_RATE;
	}
}

float get_hz(char note, char mod, int octave) {
	const char semitones[] = {'A', '_', 'B', 'C', '_', 'D', '_', 'E', 'F', '_', 'G', '_'};

	if (note == 'S') { return 0.0f; }

	int octave_diff = octave - 4;
	
	int semitone_diff = 0;
	while (note != semitones[semitone_diff]) { semitone_diff++; }
	int mod_diff = (mod == '#') - (mod == 'b');

	semitone_diff = 12*octave_diff + semitone_diff + mod_diff;
	
	float hz_diff = pow(pow(2, 1.0/12.0), semitone_diff);
	return 440.0 * hz_diff;
}

int main(int argc, char* argv[]) {
	
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) {
		SDL_Log("Failed to load SDL Audio module: %s", SDL_GetError());
		return 1;
	}

	if (argc < 2) {
		printf("You must provide an spec file\n");
		return 1;
	}
	
	char* spec_file_path = argv[1];
	FILE* spec_file = fopen(spec_file_path, "r");
	if (spec_file == NULL) {
		printf("Could not open spec file %s\n", spec_file_path);
		return 1;
	}

	int n_beeps;
	fscanf(spec_file, "%i\n", &n_beeps);
	Beep* beeps = malloc(sizeof(Beep) * n_beeps);
	
	int duration, octave;
	char note, mod;
	float hz;
	for (int i = 0; i < n_beeps; i++) {
		fscanf(spec_file, "%c%c%i %i\n", &note, &mod, &octave, &duration);
		hz = get_hz(note, mod, octave);
		beep_init(beeps + i, hz, duration, SAMPLE_RATE);
	}
	BeepIt* beep_it = beep_it_new(n_beeps, beeps);
	beep_it_print(beep_it);


	int total_beep_it_duration = beep_it_duration(beep_it);
	printf("total beep_it duration: %i\n", total_beep_it_duration);

	CallbackContainer cb_cont;
	cb_cont.beep_it = beep_it;
	cb_cont.current_samples = 0;
	cb_cont.kb_hz = 0.0;

	SDL_AudioSpec want, have;
	want.freq = SAMPLE_RATE;
	want.format = AUDIO_S16SYS;
	want.channels = 1;
	want.samples = 1028;
	want.callback = audio_callback;
	want.userdata = &cb_cont;
	
	SDL_AudioDeviceID dev_id = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
	if (dev_id == 0) {
		SDL_Log("Failed to open audio, %s", SDL_GetError());
		return 1;
	}
	if (want.format != have.format) {
		SDL_Log("Failed to get desired AudioSpec");
		return 1;
	}

	SDL_Window* win = SDL_CreateWindow("Synther", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
	if (!win) {
		SDL_Log("Failed to initialize window: %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}
	
	int quit_requested = 0;
	SDL_PauseAudioDevice(dev_id, 0);
	InputContainer input_cont;
	input_cont.ps_ptr = malloc(sizeof(PitchSet));
	ps_init(input_cont.ps_ptr);
	cb_cont.ps_ptr = input_cont.ps_ptr;

	while (!quit_requested) {
		process_input(&input_cont);
		quit_requested = input_cont.quit_requested;
		octave = input_cont.octave;
		cb_cont.kb_hz = ps_last(input_cont.ps_ptr);
	}
	beep_it_free(beep_it);
	SDL_DestroyWindow(win);
	SDL_CloseAudio();
	SDL_Quit();

	return 0;
}
