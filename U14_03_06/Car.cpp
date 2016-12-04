/*------------------------------------------------------------------------
 *  -- Car.cpp
 *
 *  Implementation File for the Car Class.
 *
 *  This contains the implementation for the Car class, which represents
 *  a moving vehicle in the Utsimo traffic system. 
 *  
 *  Author: Valentin Scheele (valentin@scheeles.de)
 * 
 *------------------------------------------------------------------------*/
#include "stdafx.h"
#include "Car.h"


Car::Car(Lane* lane, CarType* cType, NxVec3 pos, NxVec3 velo, Utsimo* uts){
	this->currentLane=lane;
	this->utsimo=uts;
	this->ID=uts->carIDGenerator.getID();
	this->carType=cType;
	this->position=pos;
	this->velocity=velo;
	this->offset=(2*Worldparams::MAX_CAR_OFFSET*((float)TRAND)/RAND_MAX)-Worldparams::MAX_CAR_OFFSET;
	this->offsetVelocity=0;

	if (lane->cars.size()>0){
		lane->cars[lane->cars.size()-1]->follower=this;
		this->leader=lane->cars[lane->cars.size()-1];
	}else{
		this->leader=0;
	}
	this->follower=0;
	
								/*if ((lane->utsindex==44)&&leader)
									if( (this->leader->position-this->position).magnitude()>50){

										utsimo->artificialTimeStep=1;
										cout<<"leaderid:"<<leader->ID<<" lane.carssize:"<<lane->cars.size()<<" velo:"<<cout<<leader->velocity.magnitude()<<"\n";
										
									}
									if (lane->utsindex==44) {
										cout<<"ID:"<<ID<<"pos:"<< position.x<<" "<<position.z<<" velo:"<<velocity.magnitude();
										if (leader) cout<<" leader:"<<leader->ID;
										cout<<"\n";
									}*/
		



	this->acceleration=0;
	this->laneChosen=false;
	this->isBlockedByNextLane=false;
	this->isZombie=false;

	//initialize: not in physics
	this->isInPhysics=false;
	this->sleepTimer=0;
	
	this->laneState=IN_LANE;
	this->lifestate=ALIVE;


	//build a collision box for eventually use NxBoxBoxIntersect
	collisionBox = NxBox(this->position,(NxVec3(carType->getLength()/2, carType->getHeight()/2, carType->getWidth()/2)),NxMat33());
	
}

