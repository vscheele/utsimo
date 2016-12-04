//Corridor.cpp
#include "stdafx.h"
#include "Corridor.h"

Corridor::Corridor(Lane* corrStart, Lane* corrEnd, Crossing* c){

	this->startLane=corrStart;
	this->endLane=corrEnd;
	directLength=(startLane->end - endLane->start).magnitude();
	this->crossing=c;
	this->rightOfWay= vector<Corridor*>(0);
	
	this->preCalculated=new NxVec3[Worldparams::INTERPOLATIONSTEPS];
	for (unsigned i =0; i<Worldparams::INTERPOLATIONSTEPS; i++){
	
		float interpolationValue=((float)i/Worldparams::INTERPOLATIONSTEPS)+0.5f/Worldparams::INTERPOLATIONSTEPS;
		
		this->preCalculated[i]=interpolate(interpolationValue);
		//this->preCalculated[i].y=crossing->getRectangle()[0].y;
																		//if (i>0) cout<<(preCalculated[i]-preCalculated[i-1]).magnitude()<<"\n";
		
		//Y-CORRECTION OF INTERPOLATED SPLINE
		this->preCalculated[i].y=crossing->getRectangle()[0].y;
	}
	calculateCurveLength();
	calculateCurvature();

	curveMaxSpeed=max(Worldparams::CURVEMAXSPEED*(1-curvature),2.0f);//-curvature*Worldparams::CURVEMAXSPEED*30;
	
	carsInPrep=0;
	rowSemaphore=0;
}


void Corridor::setRightOfWay(vector<Corridor* > rOfWay){
	this->rightOfWay=rOfWay;
	//Doppelverlinkung!!!
	for (unsigned i=0; i<rightOfWay.size(); i++){
		rightOfWay[i]->hasRightOfWay.push_back(this);
	}
}

NxVec3 Corridor::interpolate(NxReal u){
	/*/////////////////////////CATMULL-ROM
	vec=0.5* (	(2*startLane->end)+
					((-1*startLane->start + endLane->start) * u)  +
					((2* startLane->start - 5*startLane->end + 4*endLane->start - endLane->end)* u * u) +
					((-1*startLane->start + 3*startLane->end - 3*endLane->start + endLane->end) * u * u * u));
	NxVec3 vec=  u*(u*(u*(3*(startLane->end-endLane->start)-startLane->start+endLane->end)+startLane->start+startLane->start-5*startLane->end+4*endLane->start-endLane->end)+endLane->start-startLane->start)*0.5+startLane->end;
	///////////////////////LINEAR
	//NxVec3 diffVec=endLane->start-startLane->end;
	//return startLane->end+diffVec*u;
	*/
	
		NxVec3 T1=startLane->end-startLane->start;
		NxVec3 T2=endLane->end-endLane->start;

		T1.normalize();
		T2.normalize();
		T1*=Worldparams::CURVE_TENSION*this->directLength;
		T2*=Worldparams::CURVE_TENSION*this->directLength;
		float u2=u*u;
		float u3=u2*u;
		
		float h1 =  2*u3 - 3*u2 + 1;          // calculate basis function 1
		float h2 = -2*u3 + 3*u2;              // calculate basis function 2
		float h3 =   u3 - 2*u2 + u;         // calculate basis function 3
		float h4 =   u3 -  u2;              // calculate basis function 4
		return ((h1*startLane->end) + (h2*endLane->start) + (h3*T1) + (h4*T2));
	
}
NxVec3 Corridor::getInterpolatedVelocityDirection(float u){
	unsigned index=(int)(u*Worldparams::INTERPOLATIONSTEPS);

	//index not valid
	if (index<0) 
		return startLane->diffVec;
	else if (index>Worldparams::INTERPOLATIONSTEPS-2) 
		return endLane->diffVec;


	NxVec3 veloDir=preCalculated[index+1]-preCalculated[index];
	return veloDir;
}
void Corridor::calculateCurvature(){
	NxReal highestAngle=0;
	NxVec3 lastInterpolated=startLane->diffVec;
	for (unsigned i =0; i<98; i++){
		NxVec3 tmp=interpolate(((float)(i+1)/100)) - interpolate(((float)(i)/100));
		NxReal newAngle = NxAngle(tmp,lastInterpolated);
		if (newAngle>highestAngle)
			highestAngle=newAngle;
		lastInterpolated=tmp;

	}
	curvature=highestAngle*20;
}
void Corridor::calculateCurveLength(){
	curveLength=0;
	for (unsigned i =0; i<999; i++){
		curveLength+=( interpolate(((float)(i+1)/1000)+0.0005f) - interpolate(((float)(i)/1000)+0.0005f) ).magnitude();
	}

	
}
Lane* Corridor::getStartLane(){
	return this->startLane;
}
Lane* Corridor::getEndLane(){
	return this->endLane;
}
NxReal Corridor::getCurvature(){
	return this->curvature;
}
Crossing* Corridor::getCrossing(){
	return this->crossing;
}
void Corridor::addPrep(){
	this->carsInPrep++;
	for (unsigned i=0; i<hasRightOfWay.size(); i++){
		this->hasRightOfWay[i]->rowSemaphore++;
	}
}
void Corridor::removePrep(){
	this->carsInPrep--;
	for (unsigned i=0; i<hasRightOfWay.size(); i++){
		this->hasRightOfWay[i]->rowSemaphore--;
	}
}

void Corridor::addCar(Car* c){
	this->cars.push_back(c);

	//if leader has taken other corridor, change it
	if (c->leader && c->leader->corridor != this && (crossing->type!=Crossing::CURVE))
		c->leader->follower=0;
		c->leader=0;

	//if theres already a car in the corridor, make it new leader of the entrant
	if (cars.size()>1){
		c->leader=cars[cars.size()-2];
		cars[cars.size()-2]->follower=c;
	}else if (this->endLane->cars.size()>0){
		c->leader=endLane->cars[endLane->cars.size()-1];
		endLane->cars[endLane->cars.size()-1]->follower=c;
	}
	//register needed carLength in following lane
	endLane->registeredCarLengthFromCorridor+=c->carType->getLength() + Worldparams::MINCARDISTANCE;

}
void Corridor::removeCar(Car* c){
	for (unsigned i=0; i<cars.size(); i++){
		//i should always be 0.....
		if (c==cars[i]){
			cars.erase(cars.begin() + i);
		}
	}
	//register needed carLength in following lane
	endLane->registeredCarLengthFromCorridor-=c->carType->getLength() +Worldparams::MINCARDISTANCE;
		
}
bool Corridor::isCrossingFree(){
	for (unsigned i=0; i<this->crossing->corridors.size();i++){
		if (this->crossing->corridors[i].cars.size()>0) return false;
	}
	return true;
}