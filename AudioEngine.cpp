#include "AudioEngine.h"

void AudioEngine::AudioEngine() {
	audio.reset(new RtAudio);
	database.reset(new HRTFDatabase);
	recorder.reset(new Recorder(audio));
	sounds.reset(new vector<Recording>);
}

void AudioEngine::~AudioEngine() {
	recorder.cleanup(audio);
}

void AudioEngine::Record() {
	sounds->push_back(recorder.record(audio));
}

void AudioEngine::Synthesize(int recording_index, int elevation, int azimuth) {
	
}