float Car::getNewAcceleration(){
	//standard max acceleration
	float newAcc=carType->maxAcceleration;
	if (laneState == IN_LANE){
		//max Acceleration is fine
	}
	else if((distanceToLaneEnd<20) &&(laneState==Car::IN_PREPARATION)){
		float v = velocity.magnitude();
		float v2 = -v*v;
		float distance= currentLane->diffVec.magnitude()-(position-currentLane->start).magnitude()-carType->getLength()/2;
		float dontStayStoppedModifier=distance;
		//free to enter Xroads
		if (currentRightOfWay()&&!isBlockedByNextLane){
			v2+=corridor->curveMaxSpeed*corridor->curveMaxSpeed;
			distance+=carType->getLength()/2;
		//Drive until Xroads reached
		}else{
			v2+=dontStayStoppedModifier*dontStayStoppedModifier;
			
		}
		if(distance<0.1)
			newAcc=carType->minAcceleration;
		else 
			newAcc= v2/(2*distance);
		
	}

	if ((leader)&&((leader->position-position).magnitude()<20)){ 
		float fAcc = getFollowingAcceleration();
		if (fAcc<newAcc)
			newAcc=fAcc;
	}
	
	return newAcc;
}
float Car::getFollowingAcceleration(){
	float newAcc=0;
		float v = velocity.magnitude();
		float v2 = v*v;
		float vf = leader->velocity.magnitude();
		float vf2 = vf*vf;
		float distance=(leader->position-position).magnitude()
						- (leader->carType->getLength()+carType->getLength())/2 
						- Worldparams::MINCARDISTANCE;
		
		if(distance<0.1)
			newAcc=carType->minAcceleration;
		else 
			newAcc= (vf2-v2)/(2*distance);


	return newAcc;
}
NxVec3 Car::getNewVelocity(float newAcceleration, float stepInSeconds){

	NxVec3 newVelo(velocity);

	//Acceleration limits 
	if (newAcceleration>carType->maxAcceleration){
		newAcceleration=carType->maxAcceleration;
	}

	

	if ((laneState==IN_LANE)||(laneState==IN_PREPARATION)){
		float amount=newVelo.magnitude()+newAcceleration*stepInSeconds;
		if (amount<=0)	amount=Worldparams::SLEEPVALUE;
		newVelo.setMagnitude(amount);
	}else if ((laneState==IN_CROSSING)){
		float newMagni=newVelo.magnitude()+newAcceleration*stepInSeconds;
		if ((newMagni>carType->maxSpeed)||(newMagni>corridor->curveMaxSpeed))
			newMagni=min(carType->maxSpeed,corridor->curveMaxSpeed);
		float corridorStepDistance=newMagni/corridor->curveLength;
		
		//new positioning is being forced{
		if (newMagni>Worldparams::SLEEPVALUE){
			corridorForcedPosition=corridor->interpolate( corridorDistanceGone+corridorStepDistance*stepInSeconds );
			corridorForcedPosition.y=corridor->preCalculated[0].y;
			//update gone corridor faction
			corridorDistanceGone+=corridorStepDistance*stepInSeconds;
			newVelo = corridor->getInterpolatedVelocityDirection(corridorDistanceGone-0.000005f);
			newVelo.setMagnitude(newMagni);
		}
		

	}

	//velocity limits
	if (newVelo.magnitude()>carType->maxSpeed)
		newVelo.setMagnitude(carType->maxSpeed);

	return newVelo;
	
}

NxVec3 Car::getNewOrientation(){
		
	if ((distanceToLaneEnd < carType->getLength()*0.75)&&(laneState==Car::IN_PREPARATION)){
		
		NxVec3 iorient(velocity.x,
			currentLane->diffVecNorma.y * velocity.magnitude() * distanceToLaneEnd/(carType->getLength()*0.75f),
			velocity.z);
		iorient.normalize();
		return iorient;
	}else if ((distanceToLaneStart< carType->getLength()*0.25)/*&&(laneState!=Car::IN_CROSSING)*/){
		NxVec3 iorient(velocity.x,
			currentLane->diffVecNorma.y * velocity.magnitude() * distanceToLaneStart/(carType->getLength()*0.25f),
			velocity.z);
		iorient.normalize();
		return iorient;
	}


	//standard normalized velocity
	NxVec3 orient(velocity);

	//in crossing always y=0
	if (laneState==Car::IN_CROSSING){
		orient.y=0;
	}

	orient.normalize();
	return orient;
	
}


