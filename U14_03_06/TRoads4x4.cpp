//TRoads4x4.cpp
#include "stdafx.h"
#include "TRoads4x4.h"

TRoads4x4::TRoads4x4(vector<Lane*> la, int ind, NxVec3* rec){
	type=Crossing::CROSSING ;
	index=ind;
	rectangle=rec;

	lanesSize=12; //WICHTIG

	lanes= new Lane*[lanesSize];
	for (unsigned i=0; i<lanesSize;i++){
		lanes[i]=la[i];
	}
	

	la[0]->startCrossing=this;
	la[1]->startCrossing=this;
	la[2]->endCrossing=this;
	la[3]->endCrossing=this;
	la[4]->startCrossing=this;
	la[5]->startCrossing=this;
	la[6]->endCrossing=this;
	la[7]->endCrossing=this;	
	la[8]->startCrossing=this;
	la[9]->startCrossing=this;
	la[10]->endCrossing=this;
	la[11]->endCrossing=this;

	connectFollowUps();

	buildCorridors();

}
char* TRoads4x4::getStringIdentifier(){
	char* buffer;
	buffer= new char[100];
	itoa(index,buffer,10);
	strcat(buffer,"-TRoads4x4:");
	return buffer;
}

NxVec3* TRoads4x4::getRectangle(){
	return this->rectangle;
}

void TRoads4x4::printCrossing(){
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


void TRoads4x4::connectFollowUps(){
	//lanes mit nachfolger verbinden
	lanes[2]->followers.push_back(lanes[9]); //cout << "connected Lane "<<lanes[1]->utsindex<<" with Lane " << lanes[2]->utsindex<<"\n";
	lanes[3]->followers.push_back(lanes[4]); 
	lanes[3]->followers.push_back(lanes[8]); 
	lanes[6]->followers.push_back(lanes[1]); 
	lanes[7]->followers.push_back(lanes[8]); 
	lanes[10]->followers.push_back(lanes[1]);
	lanes[10]->followers.push_back(lanes[5]); 
	lanes[11]->followers.push_back(lanes[0]); 

}
void TRoads4x4::buildCorridors(){
	//corridore verbinden
	corridors.push_back(Corridor(lanes[2],lanes[9],this));
	corridors.push_back(Corridor(lanes[3],lanes[4],this));
	corridors.push_back(Corridor(lanes[3],lanes[8],this));
	corridors.push_back(Corridor(lanes[6],lanes[1],this));
	corridors.push_back(Corridor(lanes[7],lanes[8],this));
	corridors.push_back(Corridor(lanes[10],lanes[1],this));
	corridors.push_back(Corridor(lanes[10],lanes[5],this));
	corridors.push_back(Corridor(lanes[11],lanes[0],this));

	//vorfahrten eintragen
	vector<Corridor*> rightOfWay;
	

	rightOfWay.push_back(&corridors[0]);
	rightOfWay.push_back(&corridors[2]);
	rightOfWay.push_back(&corridors[5]);
	corridors[3].setRightOfWay(rightOfWay); rightOfWay.clear();
	
	rightOfWay.push_back(&corridors[2]);
	corridors[4].setRightOfWay(rightOfWay); rightOfWay.clear();

	rightOfWay.push_back(&corridors[0]);
	rightOfWay.push_back(&corridors[2]);
	corridors[6].setRightOfWay(rightOfWay); rightOfWay.clear();
}
Corridor* TRoads4x4::getCorridor(Lane* startLane, Lane* endLane){
	for (unsigned i=0;i<corridors.size();i++){
		if (corridors[i].getStartLane()==startLane && corridors[i].getEndLane()==endLane)
			return &corridors[i];
	}
	return 0;
}