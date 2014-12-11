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
	Recorder(unique_ptr<RtAudio>& audio);
	Recording record(unique_ptr<RtAudio>& audio);
	void cleanup(unique_ptr<RtAudio>& audio);

private:
	void calculateSpectrum(Recording& rec);
	unique_ptr<vector<SAMPLE> >raw_recording;
	unique_ptr<vector<SAMPLE> >buffer;
};

#endif
