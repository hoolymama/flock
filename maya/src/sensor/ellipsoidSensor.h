/*
 *  ellipsoidSensor.h
 *  animal
 *
 *  Created by Julian Mann on 30/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */


#ifndef _ellipsoidSensor
#define _ellipsoidSensor

#include <agentSensor.h>

class ellipsoidSensor : public agentSensor
{
public:
	static	MStatus		initialize();
	static	void*		creator();
	
	virtual	bool		isAbstractClass() const {return false;}
	
	// Attributes
	static MObject aPhi ;
	static MObject aScale ;
	
	static	MTypeId		id;
	
protected:
		
		virtual	MStatus		assess( MDataBlock& data, const poolData * pd, MVectorArray &assessment);
};

#endif
