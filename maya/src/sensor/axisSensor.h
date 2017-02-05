/*
 *  axisSensor.h
 *  animal
 *
 *  Created by Julian Mann on 30/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */


#ifndef _axisSensor
#define _axisSensor

#include <agentSensor.h>

class axisSensor : public agentSensor
{
public:
	static	MStatus		initialize();
	static	void*		creator();
	
	virtual	bool		isAbstractClass() const {return false;}
	
	// Attributes
	static MObject aAxisPP ;
	static MObject aPointNormal;
	static MObject aAxisNormal ;	
	static MObject aAxisDirection ;	
	static MObject aAxisOrbit ;
	static MObject aEasyOrbit ;


	static	MTypeId		id;
	
protected:
		
		virtual	MStatus		assess( MDataBlock& data, const poolData * pd, MVectorArray &assessment);
};

#endif
