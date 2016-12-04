//UserCar.cpp
#include "stdafx.h"
#include "UserCar.h"



struct CarWheelContact
{
	NxActor* car;
	NxShape* wheel;
	NxVec3 contactPoint;
	NxVec3 contactNormalForce;
	NxVec3 contactFrictionForce;
};

NxArray<CarWheelContact> wheelContactPoints;

struct WheelShapeUserData
{
	bool frontWheel;
} wheelUserDatas[4];

struct CarData
{
	NxReal _;	//add stuff later maybe.
} carData;

bool isCar(NxActor * actor)
{
	return ((&carData) == (CarData *)(actor->userData));
}

class ContactReport : public NxUserContactReport
{
	public:
	virtual void  onContactNotify(NxContactPair& pair, NxU32 events)
	{
		NxI32 carIndex = -1;
		NxAllVehicles::handlePair(pair, events);
	}

//OLD
	/*virtual void  onContactNotify(NxContactPair& pair, NxU32 events)
	{
		NxU32 carIndex = 0;
		if (isCar(pair.actors[0]))
			carIndex = 0;
		else if (isCar(pair.actors[1]))
			carIndex = 1;
		else
			return;
		//ignore the 'both are cars' case for now.

		// Iterate through contact points
		NxContactStreamIterator i(pair.stream);
		//user can call getNumPairs() here
		while (i.goNextPair())
        {
			//user can also call getShape() and getNumPatches() here
			NxShape * s = i.getShape(carIndex);
			while (i.goNextPatch())
            {
				//user can also call getPatchNormal() and getNumPoints() here
				const NxVec3& contactNormal = i.getPatchNormal();
				while (i.goNextPoint())
                {
					//user can also call getPoint() and getSeparation() here
					const NxVec3& contactPoint = i.getPoint();

					NxVec3 contactNormalForce = pair.sumNormalForce;
					NxVec3 contactFrictionForce = pair.sumFrictionForce;

					//add forces:

					//assuming front wheel drive we need to apply a force at the wheels.
					if (s->is(NX_SHAPE_CAPSULE))		//assuming only the wheels of the car are capsules, otherwise we need more checks.
														//this branch can't be pulled out of loops because we have to do a full iteration through the stream
                    {
						CarWheelContact cwc;
						cwc.car = pair.actors[carIndex];
						cwc.wheel = s;
						cwc.contactPoint = contactPoint;
						cwc.contactNormalForce = contactNormalForce;
						cwc.contactFrictionForce = contactFrictionForce;
						wheelContactPoints.pushBack(cwc);
						//#error too bad this is illegal (reentry) and also technically busted because the accumulators get zeroed after this returns.
						//pair.actors[carIndex]->addForceAtPos(NxVec3(100,0,0),contactPoint);
                    }
                }
            }		
        }
		*/
	
} carContactReportObj;



///////////////////////////////////////////////////////////////////////////////////////////
//		USERCAR MEMBER FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////

UserCar::UserCar(){

	frontWheelIsPowered = true;
	rearWheelIsPowered = false;

	isSteering= false ;

	suspSpring = 70; 
	suspDamping = 55; 
	suspBias = -0.25; 
	carMass = 80;

	steeringDelta = 0.25;
	maxMotorPower = 240;
	maxSteeringAngle = NxMath::degToRad(45.0f);

	steeringValue = 0;

	carContactReport = &carContactReportObj;
	motorForce=0;
}

void UserCar::init(NxScene* carScene){
	scene=carScene;

	wheelSpring.spring =  suspSpring;
	wheelSpring.damper =  suspDamping;
	wheelSpring.targetValue = suspBias;

	raycastWheelMaterial.restitution		= 0.0;
	raycastWheelMaterial.dynamicFriction	= 0.8;
	raycastWheelMaterial.staticFriction		= 2.0;
	raycastWheelMaterial.staticFrictionV	= 0.5;
	raycastWheelMaterial.dynamicFrictionV	= 0.5;
	raycastWheelMaterial.dirOfAnisotropy.set(1, 0, 0);
	raycastWheelMaterial.frictionCombineMode = NX_CM_MULTIPLY;
	raycastWheelMaterial.flags			|=(NX_MF_SPRING_CONTACT | NX_MF_ANISOTROPIC);
	//raycastWheelMaterial.programData	= &wheelSpring;
	//raycastWheelMaterial.programDataSize = sizeof(wheelSpring);
	raycastWheelMaterialIndex = scene->createMaterial(raycastWheelMaterial)->getMaterialIndex();
}

