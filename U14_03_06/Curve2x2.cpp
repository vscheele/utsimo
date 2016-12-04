//Curve2x2.cpp
#include "stdafx.h"
#include "Curve2x2.h"

Curve2x2::Curve2x2(vector<Lane*> la, int ind, NxVec3* rec){
	type=Crossing::CURVE;
	index=ind;
	rectangle=rec;

	lanes= new Lane*[4];
	for (unsigned i=0; i<4;i++){
		lanes[i]=la[i];
	}
	lanesSize=4;

	la[0]->startCrossing=this;
	la[1]->endCrossing=this;
	la[2]->startCrossing=this;
	la[3]->endCrossing=this;
	
	connectFollowUps();

	buildCorridors();

}
char* Curve2x2::getStringIdentifier(){
	char* buffer;
	buffer= new char[100];
	itoa(index,buffer,10);
	strcat(buffer,"-Curve2x2:");
	return buffer;
}

NxVec3* Curve2x2::getRectangle(){
	return this->rectangle;
}

void Curve2x2::printCrossing(){
	cout<<"==================\n";
	cout<<getStringIdentifier()<<"\n";
	cout<<"Lanes: ";
	for (unsigned i =0; i<lanesSize; i++){
		cout<<lanes[i]->utsindex<<" ";
	}
	cout<<"\n";
	cout<<"("<<rectangle[0].x<<","<<rectangle[0].y<<","<<rectangle[0].z<<")\t";
	cout<<"("<<rectangle[1].x<<","<<rectangle[1].y<<","<<rectangle[1].z<<")\t";
	cout<<"("<<rectangle[2].x<<","<<rectangle[2].y<<","<<rectangle[2].z<<")\t";
	cout<<"("<<rectangle[3].x<<","<<rectangle[3].y<<","<<rectangle[3].z<<")\t";
	cout<<"\n==================\n";
}


void Curve2x2::connectFollowUps(){
	lanes[1]->followers.push_back(lanes[2]); //cout << "connected Lane "<<lanes[1]->utsindex<<" with Lane " << lanes[2]->utsindex<<"\n";
	lanes[3]->followers.push_back(lanes[0]); 
}
void Curve2x2::buildCorridors(){

	corridors.push_back(Corridor(lanes[3],lanes[0],this));
	corridors.push_back(Corridor(lanes[1],lanes[2],this));

}
Corridor* Curve2x2::getCorridor(Lane* startLane, Lane* endLane){
	for (unsigned i=0;i<corridors.size();i++){
		if (corridors[i].getStartLane()==startLane && corridors[i].getEndLane()==endLane)
			return &corridors[i];
	}
	return 0;
}