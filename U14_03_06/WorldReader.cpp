//WorldReader.cpp
#include "stdafx.h"
#include "WorldReader.h"



WorldReader::WorldReader(string world){
	this->worlddir=world;
	
}

Lane* WorldReader::readLanes(int* numOfLanes){
	NxReal test=Worldparams::LANE_WIDTH;
	Lane* lanes;
	vector<Lane> tempvec;
	ifstream infile;
	string tempdir=worlddir;
	tempdir.append("/lanes.txt");
	infile.open(tempdir.c_str(), ios::in);
	string buff;

	getline(infile, buff);
	getline(infile, buff);
	while (!infile.eof()){
		int index;
		infile>>index;
		NxReal sx, sy, sz, ex, ey, ez;
		infile >> sx;
		infile >> sy; 
		infile >> sz;
		infile >> ex;
		infile >> ey;
		infile >> ez;

		getline(infile, buff);
		tempvec.push_back(Lane(NxVec3(sx,sy,sz), NxVec3(ex,ey,ez),index));
	}

	lanes= new Lane[tempvec.size()];
	*numOfLanes=(int)tempvec.size();
	
	for(unsigned i=0;i<tempvec.size(); i++){
		lanes[i]=tempvec[i];
	}
	tempvec.clear();
	return lanes;
}

 Crossing** WorldReader::readCrossings(Lane* lanes, int* numOfCrossings){
	Crossing** crossings;
	vector<Crossing*> tmpCrossings;
	ifstream infile;
	string tempdir=worlddir;
	tempdir.append("/crossings.txt");
	infile.open(tempdir.c_str(), ios::in);
	string buff;
	getline(infile, buff);
	getline(infile, buff);
	string line;
	while (getline(infile, line)){
		istringstream wss(line);
		int index;
		string desc;
		wss>>index;
		wss>>desc;
		NxReal y, x1, z1, x2, z2, x3, z3, x4, z4;
		wss >> y;
		wss >> x1; 
		wss >> z1;
		wss >> x2;
		wss >> z2;
		wss >> x3;
		wss >> z3;
		wss >> x4;
		wss >> z4;

		vector<Lane*> lanesVec;
		
		while (wss >> buff){
			lanesVec.push_back(&lanes[atoi(buff.c_str())]);
		}

		NxVec3* rec=new NxVec3[4];
		rec[0].x=x1; rec[0].y=y; rec[0].z=z1;
		rec[1].x=x2; rec[1].y=y; rec[1].z=z2;
		rec[2].x=x3; rec[2].y=y; rec[2].z=z3;
		rec[3].x=x4; rec[3].y=y; rec[3].z=z4;

		Crossing* c;
		if (!desc.compare("Curve2x2")){
			c = new Curve2x2(lanesVec,index,rec);
		}else if (!desc.compare("TRoads2x2")){
			c = new TRoads2x2(lanesVec,index,rec);
		}else if (!desc.compare("TRoads2x4")){
			c = new TRoads2x4(lanesVec,index,rec);
		}else if (!desc.compare("XRoads2x2")){
			c = new XRoads2x2(lanesVec,index,rec);
		}else if (!desc.compare("TRoads4x2")){
			c = new TRoads4x2(lanesVec,index,rec);
		}else if (!desc.compare("XRoads2x4")){
			c = new XRoads2x4(lanesVec,index,rec);
		}else if (!desc.compare("TRoads4x4")){
			c = new TRoads4x4(lanesVec,index,rec);
		}else if (!desc.compare("XRoads4x4")){
			c = new XRoads4x4(lanesVec,index,rec);
		}else if (!desc.compare("Curve4x4")){
			c = new Curve4x4(lanesVec,index,rec);
		}
		//initialize pointOfInterest and inPhysics, will it ever be implemented?
		c->pointOfInterest=0;
		c->inPhysics=false;

		tmpCrossings.push_back(c);
	}

	crossings= new Crossing*[tmpCrossings.size()];
	*numOfCrossings=(int)tmpCrossings.size();
	
	for(unsigned i=0;i<tmpCrossings.size(); i++){
		crossings[i]=tmpCrossings[i];
	}
	tmpCrossings.clear();
	return crossings;
}

