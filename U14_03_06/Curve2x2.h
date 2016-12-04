//2x2Curve.h
#ifndef CURVE2X2_H
#define CURVE2X2_H
#include "stdafx.h"
//#include "Utsimo.h"
#include "Crossing.h"
#include "Lane.h"

class Corridor;

class Curve2x2: public Crossing{
public:
	Curve2x2(vector<Lane*> lanes, int ind, NxVec3* rec);
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