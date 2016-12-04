/*------------------------------------------------------------------------
 *  -- Street.h
 *
 *  Declaration File for the Street Class
 *  
 *  
 *  Author: Valentin Scheele (valentin@scheeles.de)
 *------------------------------------------------------------------------*/

#ifndef STREET_H
#define STREET_H

#include "stdafx.h"
#include "lane.h"


class Street{
public:
	void calcRectangle();
	void calcRectangleCrossingPoints();
	vector <Lane*> lanes;
	Lane* borderLane1;
	Lane* borderLane2;

	Street(vector<Lane*> ls);
	NxVec3 rectangle[4];
	NxVec3 rectangleCrossingPoints[4];
	int rectangleCrossingPointsIndices[4];

	NxVec3 middle;
};

#endif