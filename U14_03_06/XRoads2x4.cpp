//XRoads2x4.cpp
#include "stdafx.h"
#include "Xroads2x4.h"

XRoads2x4::XRoads2x4(vector<Lane*> la, int ind, NxVec3* rec){
	type=Crossing::CROSSING ;
	index=ind;
	rectangle=rec;

	lanesSize=12; //WICHTIG

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
	la[8]->startCrossing=this;
	la[9]->startCrossing=this;
	la[10]->endCrossing=this;
	la[11]->endCrossing=this;

	connectFollowUps();

	buildCorridors();

}
char* XRoads2x4::getStringIdentifier(){
	char* buffer;
	buffer= new char[100];
	itoa(index,buffer,10);
	strcat(buffer,"-XRoads2x4:");
	return buffer;
}

NxVec3* XRoads2x4::getRectangle(){
	return this->rectangle;
}

void XRoads2x4::printCrossing(){
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


void XRoads2x4::connectFollowUps(){
	//lanes mit nachfolger verbinden
	lanes[1]->followers.push_back(lanes[2]); //cout << "connected Lane "<<lanes[1]->utsindex<<" with Lane " << lanes[2]->utsindex<<"\n";
	lanes[1]->followers.push_back(lanes[6]); 
	lanes[1]->followers.push_back(lanes[9]); 
	lanes[4]->followers.push_back(lanes[9]); 
	lanes[4]->followers.push_back(lanes[0]); 
	lanes[5]->followers.push_back(lanes[6]);
	lanes[5]->followers.push_back(lanes[8]); 
	lanes[7]->followers.push_back(lanes[8]); 
	lanes[7]->followers.push_back(lanes[0]); 
	lanes[7]->followers.push_back(lanes[3]); 
	lanes[10]->followers.push_back(lanes[3]); 
	lanes[10]->followers.push_back(lanes[6]); 
	lanes[11]->followers.push_back(lanes[0]); 
	lanes[11]->followers.push_back(lanes[2]); 
}
void XRoads2x4::buildCorridors(){
	//corridore verbinden
	corridors.push_back(Corridor(lanes[1],lanes[2],this));
	corridors.push_back(Corridor(lanes[1],lanes[6],this));
	corridors.push_back(Corridor(lanes[1],lanes[9],this));
	corridors.push_back(Corridor(lanes[4],lanes[9],this));
	corridors.push_back(Corridor(lanes[4],lanes[0],this));
	corridors.push_back(Corridor(lanes[5],lanes[6],this));
	corridors.push_back(Corridor(lanes[5],lanes[8],this));
	corridors.push_back(Corridor(lanes[7],lanes[8],this));
	corridors.push_back(Corridor(lanes[7],lanes[0],this));
	corridors.push_back(Corridor(lanes[7],lanes[3],this));
	corridors.push_back(Corridor(lanes[10],lanes[3],this));
	corridors.push_back(Corridor(lanes[10],lanes[6],this));
	corridors.push_back(Corridor(lanes[11],lanes[0],this));
	corridors.push_back(Corridor(lanes[11],lanes[2],this));

	//vorfahrten eintragen
	vector<Corridor*> rightOfWay;
	

	rightOfWay.push_back(&corridors[13]);
	corridors[0].setRightOfWay(rightOfWay); rightOfWay.clear();

	rightOfWay.push_back(&corridors[3]);
	rightOfWay.push_back(&corridors[4]);
	rightOfWay.push_back(&corridors[5]);
	rightOfWay.push_back(&corridors[6]);
	rightOfWay.push_back(&corridors[10]);
	rightOfWay.push_back(&corridors[11]);
	rightOfWay.push_back(&corridors[13]);
	corridors[1].setRightOfWay(rightOfWay); rightOfWay.clear();


	rightOfWay.push_back(&corridors[3]);
	rightOfWay.push_back(&corridors[4]);
	rightOfWay.push_back(&corridors[8]);
	rightOfWay.push_back(&corridors[10]);
	rightOfWay.push_back(&corridors[11]);
	rightOfWay.push_back(&corridors[13]);
	corridors[2].setRightOfWay(rightOfWay); rightOfWay.clear();

	rightOfWay.push_back(&corridors[10]);
	rightOfWay.push_back(&corridors[12]);
	rightOfWay.push_back(&corridors[13]);
	corridors[4].setRightOfWay(rightOfWay); rightOfWay.clear();
	
	rightOfWay.push_back(&corridors[6]);
	corridors[7].setRightOfWay(rightOfWay); rightOfWay.clear();

	rightOfWay.push_back(&corridors[3]);
	rightOfWay.push_back(&corridors[4]);
	rightOfWay.push_back(&corridors[6]);
	rightOfWay.push_back(&corridors[10]);
	rightOfWay.push_back(&corridors[11]);
	rightOfWay.push_back(&corridors[12]);
	rightOfWay.push_back(&corridors[13]);
	corridors[8].setRightOfWay(rightOfWay); rightOfWay.clear();
	
	rightOfWay.push_back(&corridors[1]);
	rightOfWay.push_back(&corridors[3]);
	rightOfWay.push_back(&corridors[4]);
	rightOfWay.push_back(&corridors[6]);
	rightOfWay.push_back(&corridors[10]);
	rightOfWay.push_back(&corridors[11]);
	corridors[9].setRightOfWay(rightOfWay); rightOfWay.clear();

	rightOfWay.push_back(&corridors[3]);
	rightOfWay.push_back(&corridors[5]);
	rightOfWay.push_back(&corridors[6]);
	corridors[11].setRightOfWay(rightOfWay); rightOfWay.clear();
}
Corridor* XRoads2x4::getCorridor(Lane* startLane, Lane* endLane){
	for (unsigned i=0;i<corridors.size();i++){
		if (corridors[i].getStartLane()==startLane && corridors[i].getEndLane()==endLane)
			return &corridors[i];
	}
	return 0;
}