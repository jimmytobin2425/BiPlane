/*
** Copyright (C) 2007-2011 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/
#include "HRTFDatabase.h"

#include	<cstdio>
#include <iostream>
#include	<cstring>
#include	<sndfile.hh>

void read_file (string fname, int azimuth, int elevation, 
	unique_ptr<vector<HRTF> >& HRTFs,
	unique_ptr<map<pair<int,int>,int> >& coordMap) {
	static SAMPLE buffer [BUFFER_SIZE] ;
	vector<SAMPLE> rsignal, lsignal;
	SndfileHandle file ;
	file = SndfileHandle (fname.c_str()) ;
	if (file.channels()>0) {
		rsignal.reserve(BUFFER_SIZE/2);
		lsignal.reserve(BUFFER_SIZE/2);
		while(file.read(buffer, BUFFER_SIZE)>0){
			for (int i=0; i<BUFFER_SIZE; i++){
				if(i%2==0){
					rsignal.push_back(buffer[i]);
				}else{
					lsignal.push_back(buffer[i]);
				}
			}
		}
		HRTFs->push_back(HRTF(rsignal.data(), lsignal.data(), elevation, azimuth));
		coordMap->emplace(make_pair(elevation, azimuth), HRTFs->size()-1);
	}
	
	
	//hrtf.reset(new HRTF(fname, rsignal.data(), lsignal.data(), elevation, azimuth));
	//pair<int,int> coord (elevation, azimuth);
	//database.get()->emplace(coord, hrtf);
	/* RAII takes care of destroying SndfileHandle object. */
} /* read_file */

string getDegree(int i){
	if (i==0) {
		return "000";
	} else if (i<100){
		return "0"+to_string(i);
	} else {
		return to_string(i);
	}
}

void HRTFDatabase::LoadHRTFFiles() {
	string prefix = "Impulse_Responses/";
	for (int i=0; i<360; i+=15) { // Elevation
		for (int j=0; j<360; j+=15){ // Azimuth
			string fname = "IRC_1003_C_R0195_T"+getDegree(i)+"_P"+getDegree(j)+".wav";
			read_file(prefix+fname, i, j, HRTFs, coordMap);
		}
	}
}

void CheckMap(unique_ptr<vector<HRTF> >& HRTFs, 
	unique_ptr<map<pair<int,int>,int> >& coordMap){
	for(int i=0; i<HRTFs->size(); i++){
		int index = coordMap->at(make_pair(HRTFs->at(i).elevation, HRTFs->at(i).azimuth));
		if(index!=i){
			cout << "Shooooot! index:" << index << " i:" << i << endl;
		}
	}
}

void findHoles(unique_ptr<map<pair<int,int>,int> >& coordMap){
	cout<<"looking for holes"<<endl;
	int elevs[7] = {0,15,30,45,345,330,315};
	for(int i=0; i<7; i++){
		for(int az=0; az<360; az+=15){
			if(coordMap->count(make_pair(elevs[i],az))<1) cout<<"Hole in map! (el,az):("<<elevs[i]<<","<<az<<")"<<endl;
		}
	}
}

HRTFDatabase::HRTFDatabase() {
	HRTFs.reset(new vector<HRTF>);
	HRTFs.get()->reserve(187);
	coordMap.reset(new map<pair<int,int>,int>);
	LoadHRTFFiles();
	//CheckMap(HRTFs, coordMap);
	//findHoles(coordMap);
}

void findThreeClosestHRTFs(int elevation, int azimuth, unique_ptr<map<pair<int,int>,int> >& coordMap, int* hrtf_idx){
	int azimuth1, azimuth2, elevation1, elevation2;
	elevation1 = (elevation/15*15)%360;
	elevation2 = ((elevation/15+1)*15)%360;
	azimuth1 = (azimuth/15)*15;
	azimuth2 = (azimuth/15+1)*15;
	hrtf_idx[0] = coordMap->at(make_pair(elevation1, azimuth1));
	hrtf_idx[1] = coordMap->at(make_pair(elevation1, azimuth2));
	hrtf_idx[2] = coordMap->at(make_pair(elevation2, azimuth1));
}

