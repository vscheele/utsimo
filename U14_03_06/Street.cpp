#include "stdafx.h"
#include "Street.h"


Street::Street(vector<Lane*> ls){

	cout << "--- Street built with lanes ";
	for (unsigned i=0; i<ls.size();i++){
		ls[i]->street=this;
		cout<<" "<<ls[i]->utsindex;

	}

	this->lanes=ls;
	calcRectangle();
	calcRectangleCrossingPoints();
	middle=(rectangle[0]+rectangle[1]+rectangle[2]+rectangle[3])/4;

	cout << "\n";
};

void Street::calcRectangle(){
	cout << " || ";
	bool firstBorderFound=false;
		for (int h=0; h< lanes.size(); h++){
			bool isBorder=true;
			for (int i=0; i< lanes.size(); i++){
				//determine by angle that the lane cant be a border	
				if ((lanes[h]->utsindex!=lanes[i]->utsindex)
					&&(NxAngle(lanes[h]->orthoNorma,lanes[h]->start - lanes[i]->end)<PI/2)){
						isBorder=false;
				}
			}
			if (isBorder){
				if (!firstBorderFound){
					this->rectangle[0]=lanes[h]->start+(lanes[h]->orthoNorma*-5);
					this->rectangle[1]=lanes[h]->end+(lanes[h]->orthoNorma*-5);
					this->borderLane1=lanes[h];
				}else{
					this->borderLane2=lanes[h];
					this->rectangle[2]=lanes[h]->start+(lanes[h]->orthoNorma*-5);
					this->rectangle[3]=lanes[h]->end+(lanes[h]->orthoNorma*-5);
				}

				firstBorderFound=true;
				cout<<lanes[h]->utsindex<< " is border. ";
			}
		}
}
void Street::calcRectangleCrossingPoints(){
	float maxDistance1=1000;
	float maxDistance2=1000;
	float maxDistance3=1000;
	float maxDistance4=1000;
	Crossing* c1 = borderLane1->endCrossing;
	Crossing* c2 = borderLane2->endCrossing;

	//check which crossing points correlate to the streetpoints and put
	//them into members
	for (int i=0; i < 4; i++ ){
		if (( (c1->rectangle[i] - this->rectangle[1]).magnitude()  < maxDistance1)
		&& (NxAngle( borderLane1->orthoNorma, c1->rectangle[i] - borderLane1->end) > PI/2)){
			this->rectangleCrossingPoints[1]=c1->rectangle[i];
			this->rectangleCrossingPointsIndices[1]=i;
			maxDistance1 = (c1->rectangle[i] - this->rectangle[1]).magnitude();
			
		}
		if (( (c1->rectangle[i]-this->rectangle[2]).magnitude()  < maxDistance2)
			&& (NxAngle( borderLane2->orthoNorma, c1->rectangle[i] - borderLane2->start) > PI/2)){
			this->rectangleCrossingPoints[2]=c1->rectangle[i];
			this->rectangleCrossingPointsIndices[2]=i;
			maxDistance2 = (c1->rectangle[i] - this->rectangle[2]).magnitude();
		}
		if (((c2->rectangle[i]-this->rectangle[3]).magnitude()  < maxDistance3)
		&& (NxAngle( borderLane2->orthoNorma, c2->rectangle[i] - borderLane2->end) > PI/2)){
			this->rectangleCrossingPoints[3]=c2->rectangle[i];
			this->rectangleCrossingPointsIndices[3]=i;
			maxDistance3 = (c2->rectangle[i] - this->rectangle[3]).magnitude();
		}
		if  (((c2->rectangle[i]-this->rectangle[0]).magnitude()  < maxDistance4)
		&& (NxAngle( borderLane1->orthoNorma, c2->rectangle[i] - borderLane1->start) > PI/2)){
			this->rectangleCrossingPoints[0]=c2->rectangle[i];
			this->rectangleCrossingPointsIndices[0]=i;
			maxDistance4 = (c2->rectangle[i] - this->rectangle[0]).magnitude();
		
		}
	}
	if (this->borderLane1->utsindex==74)
		int t=0;
	//Sink or Spawn: use the estimated points
	if ((c1->type!= Crossing::CROSSING) && (c1->type!= Crossing::CURVE)) {
		rectangleCrossingPoints[1]=rectangle[1];
		rectangleCrossingPoints[2]=rectangle[2];
	}
	if ((c2->type!= Crossing::CROSSING) && (c2->type!= Crossing::CURVE)) {
		rectangleCrossingPoints[0]=rectangle[0];
		rectangleCrossingPoints[3]=rectangle[3];
	}

}