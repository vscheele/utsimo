//Sink.cpp
#include "stdafx.h"
#include "Sink.h"

Sink::Sink(Lane *la){
	type=Crossing::SINK;

	index = -1 ;
	rectangle=new NxVec3[4];
	for (unsigned i=0; i<4;i++){
		rectangle[i]= la->start;
	}
	rectangle[0].add(rectangle[0],NxVec3(-5,0,5));
	rectangle[1].add(rectangle[1],NxVec3(-5,0,-5));
	rectangle[2].add(rectangle[2],NxVec3(5,0,-5));
	rectangle[3].add(rectangle[3],NxVec3(5,0,5));
	
	lane=la;
}

 char* Sink::getStringIdentifier(){
	return ("Sink");
}

 void Sink::connectFollowUps(){
	
}

 NxVec3* Sink::getRectangle(){
	return rectangle;
}
 void Sink::printCrossing(){
	cout<< "== Sink at Lane "<<lane->utsindex<<"\n";
}
Corridor* Sink::getCorridor(Lane* startLane, Lane* endLane){
	return 0;
}