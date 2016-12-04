/*------------------------------------------------------------------------
 *  -- UtsNx.cpp
 *
 *  Implementation File for the UtsNx Class
 *  
 *  Author: Valentin Scheele (valentin@scheeles.de)
 *
 *------------------------------------------------------------------------*/
#include "stdafx.h"
#include "UtsNx.h"
UtsNx::UtsNx(){
}

UtsNx::UtsNx(Utsimo* owningUtsimo){
	this->scene=NULL;
	this->sdk=NULL;
	this->userCarSpawned=false;
	defaultGravity = NxVec3(0,-9.8,0);
	utsimo=owningUtsimo;

	userRaycastVehicleSpawned=false;

}

void UtsNx::init(){
		// Create a memory allocator
    allocator = new UserAllocator;

	// Create the physics SDK
	sdk = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, allocator);
    if (!sdk)  return;

	sdk->setParameter(NX_MIN_SEPARATION_FOR_PENALTY, -0.05f);

	// Don't slow down jointed objects
	sdk->setParameter(NX_ADAPTIVE_FORCE, 0.0f);

    // Create the scene
    NxSceneDesc sceneDesc;
    sceneDesc.gravity                      = defaultGravity;
    sceneDesc.broadPhase               = NX_BROADPHASE_COHERENT;
    sceneDesc.collisionDetection    = true;
	sceneDesc.userContactReport		= userCar.carContactReport;
	scene = sdk->createScene(sceneDesc);


	// Create the default material
    NxMaterial* defaultMaterial = scene->getMaterialFromIndex(0); 
    defaultMaterial->setRestitution(0.0);
    defaultMaterial->setStaticFriction(0.8);
    defaultMaterial->setDynamicFriction(0.8);

	//turn on all contact notifications:
	//sdk->setActorGroupPairFlags(0, 0, NX_NOTIFY_ON_TOUCH);

	this->createBottomMeshActor();
	this->createStreetBorderMeshActor();
	this->createCrossingBorderMeshActor();

	NxVec3 urcvehpos(-52,10,-200);
	this->spawnUserRaycastVehicle(urcvehpos);

	


}

void UtsNx::step(long long stepTime){
	

	//NEEDS TO BE IN SECONDS
	float stepInSeconds=(float)stepTime/1000;


	//updateCrashedCars();
	//OLD USERCAR
	//checkUserCarCollision();

	scene->simulate(stepInSeconds);	
	scene->flushStream();
	scene->fetchResults(NX_RIGID_BODY_FINISHED, true);


	if (this->userRaycastVehicleSpawned)
	{
		updateArea();
		checkUserRaycastCarCollision();
		tickUserRaycastVehicle(stepInSeconds);
	}

	checkUserActorCollision();
	checkCrashedCarCollision();





	//OLD USERCAR
	/*if (userCarSpawned){
		userCar.tick();

		if (utsimo->stepCounter%10==0) 
			updateArea();
	}*/
}


void UtsNx::checkUserCarCollision(){
	
	for (unsigned i =0; i< utsimo->numOfLanes; i++){
		
		if ((utsimo->lanes[i].startCrossing==userCar.currentCrossing)
			||(utsimo->lanes[i].endCrossing==userCar.currentCrossing))
		{
			Lane* l=&utsimo->lanes[i];
			for (unsigned h =0 ; h <l->cars.size(); h++){
				if ( !l->cars[h]->isInPhysics
					&& userCar.actor->getGlobalPosition().distanceSquared(l->cars[h]->position) < 50)
				{
					Car* c=l->cars[h];
					c->collisionBox.center = c->middle + c->velocity * utsimo->lastStepLengthInSeconds;
					c->collisionBox.rot = NxMat33(c->getOrientationQuat());
					
					NxBox userCarBox; 
					userCar.actor->getShapes()[0]->isBox()->getWorldOBB( userCarBox );
					userCarBox.center += userCar.actor->getLinearVelocity() * utsimo->lastStepLengthInSeconds;

					if (NxBoxBoxIntersect(c->collisionBox, userCarBox)){

						createCollidingCar(c);
					}
				}//if
			}//for 
		}//if
	}//for
}

void UtsNx::checkUserRaycastCarCollision(){
	
	for (unsigned i =0; i< utsimo->numOfLanes; i++){
		
		if ((utsimo->lanes[i].startCrossing==userRaycastCar.currentCrossing)
			||(utsimo->lanes[i].endCrossing==userRaycastCar.currentCrossing))
		{
			Lane* l=&utsimo->lanes[i];
			for (unsigned h =0 ; h <l->cars.size(); h++){
				if ( !l->cars[h]->isInPhysics
					&& userRaycastCar.vehicle->_bodyActor->getGlobalPosition().distanceSquared(l->cars[h]->position) < 50)
				{
					Car* c=l->cars[h];
					c->collisionBox.center = c->middle + c->velocity * utsimo->lastStepLengthInSeconds;
					c->collisionBox.rot = NxMat33(c->getOrientationQuat());
					
					NxBox userCarBox; 
					userRaycastCar.vehicle->_bodyActor->getShapes()[0]->isBox()->getWorldOBB( userCarBox );
					userCarBox.center += userRaycastCar.vehicle->_bodyActor->getLinearVelocity() * utsimo->lastStepLengthInSeconds;

					if (NxBoxBoxIntersect(c->collisionBox, userCarBox)){

						createCollidingCar(c);
					}
				}//if
			}//for 
		}//if
	}//for
}

