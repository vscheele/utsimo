
//Utsimo.cpp

#include "stdafx.h"
#include "Utsimo.h"

int Utsimo::randomTable[RANDOMTABLESIZE];
int Utsimo::randomTableIndex=0;


Utsimo::Utsimo(){
}

void Utsimo::initAll(string worlddir){

	carIDGenerator=IDGenerator();
	//read world params
	worldparams=new Worldparams;
	worldparams->readParams(worlddir);
	WorldReader reader(worlddir);

	//numberof lanes and cars
	numOfLanes=0;
	numOfCurrentCars=0;
	artificialTimeStep=1;
	offsetTime=0;

	stepCounter=0;
	microsecondsaverage=0;

	//timer for performance
	pureStepTime=0;
	pureStepTimeWithDrawing=0;

	//generate random table
	initRand();

	//read lanes from file 
	lanes = reader.readLanes(&numOfLanes);
	//read crossings from file
	crossings = reader.readCrossings(lanes, &numOfCrossings);

	//read spawns from file
	reader.readSpawns(lanes, &singleSpawns, &rateSpawns, &spawnEventList);


	//attach sinks to lanes without endcrossing
	attachSinks();

	//read cartypes from file
	carTypes = reader.readCarTypes(&numOfCarTypes);

	//calculate Max CollisionDistance squared
	this->maxCollisionDistanceSquared=0;
	for (unsigned i=0; i< numOfCarTypes; i++){
			for (unsigned h=0; h< numOfCarTypes; h++){
				if (maxCollisionDistanceSquared 
					< (carTypes[i].getLengthSquared() + carTypes[i].getWidthSquared() 
					+ carTypes[h].getLengthSquared() + carTypes[h].getWidthSquared()))
				{

					maxCollisionDistanceSquared = carTypes[i].getLengthSquared() + carTypes[i].getWidthSquared() 
						+ carTypes[h].getLengthSquared() + carTypes[h].getWidthSquared();
				}
			}
	}
	this->maxCollisionDistanceSquared/=2;
	
	//build streets out of lanes automatically
	buildStreets();
					
	//check the read world data
	checkWorldData();
									 
	//initialize Physics
	utsNx= new UtsNx(this);
	utsNx->init();
										  
	if (Worldparams::DEBUGVIEWER){
		//initialize Debugviewer
		dbViewer=new DebugViewer(this);
		dbViewer->init();
	}

	for (int i=0;i<numOfLanes;i++){
		cout<<"Lane "<<i<<"\tstartcrossingtype: "<<lanes[i].startCrossing->type<<"\tendcrossingtype: "<<lanes[i].endCrossing->type<<"\tstartcrossingindex: "<<lanes[i].endCrossing->index<<"\n";
	}
	


	//startTime=GetTickCount();
	//stepStartTime=startTime;
	 if(!QueryPerformanceFrequency((LARGE_INTEGER *)&ticksPerSecond)) cout<< "ticks fehler";

    UINT64 tmp;
	QueryPerformanceCounter((LARGE_INTEGER* ) &tmp);
	this->startTime=1000*(float)tmp/(float)(__int64)ticksPerSecond;
	stepStartTime=startTime;
}


void Utsimo::step(){
	stepCounter++;

	//measure time
	UINT64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER* ) &currentTime);
	currentTime=1000*currentTime/(float)(__int64)ticksPerSecond    +offsetTime;


	
	//Time increments
	if (artificialTimeStep==-1) {
		if (stepStartTime<=currentTime){
			lastStepLength=currentTime-stepStartTime;
		}
		else {
			//simulate last step to be realtime
			lastStepLength=30;
			stepStartTime=currentTime-30;
		}
		//dont allow time jumpin
	} else {
		lastStepLength=artificialTimeStep;
	}
	this->lastStepLengthInSeconds=lastStepLength/1000;
	stepStartTime+=lastStepLength;
	
	//spawn cars in step
	if (this->numOfCurrentCars< Worldparams::MAX_NUMBER_OF_CARS)
		spawnCars();
	
	//handle each car
	int tmpNumOfCars=0;
	for(int laneIndex=0; laneIndex < numOfLanes; laneIndex++){
		for (unsigned carIndex=0; carIndex < lanes[laneIndex].cars.size(); carIndex++){
			//Car at normal sink
			if (lanes[laneIndex].cars[carIndex]->lifestate==Car::ZOMBIE){
				sinkCar(lanes[laneIndex].cars[carIndex]);
			}
			//Car a crashed zombie
			else if (lanes[laneIndex].cars[carIndex]->lifestate==Car::CRASHZOMBIE){

				sinkCrashCar(lanes[laneIndex].cars[carIndex]);
			}
			else {
				NxVec3 oldpos=lanes[laneIndex].cars[carIndex]->position;
				lanes[laneIndex].cars[carIndex]->doStep(lastStepLength);
			}
			tmpNumOfCars++;
		}
	}
	
	//count cars
	this->numOfCurrentCars=tmpNumOfCars;
	
	//step Physics
	this->utsNx->step(lastStepLength);

																	UINT64 tmp2;
																	QueryPerformanceCounter((LARGE_INTEGER* ) &tmp2);
																	pureStepTime=1000*tmp2/(float)(__int64)ticksPerSecond-currentTime+offsetTime;
																	if (this->stepCounter>99){
																		this->microsecondsaverage*=0.99;
																	}
																	this->microsecondsaverage+=pureStepTime*0.01;
																		
	if (Worldparams::DEBUGVIEWER){
		//update gfx
		dbViewer->step();
	}
	
																		UINT64 tmp3;
																	QueryPerformanceCounter((LARGE_INTEGER* ) &tmp3);
																	pureStepTimeWithDrawing=1000*tmp3/(float)(__int64)ticksPerSecond-currentTime+offsetTime;
	
}
void Utsimo::spawnCars(){
	
	
	//Spawn from Ratespawns
	for (unsigned i=0;i< rateSpawns.size();i++){
		if (this->stepStartTime > rateSpawns[i].lastSpawn+rateSpawns[i].spawnRate){

			Lane* lane=rateSpawns[i].lane;
			//Lane doesnt have any room left!!
			if (lane->hasPlaceforCarLength(carTypes[rateSpawns[i].cType].getLength()))
			{
				CarType* ctype=&carTypes[rateSpawns[i].cType];
				NxVec3 position=lane->start;
				NxVec3 velocity=lane->diffVec/lane->diffVec.magnitude() * 1.0 ;

				Car* bornCar= new Car(lane,ctype,position,velocity,this);
				lane->addCar(bornCar);

				rateSpawns[i].lastSpawn=(long long)stepStartTime;
			}
																	


		}
	}




	//Execute Single Spawn Commands
	if (spawnEventList.size()<1) return;
	if (this->spawnEventList[0].time < (stepStartTime-startTime)/1000){

		Lane* lane=&lanes[spawnEventList[0].lane];
		//Lane doesnt have any room left!!
		if (!lane->hasPlaceforCarLength(carTypes[spawnEventList[0].cType].getLength()))
			return;
		CarType* ctype=&carTypes[spawnEventList[0].cType];
		NxVec3 position=lane->start;
		NxVec3 velocity=lane->diffVec/lane->diffVec.magnitude() * 0.1f;
																		
		Car* bornCar= new Car(lane,ctype,position,velocity,this);
		lane->addCar(bornCar);
																		
		spawnEventList.erase(spawnEventList.begin());


	}
}


