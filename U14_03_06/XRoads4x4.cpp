//XRoads4x4.cpp
#include "stdafx.h"
#include "XRoads4x4.h"

XRoads4x4::XRoads4x4(vector<Lane*> la, int ind, NxVec3* rec){
	type=Crossing::CROSSING ;
	index=ind;
	rectangle=rec;

	lanesSize=16; //WICHTIG

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
	la[12]->startCrossing=this;
	la[13]->startCrossing=this;
	la[14]->endCrossing=this;
	la[15]->endCrossing=this;

	connectFollowUps();

	buildCorridors();

}
char* XRoads4x4::getStringIdentifier(){
	char* buffer;
	buffer= new char[100];
	itoa(index,buffer,10);
	strcat(buffer,"-XRoads4x4:");
	return buffer;
}

NxVec3* XRoads4x4::getRectangle(){
	return this->rectangle;
}

void XRoads4x4::printCrossing(){
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


void XRoads4x4::connectFollowUps(){
	//lanes mit nachfolger verbinden
	lanes[2]->followers.push_back(lanes[9]); //cout << "connected Lane "<<lanes[1]->utsindex<<" with Lane " << lanes[2]->utsindex<<"\n";
	lanes[2]->followers.push_back(lanes[13]); 
	lanes[3]->followers.push_back(lanes[4]); 
	lanes[3]->followers.push_back(lanes[8]); 
	lanes[6]->followers.push_back(lanes[13]); 
	lanes[6]->followers.push_back(lanes[1]);
	lanes[7]->followers.push_back(lanes[8]); 
	lanes[7]->followers.push_back(lanes[12]); 
	lanes[10]->followers.push_back(lanes[1]); 
	lanes[10]->followers.push_back(lanes[5]); 
	lanes[11]->followers.push_back(lanes[12]); 
	lanes[11]->followers.push_back(lanes[0]); 
	lanes[14]->followers.push_back(lanes[5]); 
	lanes[14]->followers.push_back(lanes[9]); 
	lanes[15]->followers.push_back(lanes[0]); 
	lanes[15]->followers.push_back(lanes[4]); 

}
void XRoads4x4::buildCorridors(){
	//corridore verbinden
	corridors.push_back(Corridor(lanes[2],lanes[9],this));
	corridors.push_back(Corridor(lanes[2],lanes[13],this));
	corridors.push_back(Corridor(lanes[3],lanes[4],this));
	corridors.push_back(Corridor(lanes[3],lanes[8],this));
	corridors.push_back(Corridor(lanes[6],lanes[13],this));
	corridors.push_back(Corridor(lanes[6],lanes[1],this));
	corridors.push_back(Corridor(lanes[7],lanes[8],this));
	corridors.push_back(Corridor(lanes[7],lanes[12],this));
	corridors.push_back(Corridor(lanes[10],lanes[1],this));
	corridors.push_back(Corridor(lanes[10],lanes[5],this));
	corridors.push_back(Corridor(lanes[11],lanes[12],this));
	corridors.push_back(Corridor(lanes[11],lanes[0],this));
	corridors.push_back(Corridor(lanes[14],lanes[5],this));
	corridors.push_back(Corridor(lanes[14],lanes[9],this));
	corridors.push_back(Corridor(lanes[15],lanes[0],this));
	corridors.push_back(Corridor(lanes[15],lanes[4],this));

	//vorfahrten eintragen
	vector<Corridor*> rightOfWay;
	

	rightOfWay.push_back(&corridors[8]);
	rightOfWay.push_back(&corridors[11]);
	corridors[1].setRightOfWay(rightOfWay); rightOfWay.clear();
	
	rightOfWay.push_back(&corridors[0]);
	rightOfWay.push_back(&corridors[1]);
	rightOfWay.push_back(&corridors[3]);
	rightOfWay.push_back(&corridors[8]);
	rightOfWay.push_back(&corridors[9]);
	rightOfWay.push_back(&corridors[11]);
	corridors[4].setRightOfWay(rightOfWay); rightOfWay.clear();

	rightOfWay.push_back(&corridors[0]);
	rightOfWay.push_back(&corridors[1]);
	rightOfWay.push_back(&corridors[3]);
	rightOfWay.push_back(&corridors[8]);
	rightOfWay.push_back(&corridors[9]);
	rightOfWay.push_back(&corridors[12]);
	rightOfWay.push_back(&corridors[15]);
	corridors[5].setRightOfWay(rightOfWay); rightOfWay.clear();

	rightOfWay.push_back(&corridors[3]);
	corridors[6].setRightOfWay(rightOfWay); rightOfWay.clear();

	rightOfWay.push_back(&corridors[0]);
	rightOfWay.push_back(&corridors[3]);
	rightOfWay.push_back(&corridors[8]);
	rightOfWay.push_back(&corridors[9]);
	rightOfWay.push_back(&corridors[10]);
	rightOfWay.push_back(&corridors[11]);
	corridors[7].setRightOfWay(rightOfWay); rightOfWay.clear();

	rightOfWay.push_back(&corridors[0]);
	rightOfWay.push_back(&corridors[3]);
	corridors[9].setRightOfWay(rightOfWay); rightOfWay.clear();

	rightOfWay.push_back(&corridors[0]);
	rightOfWay.push_back(&corridors[1]);
	rightOfWay.push_back(&corridors[3]);
	rightOfWay.push_back(&corridors[8]);
	rightOfWay.push_back(&corridors[9]);
	rightOfWay.push_back(&corridors[11]);
	corridors[12].setRightOfWay(rightOfWay); rightOfWay.clear();

	rightOfWay.push_back(&corridors[0]);	
	rightOfWay.push_back(&corridors[1]);
	rightOfWay.push_back(&corridors[4]);
	rightOfWay.push_back(&corridors[7]);
	rightOfWay.push_back(&corridors[8]);
	rightOfWay.push_back(&corridors[9]);
	rightOfWay.push_back(&corridors[11]);
	corridors[13].setRightOfWay(rightOfWay); rightOfWay.clear();

	rightOfWay.push_back(&corridors[11]);
	corridors[14].setRightOfWay(rightOfWay); rightOfWay.clear();

	rightOfWay.push_back(&corridors[0]);
	rightOfWay.push_back(&corridors[1]);
	rightOfWay.push_back(&corridors[2]);
	rightOfWay.push_back(&corridors[3]);
	rightOfWay.push_back(&corridors[8]);
	rightOfWay.push_back(&corridors[11]);
	corridors[15].setRightOfWay(rightOfWay); rightOfWay.clear();
}
Corridor* XRoads4x4::getCorridor(Lane* startLane, Lane* endLane){
	for (unsigned i=0;i<corridors.size();i++){
		if (corridors[i].getStartLane()==startLane && corridors[i].getEndLane()==endLane)
			return &corridors[i];
	}
	return 0;
}