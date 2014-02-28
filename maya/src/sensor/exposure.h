/*
 *  exposure.h
 *  animal
 *
 *  Created by Julian Mann on 30/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */



#ifndef _exposure
#define _exposure

#include <agentSensor.h>

class exposure : public agentSensor
{
public:
	static	MStatus		initialize();
	static	void*		creator();
	
	virtual	bool		isAbstractClass() const {return false;}
	
	// Attributes
	
	
	static	MTypeId		id;
	
protected:
		
		virtual	MStatus		assess( MDataBlock& data, const poolData * pd, MVectorArray &assessment);
};

#endif
