#include <stdio.h>
#include <stdlib.h>

#include "beep_it.h"
#include "beep.h"


BeepIt* beep_it_new(int size, Beep beeps[]) {
	BeepIt* beep_it = malloc(sizeof(BeepIt));

	beep_it->current = 0;
	beep_it->beeps = beeps;
	beep_it->size = size;
	
	return beep_it;
}

Beep beep_it_current(BeepIt* beep_it) {
	return beep_it->beeps[beep_it->current];
}

Beep* beep_it_current_ptr(BeepIt* beep_it_ptr) {
	return beep_it_ptr->beeps + beep_it_ptr->current;
}

Beep beep_it_advance(BeepIt* beep_it, int n) {
	beep_it->current += n;
	while (beep_it->current >= beep_it->size) {
		beep_it-> current -= beep_it->size;
	}
	return beep_it_current(beep_it);	
}

Beep* beep_it_advance_ptr(BeepIt* beep_it_ptr, int n) {
	beep_reset(beep_it_current_ptr(beep_it_ptr));
	beep_it_ptr->current += n;
	while(beep_it_ptr->current >= beep_it_ptr->size) {
		beep_it_ptr->current -= beep_it_ptr->size;
	}
	return beep_it_current_ptr(beep_it_ptr);
}

void beep_it_reset(BeepIt* beep_it_ptr, int sample_rate) {
	for (int i = 0; i < beep_it_ptr->size; i++) {
		beep_reset(beep_it_ptr->beeps + i);
	}
	beep_it_ptr->current = 0;
}

int beep_it_duration(BeepIt* beep_it) {
	int duration = 0;
	for (int i = 0; i < beep_it->size; i++) {
		duration += beep_it->beeps[i].duration;
	}

	return duration;
}

void beep_it_free(BeepIt* beep_it) {
	free(beep_it->beeps);
	free(beep_it);
}

void beep_it_print(BeepIt* beep_it) {

	for (int i = 0; i < beep_it->size; i++) {
		printf((i == beep_it->current) ? "+ " : "  ");
		beep_print(&(beep_it->beeps[i]));
	}
}

