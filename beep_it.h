#include "beep.h"

#ifndef BEEP_IT
#define BEEP_IT
typedef struct {
	int current;
	int size;
	//Beep* beeps[];
	//Beep** beeps;
	Beep* beeps;
} BeepIt;
#endif

BeepIt* beep_it_new(int size, Beep* beeps);

Beep beep_it_current(BeepIt* beep_it);

Beep* beep_it_current_ptr(BeepIt* beep_it_ptr);

Beep beep_it_advance(BeepIt* beep_it, int n);

Beep* beep_it_advance_ptr(BeepIt* beep_it_ptr, int n);

void beep_it_reset(BeepIt* beep_it_ptr, int sample_rate);

int beep_it_duration(BeepIt* beep_it);

void beep_it_free(BeepIt* beep_it);

void beep_it_print(BeepIt* beep_it);
