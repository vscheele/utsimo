//Worldparams.cpp

#include "stdafx.h"
#include "Worldparams.h"




void Worldparams::readParams(string worlddir){
	ifstream infile;
	string tempdir=worlddir;
	tempdir.append("/worldparams.txt");
	infile.open(tempdir.c_str(), ios::in);
	string buff;
	getline(infile, buff);
	while (!infile.eof()){
		infile >> buff;
		cout<<buff<<" read\n";
		if (!buff.compare("MAX_NUMBER_OF_CARS"))
			infile>>Worldparams::MAX_NUMBER_OF_CARS;
		else if (!buff.compare("MAX_CAR_OFFSET"))
				infile>>Worldparams::MAX_CAR_OFFSET;
		else if (!buff.compare("LANE_WIDTH"))
				infile>>Worldparams::LANE_WIDTH;	
		else if (!buff.compare("CURVE_TENSION"))
				infile>>Worldparams::CURVE_TENSION;		
		else if (!buff.compare("LANECHANGERANGE"))
				infile>>Worldparams::LANECHANGERANGE;
		else if (!buff.compare("INTERPOLATIONSTEPS"))
				infile>>Worldparams::INTERPOLATIONSTEPS;		
		else if (!buff.compare("CURVEMAXSPEED"))
				infile>>Worldparams::CURVEMAXSPEED;		
		else if (!buff.compare("RIGHTOFWAYFORESIGHT"))
				infile>>Worldparams::RIGHTOFWAYFORESIGHT;		
		else if (!buff.compare("MINCARDISTANCE"))
				infile>>Worldparams::MINCARDISTANCE;		
		else if (!buff.compare("Y_CROSSING_CORRECTION"))
				infile>>Worldparams::Y_CROSSING_CORRECTION;
		else if (!buff.compare("SLEEPVALUE"))
				infile>>Worldparams::SLEEPVALUE;
		else if (!buff.compare("OFFSETCHANGERATE"))
				infile>>Worldparams::OFFSETCHANGERATE;
		else if (!buff.compare("COLLISION_MESH_BORDER_HEIGHT"))
				infile>>Worldparams::COLLISION_MESH_BORDER_HEIGHT;
		else if (!buff.compare("DEBUGVIEWER"))
				infile>>Worldparams::DEBUGVIEWER;
		else if (!buff.compare("DESPAWNTIME"))
				infile>>Worldparams::DESPAWNTIME;
		else if (!buff.compare("MAX_EXPECTED_CRASHEDCAR_TRANSLATION"))
				infile>>Worldparams::MAX_EXPECTED_CRASHEDCAR_TRANSLATION;
		
		getline(infile, buff);

	}
}

unsigned Worldparams::MAX_NUMBER_OF_CARS=0;
NxReal Worldparams::MAX_CAR_OFFSET=0;
NxReal Worldparams::LANE_WIDTH=0;
NxReal Worldparams::CURVE_TENSION=0;
NxReal Worldparams::LANECHANGERANGE=0.1f;
unsigned Worldparams::INTERPOLATIONSTEPS=100;
NxReal Worldparams::CURVEMAXSPEED=10;
NxReal Worldparams::RIGHTOFWAYFORESIGHT=50;
NxReal Worldparams::MINCARDISTANCE=5.0;
NxReal Worldparams::Y_CROSSING_CORRECTION=50.0;
NxReal Worldparams::OFFSETCHANGERATE= 1.0;
NxReal Worldparams::SLEEPVALUE= 0.001;
NxReal Worldparams::COLLISION_MESH_BORDER_HEIGHT=100;
NxVec3 Worldparams::SKY_VECTOR=NxVec3(0,1,0);
int Worldparams::DEBUGVIEWER=1;
NxReal Worldparams::DESPAWNTIME= 10000;
NxReal Worldparams::MAX_EXPECTED_CRASHEDCAR_TRANSLATION=50;
