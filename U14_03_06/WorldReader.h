// Utsimo.h
#ifndef WORLDREADER_H
#define WORLDREADER_H
#include "stdafx.h"

//#include "Utsimo.h"
#include "Crossing.h"
#include "Sink.h"
#include "Singlespawn.h"
#include "RateSpawn.h"
#include "Curve2x2.h"
#include "Curve4x4.h"
#include "TRoads2x2.h"
#include "TRoads2x4.h"
#include "TRoads4x2.h"
#include "XRoads2x2.h"
#include "TRoads4x4.h"
#include "XRoads2x4.h"
#include "XRoads4x4.h"
#include "Lane.h"
#include "Worldparams.h"
#include "CarType.h"


class Lane;
class CarType;
class SpawnEvent;
class Crossing;
class RateSpawn;
class SingleSpawn;

class WorldReader{
public:
	WorldReader(string world);
	Lane* readLanes(int* numOfLanes);
	Crossing** readCrossings(Lane* lanes, int* numOfCrossings);
	void readSpawns(Lane* lanes, vector<SingleSpawn>* sSpawns, vector<RateSpawn>* rSpawns, vector<SpawnEvent>* spawnEventList);
	CarType* readCarTypes(int* numOfCarTypes);

	string worlddir;
	

};

#endif