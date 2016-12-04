// Starter.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include "Utsimo.h"


 int main(){

	Utsimo uts;
	uts.initAll("world2");

	NxSphereShapeDesc kugelshape;
	kugelshape.radius=1;
	NxActorDesc actorDesc;
	actorDesc.shapes.push_back(&kugelshape);
	actorDesc.globalPose.t=NxVec3(-20,2,0);
	NxBodyDesc bodyDesc;
	actorDesc.body = &bodyDesc;
	actorDesc.density = 1000;
	UserActor kugel(actorDesc);
	kugel.obb.center=actorDesc.globalPose.t;
	kugel.obb.extents=NxVec3(1,1,1);
	uts.utsNx->addUserActor(&kugel);

	while(1){
		NxBoxShapeDesc boxShapeDesc;
		kugel.obb.center=kugel.actor->getGlobalPosition();
		if (!(rand()%10))
		kugel.actor->addForce(1000*NxVec3(rand()%101-50,0,rand()%101-50),NX_IMPULSE);
		
		uts.step();
	}


	return 1;
}