void Car::setBehaviour(){


	//START PREP AT RIGHTOFWAYFORESIGHT
	if (	(laneState==Car::IN_LANE) && (currentLane->endCrossing->type!=Crossing::SINK)
			&&( distanceToLaneEnd< max(Worldparams::RIGHTOFWAYFORESIGHT, carType->getLength()))
			/*&& ((position-currentLane->start).magnitude()>0.75*currentLane->diffVec.magnitude())*/
			)
	{
		chooseLane();
		corridor=currentLane->endCrossing->getCorridor(currentLane, followingLane);
		corridor->addPrep();
		laneState=Car::IN_PREPARATION;
	}



	//CHANGE FROM LANE TO CROSSING
	if ((distanceToLaneStart-currentLane->diffVec.magnitude())>-Worldparams::LANECHANGERANGE){
		//must be arriving at sink, die.
		if (laneState==IN_LANE){
			beZombie();
		}else if ((laneState==Car::IN_PREPARATION)/*&&currentRightOfWay()*/){
			changeToCrossing();
																			//if (currentLane->utsindex==44) cout <<"\n"<<ID<< " enterd.";
			this->corridor->addCar(this);

		}
	}
	
	//IF car has already a corrior, target Lane is full and has a standing car, change lane!!!
	if ((this==currentLane->cars[0])&&(laneState==Car::IN_PREPARATION) && this->isBlockedByNextLane){

		this->corridor->removePrep();
		chooseLane();
		corridor=currentLane->endCrossing->getCorridor(currentLane, followingLane);
		corridor->addPrep();
	}

	//CHANGE FROM CROSSING TO LANE
	if (	(laneState==Car::IN_CROSSING)
		&& (corridorDistanceGone>1))
	{
		currentLane->removeCar(this);
		currentLane=followingLane;
		this->currentLane->addCar(this);
		laneChosen=false;

		this->corridor->removeCar(this);
		corridor->removePrep();
		
		laneState=Car::IN_LANE;
		NxReal oldVelo=velocity.magnitude();
		this->updatePositionInfo();
		position=currentLane->start + this->distanceToLaneStart*currentLane->diffVecNorma;
		velocity=currentLane->diffVec;
		velocity.setMagnitude(oldVelo);
	}
}

void Car::changeToCrossing(){
	
	corridorDistanceGone=0;

	laneState=Car::IN_CROSSING;
}


void Car::chooseLane(){
	if (currentLane->followers.size()>0){
		//only choose per random
		followingLane= currentLane->followers[TRAND%currentLane->followers.size()];
		//followingLane=currentLane->followers[ID%2+1];
		/*if ( (ID==3) || (ID==4))
			followingLane= currentLane->followers[0];
		followingLane= currentLane->followers[currentLane->followers.size()-1];
		if (currentLane->utsindex==6){
			followingLane= currentLane->followers[0];
			//cout << corridor->rightOfWay[0]->endLane->utsindex<<"is index\n";
		}*/
	}
	laneChosen=true;
}
void Car::doStep(long long stepTime){
	float stepInSeconds=(float) stepTime/1000;

	//update distances to start and end 
	updatePositionInfo();
	
	
	//current behaviour state
	setBehaviour();


	
	//car in preparation: check if target lane is blocked
	if (laneState==Car::IN_PREPARATION) updateNextLaneBlock();

	//get newAcceleration and velocity
	float newAcc= getNewAcceleration();
	NxVec3 newVelo=getNewVelocity(newAcc, stepInSeconds);

	acceleration=newAcc;

	orientation=getNewOrientation();


	//No strong turns in a step, there MUST be some numeric problem
	if (abs(NxAngle(velocity,newVelo))<1.6)
		velocity=newVelo;

	NxVec3 oldpos(position);

	if (laneState==IN_CROSSING){
		//update forced position from velocity 
		position=this->corridorForcedPosition;
	} else {
		if (newVelo.magnitude()> Worldparams::SLEEPVALUE)
			position+=velocity*stepInSeconds;
	}


    
	//handle crashed Car
	if (this->isInPhysics){
		if (!actor->isSleeping()){
			this->sleepTimer=stepInSeconds;
		}
		else{
			this->sleepTimer+=stepInSeconds;
			if (sleepTimer >= Worldparams::DESPAWNTIME){
				this->lifestate=Car::CRASHZOMBIE;
			}
		}
		//physics knows the middle position
		middle=this->actor->getGlobalPosition();
	}
	else {
		middle=velocity; 
		middle.setMagnitude(carType->getLength()*0.25f); 
		middle+=this->offsetPosition;
		//OLD WRONG VERSION middle+=this->currentLane->skyNorma*carType->height/2;
		middle+=this->getCurrentSkyVector()*carType->getHeight()/2;
	}


	//reoffset!
	if (velocity.magnitude()>5){
		reOffset(stepInSeconds);
	}

	//Physics Data is always right!
	if (this->isInPhysics) actorDataPositioning();
	//keep Quaternion Data right, although car is only in simple simulation
	//else this->updateOrientationQuat();

	


	//test fo Collision with UserCar
	if (!this->isInPhysics){
		//reoffset
		NxVec3 offsetVector =velocity.cross(Worldparams::SKY_VECTOR);
		offsetVector.setMagnitude(offset);
		this->offsetPosition=position+offsetVector;
		
		//collision checking
		//collisionTesting();
	}

	
}
void Car::reOffset(float stepInSeconds){
	
	offsetVelocity+=velocity.magnitude()*Worldparams::OFFSETCHANGERATE*((TRAND%3)-1)*stepInSeconds;
	offset+=offsetVelocity;
	if (offset < -Worldparams::MAX_CAR_OFFSET){
		offset=-Worldparams::MAX_CAR_OFFSET;
		offsetVelocity=0;
	}else if (offset > Worldparams::MAX_CAR_OFFSET){
		offset=Worldparams::MAX_CAR_OFFSET;
		offsetVelocity=0;
	}
	

}
void Car::actorDataPositioning(){
	
	this->orientation=actor->getGlobalOrientationQuat().rot(NxVec3(1,0,0));
	this->orientationQuat=actor->getGlobalOrientationQuat();
	this->velocity=actor->getLinearVelocity();
	this->position=actor->getGlobalPosition() 
				- this->orientation*carType->getLength()*0.25
				//OLD WRONG VERSION - carType->height/2*currentLane->skyNorma;
				- carType->getHeight()/2*this->getCurrentSkyVector();
}
void Car::updateOrientationQuat(){
	float angle1=180*XZAngle(NxVec3(orientation.x,0,orientation.z),NxVec3(1,0,0))/PI;
	float angle2=180*AngleToXZ(orientation)/PI;

	orientationQuat= NxQuat(angle2,orientation.cross(Worldparams::SKY_VECTOR))
		* NxQuat(angle1,NxVec3(0,1,0)); 

}
NxQuat Car::getOrientationQuat(){
	if (!this->isInPhysics) this->updateOrientationQuat();
	return this->orientationQuat;
}

