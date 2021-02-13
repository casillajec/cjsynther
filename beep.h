
#ifndef BEEP
#define BEEP
typedef struct Beeps {
	int duration;
	float hz; 
	int total_samples;
	float step;
	int current_sample;
} Beep;
#endif

void beep_init(Beep* beep_ptr, float hz, int duration, int sample_rate);

void beep_reset(Beep* beep_ptr);

Beep* beep_new(int duration, float hz, int sample_rate);

void beep_print(Beep *beep);

float pitch_from_rdiff(int r_semitone_diff, int octave);
