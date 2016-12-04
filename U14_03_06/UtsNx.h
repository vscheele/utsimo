/*------------------------------------------------------------------------
 *  -- UtsNx.h
 *
 *  Declaration File for the UtsNx Class
 *
 * The UtsNx Singleton is the Physics Part of the UTSiMo System. it 
 * uses a full NovodeX Simulation with dynamically added Actors to
 * simulate the current area of interest, as defined by the "inPhysics"- Flag
 * in both the Lane and the Crossing Object. These are dependent of the
 * pointOfInterest in the Utsimo Singleton
 *  
 *  
 *  Author: Valentin Scheele (valentin@scheeles.de)
 *
 *------------------------------------------------------------------------*/
#ifndef UTSNX_H
#define UTSNX_H

#include "stdafx.h"
#include "UserCar.h"
#include "Utsimo.h"
#include "UserActor.h"
#include "RayCastCar/SampleRaycastcar.h"
#include "RayCastCar/NxAllVehicles.h"
#include "RayCastCar/NxVehicle.h"
#include "UserRaycastCar.h"


class Utsimo;
class Car;


class UtsNx{
public:

		UtsNx();
		UtsNx(Utsimo* owningUtsimo);
		void init();
		void step(long long stepTime);
		void spawnUserCar();
		void tickUserRaycastVehicle(NxReal stepInSeconds);
		
		void UtsNx::createCollidingCar(Car* c);
		void removeCar(Car* c);
		void addUserActor(UserActor* uActor);
		void removeUserActor(UserActor* uActor);

		void checkUserCarCollision();
		void checkUserRaycastCarCollision();
		void checkUserActorCollision();
		void checkCrashedCarCollision();

		vector<UserActor*> userActors;
		vector<Car*> crashingCars;
		
		UserAllocator* allocator;
		NxPhysicsSDK* sdk;
		NxScene* scene;
		NxVec3 defaultGravity;
		UserCar	userCar;
		
		UserRaycastCar userRaycastCar;
		NxReal userRaycastVehicleSteering;
		NxReal userRaycastVehicleAcceleration;
		bool userRaycastVehicleSpawned;

	

		bool userCarSpawned;

		NxActor* groundPlane;

		Utsimo* utsimo;
		
		/*interface**************************************************************/
		void spawnUserRaycastVehicle(NxVec3 position);
		NxScene* getScene(){return scene;}

		/***************************************************************/


private:
		void createGroundPlane();
		void createBottomMeshActor();
		void createStreetBorderMeshActor();
		void createCrossingBorderMeshActor();
		void updateArea();


};

#endif