void WorldReader::readSpawns(Lane* lanes, vector<SingleSpawn>* sSpawns, vector<RateSpawn>* rSpawns, vector<SpawnEvent>* spawnEventList){
	//SingleSpawn** singleSpawns;
	//RateSpawn** rateSpawns;
	//vector<SingleSpawn*> tmpSingleSpawns;
	//vector<RateSpawn*> tmpRateSpawns;
	ifstream infile;
	string tempdir=worlddir;
	tempdir.append("/spawns.txt");
	infile.open(tempdir.c_str(), ios::in);
	string buff;
	getline(infile, buff);
	getline(infile, buff);
	string line;
	while (getline(infile, line)){
		istringstream wss(line);
		int index;
		int lane;
		string desc;
		wss>>index;
		wss>>lane;
		wss>>desc;

		//Crossing* s;
		if(!desc.compare("Single")){
			vector<int> ctypes;
			vector<NxReal> times;
			int ctype;
			NxReal time;
			while (wss >> ctype){
				wss >>time;
				ctypes.push_back(ctype);
				times.push_back(time);

				spawnEventList->push_back (SpawnEvent(ctype,time,lane));

			}
			sSpawns->push_back(SingleSpawn(&lanes[lane],index,ctypes,times));
			//lanes[lane].startCrossing=&(sSpawns->back());

		}else if(!desc.compare("Rate")){
			NxReal rate;
			int ctype;
			wss>>rate;
			wss>>ctype;
			rSpawns->push_back(RateSpawn(&lanes[lane],index,ctype,rate));

			//lanes[lane].startCrossing=&(rSpawns->back());
		}
	}
	/*
	//SingleSpawns Global Eintragen
	sSpawns= new SingleSpawn*[tmpSingleSpawns.size()];
	*numOfSingleSpawns=(int)tmpSingleSpawns.size();
	
	for(unsigned i=0;i<tmpSingleSpawns.size(); i++){
		*sSpawns[i]=*tmpSingleSpawns[i];
	}
	tmpSingleSpawns.clear();
	*/

	sort(spawnEventList->begin(),spawnEventList->end());
		
}
 CarType* WorldReader::readCarTypes(int* numOfCarTypes){
	CarType* types;
	vector<CarType> tmpTypes;
	ifstream infile;
	string tempdir=worlddir;
	tempdir.append("/cartypes.txt");
	infile.open(tempdir.c_str(), ios::in);
	string buff;
	getline(infile, buff);
	getline(infile, buff);
	string line;

	while (getline(infile, line)){
		istringstream wss(line);
		int index;
		wss>>index;
		CarType* c = new CarType(index);
		//vector<string> params;
		//vector<Nxreal> paramvalues;
		string param;
		NxReal paramvalue;
		while( wss >> param){
			wss >> paramvalue;
			if (!param.compare("maxSpeed")){
				c->maxSpeed=paramvalue;
			}else if (!param.compare("maxAcceleration")){
				c->maxAcceleration=paramvalue;
			}else if (!param.compare("minAcceleration")){
				c->minAcceleration=paramvalue;
			}else if (!param.compare("length")){
				c->setLength(paramvalue);
				c->setHeight(paramvalue/2);
			}else if (!param.compare("width")){
				c->setWidth(paramvalue);
			}
		}
		tmpTypes.push_back(*c);
	}
	types= new CarType[tmpTypes.size()];
	*numOfCarTypes=(int)tmpTypes.size();
	
	for(unsigned i=0;i<tmpTypes.size(); i++){
		types[i]=tmpTypes[i];
	}
	tmpTypes.clear();
	return types;

 }