#ifndef __RECORDER_H__
#define __RECORDER_H__

#include <vector>
#include <cstring>

#include "common.h"
#include "RtAudio.h"
#include "chuck_fft.h"

struct Recording
{
	unique_ptr<vector<complex*> >spectrum_frames;
};


class Recorder {

public:
	Recorder();
	Recording record();
	SAMPLE** getRecording();
	int getStatus();
	void cleanup();

private:
	RtAudio audio;
	void setStatus(int update);
	void calculateSpectrum(Recording& rec);
	unique_ptr<vector<SAMPLE> >raw_recording;
	unique_ptr<vector<SAMPLE> >buffer;
	unique_ptr<vector<complex*> >spectrum_frames;
	int status;
};

#endif
