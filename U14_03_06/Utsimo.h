// Utsimo.h
#define DLL __declspec(dllexport)

#ifndef UTSIMO_H
#define UTSIMO_H

#include "stdafx.h"

#include "IDGenerator.h"
#include "Worldparams.h"
#include "CarType.h"

#include "Lane.h"
#include "Crossing.h"
#include "Corridor.h"
#include "SpawnEvent.h"
#include "Sink.h"

#include "Crossing.h"
#include "WorldReader.h"

#include "UtsNx.h"
#include "DebugViewer.h"

//#include "Logger.h"

class UtsNx;
class DebugViewer;
class Lane;
class Street;
class DLL Crossing;
class DLL Sink;
class Car;
class DLL SingleSpawn;
class DLL RateSpawn;


class DLL Utsimo{
public:

	
	Utsimo();
	void setPointOfInterest(NxVec3 poi);
	NxVec3 getPointOfInterest();

	Worldparams* worldparams;
	Lane* lanes;
	Crossing** crossings;
	vector<SingleSpawn> singleSpawns;
	vector<RateSpawn> rateSpawns;
	CarType* carTypes;
	vector<SpawnEvent> spawnEventList;
	vector<Street> streets;
	vector<Sink> sinks;



	float startTime;
	float lastStepLength;
	float lastStepLengthInSeconds;
	float stepStartTime;
	float pureStepTime;
	float pureStepTimeWithDrawing;
	float artificialTimeStep;
	float offsetTime;

	float maxCollisionDistanceSquared;

	UINT64 ticksPerSecond;
	float stepDuration;
	float lastDrawingDuration;
	float microsecondsaverage;

	int stepCounter;

	DebugViewer* dbViewer;
	IDGenerator carIDGenerator;
	UtsNx* utsNx;

	int numOfLanes;
	int numOfCrossings;
	int numOfCarTypes;

	int numOfCurrentCars;

	NxVec3 pointOfInterest;

	static int randomTable[];
	static int randomTableIndex;
	static int getRand();
	static void initRand();
	void reOffsetTime();
	
	/*interface**************************************************************/
	UtsNx* getUtsNx(){return utsNx;}

	void initAll(string world);
	void step();
	void setTimeStep(int stepLength){reOffsetTime(); artificialTimeStep=(float) stepLength;}
	Lane* getLanes(){return lanes;}

	/*************************************************************************/


protected:
	void readSpawns();
	void readLanes();
	void readCrossings();
	void readCarTypes();
	void attachSinks();
	void connectLanes();
	void buildCorridors();
	void buildSpawnEventList();
	void buildStreets();
	void spawnCars();
	void sinkCar(Car* c);
	void sinkCrashCar(Car* c);
	void checkWorldData();
};

#endif //USTIMO_H