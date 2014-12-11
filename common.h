#ifndef __COMMON_H__
#define __COMMON_H__

using namespace std;

#include <string>
#include <utility>
#include <iostream>

const int MY_IN_CHANNELS = 1;
const int MY_OUT_CHANNELS = 2;
const int MY_SRATE = 44100;
const float MY_PIE = 3.14159265358979;
const int MY_RECORDING_LENGTH_S = 2;
// corresponding format for RtAudio
#define MY_FORMAT RTAUDIO_FLOAT32
#define BUFFER_SIZE 1024
typedef float SAMPLE;

struct HRTF {
	SAMPLE * rsignal; 
	SAMPLE * lsignal;
	int elevation;
	int azimuth;

	HRTF(SAMPLE * rsig, SAMPLE * lsig, int elev, int az){
		rsignal = rsig;
		lsignal = lsig;
		elevation = elev;
		azimuth = az;
	}
};

#endif