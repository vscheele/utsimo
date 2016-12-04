//Troads2x4.cpp
#include "stdafx.h"
#include "Troads2x4.h"

TRoads2x4::TRoads2x4(vector<Lane*> la, int ind, NxVec3* rec){
	type=Crossing::CROSSING ;
	index=ind;
	rectangle=rec;

	lanesSize=8; //WICHTIG

	lanes= new Lane*[lanesSize];
	for (unsigned i=0; i<lanesSize;i++){
		lanes[i]=la[i];
	}
	

	la[0]->startCrossing=this;
	la[1]->endCrossing=this;
	la[2]->startCrossing=this;
	la[3]->startCrossing=this;
	la[4]->endCrossing=this;
	la[5]->endCrossing=this;
	la[6]->startCrossing=this;
	la[7]->endCrossing=this;	
	connectFollowUps();

	buildCorridors();

}
char* TRoads2x4::getStringIdentifier(){
	char* buffer;
	buffer= new char[100];
	itoa(index,buffer,10);
	strcat(buffer,"-TRoads2x4:");
	return buffer;
}

NxVec3* TRoads2x4::getRectangle(){
	return this->rectangle;
}

void TRoads2x4::printCrossing(){
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


void TRoads2x4::connectFollowUps(){
	//lanes mit nachfolger verbinden
	lanes[1]->followers.push_back(lanes[2]); //cout << "connected Lane "<<lanes[1]->utsindex<<" with Lane " << lanes[2]->utsindex<<"\n";
	lanes[1]->followers.push_back(lanes[6]); 
	lanes[4]->followers.push_back(lanes[0]); 
	lanes[5]->followers.push_back(lanes[6]); 
	lanes[7]->followers.push_back(lanes[0]); 
	lanes[7]->followers.push_back(lanes[3]); 
}
void TRoads2x4::buildCorridors(){
	//corridore verbinden
	corridors.push_back(Corridor(lanes[1],lanes[2],this));
	corridors.push_back(Corridor(lanes[1],lanes[6],this));
	corridors.push_back(Corridor(lanes[4],lanes[0],this));
	corridors.push_back(Corridor(lanes[5],lanes[6],this));
	corridors.push_back(Corridor(lanes[7],lanes[0],this));
	corridors.push_back(Corridor(lanes[7],lanes[3],this));


	//vorfahrten eintragen
	vector<Corridor*> rightOfWay;
	
	rightOfWay.push_back(&corridors[2]);
	rightOfWay.push_back(&corridors[3]);
	corridors[1].setRightOfWay(rightOfWay); rightOfWay.clear();

	rightOfWay.push_back(&corridors[2]);
	corridors[4].setRightOfWay(rightOfWay); rightOfWay.clear();

	rightOfWay.push_back(&corridors[1]);
	rightOfWay.push_back(&corridors[2]);
	corridors[5].setRightOfWay(rightOfWay); rightOfWay.clear();

}
Corridor* TRoads2x4::getCorridor(Lane* startLane, Lane* endLane){
	for (unsigned i=0;i<corridors.size();i++){
		if (corridors[i].getStartLane()==startLane && corridors[i].getEndLane()==endLane)
			return &corridors[i];
	}
	return 0;
}