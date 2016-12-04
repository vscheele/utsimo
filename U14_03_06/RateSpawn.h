//RateSpawn.h

#ifndef RATESPAWN_H
#define RATESPAWN_H
#include "stdafx.h"
//#include "Utsimo.h"
#include "Crossing.h"
#include "Lane.h"


class Corridor;


class RateSpawn : public Crossing {

public:
	RateSpawn(Lane* la, int ind, int cType, NxReal rate);

	char* getStringIdentifier();
	void connectFollowUps();
	NxVec3* getRectangle();
	void printCrossing();
	Corridor* getCorridor(Lane* startLane, Lane* endLane);

	Lane* lane;
	int cType;
	NxReal spawnRate;
	long long lastSpawn;

};


#endif