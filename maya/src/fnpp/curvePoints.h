/*
 *  curvePoints.h
 *  feather
 *
 *  Created by Julian Mann on 26/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */



#ifndef curvePoints_H
#define curvePoints_H

#include <maya/MPlug.h>
#include <maya/MPxNode.h>

class curvePoints : public MPxNode
{
public:
	curvePoints();
	virtual ~curvePoints();
	virtual MStatus	compute( const MPlug&, MDataBlock& );
	
	static  void *	creator();
	static  MStatus	initialize();
	static	MTypeId		id;
	
	enum Spac { kParametric, kArcLen};

private:
		
	static	MObject	 aCurve;
	static	MObject  aNumPoints;	
	static	MObject	 aCurves;

	static	MObject	 aSpacing;
	static	MObject  aOutPoints;

};
#endif



