//Sink.h
#ifndef SINK_H
#define SINK_H

#include "stdafx.h"
//#include "Utsimo.h"
#include "Crossing.h"
//#include "Corridor.h"
#include "Lane.h"

class Lane;
class Corridor;


class Sink : public Crossing {
public:
	Sink(Lane* la);

	virtual char* getStringIdentifier();
	virtual void connectFollowUps();
	virtual NxVec3* getRectangle(); 
	virtual void printCrossing();
	virtual Corridor* getCorridor(Lane* startLane, Lane* endLane);

	Lane* lane ;

};

#endif