/*------------------------------------------------------------------------
 *  -- UserCar.h
 *
 *  Declaration File for the UserCar Class
 *  
 *  
 *  Author: Valentin Scheele (valentin@scheeles.de)
 *------------------------------------------------------------------------*/

#ifndef USERCAR_H
#define USERCAR_H

#include "stdafx.h"
#include "RayCastCar/NxAllVehicles.h"

class Lane;
class Crossing;

class UserCar{
public:
	UserCar();
	void init(NxScene* carScene);
	void tick();
	void create(NxVec3& pos);
	void standUp();
	

	 bool frontWheelIsPowered;
	 bool rearWheelIsPowered;

	 bool isSteering;

	 NxReal suspSpring; 
	 NxReal suspDamping; 
	 NxReal suspBias; 
	 NxReal carMass;

	NxReal steeringDelta;
	NxReal maxMotorPower;
	NxReal maxSteeringAngle;

	NxReal motorForce;
	NxReal steeringValue;

	NxSpringDesc wheelSpring;
	NxMaterialDesc raycastWheelMaterial;
	NxMaterialIndex raycastWheelMaterialIndex;

	NxUserContactReport* carContactReport;

	Lane* currentLane;
	Crossing* currentCrossing;

	NxScene* scene;

	NxActor* actor;
	
};
#endif