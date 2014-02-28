/*
 *  aimPP.h
 *  animal
 *
 *  Created by Julian Mann on 01/08/2006.
 *  Copyright 2006 HoolyMama. All rights reserved.
 *
 */

#ifndef _aimPP
#define _aimPP


#include <maya/MPxNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MTypeId.h> 
#include "errorMacros.h"

class aimPP : public MPxNode
{
public:
	aimPP();
	virtual				~aimPP(); 
	
	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );
	
	static  void*		creator();
	static  MStatus		initialize();
	
	
	
	
	
public:
	

		
	static  MObject		aFront;	
	static  MObject		aUp;      
	
	static  MObject		aFrontAxis;	
	static  MObject		aUpAxis;      
	
	static  MObject		aInversions;   
	static  MObject		aRotateOrder;
	static  MObject		aOutputUnit;
	static  MObject		aOutputType;
	
	static  MObject		aOutX;
	static  MObject		aOutY;
	static  MObject		aOutZ;
	static  MObject		aOut;
	
	
	
	static	MTypeId		id;
};

#endif