void UserCar::tick(){
	NxReal steeringAngle = steeringValue * maxSteeringAngle;

	NxArray<CarWheelContact>::iterator i = wheelContactPoints.begin();
	while(i != wheelContactPoints.end())
	{
		CarWheelContact& cwc = *i;

		WheelShapeUserData* wheelData = (WheelShapeUserData *)(cwc.wheel->userData);

		//apply to powered wheels only.
		if (wheelData->frontWheel)
	    {
			//steering:
			NxMat33 wheelOrientation = cwc.wheel->getLocalOrientation();
			wheelOrientation.setColumn(0,  NxVec3(NxMath::cos(steeringAngle), 0,  NxMath::sin(steeringAngle) ));
			wheelOrientation.setColumn(2,  NxVec3(NxMath::sin(steeringAngle), 0, -NxMath::cos(steeringAngle) ));
			cwc.wheel->setLocalOrientation(wheelOrientation);

			if (frontWheelIsPowered)
			{
				//get the world space orientation:
				wheelOrientation = cwc.wheel->getGlobalOrientation();
				NxVec3 steeringDirection;
				wheelOrientation.getColumn(0, steeringDirection);

				//the power direction of the front wheel is the wheel's axis as it is steered.
				if (motorForce)
				{
					cwc.car->addForceAtPos(steeringDirection * motorForce,cwc.contactPoint);
				}
				cwc.car->addForceAtPos(steeringDirection * this->actor->getLinearVelocity().magnitude(),cwc.contactPoint);
			}
		}
		if (!wheelData->frontWheel && rearWheelIsPowered)
		{
			//get the orientation of this car:
			NxMat33 m = cwc.car->getGlobalOrientation();
			NxVec3 carForwardAxis;
			m.getColumn(0, carForwardAxis);
			//the power direction of the rear wheel is always the car's length axis.
			cwc.car->addForceAtPos(carForwardAxis * motorForce,cwc.contactPoint);
		}
		i++;
	}

	wheelContactPoints.clear();
}

void UserCar::create(NxVec3& pos){
	// Create body
	NxBodyDesc bodyDesc;
	NxBoxShapeDesc boxDesc;
	NxCapsuleShapeDesc capsuleDesc[4];
	NxActorDesc actorDesc;

	bodyDesc.mass = carMass;
	//bodyDesc.massLocalPose.t-=NxVec3(0,0,0);

	const NxReal height = 0.3;
	const NxReal width = 2;
	const NxReal length = 4;

	boxDesc.dimensions.set(length*0.65, height*0.85, width*0.65);
	actorDesc.shapes.pushBack(&boxDesc);

	capsuleDesc[0].radius = 0.1;
	capsuleDesc[0].height = 1;
	capsuleDesc[0].flags = NX_SWEPT_SHAPE;
	capsuleDesc[0].localPose.M.setColumn(1, NxVec3(0,-1, 0));
	capsuleDesc[0].localPose.M.setColumn(2, NxVec3(0, 0,-1));	//rotate 180 degrees around x axis to cast downward!
	capsuleDesc[0].localPose.t.set(length*0.5, -height, width*0.5);
	capsuleDesc[0].materialIndex = raycastWheelMaterialIndex;
	capsuleDesc[0].userData = &(wheelUserDatas[0]);
	actorDesc.shapes.pushBack(&capsuleDesc[0]);
	wheelUserDatas[0].frontWheel = true;

	capsuleDesc[1].radius = capsuleDesc[0].radius;
	capsuleDesc[1].height = capsuleDesc[0].height;
	capsuleDesc[1].flags = capsuleDesc[0].flags;
	capsuleDesc[1].localPose.M = capsuleDesc[0].localPose.M;	
	capsuleDesc[1].localPose.t.set(length*0.5, -height, -width*0.5);
	capsuleDesc[1].materialIndex = raycastWheelMaterialIndex;
	capsuleDesc[1].userData = &(wheelUserDatas[1]);
	actorDesc.shapes.pushBack(&capsuleDesc[1]);
	wheelUserDatas[1].frontWheel = true;

	capsuleDesc[2].radius = capsuleDesc[0].radius;
	capsuleDesc[2].height = capsuleDesc[0].height;
	capsuleDesc[2].flags = capsuleDesc[0].flags;
	capsuleDesc[2].localPose.M = capsuleDesc[0].localPose.M;
	capsuleDesc[2].localPose.t.set(-length*0.5, -height, -width*0.5);
	capsuleDesc[2].materialIndex = raycastWheelMaterialIndex;
	capsuleDesc[2].userData = &(wheelUserDatas[2]);
	actorDesc.shapes.pushBack(&capsuleDesc[2]);
	wheelUserDatas[2].frontWheel = false;

	capsuleDesc[3].radius = capsuleDesc[0].radius;
	capsuleDesc[3].height = capsuleDesc[0].height;
	capsuleDesc[3].flags = capsuleDesc[0].flags;
	capsuleDesc[3].localPose.M = capsuleDesc[0].localPose.M;
	capsuleDesc[3].localPose.t.set(-length*0.5, -height, width*0.5);
	capsuleDesc[3].materialIndex = raycastWheelMaterialIndex;
	capsuleDesc[3].userData = &(wheelUserDatas[3]);
	actorDesc.shapes.pushBack(&capsuleDesc[3]);
	wheelUserDatas[3].frontWheel = false;

	actorDesc.body			= &bodyDesc;
	actorDesc.globalPose.t  = pos;
	this->actor = scene->createActor(actorDesc);
	actor->userData = &carData;		//so we can recoginze it in the contact stream
	actor->wakeUp(1e10);			//don't go to sleep.
	
}


void UserCar::standUp(){
	this->actor->setGlobalPose(NxMat34(NxQuat(0, NxVec3(1,0,0)),NxVec3(0,5,0)+this->actor->getGlobalPosition()));
}

