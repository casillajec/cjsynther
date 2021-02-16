#include <math.h>

#include "oscilators.h"

const float TAU = 2.0 * M_PI;
const float PI_HALF = M_PI / 2.0f;

float osc_f(float pitch, float time, int osc, int pitch_mod, float mod_freq, float mod_amplitude) {
	float output;

	float freq = (pitch * TAU * time) + pitch_mod*(mod_amplitude * pitch * sin(mod_freq * TAU * time));

	switch (osc) {
		case OSC_SIN:
			output = sin(freq);
			break;
		case OSC_SQUARE:
			output = sin(freq) <= 0 ? -0.6f : 0.6f;
			break;
		case OSC_TRIANGLE:
			output = asin(sin(freq)) / PI_HALF;
			break;
		case OSC_EXP:
			output = sin(freq);
			output = (pow(output, 20) + pow(output, 25)) / 2;
			break;
		default:
			output = 0.0f;
	}

	return output;
}
