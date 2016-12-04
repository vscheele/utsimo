//UserRaycastCar.h

#ifndef USERRAYCASTCAR_H
#define USERRAYCASTCAR_H


#include "RayCastCar/NxAllVehicles.h"
#include "RayCastCar/NxVehicle.h"

class UserRaycastCar{
public:
	NxVehicle* vehicle;
	Lane* currentLane;
	Crossing* currentCrossing;
};



#endif