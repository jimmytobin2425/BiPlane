#ifndef __PLAYER_H__
#define __PLAYER_H__

class Player {
public:
	Player(unique_ptr<RtAudio>& audio);
	void playBuffer(SAMPLE* buffer);
};

#endif