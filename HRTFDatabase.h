#ifndef __HRTFDATABASE_H__
#define __HRTFDATABASE_H__

#include <map>
#include <string>
#include <vector>

#include "common.h"

class HRTFDatabase {
	public:
		HRTFDatabase();
		HRTF InterpolatedHRTF(int elevation, int azimuth);

	protected:
		void LoadHRTFFiles();
		unique_ptr<vector<HRTF> >HRTFs;
		unique_ptr<map<pair<int,int>,int> > coordMap;
};

#endif