#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "beep.h"


int beep_total_samples(Beep* beep_ptr, int sample_rate) {
	return ((float)beep_ptr->duration/1000) * sample_rate; 
}

void beep_init(Beep* beep_ptr, float hz, int duration, int sample_rate) {
	beep_ptr->duration = duration;
	beep_ptr->hz = hz;
	beep_ptr->total_samples = beep_total_samples(beep_ptr, sample_rate);
	beep_ptr->step = (hz * 2.0 * M_PI) / sample_rate;
	beep_ptr->current_sample = 0;
}

void beep_reset(Beep* beep_ptr) {
	beep_ptr->current_sample = 0;
}

Beep* beep_new(int duration, float hz, int sample_rate) {
	Beep* beep_ptr = malloc(sizeof(Beep));
	if (!beep_ptr) {
		return NULL;
	}
	
	beep_init(beep_ptr, hz, duration, sample_rate);

	return beep_ptr;
}

void beep_print(Beep *beep) {
	printf("Beep hz: %f | dur: %i | total_samples: %i\n", beep->hz, beep->duration, beep->total_samples);
}

float pitch_from_rdiff(int r_semitone_diff, int octave) {

	if (r_semitone_diff < 0)
		return 0.0f;

	int octave_diff = octave - 4;

	int semitone_diff = 12*octave_diff + r_semitone_diff;
	
	float pitch_factor = pow(pow(2, 1.0/12.0), semitone_diff);

	return 440.0 * pitch_factor;
	
}

float pitch_from_notation(char note, char mod, int octave) {
	const char semitones[] = {'A', '_', 'B', 'C', '_', 'D', '_', 'E', 'F', '_', 'G', '_'};

	int r_semitone_diff = -1;
	if (note != 'S') {
		r_semitone_diff = 0;
		while (note != semitones[r_semitone_diff]) { r_semitone_diff++; }
		int mod_diff = (mod == '#') - (mod == 'b');
	}

	return pitch_from_rdiff(r_semitone_diff, octave);
}