void UtsNx::checkCrashedCarCollision(){
	
	for (unsigned i=0; i< crashingCars.size(); i++){

		for (unsigned h=0; h < utsimo->numOfLanes; h++){
			//crashed car is near lane
			if (distanceToSegment(crashingCars[i]->position,
				utsimo->lanes[h].start,
				utsimo->lanes[h].end)
				< Worldparams::MAX_EXPECTED_CRASHEDCAR_TRANSLATION)
			{
				Lane* l= &utsimo->lanes[h];
				for (unsigned j =0 ; j <l->cars.size(); j++){
					if ( !l->cars[j]->isInPhysics
						&& crashingCars[i]->actor->getGlobalPosition().distanceSquared(l->cars[j]->position) < 50)
					{
						Car* c=l->cars[j];
						c->collisionBox.center = c->middle + c->velocity * utsimo->lastStepLengthInSeconds;
						c->collisionBox.rot = NxMat33(c->getOrientationQuat());

						NxBox crashingCarBox; 
						crashingCars[i]->actor->getShapes()[0]->isBox()->getWorldOBB( crashingCarBox );
						crashingCarBox.center += crashingCars[i]->actor->getLinearVelocity() * utsimo->lastStepLengthInSeconds;

						if (NxBoxBoxIntersect(c->collisionBox, crashingCarBox)){

							createCollidingCar(c);
						}
					}//if
				}//for 
			} //if
		} //for
	}//for
}

void UtsNx::checkUserActorCollision(){

	for (unsigned i=0; i< userActors.size(); i++){

		for (unsigned h=0; h < utsimo->numOfLanes; h++){
			//crashed car is near lane
			if (distanceToSegment(userActors[i]->obb.center,
				utsimo->lanes[h].start,
				utsimo->lanes[h].end)
				< Worldparams::MAX_EXPECTED_CRASHEDCAR_TRANSLATION)
			{
				Lane* l= &utsimo->lanes[h];
				for (unsigned j =0 ; j <l->cars.size(); j++){
					if ( !l->cars[j]->isInPhysics
						&& userActors[i]->actor->getGlobalPosition().distanceSquared(l->cars[j]->position) < 50)
					{
						Car* c=l->cars[j];
						c->collisionBox.center = c->middle + c->velocity * utsimo->lastStepLengthInSeconds;
						c->collisionBox.rot = NxMat33(c->getOrientationQuat());

						NxBox userActorFutureBox = userActors[i]->obb; 
		
						userActorFutureBox.center += userActors[i]->actor->getLinearVelocity() * utsimo->lastStepLengthInSeconds;

						if (NxBoxBoxIntersect(c->collisionBox, userActors[i]->obb)){

							createCollidingCar(c);
						}
					}//if
				}//for 
			} //if
		} //for
	}//for
}

