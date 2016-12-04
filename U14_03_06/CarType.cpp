/*------------------------------------------------------------------------
 *  -- CarType.cpp
 *
 *  Implementation File for the CarType Class.
 *
 *  This contains the implementation for the CarType class.
 *  
 *  The Constructor ist not responsible for initializing the values,
 *  this should be done by the Object creating the Instance.
 *
 *  Author: Valentin Scheele (valentin@scheeles.de)
 * 
 *------------------------------------------------------------------------*/


//include precompiled headers
#include "stdafx.h"

//include header file
#include "CarType.h"

//! Emtpy Constructor for STL containers 
/*
* TODO: Is this really necessary?
*
*/
CarType::CarType(){
}

//! Constructor with given global ID
/*
* Initializes member index, which is also the array index in Utsimos global Cartype[]
* also Initializes all parameters to some (useless) values, to prevent access errors.
* TODO: Is index unique?
*
* @param ind The index of the car type, as specified int the cartypes.txt
*/
CarType::CarType(int ind){

	//only important initialization, the others should be overwritten by
	//the WorldReader Singleton when reading from cartypes.txt
	this->index=ind;

	this->maxAcceleration=4;
	this->minAcceleration=-4; 
	this->maxSpeed=100;
	this->setLength(4.0);
	this->setWidth(2.0);
	this->setHeight(2.0);
}
void CarType::setHeight(NxReal h){
	this->height=h;
}
void CarType::setLength(NxReal l){
	this->length=l;
	this->lengthSquared=l*l;
}
void CarType::setWidth(NxReal w){
	this->width=w;
	this->widthSquared=w*w;
}
