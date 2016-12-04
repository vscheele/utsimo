//WorldParams.h
#ifndef WORLDPARAMS_H
#define WORLDPARAMS_H
#include "stdafx.h"



class Worldparams{
public: 
	 static unsigned MAX_NUMBER_OF_CARS;
	 static NxReal MAX_CAR_OFFSET;
	 static NxReal OFFSETCHANGERATE;
	 static NxReal LANE_WIDTH;
	 static NxReal CURVE_TENSION;
	 static NxReal LANECHANGERANGE;
	static unsigned INTERPOLATIONSTEPS;
	static NxReal CURVEMAXSPEED;
	static NxReal RIGHTOFWAYFORESIGHT;
	static NxReal MINCARDISTANCE;
	static NxReal Y_CROSSING_CORRECTION;
	static NxReal SLEEPVALUE;	 
	 static NxVec3 SKY_VECTOR; 
	 static NxReal COLLISION_MESH_BORDER_HEIGHT;
	 static int DEBUGVIEWER;
	  static NxReal DESPAWNTIME;
	  static NxReal MAX_EXPECTED_CRASHEDCAR_TRANSLATION;

	 
	void readParams(string worlddir);
	
};

#endif 