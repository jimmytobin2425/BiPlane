#ifndef __AUDIOENGINE_H__
#define __AUDIOENGINE_H__

#include "common.h"
#include "RtAudio.h"
#include "HRTFDatabase.h"
#include "Recorder.h"

class AudioEngine(){
	
public:
	AudioEngine();
	~AudioEngine();
	void Synthesize(int recording_index, int elevation, int azimuth);
	void Record();

private:
	unique_ptr<RtAudio> audio;
	unique_ptr<vector<Recording> > sounds;
	unique_ptr<Recorder> recorder;
	unique_ptr<HRTFDatabase> database;
}

#endif