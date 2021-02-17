#include "key_pitch_stack.h"
#include "oscilators.h"

#include "state.h"


void synth_state_init(SynthState* st_ptr, BeepIt* beep_it_ptr, KeyPitchStack* kps_ptr) {
	st_ptr->quit_requested = 0;
	
	st_ptr->beep_it = beep_it_ptr;

	st_ptr->current_samples = 0;
	st_ptr->oscilator = OSC_SIN;
	st_ptr->octave = 3;
	st_ptr->kps_ptr = kps_ptr;

	st_ptr->pitch_mod = 0;
	st_ptr->mod_freq = 1.5f;
	st_ptr->mod_amplitude = 0.01f;
	
	for (int i = 0; i < WAVE_GRAPH_LENGTH; i++) {
		st_ptr->wave_graph[i].x = 0;
		st_ptr->wave_graph[i].y = 0;
	}
}

void update_wave_graph(SynthState* st_ptr, Sint16 pitch, int y0, int x0) {
	for (int i = 0; i < WAVE_GRAPH_LENGTH - 1; i++) {
		st_ptr->wave_graph[i] = st_ptr->wave_graph[i+1];
		st_ptr->wave_graph[i].x = st_ptr->wave_graph[i].x - 1;
	}

	st_ptr->wave_graph[WAVE_GRAPH_LENGTH - 1].x = x0 + WAVE_GRAPH_LENGTH;
	float mapped = ((pitch - (-28000)) * (60)) / (28000 - (-28000));  // AMPLITUDE IS 28000 and screen height is 60
	st_ptr->wave_graph[WAVE_GRAPH_LENGTH - 1].y = y0 + 60 - mapped;
}
