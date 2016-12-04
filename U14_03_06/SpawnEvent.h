//SpawnEvent.h

#ifndef SPAWNEVENT_H
#define SPAWNEVENT_H
#include "stdafx.h"
//#include "Utsimo.h"

class SpawnEvent{
public:
	int cType;
	NxReal time;
	int lane;

	SpawnEvent::SpawnEvent(int cT, NxReal spawnTime, int spawnLane){
		cType=cT;
		time=spawnTime;
		lane=spawnLane;
		
	}
	SpawnEvent::operator<(SpawnEvent b){
		if (this->time< b.time) return true;
		else return false;
	}
};



#endif