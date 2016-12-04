#ifndef CARTYPE_H
#define CARTYPE_H

//include precompiled headers
#include "stdafx.h"
//include Utsimo system header 
//#include "Utsimo.h"


/** \brief Data Structure for the definition of a specific car type.
*  
*
* This class contains all parameters describing a specific car type. Each
* vehicle instance should have a pointer to an instance of this class, 
* describing its physical attributes.
* Instances of this class are made by the WorldReader Singleton, which reads
* the user-defined cartypes from a textfile. It is supposed to initialize
* the parameters, too.
*
* There are no access methods: this class is supposed to be
* an open data container for other methods. 
* \sa WorldReader
*
*/
class CarType{
public:

	CarType();
	CarType(int ind);

	void setHeight(NxReal h);
	NxReal getHeight(){ return height ;}
	void setWidth(NxReal w);
	NxReal getWidth(){ return width ;}
	NxReal getWidthSquared(){ return widthSquared ;}
	void setLength(NxReal l);
	NxReal getLength(){ return length ;}
	NxReal getLengthSquared(){ return lengthSquared ;}

	//! unique ID and index 
    /* 
    * Global ID, also array index in Utsimos global cartype-array.
    */
	int index;

	//! maximum acceleration 
    /* 
	* The maximum acceleration for this CarType.
    */
	NxReal maxAcceleration;

	//! minimum acceleration 
    /* 
    * The minimum acceleration for this CarType (negative value)
    */
	NxReal minAcceleration;

	//! maximum speed 
    /* 
    * The maximum speed for this CarType
    */
	NxReal maxSpeed;



protected:
	//! vehicle length
    /*  
    * The length of the vehicle for this CarType 
    */
	NxReal length;
	NxReal lengthSquared;

	//! vehicle width 
    /* 
    * The width of the vehicle for this CarType
    */
	NxReal width;
	NxReal widthSquared;

	//! vehicle height 
    /* 
    * The height of the vehicle for this CarType
    */
	NxReal height;
};
#endif