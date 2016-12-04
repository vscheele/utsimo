//Lane.cpp
#include "stdafx.h"
#include "Lane.h"

Lane::Lane(){
	//Pointer werden erst einmal auf 0 gesetzt
	this->startCrossing=0;
	this->endCrossing=0;
	this->street=0;
	registeredCarLengthFromCorridor=0;
;}

Lane::Lane(NxVec3 s,NxVec3 e, int utsi){
	//Pointer werden erst einmal auf 0 gesetzt
	startCrossing=0;
	endCrossing=0;
	street=0;
	registeredCarLengthFromCorridor=0;

	this->start=s;
	this->end=e;
	this->utsindex=utsi;

	this->orthoNorma=(start-end).cross(Worldparams::SKY_VECTOR); this->orthoNorma.normalize();
	this->diffVec=end-start;
	this->diffVecNorma=diffVec; diffVecNorma.normalize();
	this->skyNorma=diffVec.cross(orthoNorma); skyNorma.normalize();


	rectangle[0]=start+orthoNorma*Worldparams::LANE_WIDTH/2;
	rectangle[1]=start-orthoNorma*Worldparams::LANE_WIDTH/2;
	rectangle[2]=end-orthoNorma*Worldparams::LANE_WIDTH/2;
	rectangle[3]=end+orthoNorma*Worldparams::LANE_WIDTH/2;
	
}
void Lane::setPoints(NxVec3 s, NxVec3 e){
	this->start=s;
	this->end=e;
}
void Lane::setIndex(int utsi){
	this->utsindex=utsi;
}

bool Lane::hasPlaceforCarLength(NxReal carLength){
	//Test Lane already full
	if (cars.size() == 0)
		return true;
	else if ( (cars[cars.size()-1]->position-start).magnitude() < 
				(0.25 * cars[cars.size()-1]->carType->getLength() + 0.75*carLength+Worldparams::MINCARDISTANCE)){
		return false;
	}
	return true;

}
void Lane::addCar(Car* c){
	//add car to lane vector
	this->cars.push_back(c);

	//if theres already a car in lane, make it leader, else clear leader
	if (cars.size()>1){
		c->leader=cars[cars.size()-2];
		cars[cars.size()-2]->follower=c;
	} else {
		if (c->leader) c->leader->follower=0;
		c->leader=0;
	}

	//if follower not in preceeding crossing, clear it
	if ((c->follower)&&(c->follower->laneState!=Car::IN_CROSSING)&&(startCrossing->type!=Crossing::CURVE)){
		c->follower->leader=0;
		c->follower=0;
	}
}
void Lane::removeCar(Car* c){
	for (unsigned i=0; i<cars.size(); i++){
		if (c==cars[i]){
			cars.erase(cars.begin() + i);
		}
	}
}
