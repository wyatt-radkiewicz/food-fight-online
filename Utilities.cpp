#include "Utilities.h"

#include <ctime>
#include <random>

void randomSeed() {
	srand(time(nullptr));
}
int getRandom(int min, int max) {
	float percent = (float)rand() / (float)RAND_MAX;

	return (int)(((max - min) * percent) + min);
}
int randChoose(int count, ...) {
	va_list ops;
	int j;
	double tot = 0;
	int randi = getRandom(0, count);
	va_start(ops, count); //Requires the last fixed parameter (to get the address)
	for (j = 0; j < count; j++)
		if (j == randi)
			return va_arg(ops, int);
	va_end(ops);
	return 0;
}
int sign(int x) {
	if (x > 0) {
		return 1;
	}
	else if (x < 0) {
		return -1;
	}
	else {
		return 0;
	}
}
float sign(float x) {
	if (x > 0.0f) {
		return 1.0f;
	}
	else if (x < 0.0f) {
		return -1.0f;
	}
	else {
		return 0.0f;
	}
}