void UtsNx::createGroundPlane(){
	 // Create a plane with default descriptor
    NxPlaneShapeDesc planeDesc;
    NxActorDesc actorDesc;
    actorDesc.shapes.pushBack(&planeDesc);
	actorDesc.globalPose.t=NxVec3(0,-100,0);
    groundPlane=scene->createActor(actorDesc);


}
void UtsNx::createBottomMeshActor(){
	
	NxActorDesc actorDesc;

	NxTriangleMeshDesc triangleDesc;
	NxTriangleMeshShapeDesc triangleShapeDesc;

	float* verts = new float[utsimo->numOfCrossings*24 + utsimo->streets.size()*24];
	for (int i=0; i< utsimo->numOfCrossings;i ++){
		
				verts[i*24]		=utsimo->crossings[i]->rectangle[0].x;	//0
				verts[i*24+1]	=utsimo->crossings[i]->rectangle[0].y;	
				verts[i*24+2]	=utsimo->crossings[i]->rectangle[0].z;
				verts[i*24+3]	=utsimo->crossings[i]->rectangle[1].x;	//1
				verts[i*24+4]	=utsimo->crossings[i]->rectangle[1].y;	
				verts[i*24+5]	=utsimo->crossings[i]->rectangle[1].z;
				verts[i*24+6]	=utsimo->crossings[i]->rectangle[2].x;	//2
				verts[i*24+7]	=utsimo->crossings[i]->rectangle[2].y;	
				verts[i*24+8]	=utsimo->crossings[i]->rectangle[2].z;
				verts[i*24+9]	=utsimo->crossings[i]->rectangle[3].x;	//3
				verts[i*24+10]	=utsimo->crossings[i]->rectangle[3].y;	
				verts[i*24+11]	=utsimo->crossings[i]->rectangle[3].z;

				verts[i*24+12]	=utsimo->crossings[i]->rectangle[0].x;	//4
				verts[i*24+13]	=utsimo->crossings[i]->rectangle[0].y+Worldparams::COLLISION_MESH_BORDER_HEIGHT;	
				verts[i*24+14]	=utsimo->crossings[i]->rectangle[0].z;
				verts[i*24+15]	=utsimo->crossings[i]->rectangle[1].x;	//5
				verts[i*24+16]	=utsimo->crossings[i]->rectangle[1].y+Worldparams::COLLISION_MESH_BORDER_HEIGHT;	
				verts[i*24+17]	=utsimo->crossings[i]->rectangle[1].z;
				verts[i*24+18]	=utsimo->crossings[i]->rectangle[2].x;	//6
				verts[i*24+19]	=utsimo->crossings[i]->rectangle[2].y+Worldparams::COLLISION_MESH_BORDER_HEIGHT;	
				verts[i*24+20]	=utsimo->crossings[i]->rectangle[2].z;
				verts[i*24+21]	=utsimo->crossings[i]->rectangle[3].x;	//7
				verts[i*24+22]	=utsimo->crossings[i]->rectangle[3].y+Worldparams::COLLISION_MESH_BORDER_HEIGHT;	
				verts[i*24+23]	=utsimo->crossings[i]->rectangle[3].z;
	}
	for (int i=0; i< utsimo->streets.size();i ++){
		
				verts[utsimo->numOfCrossings*24+i*24]		=utsimo->streets[i].rectangleCrossingPoints[0].x;	//0
				verts[utsimo->numOfCrossings*24+i*24+1]		=utsimo->streets[i].rectangleCrossingPoints[0].y;	
				verts[utsimo->numOfCrossings*24+i*24+2]		=utsimo->streets[i].rectangleCrossingPoints[0].z;
				verts[utsimo->numOfCrossings*24+i*24+3]		=utsimo->streets[i].rectangleCrossingPoints[1].x;	//1
				verts[utsimo->numOfCrossings*24+i*24+4]		=utsimo->streets[i].rectangleCrossingPoints[1].y;	
				verts[utsimo->numOfCrossings*24+i*24+5]		=utsimo->streets[i].rectangleCrossingPoints[1].z;
				verts[utsimo->numOfCrossings*24+i*24+6]		=utsimo->streets[i].rectangleCrossingPoints[2].x;	//2
				verts[utsimo->numOfCrossings*24+i*24+7]		=utsimo->streets[i].rectangleCrossingPoints[2].y;	
				verts[utsimo->numOfCrossings*24+i*24+8]		=utsimo->streets[i].rectangleCrossingPoints[2].z;
				verts[utsimo->numOfCrossings*24+i*24+9]		=utsimo->streets[i].rectangleCrossingPoints[3].x;	//3
				verts[utsimo->numOfCrossings*24+i*24+10]	=utsimo->streets[i].rectangleCrossingPoints[3].y;	
				verts[utsimo->numOfCrossings*24+i*24+11]	=utsimo->streets[i].rectangleCrossingPoints[3].z;

				verts[utsimo->numOfCrossings*24+i*24+12]	=utsimo->streets[i].rectangleCrossingPoints[0].x;	//4
				verts[utsimo->numOfCrossings*24+i*24+13]	=utsimo->streets[i].rectangleCrossingPoints[0].y+Worldparams::COLLISION_MESH_BORDER_HEIGHT;	
				verts[utsimo->numOfCrossings*24+i*24+14]	=utsimo->streets[i].rectangleCrossingPoints[0].z;
				verts[utsimo->numOfCrossings*24+i*24+15]	=utsimo->streets[i].rectangleCrossingPoints[1].x;	//5
				verts[utsimo->numOfCrossings*24+i*24+16]	=utsimo->streets[i].rectangleCrossingPoints[1].y+Worldparams::COLLISION_MESH_BORDER_HEIGHT;	
				verts[utsimo->numOfCrossings*24+i*24+17]	=utsimo->streets[i].rectangleCrossingPoints[1].z;
				verts[utsimo->numOfCrossings*24+i*24+18]	=utsimo->streets[i].rectangleCrossingPoints[2].x;	//6
				verts[utsimo->numOfCrossings*24+i*24+19]	=utsimo->streets[i].rectangleCrossingPoints[2].y+Worldparams::COLLISION_MESH_BORDER_HEIGHT;	
				verts[utsimo->numOfCrossings*24+i*24+20]	=utsimo->streets[i].rectangleCrossingPoints[2].z;
				verts[utsimo->numOfCrossings*24+i*24+21]	=utsimo->streets[i].rectangleCrossingPoints[3].x;	//7
				verts[utsimo->numOfCrossings*24+i*24+22]	=utsimo->streets[i].rectangleCrossingPoints[3].y+Worldparams::COLLISION_MESH_BORDER_HEIGHT;	
				verts[utsimo->numOfCrossings*24+i*24+23]	=utsimo->streets[i].rectangleCrossingPoints[3].z;
	}

	int* inds = new int[utsimo->numOfCrossings*6+utsimo->streets.size()*18];
	
	//define crossing vertices
	for (int i=0; i< utsimo->numOfCrossings;i++){
		inds[i*6]= i*8+2; inds[i*6+1]= i*8+1; inds[i*6+2]= i*8;
		inds[i*6+3]= i*8; inds[i*6+4]= i*8+3; inds[i*6+5]= i*8+2;
	}

	//indices for street vertices
	int streetOffset=utsimo->numOfCrossings*6;

	//define street vertices
	for (int i=0; i< utsimo->streets.size(); i++){
		Crossing* c1 = utsimo->streets[i].borderLane1->startCrossing;
		Crossing* c2 = utsimo->streets[i].borderLane1->endCrossing;
		if ( (c1->type==Crossing::CROSSING) || (c1->type==Crossing::CURVE) ){
			inds[streetOffset+i*6]=c1->index*8 + utsimo->streets[i].rectangleCrossingPointsIndices[0];
			inds[streetOffset+i*6+4]=c1->index*8 + utsimo->streets[i].rectangleCrossingPointsIndices[3];
			inds[streetOffset+i*6+5]=c1->index*8 + utsimo->streets[i].rectangleCrossingPointsIndices[0];
		}else{
			inds[streetOffset+i*6]= utsimo->numOfCrossings*8 + i*8 + 0 ;
			inds[streetOffset+i*6+4]= utsimo->numOfCrossings*8  + i*8 + 3 ;
			inds[streetOffset+i*6+5]= utsimo->numOfCrossings*8  + i*8 + 0 ;
			
		}
		if ( (c2->type==Crossing::CROSSING) || (c2->type==Crossing::CURVE) ){
			inds[streetOffset+i*6+1]=c2->index*8 + utsimo->streets[i].rectangleCrossingPointsIndices[1];
			inds[streetOffset+i*6+2]=c2->index*8 + utsimo->streets[i].rectangleCrossingPointsIndices[2];
			inds[streetOffset+i*6+3]=c2->index*8 + utsimo->streets[i].rectangleCrossingPointsIndices[2];
		}else{
			inds[streetOffset+i*6+1]= utsimo->numOfCrossings*8 + i*8 + 1 ;
			inds[streetOffset+i*6+2]= utsimo->numOfCrossings*8  + i*8 + 2 ;
			inds[streetOffset+i*6+3]= utsimo->numOfCrossings*8 + i*8 + 2 ;
			
		}
	
	}

	triangleDesc.numVertices = utsimo->numOfCrossings*8 + utsimo->streets.size()*8;
	triangleDesc.numTriangles = utsimo->numOfCrossings*2+utsimo->streets.size()*2;
	triangleDesc.pointStrideBytes = sizeof(NxVec3);
	triangleDesc.triangleStrideBytes = 3*sizeof(NxU32);
	triangleDesc.points = verts;
	triangleDesc.triangles = inds;
	triangleDesc.flags = 0;

	NxInitCooking();

	// Cooking from memory
	MemoryWriteBuffer buf;
	bool status = NxCookTriangleMesh(triangleDesc, buf);
	triangleShapeDesc.meshData = sdk->createTriangleMesh(MemoryReadBuffer(buf.data));
	actorDesc.shapes.pushBack(&triangleShapeDesc);



	scene->createActor(actorDesc);

}
		
