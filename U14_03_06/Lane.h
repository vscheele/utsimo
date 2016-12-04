//Lane.h
#ifndef LANE_H
#define LANE_H
#include "stdafx.h"
#include "Car.h"
#include "Corridor.h"
#include "Crossing.h"
#include "Worldparams.h"
#include "Street.h"
//#include "Utsimo.h"


class Car;
class Corridor;
//class Crossing;

class Lane {
public:
	Crossing* startCrossing;
	Crossing* endCrossing;

	Street* street;

	Lane();
	Lane(NxVec3 s, NxVec3 e, int utsi);
	NxVec3 start, end;
	NxVec3 rectangle[4];
	NxVec3 orthoNorma;
	NxVec3 diffVec;
	NxVec3 diffVecNorma;
	NxVec3 skyNorma;

	int utsindex;
	vector<Lane*> followers;
	vector<Corridor*> possibleCorridors;
	vector<Car*> cars;
	void setPoints(NxVec3 start, NxVec3 end);
	void setIndex(int utsi);
	void addCar(Car* c);
	void removeCar(Car* c);
	bool hasPlaceforCarLength(NxReal carLength);
	float registeredCarLengthFromCorridor;

};
#endif 