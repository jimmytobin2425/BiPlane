#include "Recorder.h"

void appendToRecording(SAMPLE * input, vector<SAMPLE> * raw_recording, int numFrames){
	int space_left = raw_recording->capacity() - raw_recording->size();
	if(space_left>0) {
		if(space_left >= numFrames){
			for(int i=0;i<numFrames;i++)raw_recording->push_back(input[i]);
		} else {
			for(int i=0;i<space_left;i++)raw_recording->push_back(input[i]);
		}
	}
}

//-----------------------------------------------------------------------------
// name: callme()
// desc: audio callback
//-----------------------------------------------------------------------------
int callme( void * outputBuffer, void * inputBuffer, unsigned int numFrames,
            double streamTime, RtAudioStreamStatus status, void * data )
{
    // cast!
    SAMPLE * input = (SAMPLE *)inputBuffer;
    SAMPLE * output = (SAMPLE *)outputBuffer;
    vector<SAMPLE> *rec = (vector<SAMPLE>*)data;
    //vector<SAMPLE> * rec = *(reinterpret_cast<vector<SAMPLE> **>(data));
    appendToRecording(input,rec, numFrames);
    memset(output, 0, numFrames);
    return 0;
}

Recorder::Recorder(){
	setStatus(0);
	raw_recording.reset(new vector<SAMPLE>);

	// Ge's code to get RtAudio to start up
	// variables
    unsigned int bufferBytes = 0;
    unsigned int bufferFrames = BUFFER_SIZE;

    buffer.reset(new std::vector<SAMPLE>);
    buffer->reserve(BUFFER_SIZE);

    // check for audio devices
    if( audio.getDeviceCount() < 1 )
    {
        // nopes
        cout << "no audio devices found!" << endl;
        exit( 1 );
    }
    audio.showWarnings( true );

    RtAudio::StreamParameters iParams, oParams;
    iParams.deviceId = audio.getDefaultInputDevice();
    iParams.nChannels = MY_IN_CHANNELS;
    iParams.firstChannel = 0;
    oParams.deviceId = audio.getDefaultOutputDevice();
    oParams.nChannels = MY_OUT_CHANNELS;
    oParams.firstChannel = 0;

    // create stream options
    RtAudio::StreamOptions options;

    // go for it
    try {
        // open a stream
        audio.openStream( &oParams, &iParams, MY_FORMAT, MY_SRATE, &bufferFrames, &callme, (void *)raw_recording.get(), &options );
    }
    catch( RtError& e )
    {
        // error!
        cout << e.getMessage() << endl;
        exit( 1 );
    }
   
}

Recording Recorder::record(){
	int max_capacity = MY_SRATE * MY_RECORDING_LENGTH_S;
	raw_recording->reserve(max_capacity);
	 try {
        // start stream
        audio.startStream();
        // wait for recording to fill up.
        while(raw_recording->size() < raw_recording->capacity()){}
        // stop the stream.
        setStatus(1);
    }
    catch( RtError& e )
    {
        // print error message
        cout << e.getMessage() << endl;
        setStatus(3);
        if( audio.isStreamOpen() )
        audio.stopStream();
    }
    Recording rec;
    calculateSpectrum(rec);
	if( audio.isStreamOpen() )
        audio.stopStream();
    return rec;
}

void Recorder::calculateSpectrum(Recording& rec){
	rec.spectrum_frames.reset(new vector<complex*>);
	SAMPLE *fftbuffer = new SAMPLE[BUFFER_SIZE];
	int windowSize = BUFFER_SIZE;
	SAMPLE * window = new SAMPLE[windowSize];
	hanning( window, windowSize );

	for(int i=0;i<raw_recording->size()-BUFFER_SIZE+1;i+=BUFFER_SIZE){
		memcpy( fftbuffer, raw_recording->data()+i, sizeof(SAMPLE)*BUFFER_SIZE );
		apply_window( fftbuffer, window, windowSize );
		rfft( fftbuffer, windowSize/2, true );
		complex * cbuf = new complex[BUFFER_SIZE/2];
		memcpy(cbuf, fftbuffer, sizeof(SAMPLE)*BUFFER_SIZE);
		rec.spectrum_frames->push_back(cbuf);
	}
}

void Recorder::cleanup(){
	if( audio.isStreamOpen() )
        audio.closeStream();
}

// Returns status of recording
// 0 : recording not started
// 1 : recording in memory
// 2 : spectral frames in memory
// 3 : error
int Recorder::getStatus(){
	return status;
}

void Recorder::setStatus(int update){
	status = update;
}




