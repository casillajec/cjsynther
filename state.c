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
}
