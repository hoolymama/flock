/*
 *  cohesion.h
 *  animal
 *
 *  Created by Julian Mann on 30/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */

/*
 *  cohesion.h
 *  animal
 *
 *  Created by Julian Mann on 30/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */


#ifndef _cohesion
#define _cohesion

#include <agentSensor.h>

class cohesion : public agentSensor
{
public:
	static	MStatus		initialize();
	static	void*		creator();
	
	virtual	bool		isAbstractClass() const {return false;}
	
	// Attributes
	
	
	
	
	// static	MObject		aSwirlMag;	
	// static	MObject		aGoalMag;	

	static	MObject		aGoalDecay;
	
	static	MTypeId		id;
	
protected:
		
		virtual	MStatus		assess( MDataBlock& data, const poolData * pd, MVectorArray &assessment);
};

#endif
