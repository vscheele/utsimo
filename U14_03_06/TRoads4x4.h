//TRoads4x4.h
#ifndef TROADS4X4_H
#define TROADS4X4_H
#include "stdafx.h"
//#include "Utsimo.h"
#include "Crossing.h"
#include "Lane.h"

class Corridor;

class TRoads4x4: public Crossing{
public:
	TRoads4x4(vector<Lane*> lanes, int ind, NxVec3* rec);
	char* getStringIdentifier();
	void connectFollowUps();
	NxVec3* getRectangle();
	void printCrossing();
	Corridor* getCorridor(Lane* startLane, Lane* endLane);
	


	Lane** lanes;
	unsigned lanesSize;

protected:
	void buildCorridors();
};

#endif