void UtsNx::createCrossingBorderMeshActor(){

	NxActorDesc actorDesc;

	NxTriangleMeshDesc triangleDesc;
	NxTriangleMeshShapeDesc triangleShapeDesc;

	vector<float> tmpInds;

	float* verts = new float[utsimo->numOfCrossings*24];
	
	for (int i=0; i< utsimo->numOfCrossings;i ++){
		
				verts[i*24]		=utsimo->crossings[i]->rectangle[0].x;	//0
				verts[i*24+1]	=utsimo->crossings[i]->rectangle[0].y;	
				verts[i*24+2]	=utsimo->crossings[i]->rectangle[0].z;
				verts[i*24+3]	=utsimo->crossings[i]->rectangle[1].x;	//1
				verts[i*24+4]	=utsimo->crossings[i]->rectangle[1].y;	
				verts[i*24+5]	=utsimo->crossings[i]->rectangle[1].z;
				verts[i*24+6]	=utsimo->crossings[i]->rectangle[2].x;	//2
				verts[i*24+7]	=utsimo->crossings[i]->rectangle[2].y;	
				verts[i*24+8]	=utsimo->crossings[i]->rectangle[2].z;
				verts[i*24+9]	=utsimo->crossings[i]->rectangle[3].x;	//3
				verts[i*24+10]	=utsimo->crossings[i]->rectangle[3].y;	
				verts[i*24+11]	=utsimo->crossings[i]->rectangle[3].z;

				verts[i*24+12]	=utsimo->crossings[i]->rectangle[0].x;	//4
				verts[i*24+13]	=utsimo->crossings[i]->rectangle[0].y+Worldparams::COLLISION_MESH_BORDER_HEIGHT;	
				verts[i*24+14]	=utsimo->crossings[i]->rectangle[0].z;
				verts[i*24+15]	=utsimo->crossings[i]->rectangle[1].x;	//5
				verts[i*24+16]	=utsimo->crossings[i]->rectangle[1].y+Worldparams::COLLISION_MESH_BORDER_HEIGHT;	
				verts[i*24+17]	=utsimo->crossings[i]->rectangle[1].z;
				verts[i*24+18]	=utsimo->crossings[i]->rectangle[2].x;	//6
				verts[i*24+19]	=utsimo->crossings[i]->rectangle[2].y+Worldparams::COLLISION_MESH_BORDER_HEIGHT;	
				verts[i*24+20]	=utsimo->crossings[i]->rectangle[2].z;
				verts[i*24+21]	=utsimo->crossings[i]->rectangle[3].x;	//7
				verts[i*24+22]	=utsimo->crossings[i]->rectangle[3].y+Worldparams::COLLISION_MESH_BORDER_HEIGHT;	
				verts[i*24+23]	=utsimo->crossings[i]->rectangle[3].z;

				//Check for each crossing border if it is connected to a lane,
				//if not, add new border to mesh
				for (unsigned h=0; h<4;h++){
					int nexth;
					if (h==3) nexth=0; else nexth=h+1;
					bool usedBorder=false;
					for (unsigned j=0; j< utsimo->crossings[i]->corridors.size(); j++){
						NxVec3 start=utsimo->crossings[i]->corridors[j].startLane->end;
						NxVec3 end=utsimo->crossings[i]->corridors[j].endLane->start;
						float test1=distanceToSegment(start,utsimo->crossings[i]->rectangle[h],utsimo->crossings[i]->rectangle[nexth]);
						float test2=distanceToSegment(end,utsimo->crossings[i]->rectangle[h],utsimo->crossings[i]->rectangle[nexth]);
						if ( (distanceToSegment(start,utsimo->crossings[i]->rectangle[h],utsimo->crossings[i]->rectangle[nexth]) < 1.5)
							||(distanceToSegment(end,utsimo->crossings[i]->rectangle[h],utsimo->crossings[i]->rectangle[nexth]) < 1.5))
							usedBorder=true;
					}
					if (!usedBorder){

						tmpInds.push_back(i*8+nexth+4); 	tmpInds.push_back(i*8+h+4);		tmpInds.push_back(i*8+h);
						tmpInds.push_back(i*8+h);			tmpInds.push_back(i*8+nexth);	tmpInds.push_back(i*8+nexth+4);
					}
				}
	}

	int* inds = new int[tmpInds.size()];
	for (unsigned i =0; i< tmpInds.size(); i++) {
		inds[i]=tmpInds[i];
	}


	triangleDesc.numVertices = utsimo->numOfCrossings*8;
	triangleDesc.numTriangles = tmpInds.size()/3;
	triangleDesc.pointStrideBytes = sizeof(NxVec3);
	triangleDesc.triangleStrideBytes = 3*sizeof(NxU32);
	triangleDesc.points = verts;
	triangleDesc.triangles = inds;
	triangleDesc.flags = 0;

	//only cook if triangles exist
	if (triangleDesc.numTriangles>0){
		NxInitCooking();

		// Cooking from memory
		MemoryWriteBuffer buf;
		bool status = NxCookTriangleMesh(triangleDesc, buf);
		triangleShapeDesc.meshData = sdk->createTriangleMesh(MemoryReadBuffer(buf.data));
		actorDesc.shapes.pushBack(&triangleShapeDesc);

		scene->createActor(actorDesc);
	}

}