void calculateGains(int elevation, int azimuth, int *hrtf_idx, double *gains, unique_ptr<vector<HRTF> >& HRTFs) {
	//position matrix
	double A[3][3];
	double inverse[3][3];

	double virtual_position[3] = {(double)elevation, (double)azimuth, 1.0};
	for (int i=0; i<3; i++){
		A[i][0] = (double)HRTFs->at(hrtf_idx[i]).elevation;
		A[i][1] = (double)HRTFs->at(hrtf_idx[i]).azimuth;
		A[i][2] = 1.0;
	}
	double determinant =   A[0][0]*(A[1][1]*A[2][2]-A[2][1]*A[1][2])
                         -(A[0][1]*(A[1][0]*A[2][2]-A[1][2]*A[2][0]))
                         +(A[0][2]*(A[1][0]*A[2][1]-A[1][1]*A[2][0]));
    if (determinant == 0) {
    	cout<<"OH NO! SINGLUARITY!!!"<<endl;
    	return;
    }
	double invdet = 1.0/determinant;
	inverse[0][0] =  (A[1][1]*A[2][2]-A[2][1]*A[1][2])*invdet;
	inverse[1][0] = -1*(A[0][1]*A[2][2]-A[0][2]*A[2][1])*invdet;
	inverse[2][0] =  (A[0][1]*A[1][2]-A[0][2]*A[1][1])*invdet;
	inverse[0][1] = -1*(A[1][0]*A[2][2]-A[1][2]*A[2][0])*invdet;
	inverse[1][1] =  (A[0][0]*A[2][2]-A[0][2]*A[2][0])*invdet;
	inverse[2][1] = -1*(A[0][0]*A[1][2]-A[1][0]*A[0][2])*invdet;
	inverse[0][2] =  (A[1][0]*A[2][1]-A[2][0]*A[1][1])*invdet;
	inverse[1][2] = -1*(A[0][0]*A[2][1]-A[2][0]*A[0][1])*invdet;
	inverse[2][2] =  (A[0][0]*A[1][1]-A[1][0]*A[0][1])*invdet;

	gains[0] = virtual_position[0]*inverse[0][0]+virtual_position[1]*inverse[1][0]+virtual_position[2]*inverse[2][0];
	gains[1] = virtual_position[0]*inverse[0][1]+virtual_position[1]*inverse[1][1]+virtual_position[2]*inverse[2][1];
	gains[2] = virtual_position[0]*inverse[0][2]+virtual_position[1]*inverse[1][2]+virtual_position[2]*inverse[2][2];

}

HRTF Interpolate(int elevation, int azimuth, int* hrtf_idx, double* gains, unique_ptr<vector<HRTF> >& HRTFs) {
	vector<SAMPLE> rsignal;
	vector<SAMPLE> lsignal;
	rsignal.reserve(BUFFER_SIZE);
	lsignal.reserve(BUFFER_SIZE);
	for(int i=0; i<BUFFER_SIZE/2; i++){
		rsignal.push_back(gains[0]*HRTFs->at(hrtf_idx[0]).rsignal[i]+gains[1]*HRTFs->at(hrtf_idx[1]).rsignal[i]+gains[2]*HRTFs->at(hrtf_idx[2]).rsignal[i]);
		lsignal.push_back(gains[0]*HRTFs->at(hrtf_idx[0]).lsignal[i]+gains[1]*HRTFs->at(hrtf_idx[1]).lsignal[i]+gains[2]*HRTFs->at(hrtf_idx[2]).lsignal[i]);
	}
	return HRTF(rsignal.data(), lsignal.data(), elevation, azimuth);
}

// Interpolate HRTF signals by using Vector Based Amplitude Panning (VBAP)
HRTF interpolateHRTF(int elevation, int azimuth, int* hrtf_idx, unique_ptr<vector<HRTF> >& HRTFs){
	double gains[3];
	calculateGains(elevation, azimuth, hrtf_idx, gains, HRTFs);
	return Interpolate(elevation, azimuth, hrtf_idx, gains, HRTFs);
}

HRTF HRTFDatabase::InterpolatedHRTF(int elevation, int azimuth){
	int hrtf_idx[3];
	findThreeClosestHRTFs(elevation, azimuth, coordMap, hrtf_idx);
	return interpolateHRTF(elevation, azimuth, hrtf_idx, HRTFs);
}

