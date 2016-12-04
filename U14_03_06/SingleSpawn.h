//SingleSpawn.h

#ifndef SINGLESPAWN_H
#define SINGLESPAWN_H
#include "stdafx.h"
//#include "Utsimo.h"
#include "Crossing.h"
#include "Lane.h"


class Corridor;


class SingleSpawn : public Crossing {

public:
	SingleSpawn(Lane* la, int ind, vector<int> cTypes, vector<NxReal> sTimes);

	char* getStringIdentifier();
	void connectFollowUps();
	NxVec3* getRectangle();
	void printCrossing();
	Corridor* getCorridor(Lane* startLane, Lane* endLane);

	Lane* lane;
	vector<int> carTypes;
	vector<NxReal> spawnTimes;

};


#endif