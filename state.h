#include "beep_it.h"
#include "key_pitch_stack.h"

# ifndef SYNTH_STATE
#define SYNTH_STATE

typedef struct {
	int quit_requested;
	BeepIt* beep_it;
	int current_samples;
	int oscilator;
	int octave;
	KeyPitchStack* kps_ptr;
	int pitch_mod;
	float mod_freq;
	float mod_amplitude;
} SynthState;

#endif

void synth_state_init(SynthState* st_ptr, BeepIt* beep_it_ptr, KeyPitchStack* kps_ptr);
