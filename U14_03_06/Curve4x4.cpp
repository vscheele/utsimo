//Curve4x4.cpp
#include "stdafx.h"
#include "Curve4x4.h"

Curve4x4::Curve4x4(vector<Lane*> la, int ind, NxVec3* rec){
	type=Crossing::CURVE;
	index=ind;
	rectangle=rec;

	lanesSize=8; //WICHTIG

	lanes= new Lane*[lanesSize];
	for (unsigned i=0; i<lanesSize;i++){
		lanes[i]=la[i];
	}
	lanesSize=8;

	la[0]->startCrossing=this;
	la[1]->startCrossing=this;
	la[2]->endCrossing=this;
	la[3]->endCrossing=this;
	la[4]->startCrossing=this;
	la[5]->startCrossing=this;
	la[6]->endCrossing=this;
	la[7]->endCrossing=this;
	
	connectFollowUps();

	buildCorridors();

}
char* Curve4x4::getStringIdentifier(){
	char* buffer;
	buffer= new char[100];
	itoa(index,buffer,10);
	strcat(buffer,"-Curve4x4:");
	return buffer;
}

NxVec3* Curve4x4::getRectangle(){
	return this->rectangle;
}

void Curve4x4::printCrossing(){
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


void Curve4x4::connectFollowUps(){
	lanes[2]->followers.push_back(lanes[5]);
	lanes[3]->followers.push_back(lanes[4]);//cout << "connected Lane "<<lanes[1]->utsindex<<" with Lane " << lanes[2]->utsindex<<"\n";
	lanes[6]->followers.push_back(lanes[1]);
	lanes[7]->followers.push_back(lanes[0]);
}
void Curve4x4::buildCorridors(){

	corridors.push_back(Corridor(lanes[2],lanes[5],this));
	corridors.push_back(Corridor(lanes[3],lanes[4],this));
	corridors.push_back(Corridor(lanes[6],lanes[1],this));
	corridors.push_back(Corridor(lanes[7],lanes[0],this));

}
Corridor* Curve4x4::getCorridor(Lane* startLane, Lane* endLane){
	for (unsigned i=0;i<corridors.size();i++){
		if (corridors[i].getStartLane()==startLane && corridors[i].getEndLane()==endLane)
			return &corridors[i];
	}
	return 0;
}