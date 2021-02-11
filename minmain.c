#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

const int AMPLITUDE = 28000/2;
const int SAMPLE_RATE = 44100;  // Samples per second
const double TAU = M_PI * 2;

double attack_release(int current_sample) {
	int attack_tresh = SAMPLE_RATE * 0.05;
	int release_tresh = SAMPLE_RATE - (SAMPLE_RATE * 0.5);

	int is_attack = (current_sample <= attack_tresh);
	int is_sustain = (attack_tresh < current_sample && current_sample < release_tresh);
	int is_release = (current_sample >= release_tresh);
	
	int smoothing = 1;
	//double attack_factor = (double)current_sample / (double)attack_tresh;
	double attack_factor = pow((double)current_sample / (double)attack_tresh, smoothing);
	//double release_factor = (double)(SAMPLE_RATE - current_sample) / (double)release_tresh;
	double release_factor = pow((double)(SAMPLE_RATE - current_sample) / (double)release_tresh, smoothing);

	return (is_attack * attack_factor) + (is_sustain) + (is_release * release_factor);
}

void audio_callback(void *userdata, Uint8* stream, int bytes) {
	Sint16* buffer = (Sint16*) stream;
	int* sample_nr_ptr = (int*) userdata;
	int length = bytes / sizeof(Sint16);  // Specific for AUDIO_SYS16 since stream is Uint8

	float step = (440.0f * 2.0f*M_PI) / SAMPLE_RATE;
	for (int i = 0; i < length; i++) {
		if (*sample_nr_ptr > SAMPLE_RATE) {
			buffer[i] = 0;
		} else {
			double time = (double)(*sample_nr_ptr) / (double)SAMPLE_RATE;
			double attack_release_factor = attack_release(*sample_nr_ptr);
			buffer[i] = (Sint16)(AMPLITUDE * sin(440.0f * TAU * time) * attack_release_factor);
			//buffer[i] = (Sint16)(AMPLITUDE * sin(step * (*sample_nr_ptr)) * attack_release_factor);
			(*sample_nr_ptr)++;
		}
	}
}

int main(int argc, char* argv[]) {
	if (SDL_Init(SDL_INIT_AUDIO) != 0) {
		SDL_Log("Failed to load SDL Audio module: %s", SDL_GetError());
		return 1;
	}

	int sample_nr = 0;
	SDL_AudioSpec want, have;
	want.freq = SAMPLE_RATE;
	want.format = AUDIO_S16SYS;
	want.channels = 1;
	want.samples = 4096;
	want.callback = audio_callback;
	want.userdata = &sample_nr;

	SDL_AudioDeviceID dev_id = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
	if (dev_id == 0) {
		SDL_Log("Failed to open audio, %s", SDL_GetError());
		return 1;
	}
	if (want.format != have.format) {
		SDL_Log("Failed to get desired AudioSpec");
		return 1;
	}
	printf("want.freq: %i | have.freq: %i\n", want.freq, have.freq);
	printf("want.samples: %i | have.samples: %i\n", want.samples, have.samples);

	SDL_PauseAudioDevice(dev_id, 0);
	SDL_Delay(2000);

	//SDL_PauseAudioDevice(dev_id, 1);
	SDL_LockAudioDevice(dev_id);
	printf("sample_nr: %i\n", sample_nr);
	sample_nr = 0;
	//SDL_PauseAudioDevice(dev_id, 0);
	SDL_UnlockAudioDevice(dev_id);

	SDL_Delay(2000);
	SDL_PauseAudioDevice(dev_id, 1);
	printf("sample_nr: %i\n", sample_nr);

	SDL_CloseAudio();

	return 0;
}