void UtsNx::createStreetBorderMeshActor(){

	NxActorDesc actorDesc;
	//NxConvexMeshDesc* convexDesc= new NxConvexMeshDesc[utsimo->streets.size()];
	//NxConvexShapeDesc* convexShapeDesc= new NxConvexShapeDesc[utsimo->streets.size()];
	NxTriangleMeshDesc* triangleDesc= new NxTriangleMeshDesc[utsimo->streets.size()];
	NxTriangleMeshShapeDesc* triangleShapeDesc= new NxTriangleMeshShapeDesc[utsimo->streets.size()];

	NxBoxShapeDesc* sideBoxDesc1= new NxBoxShapeDesc[utsimo->streets.size()];
	NxBoxShapeDesc* sideBoxDesc2= new NxBoxShapeDesc[utsimo->streets.size()];
	
	for (int i=0; i< utsimo->streets.size();i ++){
	/************************************************************************************Triangle*/
		float verts[24]={
				utsimo->streets[i].rectangleCrossingPoints[0].x,	//0
				utsimo->streets[i].rectangleCrossingPoints[0].y,
				utsimo->streets[i].rectangleCrossingPoints[0].z,
				utsimo->streets[i].rectangleCrossingPoints[1].x,	 //1
				utsimo->streets[i].rectangleCrossingPoints[1].y,
				utsimo->streets[i].rectangleCrossingPoints[1].z,
				utsimo->streets[i].rectangleCrossingPoints[2].x,	//2
				utsimo->streets[i].rectangleCrossingPoints[2].y,
				utsimo->streets[i].rectangleCrossingPoints[2].z,
				utsimo->streets[i].rectangleCrossingPoints[3].x,	//3
				utsimo->streets[i].rectangleCrossingPoints[3].y,
				utsimo->streets[i].rectangleCrossingPoints[3].z,

				utsimo->streets[i].rectangleCrossingPoints[0].x,	//4
				utsimo->streets[i].rectangleCrossingPoints[0].y+Worldparams::COLLISION_MESH_BORDER_HEIGHT,
				utsimo->streets[i].rectangleCrossingPoints[0].z,
				utsimo->streets[i].rectangleCrossingPoints[1].x,	//5
				utsimo->streets[i].rectangleCrossingPoints[1].y+Worldparams::COLLISION_MESH_BORDER_HEIGHT,
				utsimo->streets[i].rectangleCrossingPoints[1].z,
				utsimo->streets[i].rectangleCrossingPoints[2].x,	//6
				utsimo->streets[i].rectangleCrossingPoints[2].y+Worldparams::COLLISION_MESH_BORDER_HEIGHT,
				utsimo->streets[i].rectangleCrossingPoints[2].z,
				utsimo->streets[i].rectangleCrossingPoints[3].x,	//7
				utsimo->streets[i].rectangleCrossingPoints[3].y+Worldparams::COLLISION_MESH_BORDER_HEIGHT,
				utsimo->streets[i].rectangleCrossingPoints[3].z

		};
		int inds[12]={
			0,4,5,
			5,1,0,
			2,6,7,
			7,3,2
	
		};

		triangleDesc[i].numVertices = 8;
		triangleDesc[i].numTriangles = 4;
		triangleDesc[i].pointStrideBytes = sizeof(NxVec3);
		triangleDesc[i].triangleStrideBytes = 3*sizeof(NxU32);
		triangleDesc[i].points = verts;
		triangleDesc[i].triangles = inds;
		triangleDesc[i].flags = 0;

		NxInitCooking();

		// Cooking from memory
		MemoryWriteBuffer buf;
		bool status = NxCookTriangleMesh(triangleDesc[i], buf);
		triangleShapeDesc[i].meshData = sdk->createTriangleMesh(MemoryReadBuffer(buf.data));
		actorDesc.shapes.pushBack(&triangleShapeDesc[i]);
		/************************************************************************************Triangle*/
	
			
	}

	scene->createActor(actorDesc);
	
}

