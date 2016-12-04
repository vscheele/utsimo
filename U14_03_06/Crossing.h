//Crossing.h
//#pragma message("Crossing is included")
#ifndef CROSSING_H
#define CROSSING_H
#include "stdafx.h"
//#include "Utsimo.h"
//#include "Corridor.h"

class Lane;
class Corridor;

 class Crossing{
	
public:
	virtual char* getStringIdentifier()=0;
	virtual void connectFollowUps()=0;
	virtual NxVec3* getRectangle()=0;
	virtual void printCrossing()=0;
	virtual Corridor* getCorridor(Lane* startLane, Lane* endLane)=0;


	int type;
	int index;
	int pointOfInterest;
	bool inPhysics;
	vector<Corridor> corridors;
	NxVec3* rectangle;

	enum Type{CROSSING , CURVE, SINK, SINGLE_SPAWN, RATE_SPAWN};
	
};


#endif