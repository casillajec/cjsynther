#include <math.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL_ttf.h>

#include "beep.h"
#include "beep_it.h"
#include "input.h"
#include "state.h"
#include "oscilators.h"

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
	int length = bytes / sizeof(Sint16);  // Specific for AUDIO_SYS16 since buffer is Uint8

	SynthState* st_ptr = (SynthState*) userdata;
	BeepIt* beep_it = st_ptr->beep_it;
	KeyPitchStack* kps_ptr = st_ptr->kps_ptr;

	int oscilator = st_ptr->oscilator;
	int pitch_mod = st_ptr->pitch_mod;
	float mod_freq = st_ptr->mod_freq,
	      mod_amplitude = st_ptr->mod_amplitude;
	Beep* beep_ptr = beep_it_current_ptr(beep_it);

	for (int i = 0; i < length; i++) {
		if (beep_ptr->current_sample > beep_ptr->total_samples) {
			beep_ptr = beep_it_advance_ptr(beep_it, 1);
		}

		float attack_release = attack_release_factor(beep_ptr->current_sample, beep_ptr->total_samples);
		float time = (float)st_ptr->current_samples / SAMPLE_RATE;
		
		float acc = 0.0f;
		for (int j = 0; j < kps_ptr->size; j++) {
			acc += osc_f(kps_ptr->pitches[j], time, oscilator, pitch_mod, mod_freq, mod_amplitude);
		}
		acc += attack_release * osc_f(beep_ptr->hz, time, oscilator, pitch_mod, mod_freq, mod_amplitude);
		acc = acc / (kps_ptr->size + 1);

		buffer[i] = (Sint16)(AMPLITUDE * acc);

		update_wave_graph(st_ptr, buffer[i], 60, 10);

		beep_ptr->current_sample++;
		st_ptr->current_samples++;
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

BeepIt* parse_spec_file(FILE* spec_file) {

	int n_beeps = 0;
	char ch;
	while (!feof(spec_file)) {
		ch = fgetc(spec_file);
		n_beeps += (ch == '\n');
	}
	rewind(spec_file);
	
	Beep* beeps = malloc(sizeof(Beep) * n_beeps);
	if (!beeps)
		return NULL;

	int duration, octave;
	char note, mod;
	float hz;
	for (int i = 0; i < n_beeps; i++) {
		fscanf(spec_file, "%c%c%i %i\n", &note, &mod, &octave, &duration);
		hz = get_hz(note, mod, octave);
		beep_init(beeps + i, hz, duration, SAMPLE_RATE);
	}

	return beep_it_new(n_beeps, beeps);
}

const SDL_Color white = {255, 255, 255};

int update_window(SynthState* st_ptr, SDL_Renderer* renderer_ptr, SDL_Rect* rect_ptr, TTF_Font* font_ptr) {

	char buffer[1024];

	sprintf(buffer, "Pitch Mod: %i Mod Freq: %f Mod Amplitude: %f", st_ptr->pitch_mod, st_ptr->mod_freq, st_ptr->mod_amplitude);

	SDL_Surface* surface_ptr = TTF_RenderText_Solid(font_ptr, buffer, white);
	if (!surface_ptr) {
		SDL_Log("Failed to create surface: %s", SDL_GetError());
		SDL_FreeSurface(surface_ptr);
		return 0;
	}

	SDL_Texture* texture_ptr = SDL_CreateTextureFromSurface(renderer_ptr, surface_ptr);
	SDL_FreeSurface(surface_ptr);
	if (!texture_ptr) {
		SDL_Log("Failed to create texture: %s", SDL_GetError());
		SDL_DestroyTexture(texture_ptr);
		return 0;
	}
	SDL_RenderClear(renderer_ptr);
	SDL_RenderCopy(renderer_ptr, texture_ptr, NULL, rect_ptr);
	
	SDL_SetRenderDrawColor(renderer_ptr, 255, 255, 255, 255);
	if (SDL_RenderDrawPoints(renderer_ptr, st_ptr->wave_graph, WAVE_GRAPH_LENGTH) != 0) {
		SDL_Log("Failed to draw wave graph: %s", SDL_GetError());
		SDL_DestroyTexture(texture_ptr);
	}
	SDL_SetRenderDrawColor(renderer_ptr, 0, 0, 0, 255);

	SDL_RenderPresent(renderer_ptr);
	SDL_DestroyTexture(texture_ptr);

	return 1;
}


int main(int argc, char* argv[]) {
	
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) {
		SDL_Log("Failed to load SDL Audio module: %s", SDL_GetError());
		return 1;
	}

	if (TTF_Init() == -1) {
		SDL_Log("Failed to initialize SDL_ttf: %s", TTF_GetError());
	}
	
	BeepIt* beep_it;	
	if (argc < 2) {  // No spec file
		Beep* beeps = malloc(sizeof(Beep));
		if (!beeps) {
			printf("Could not allocate beeps\n");
			return 1;
		}
		beep_init(beeps, 0.0, 1000, SAMPLE_RATE);
		beep_it = beep_it_new(1, beeps);
	
	} else {
		char* spec_file_path = argv[1];
		FILE* spec_file = fopen(spec_file_path, "r");
		if (spec_file == NULL) {
			printf("Could not open spec file %s\n", spec_file_path);
			return 1;
		}

		beep_it = parse_spec_file(spec_file);
		if (!beep_it) {
			printf("Could not allocate beep_it\n");
			return 1;
		}
	}
	beep_it_print(beep_it);

	KeyPitchStack kps;
	kps_init(&kps);
	SynthState st;
	synth_state_init(&st, beep_it, &kps);

	SDL_AudioSpec want, have;
	want.freq = SAMPLE_RATE;
	want.format = AUDIO_S16SYS;
	want.channels = 1;
	want.samples = 512;
	want.callback = audio_callback;
	want.userdata = &st;
	
	SDL_AudioDeviceID dev_id = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
	if (dev_id == 0) {
		SDL_Log("Failed to open audio, %s", SDL_GetError());
		return 1;
	}
	if (want.format != have.format) {
		SDL_Log("Failed to get desired AudioSpec");
		return 1;
	}

	SDL_Window* win_ptr = SDL_CreateWindow("CJSynther", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 120, 0);
	if (!win_ptr) {
		SDL_Log("Failed to initialize window: %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	SDL_Renderer* renderer_ptr = SDL_CreateRenderer(win_ptr, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer_ptr) {
		SDL_Log("Failed to initialize renderer: %s", SDL_GetError());
		SDL_DestroyWindow(win_ptr);
		SDL_Quit();
		return 1;
	}
	TTF_Font* font_ptr = TTF_OpenFont("LiberationMono-Regular.ttf", 24);
	if (!font_ptr) {
		printf("Failed to load font\n");
		SDL_DestroyRenderer(renderer_ptr);
		SDL_DestroyWindow(win_ptr);
		SDL_Quit();
		return 1;
	}

	SDL_Rect message_rect;
	message_rect.x = 0;
	message_rect.y = 0;
	message_rect.w = 640;
	message_rect.h = 60;

	SDL_PauseAudioDevice(dev_id, 0);
	while (!st.quit_requested) {
		process_input(&st);

		if (!update_window(&st, renderer_ptr, &message_rect, font_ptr)) {
			break;
		}
	}
	SDL_PauseAudioDevice(dev_id, 1);

	beep_it_free(beep_it);
	SDL_DestroyWindow(win_ptr);  // Sometimes causes segfault for some reason
	SDL_CloseAudio();
	SDL_Quit();

	return 0;
}
