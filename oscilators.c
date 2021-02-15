#include <math.h>

#include "oscilators.h"

const float TAU = 2.0 * M_PI;

float osc_f(float pitch, float time, int osc, int pitch_mod, float mod_freq, float mod_amplitude) {
	float output;

	float freq = (pitch * TAU * time) + pitch_mod*(mod_amplitude * pitch * sin(mod_freq * TAU * time));

	switch (osc) {
		case OSC_SIN:
			output = sin(freq);
			break;
		case OSC_SQUARE:
			output = sin(freq) <= 0 ? -1.0f : 1.0f;
			break;
		case OSC_TRIANGLE:
			output = asin(sin(freq));
			break;
		case OSC_EXP:
			output = sin(freq);
			output = pow(output, 20) + pow(output, 25);
			break;
		default:
			output = 0.0f;
	}

	return output;
}
