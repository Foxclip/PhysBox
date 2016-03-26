#include "utils.h"
#include <cstdlib>

double random() {
	return (double)rand()/(double)RAND_MAX;
}

Color randomColor() {
	return { (unsigned char)(random()*256), (unsigned char)(random()*256), (unsigned char)(random()*256) };
}

double randomBetween(double min, double max) {
	return random() * (max - min) + min;
}