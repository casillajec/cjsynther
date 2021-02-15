#include <math.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL_ttf.h>

#include "beep.h"
#include "beep_it.h"
#include "input.h"
#include "oscilators.h"

typedef struct {
	BeepIt* beep_it;
	int current_samples;
	int oscilator;
	KeyPitchStack* kps_ptr;
	int pitch_mod;
	float mod_freq;
	float mod_amplitude;
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
	int length = bytes / sizeof(Sint16);  // Specific for AUDIO_SYS16 since buffer is Uint8

	CallbackContainer* cb_cont = (CallbackContainer*) userdata;
	BeepIt* beep_it = cb_cont->beep_it;
	KeyPitchStack* kps_ptr = cb_cont->kps_ptr;
	int oscilator = cb_cont->oscilator;
	int pitch_mod = cb_cont->pitch_mod;
	float mod_freq = cb_cont->mod_freq, mod_amplitude = cb_cont->mod_amplitude;

	Beep* beep_ptr = beep_it_current_ptr(beep_it);
	for (int i = 0; i < length; i++) {
		if (beep_ptr->current_sample > beep_ptr->total_samples) {
			beep_ptr = beep_it_advance_ptr(beep_it, 1);
		}

		float attack_release = attack_release_factor(beep_ptr->current_sample, beep_ptr->total_samples);
		float time = (float)cb_cont->current_samples / SAMPLE_RATE;

		float melody_osc = sin(beep_ptr->hz * 2.0 * M_PI * time);

		float kb_osc = 0.0;
		for (int j = 0; j < kps_ptr->size; j++) {
			kb_osc += osc_f(kps_ptr->pitches[j], time, oscilator, pitch_mod, mod_freq, mod_amplitude);
		}
		if (kps_ptr->size != 0)
			kb_osc = kb_osc / kps_ptr->size;

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

	InputContainer input_cont;
	input_cont.kps_ptr = malloc(sizeof(KeyPitchStack));
	kps_init(input_cont.kps_ptr);
	input_cont.octave = 3;
	input_cont.quit_requested = 0;
	input_cont.oscilator = OSC_SIN;
	input_cont.pitch_mod = 0;
	input_cont.mod_freq = 1.5f;
	input_cont.mod_amplitude = 0.01f;

	CallbackContainer cb_cont;
	cb_cont.beep_it = beep_it;
	cb_cont.current_samples = 0;
	cb_cont.oscilator = input_cont.oscilator;
	cb_cont.kps_ptr = input_cont.kps_ptr;
	cb_cont.pitch_mod = input_cont.pitch_mod;
	cb_cont.mod_freq = input_cont.mod_freq;
	cb_cont.mod_amplitude = input_cont.mod_amplitude;

	SDL_AudioSpec want, have;
	want.freq = SAMPLE_RATE;
	want.format = AUDIO_S16SYS;
	want.channels = 1;
	want.samples = 512;
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

	SDL_Window* win = SDL_CreateWindow("CJSynther", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 120, 0);
	if (!win) {
		SDL_Log("Failed to initialize window: %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		SDL_Log("Failed to initialize renderer: %s", SDL_GetError());
		SDL_DestroyWindow(win);
		SDL_Quit();
		return 1;
	}
	TTF_Font* font = TTF_OpenFont("LiberationMono-Regular.ttf", 24);
	if (!font) {
		printf("Failed to load font\n");
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return 1;
	}
	SDL_Color white = {255, 255, 255};
	SDL_Surface* message_surface;
	SDL_Texture* message_texture;

	SDL_Rect message_rect;
	message_rect.x = 0;
	message_rect.y = 0;
	message_rect.w = 640;
	message_rect.h = 60;

	char buffer[300];

	SDL_PauseAudioDevice(dev_id, 0);
	while (!input_cont.quit_requested) {
		process_input(&input_cont);
		cb_cont.oscilator = input_cont.oscilator;
		cb_cont.pitch_mod = input_cont.pitch_mod;
		cb_cont.mod_freq = input_cont.mod_freq;
		cb_cont.mod_amplitude = input_cont.mod_amplitude;

		sprintf(buffer, "Pitch Mod: %i Mod Freq: %f Mod Amplitude: %f", cb_cont.pitch_mod, cb_cont.mod_freq, cb_cont.mod_amplitude);
		message_surface = TTF_RenderText_Solid(font, buffer, white);
		if (!message_surface) {
			SDL_Log("Failed to create surface: %s", SDL_GetError());
			break;
		}
		message_texture = SDL_CreateTextureFromSurface(renderer, message_surface);
		SDL_FreeSurface(message_surface);
		if (!message_texture) {
			SDL_Log("Failed to create texture: %s", SDL_GetError());
			break;
		}
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, message_texture, NULL, &message_rect);
		SDL_RenderPresent(renderer);
		SDL_DestroyTexture(message_texture);
	}
	SDL_PauseAudioDevice(dev_id, 1);

	beep_it_free(beep_it);
	SDL_FreeSurface(message_surface);
	SDL_DestroyTexture(message_texture);
	SDL_DestroyWindow(win);  // Sometimes causes segfault for some reason
	SDL_CloseAudio();
	SDL_Quit();

	return 0;
}