void UtsNx::spawnUserCar(){

	userCar.init(this->scene);

	userCar.create(NxVec3(184,25,-41));
	this->userCarSpawned=true;
	this->updateArea();
}

void UtsNx::spawnUserRaycastVehicle(NxVec3 position){

	gScene=this->scene;
	createCarWithDesc(position,true,true,false,false,false,this->sdk);

	this->userRaycastCar.vehicle = NxAllVehicles::getActiveVehicle();
	userRaycastVehicleSteering=0;
	userRaycastVehicleAcceleration=0;

	userRaycastVehicleSpawned=true;
}

void UtsNx::tickUserRaycastVehicle(NxReal stepInSeconds){
	//cout <<userRaycastVehicleSteering<<"\t"<<userRaycastVehicleAcceleration<<"\n";
	if (NxAllVehicles::getActiveVehicle())
			NxAllVehicles::getActiveVehicle()->control(userRaycastVehicleSteering, false, userRaycastVehicleAcceleration, false, 0);
	NxAllVehicles::updateAllVehicles(stepInSeconds);
	userRaycastVehicleSteering=0;
	userRaycastVehicleAcceleration=0;

}

void UtsNx::createCollidingCar(Car* c){

	if (c->isInPhysics) cout<<"CAR ALREADY IN SIM";
		
	NxBoxShapeDesc boxShapeDesc;
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	actorDesc.body = &bodyDesc;
	actorDesc.density = 10;

	boxShapeDesc.dimensions.set(c->carType->getLength()/2,c->carType->getHeight()/2,c->carType->getWidth()/2);

	//Turn box so it has the same Orientation TODO VELOCITY RIGHT, OR BETTER USE ORI
	float angle1=180*XZAngle(NxVec3(c->velocity.x,0,c->velocity.z),NxVec3(1,0,0))/PI;
	float angle2=180*AngleToXZ(c->velocity)/PI;

	actorDesc.globalPose.M.fromQuat(NxQuat(angle2,c->velocity.cross(Worldparams::SKY_VECTOR))
		* NxQuat(angle1,NxVec3(0,1,0))); 
	actorDesc.globalPose.t=c->middle;

	actorDesc.shapes.push_back(&boxShapeDesc);
	
	c->actor=scene->createActor(actorDesc);
	c->actor->setLinearVelocity(c->velocity);
	c->isInPhysics=true;
	c->offset=0;

	this->crashingCars.push_back(c);
}



