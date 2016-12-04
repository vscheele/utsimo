/*------------------------------------------------------------------------
 *  -- Car.h
 *
 *  Declaration File for the Car Class.
 *
 *  This contains the declaration for the Car class, which represents
 *  a moving vehicle in the Utsimo traffic system. 
 *  
 *  Author: Valentin Scheele (valentin@scheeles.de)
 * 
 *------------------------------------------------------------------------*/
#ifndef CAR_H
#define CAR_H

//include precompiled headers
#include "stdafx.h"
//include main system header
#include "Utsimo.h"
#include "CarType.h"
#include "Lane.h"
#include "Corridor.h"



//make system known, each vehicle has a pointer to the system it belongs to
class Utsimo;
class Lane;
class Corridor;

//! Represents a moving vehicle.
/*
* The Class represents a moving vehicle in the Utsimo system, with all its 
* necessary properties. The Constructor initializes already most of the 
* parameters, like the lane the car's driving on, a pointer to its CarType,
* and its beginng position and velocity. 
*
* The doStep - method is being called on each simulation step. A car can 
* have different states, each declared in the enumerations LANESTATE
* and SPEEDBEHAVIOR
*
*/
class Car{
public:






	Car(Lane* lane, CarType* cType, NxVec3 pos, NxVec3 velo, Utsimo* uts);

	void doStep(long long stepTime);
	bool sinkTest();
	void setBehaviour();
	void chooseLane();
	void changeToCrossing();
	void beZombie();
	bool currentRightOfWay();
	float getNewAcceleration();
	float getFollowingAcceleration();
	NxVec3 getNewVelocity(float newAcceleration, float stepInSeconds);
	NxVec3 getNewOrientation();


	void reOffset(float stepInSeconds);


	void updateNextLaneBlock();
	void updatePositionInfo();

	void actorDataPositioning();

	void updateOrientationQuat();

	bool collidesWith(Car *c);
	bool isNextLaneFull();


	void collisionTesting();
		void checkUserCarCollision();
		void checkUserActorCollision();
		void checkCrashedCarCollision();

	NxVec3 getCurrentSkyVector();

	bool laneChosen;
	bool isBlockedByNextLane;
	bool isZombie;
	bool isInPhysics;
	float sleepTimer;

	NxActor* actor;
	NxBox collisionBox;

	int ID;
	CarType* carType;
	Lane* currentLane;
	Lane* followingLane;
	Car* leader;
	Car* follower;

	Corridor* corridor;
	NxReal corridorDistanceGone;

	NxVec3 position;
	NxVec3 offsetPosition;
	NxVec3 middle;
	NxVec3 corridorForcedPosition;
	NxVec3 velocity;
	NxVec3 orientation;
	NxQuat orientationQuat;
	NxReal acceleration;
	NxReal offset;
	NxReal offsetVelocity;
	NxReal distanceToLaneEnd;
	NxReal distanceToLaneStart;
	Utsimo* utsimo;

	int laneState;
	int lifestate;


	//////////////////////////////////////////////////////////Interface							
	Car* getLeader(){return leader;}
	Car* getFollower(){return follower;}

	Corridor* getCorridor(){return corridor;}
	Lane* getLane(){return currentLane;}
	NxVec3 getPosition(){return position;}
	NxVec3 getVelocity(){return velocity;}
	NxVec3 getorientation(){return orientation;}
	NxQuat getOrientationQuat();
	
	//////////////////////////////////////////////////////////Interface
	
	/*Car* leader;
	Car* follower;

	Corridor* corridor;
	NxReal corridorDistanceGone;

	NxVec3 position;
	NxVec3 offsetPosition;
	NxVec3 middle;
	NxVec3 corridorForcedPosition;
	NxVec3 velocity;
	NxVec3 orientation;
	NxQuat orientationQuat;
	NxReal acceleration;
	NxReal offset;
	NxReal offsetVelocity;
	NxReal distanceToLaneEnd;
	NxReal distanceToLaneStart;
	Utsimo* utsimo;*/


	enum LANESTATE{IN_LANE, IN_PREPARATION, IN_CROSSING};
	enum LIFESTATE{ALIVE, ZOMBIE, CRASHZOMBIE};
};


#endif