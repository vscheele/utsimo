//IDGenerator.cpp
#include "stdafx.h"
#include "IDGenerator.h"


IDGenerator::IDGenerator(){
	lastGivenID=0;
}
int IDGenerator::getID()
{
	lastGivenID++;
	return lastGivenID;
	
}