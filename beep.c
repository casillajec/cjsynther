#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "beep.h"

const double TAU = 2.0f * M_PI;

int beep_total_samples(Beep* beep_ptr, int sample_rate) {
	return ((float)beep_ptr->duration/1000) * sample_rate; 
}

void beep_init(Beep* beep_ptr, float hz, int duration, int sample_rate) {
	beep_ptr->duration = duration;
	beep_ptr->hz = hz;
	beep_ptr->total_samples = beep_total_samples(beep_ptr, sample_rate);
	beep_ptr->step = (hz * TAU) / sample_rate;
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
