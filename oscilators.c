#include <math.h>

#include "oscilators.h"

float osc_f(float pitch, float time, int osc) {
	float output;

	switch (osc) {
		case OSC_SIN:
			output = sin(pitch * 2.0 * M_PI * time);
			break;
		case OSC_SQUARE:
			output = sin(pitch * 2.0 * M_PI * time) <= 0 ? -1.0f : 1.0f;
			break;
		case OSC_TRIANGLE:
			output = asin(sin(pitch * 2.0 * M_PI * time));
			break;
		case OSC_EXP:
			output = sin(pitch * 2.0 * M_PI * time);
			output = pow(output, 20) + pow(output, 25);
			break;
		default:
			output = 0.0f;
	}

	return output;
}