void UtsNx::removeCar(Car* c){
	scene->releaseActor(*c->actor);
	c->isInPhysics=false;
	c->actor=0;

	for (unsigned i=0; i<crashingCars.size(); i++){
		if (c==crashingCars[i]){
			crashingCars.erase(crashingCars.begin() + i);
		}
	}

}

void UtsNx::updateArea(){
	//insert nearest lane
	//////////////////////////////////////OLD USERCAR
	/*float lowestDistance=10000;
	for (int i=0; i<utsimo->numOfLanes; i++){
		float thisDistance=distanceToSegment(userCar.actor->getGlobalPosition(), utsimo->lanes[i].start, utsimo->lanes[i].end);
		if (thisDistance<lowestDistance){
			userCar.currentLane=&utsimo->lanes[i];
			lowestDistance=thisDistance;
		}
	}
	//nearest Crossing is active
	if (userCar.currentLane->start.distanceSquared(userCar.actor->getGlobalPosition())<
		userCar.currentLane->end.distanceSquared(userCar.actor->getGlobalPosition()))
		userCar.currentCrossing=userCar.currentLane->startCrossing;
	else 
		userCar.currentCrossing=userCar.currentLane->endCrossing;
	//////////////////////////////////////OLD USERCAR*/
	//NEW RAYCASTCAR
	float lowestDistance=10000;
	for (int i=0; i<utsimo->numOfLanes; i++){
		float thisDistance=distanceToSegment(userRaycastCar.vehicle->_bodyActor->getGlobalPosition(), utsimo->lanes[i].start, utsimo->lanes[i].end);
		if (thisDistance<lowestDistance){
			userRaycastCar.currentLane=&utsimo->lanes[i];
			lowestDistance=thisDistance;
		}
	}
	//nearest Crossing is active
	if (userRaycastCar.currentLane->start.distanceSquared(userRaycastCar.vehicle->_bodyActor->getGlobalPosition())<
		userRaycastCar.currentLane->end.distanceSquared(userRaycastCar.vehicle->_bodyActor->getGlobalPosition()))
		userRaycastCar.currentCrossing=userRaycastCar.currentLane->startCrossing;
	else 
		userRaycastCar.currentCrossing=userRaycastCar.currentLane->endCrossing;

}


void UtsNx::addUserActor(UserActor* uActor){
	this->userActors.push_back (uActor);
	uActor->actor=this->scene->createActor(uActor->actorDesc);
}

void UtsNx::removeUserActor(UserActor* uActor){
	scene->releaseActor(*uActor->actor);
	for (unsigned i=0; i<userActors.size(); i++){
		if (uActor==userActors[i]){
			userActors.erase(userActors.begin() + i);
		}
	}

}



