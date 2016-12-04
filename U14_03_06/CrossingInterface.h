//CrossingInterface.h

class CrossingInterface{
	virtual bool isSpawn() =0;
	virtual char* getStringIdentifier()=0;
	virtual vector<Lane*> getFollowUps(int index)=0;
	virtual NxVec3* getRectangle()=0;
	virtual void printCrossing()=0;
};