//Xroads2x2.cpp
#include "stdafx.h"
#include "Xroads2x2.h"

XRoads2x2::XRoads2x2(vector<Lane*> la, int ind, NxVec3* rec){
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
	la[3]->endCrossing=this;
	la[4]->startCrossing=this;
	la[5]->endCrossing=this;
	la[6]->startCrossing=this;
	la[7]->endCrossing=this;	
	connectFollowUps();

	buildCorridors();

}
char* XRoads2x2::getStringIdentifier(){
	char* buffer;
	buffer= new char[100];
	itoa(index,buffer,10);
	strcat(buffer,"-XRoads2x2:");
	return buffer;
}

NxVec3* XRoads2x2::getRectangle(){
	return this->rectangle;
}

void XRoads2x2::printCrossing(){
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


void XRoads2x2::connectFollowUps(){
	//lanes mit nachfolger verbinden
	lanes[1]->followers.push_back(lanes[2]); //cout << "connected Lane "<<lanes[1]->utsindex<<" with Lane " << lanes[2]->utsindex<<"\n";
	lanes[1]->followers.push_back(lanes[4]); 
	lanes[1]->followers.push_back(lanes[6]); 
	lanes[3]->followers.push_back(lanes[4]); 
	lanes[3]->followers.push_back(lanes[6]); 
	lanes[3]->followers.push_back(lanes[0]);
	lanes[5]->followers.push_back(lanes[6]); 
	lanes[5]->followers.push_back(lanes[0]); 
	lanes[5]->followers.push_back(lanes[2]); 
	lanes[7]->followers.push_back(lanes[0]); 
	lanes[7]->followers.push_back(lanes[2]); 
	lanes[7]->followers.push_back(lanes[4]); 
}
void XRoads2x2::buildCorridors(){
	//corridore verbinden
	corridors.push_back(Corridor(lanes[1],lanes[2],this));
	corridors.push_back(Corridor(lanes[1],lanes[4],this));
	corridors.push_back(Corridor(lanes[1],lanes[6],this));
	corridors.push_back(Corridor(lanes[3],lanes[4],this));
	corridors.push_back(Corridor(lanes[3],lanes[6],this));
	corridors.push_back(Corridor(lanes[3],lanes[0],this));
	corridors.push_back(Corridor(lanes[5],lanes[6],this));
	corridors.push_back(Corridor(lanes[5],lanes[0],this));
	corridors.push_back(Corridor(lanes[5],lanes[2],this));
	corridors.push_back(Corridor(lanes[7],lanes[0],this));
	corridors.push_back(Corridor(lanes[7],lanes[2],this));
	corridors.push_back(Corridor(lanes[7],lanes[4],this));

	//vorfahrten eintragen
	vector<Corridor*> rightOfWay;
	
	rightOfWay.push_back(&corridors[6]);
	rightOfWay.push_back(&corridors[7]);
	corridors[2].setRightOfWay(rightOfWay); rightOfWay.clear();

	rightOfWay.push_back(&corridors[1]);
	corridors[3].setRightOfWay(rightOfWay); rightOfWay.clear();

	rightOfWay.push_back(&corridors[1]);
	rightOfWay.push_back(&corridors[2]);
	rightOfWay.push_back(&corridors[6]);
	rightOfWay.push_back(&corridors[7]);
	rightOfWay.push_back(&corridors[8]);
	corridors[4].setRightOfWay(rightOfWay); rightOfWay.clear();

	rightOfWay.push_back(&corridors[1]);
	rightOfWay.push_back(&corridors[2]);
	rightOfWay.push_back(&corridors[7]);
	rightOfWay.push_back(&corridors[8]);
	rightOfWay.push_back(&corridors[9]);
	rightOfWay.push_back(&corridors[10]);
	corridors[5].setRightOfWay(rightOfWay); rightOfWay.clear();

	rightOfWay.push_back(&corridors[0]);
	rightOfWay.push_back(&corridors[1]);
	corridors[8].setRightOfWay(rightOfWay); rightOfWay.clear();

	rightOfWay.push_back(&corridors[7]);
	corridors[9].setRightOfWay(rightOfWay); rightOfWay.clear();

	rightOfWay.push_back(&corridors[0]);
	rightOfWay.push_back(&corridors[1]);
	rightOfWay.push_back(&corridors[2]);
	rightOfWay.push_back(&corridors[7]);
	rightOfWay.push_back(&corridors[8]);
	corridors[10].setRightOfWay(rightOfWay); rightOfWay.clear();

	rightOfWay.push_back(&corridors[1]);
	rightOfWay.push_back(&corridors[2]);
	rightOfWay.push_back(&corridors[3]);
	rightOfWay.push_back(&corridors[4]);
	rightOfWay.push_back(&corridors[7]);
	rightOfWay.push_back(&corridors[8]);
	corridors[11].setRightOfWay(rightOfWay); rightOfWay.clear();
}
Corridor* XRoads2x2::getCorridor(Lane* startLane, Lane* endLane){
	for (unsigned i=0;i<corridors.size();i++){
		if (corridors[i].getStartLane()==startLane && corridors[i].getEndLane()==endLane)
			return &corridors[i];
	}
	return 0;
}