///////////TRASH




		/************************************************************************************CONVEX
		// Compute hull
		NxVec3 verts[8] = 
		{	
				utsimo->streets[i].rectangleCrossingPoints[0],
				utsimo->streets[i].rectangleCrossingPoints[1],
				utsimo->streets[i].rectangleCrossingPoints[2],
				utsimo->streets[i].rectangleCrossingPoints[3],
				utsimo->streets[i].rectangleCrossingPoints[0]+NxVec3(0,-5,0),
				utsimo->streets[i].rectangleCrossingPoints[1]+NxVec3(0,-5,0),
				utsimo->streets[i].rectangleCrossingPoints[2]+NxVec3(0,-5,0),
				utsimo->streets[i].rectangleCrossingPoints[3]+NxVec3(0,-5,0)	
		};

		
		// Create descriptor for convex mesh

		convexDesc[i].numVertices			= 8;
		convexDesc[i].pointStrideBytes		= sizeof(NxVec3);
		convexDesc[i].points				= verts;
		convexDesc[i].flags				= NX_CF_COMPUTE_CONVEX;


		convexShapeDesc[i].localPose.t		= NxVec3(0,0,0);

		NxInitCooking();

		// Cooking from memory
		MemoryWriteBuffer buf;
		bool status = NxCookConvexMesh(convexDesc[i], buf);
		convexShapeDesc[i].meshData = sdk->createConvexMesh(MemoryReadBuffer(buf.data));
		actorDesc.shapes.pushBack(&convexShapeDesc[i]);
		/************************************************************************************CONVEX*/


		/*/close sides with boxes//////////////////////////////////////////////////
		
		NxVec3 leftborder=utsimo->streets[i].rectangleCrossingPoints[1]-utsimo->streets[i].rectangleCrossingPoints[0];
		NxVec3 rightborder=utsimo->streets[i].rectangleCrossingPoints[3]-utsimo->streets[i].rectangleCrossingPoints[2];
		sideBoxDesc1[i].dimensions.set(leftborder.magnitude()/2,5,0.5);
		sideBoxDesc1[i].localPose.t = utsimo->streets[i].rectangleCrossingPoints[0]+leftborder/2;
		float angle1=180*XZAngle(NxVec3(leftborder.x,0,leftborder.z),NxVec3(1,0,0))/PI;
		float angle1b=180*AngleToXZ(leftborder)/PI;
		sideBoxDesc1[i].localPose.M.fromQuat(NxQuat(angle1b,leftborder.cross(Worldparams::SKY_VECTOR))
											* NxQuat(angle1,NxVec3(0,1,0)));
		actorDesc.shapes.pushBack(&sideBoxDesc1[i]);

		sideBoxDesc2[i].dimensions.set(rightborder.magnitude()/2,5,0.5);
		sideBoxDesc2[i].localPose.t = utsimo->streets[i].rectangleCrossingPoints[2]+rightborder/2;
		float angle2=180*XZAngle(NxVec3(rightborder.x,0,rightborder.z),NxVec3(1,0,0))/PI;
		float angle2b=180*AngleToXZ(rightborder)/PI;
		sideBoxDesc2[i].localPose.M.fromQuat(NxQuat(angle2b,rightborder.cross(Worldparams::SKY_VECTOR))
											*NxQuat(angle2,NxVec3(0,1,0)));
		actorDesc.shapes.pushBack(&sideBoxDesc2[i]);*/

	/*


	//THE SAME WITH CROSSINGS!
	//NxActorDesc crossingsActorDesc;
	NxTriangleMeshDesc* crossingsTriangleDesc= new NxTriangleMeshDesc[utsimo->numOfCrossings ];
	NxTriangleMeshShapeDesc* crossingsTriangleShapeDesc= new NxTriangleMeshShapeDesc[utsimo->numOfCrossings ];

	for (int i=0; i< utsimo->numOfCrossings;i ++){
		
		float verts[12]={
				utsimo->crossings[i]->rectangle[0].x,
				utsimo->crossings[i]->rectangle[0].y,
				utsimo->crossings[i]->rectangle[0].z,
				utsimo->crossings[i]->rectangle[1].x,
				utsimo->crossings[i]->rectangle[1].y,
				utsimo->crossings[i]->rectangle[1].z,
				utsimo->crossings[i]->rectangle[2].x,
				utsimo->crossings[i]->rectangle[2].y,
				utsimo->crossings[i]->rectangle[2].z,
				utsimo->crossings[i]->rectangle[3].x,
				utsimo->crossings[i]->rectangle[3].y,
				utsimo->crossings[i]->rectangle[3].z,		
		};
		int inds[6]={
			2,1,0,
			0,3,2
		};

		crossingsTriangleDesc[i].numVertices = 4;
		crossingsTriangleDesc[i].numTriangles = 2;
		crossingsTriangleDesc[i].pointStrideBytes = sizeof(NxVec3);
		crossingsTriangleDesc[i].triangleStrideBytes = 3*sizeof(NxU32);
		crossingsTriangleDesc[i].points = verts;
		crossingsTriangleDesc[i].triangles = inds;
		crossingsTriangleDesc[i].flags = 0;

		NxInitCooking();

		// Cooking from memory
		MemoryWriteBuffer buf;
		bool status = NxCookTriangleMesh(crossingsTriangleDesc[i], buf);
		crossingsTriangleShapeDesc[i].meshData = sdk->createTriangleMesh(MemoryReadBuffer(buf.data));
		actorDesc.shapes.pushBack(&crossingsTriangleShapeDesc[i]);
		
		///////////////Close Crossing Borders without lanes

	
			for (unsigned h=0; h<3;h++){
				bool usedBorder=false;
				for (unsigned j=0; j< utsimo->crossings[i]->corridors.size(); j++){
					NxVec3 start=utsimo->crossings[i]->corridors[j].startLane->end;
					NxVec3 end=utsimo->crossings[i]->corridors[j].endLane->start;
					if ( (distanceToSegment(start,utsimo->crossings[i]->rectangle[h],utsimo->crossings[i]->rectangle[h+1]) < 2)
						||(distanceToSegment(end,utsimo->crossings[i]->rectangle[h],utsimo->crossings[i]->rectangle[h+1]) < 2))
						usedBorder=true;
				}
				if (!usedBorder){


				}
			}

	}
	*/