#include "common.h"
#include "HRTFDatabase.h"
#include "Recorder.h"

int main(){
	HRTFDatabase database;
	Recorder recorder;
	Recording rec1 = recorder.record();
	Recording rec2 = recorder.record();
	recorder.cleanup();
	HRTF h = database.InterpolatedHRTF(32, 47);
	
	return 1;
}