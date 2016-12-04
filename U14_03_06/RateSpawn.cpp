//RateSpawn.cpp
#include "stdafx.h"
#include "RateSpawn.h"

RateSpawn::RateSpawn(Lane* la, int ind, int cType, NxReal rate){
	this->type=Crossing::RATE_SPAWN;
	index=ind;
	rectangle=new NxVec3[4];
	for (unsigned i=0; i<4;i++){
		rectangle[i]= la->start;
	}
	rectangle[0].add(rectangle[0],NxVec3(-1.5,0,1.5));
	rectangle[1].add(rectangle[1],NxVec3(-1.5,0,-1.5));
	rectangle[2].add(rectangle[2],NxVec3(1.5,0,-1.5));
	rectangle[3].add(rectangle[3],NxVec3(1.5,0,1.5));
	lane=la;
	this->cType=cType;
	this->spawnRate=rate;
	this->lastSpawn=0;
	//la->startCrossing =this;
}

char* RateSpawn::getStringIdentifier(){
	char* buffer;
	buffer= new char[100];
	itoa(index,buffer,10);
	strcat(buffer,"-RateSpawn:");
	return buffer;
}

NxVec3* RateSpawn::getRectangle(){
	return this->rectangle;
}

void RateSpawn::printCrossing(){
	cout<<"*************************\n";
	cout<<getStringIdentifier()<<"\n";
	cout<<"an Lane: ";
	
	cout<<lane->utsindex<<" ";
	
	cout<<"\n";
	cout<<"("<<rectangle[0].x<<","<<rectangle[0].y<<","<<rectangle[0].z<<")\t";
	cout<<"("<<rectangle[1].x<<","<<rectangle[1].y<<","<<rectangle[1].z<<")\t";
	cout<<"("<<rectangle[2].x<<","<<rectangle[2].y<<","<<rectangle[2].z<<")\t";
	cout<<"("<<rectangle[3].x<<","<<rectangle[3].y<<","<<rectangle[3].z<<")\t";
	cout<<"\n*************************\n";
}


void RateSpawn::connectFollowUps(){
	
}
Corridor* RateSpawn::getCorridor(Lane* startLane, Lane* endLane){
	return 0;
}