void Car::beZombie(){
	lifestate=Car::ZOMBIE;
}

bool Car::isNextLaneFull(){
	if (!corridor) return false;
	//Test if endlane of corridor already full 
	//if lane is empty drive on
	if ((corridor->endLane->cars.size() == 0) || (corridor->getCrossing()->type==Crossing::CURVE))
		return false;
	
	//if last car in nextlane isnt far enough, stop
	if (corridor->endLane->hasPlaceforCarLength(carType->getLength()
		+Worldparams::MINCARDISTANCE
		+corridor->endLane->registeredCarLengthFromCorridor))
		return false;
	

	//standard:full 
	return true;

}
bool Car::currentRightOfWay(){
	if (laneState==Car::IN_LANE) return true;
	//if theres a crossing/merging corridor in use, ignore rigth of way, dont drive!
	for (unsigned hrow=0; hrow< corridor->hasRightOfWay.size(); hrow++){
		if (corridor->hasRightOfWay[hrow]->cars.size()>0) {
			return false;
		}
	}
	if (corridor->rowSemaphore==0)
		return true;

	for (unsigned rowIndex =0;  rowIndex< corridor->rightOfWay.size(); rowIndex++){
		if (corridor->rightOfWay[rowIndex]->cars.size()>0){	
		return false;
		}else if (corridor->rightOfWay[rowIndex]->carsInPrep>0){
			//complicated blocking
			Car* firstCar= corridor->rightOfWay[rowIndex]->startLane->cars[0];
			bool firstCarIsBlockedByThisLane=false;
			for (unsigned i=0; i< corridor->rightOfWay.size(); i++){
				if ((firstCar->corridor== corridor->rightOfWay[i])
					&& (firstCar->isBlockedByNextLane==false))
					return false;
					
			}		
		}
	}
	return true;
}
void Car::updateNextLaneBlock(){
	//Curve doesnt have next Lane blocked, leader is controlling speed
	if ((corridor->getCrossing()->type==Crossing::CURVE)
		|| (laneState!=Car::IN_PREPARATION)){
		isBlockedByNextLane=false;
		return;
	}

	if (!isBlockedByNextLane){
		if (isNextLaneFull()){
			isBlockedByNextLane=true;
		}
	}else{
		if (corridor->isCrossingFree() && (!isNextLaneFull())){
			isBlockedByNextLane=false;
		}
	}
}
void Car::updatePositionInfo(){
	this->distanceToLaneStart = (position-currentLane->start).magnitude();
	this->distanceToLaneEnd = (position-currentLane->end).magnitude();
	
}