void Utsimo::sinkCar(Car* c){
	utsNx->removeCar(c);

	c->currentLane->removeCar(c);
	if (c->follower){
		c->follower->leader=0;
	}
	delete c;
}
void Utsimo::sinkCrashCar(Car* c){
	
	c->currentLane->removeCar(c);

	if (c->laneState==Car::IN_CROSSING)
		c->corridor->removeCar(c);

	utsNx->removeCar(c);
	if ((c->laneState==Car::IN_PREPARATION)||(c->laneState==Car::IN_CROSSING)){
		c->corridor->removePrep();
	}
	if (c->follower){
		c->follower->leader=c->leader;
	}
	if (c->leader){
		c->leader->follower=c->follower;
	}
	delete c;

}


void Utsimo::attachSinks(){
	for (int i=0;i<numOfLanes;i++){
		if (!lanes[i].endCrossing) { 
			cout<<" - Lane "<<i<<" ends in a Sink\n";
			lanes[i].endCrossing= new Sink(&lanes[i]);
		}
		if (!lanes[i].startCrossing) { 
			cout<<" - Lane "<<i<<" starts in a Sink\n";
			lanes[i].startCrossing= new Sink(&lanes[i]);
		}
	}
}

void Utsimo::reOffsetTime(){
	UINT64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER* ) &currentTime);
	currentTime=1000*currentTime/(float)(__int64)ticksPerSecond    +offsetTime;
	offsetTime+=this->stepStartTime-currentTime;
}
	
void Utsimo::setPointOfInterest(NxVec3 poi){
	this->pointOfInterest=poi;
}
NxVec3 Utsimo::getPointOfInterest(){
	return pointOfInterest;
}

void Utsimo::buildStreets(){
	for (int i=0; i< numOfLanes; i++){
		if (!lanes[i].street){
			vector<Lane*> streetLanes;
			streetLanes.push_back(&lanes[i]);
			for (int h=0; h< numOfLanes; h++){
				//lanes are next to each other (by checking ends)
				if ((h!=i)
					&& (!lanes[h].street)
					&& ( (lanes[h].startCrossing==lanes[i].startCrossing)
						|| (lanes[h].startCrossing==lanes[i].endCrossing))
					&& ( (lanes[h].endCrossing==lanes[i].startCrossing)
						|| (lanes[h].endCrossing==lanes[i].endCrossing)))
				{
					streetLanes.push_back(&lanes[h]);
				}
				//special case spawn/sink
				else if( ((lanes[h].endCrossing->type==Crossing::SINK)
								&& (lanes[h].startCrossing==lanes[i].endCrossing))
							|| ((lanes[i].endCrossing->type==Crossing::SINK)
								&& (lanes[i].startCrossing==lanes[h].endCrossing)))
				{
					if (NxAngle(lanes[h].diffVec,-lanes[i].diffVec) < 0.5)
						streetLanes.push_back(&lanes[h]);
				}


			}
			streets.push_back(Street(streetLanes));
		}
	}
	
}

void Utsimo::checkWorldData(){
	for (unsigned i =0; i< this->numOfCrossings; i++){
		crossings[i]->printCrossing();
		if (crossings[i]->index!=i){
			cout<<"CROSSING INDICES DO NOT MATCH"<<"\n";
			exit(0);
		}
	}
}

int Utsimo::getRand(){
	Utsimo::randomTableIndex--;
	if (Utsimo::randomTableIndex<0)
		Utsimo::randomTableIndex=RANDOMTABLESIZE-1;
	return Utsimo::randomTable[randomTableIndex];
}
void Utsimo::initRand(){
	for (int i=0; i< RANDOMTABLESIZE; i++){
		Utsimo::randomTable[i]=rand();
	}
	Utsimo::randomTableIndex=0;
}

/////////////////////DLL
BOOL __stdcall _DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}