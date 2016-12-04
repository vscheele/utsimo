/*------------------------------------------------------------------------
 *  -- Crossing.h
 *
 *  Declaration file for the Corridor Interface.
 *
 *  Author: Valentin Scheele (valentin@scheeles.de)
 * 
 *------------------------------------------------------------------------*/
#ifndef CORRIDOR_H
#define CORRIDOR_H
#include "stdafx.h"
//#include "Utsimo.h"
#include "Lane.h"
#include "Crossing.h"
#include "Car.h"

class Car;
class Crossing;
class Lane;

class Corridor{
public:
	Corridor(Lane* corrStart, Lane* corrEnd, Crossing* c);
	
	NxVec3 interpolate(NxReal u);
	Lane* getStartLane();
	Lane* getEndLane();
	NxReal getCurvature();
	Crossing* getCrossing();
	void setRightOfWay(vector<Corridor*>);
	NxVec3 getInterpolatedVelocityDirection(float u);
	void addCar(Car* c);
	void removeCar(Car* c);
	void addPrep();
	void removePrep();

	bool isCrossingFree();

	NxVec3* preCalculated;
	vector<Corridor*> rightOfWay;
	vector<Corridor*> hasRightOfWay;

	vector<Car*> cars;

	NxReal curvature;
	NxReal directLength;
	NxReal curveLength;
	NxReal curveMaxSpeed;
	unsigned carsInPrep;
	Lane* startLane;
	Lane* endLane;

	int rowSemaphore;



protected:
	void calculateCurvature();
	void calculateCurveLength();

	Crossing* crossing;
	

	



};

#endif