NxVec3 Car::getCurrentSkyVector(){
	if (laneState==Car::IN_CROSSING)
		return Worldparams::SKY_VECTOR;
	else
		return currentLane->skyNorma;
}

void Car::checkUserCarCollision(){
	//preweave per distance
	if (utsimo->utsNx->userCar.actor->getGlobalPosition().distanceSquared(position) 
		< 100/*(carType->getLengthSquared() + carType->getWidthSquared())*/)
	{
		//where are you next step?
		collisionBox.center = this->middle + velocity *  utsimo->lastStepLengthInSeconds;
		collisionBox.rot = NxMat33(this->getOrientationQuat());

		NxBox userCarBox;
		utsimo->utsNx->userCar.actor->getShapes()[0]->isBox()->getWorldOBB( userCarBox );
		//where are you next step?
		userCarBox.center += utsimo->utsNx->userCar.actor->getLinearVelocity() * utsimo->lastStepLengthInSeconds;


		//Exact collision checking
		if (NxBoxBoxIntersect(collisionBox, userCarBox)){
			utsimo->utsNx->createCollidingCar(this);
		}
	}
}

void Car::checkUserActorCollision(){
	for (unsigned i =0; i< utsimo->utsNx->userActors.size(); i++ ){
			
	}
}
void Car::checkCrashedCarCollision(){
}

void Car::collisionTesting(){

	checkUserCarCollision();
	checkUserActorCollision();
	checkCrashedCarCollision();


	/*//////////////////////////////////////////////////////////////////////////////////VERRRRRRY OLD COLLISION CHECKING 
	//all cars in current lane
	for (unsigned i=0;i< currentLane->cars.size();i++){
		//preweaving
		if (this->position.distance(currentLane->cars[i]->position)
			< (this->carType->length + currentLane->cars[i]->carType->length))
			break;
		if ((currentLane->cars[i]!=this) && collidesWith(currentLane->cars[i])){
			cout<<ID <<" collision in same Lane with "<< currentLane->cars[i]->ID<<"\n";
		}
	}
	//if car near crossing, all cars in crossing
	if(laneState!=Car::IN_LANE){
		for (unsigned h=0;h< corridor->getCrossing()->corridors.size();h++){
			for (unsigned j=0;j< corridor->getCrossing()->corridors[h].cars.size();j++){
				Car* c=corridor->getCrossing()->corridors[h].cars[j];
				//preweaving
				if (this->position.distance(c->position)
						< (this->carType->length + c->carType->length))
						break;
				if ((c != this) && collidesWith(c)){
					cout<<ID <<" collision in Crossing with "<< c->ID<<"\n";
					
					
				}
			}
		}
	}
	*/////////////////////////////////////////////////////////////END
}

bool Car::collidesWith(Car* car2){
	
	_RotRect r1, r2;

	r1.ang=NxAngle(velocity,NxVec3(0,0,1));
	r1.C.x=middle.x;
	r1.C.y=middle.z;
	r1.S.x=carType->getWidth()/2;
	r1.S.y=carType->getLength()/2;

	r2.ang=NxAngle(car2->velocity,NxVec3(0,0,1));
	r2.C.x=car2->middle.x;
	r2.C.y=car2->middle.z;
	r2.S.x=car2->carType->getWidth()/2;
	r2.S.y=car2->carType->getLength()/2;

	return (bool)RotRectsCollision(&r1,&r2);

}