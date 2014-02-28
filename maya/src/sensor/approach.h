/*
 *  approach.h
 *  animal
 *
 *  Created by Julian Mann on 30/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */
/*
 *  approach.h
 *  animal
 *
 *  Created by Julian Mann on 29/07/2006.
 *  Copyright 2006 hoolyMama. All rights reserved.
 *
 */

#ifndef _approach
#define _approach

#include <agentSensor.h>

class approach : public agentSensor
{
public:
	static	MStatus		initialize();
	static	void*		creator();
	
	virtual	bool		isAbstractClass() const {return false;}
	
	// Attributes

	
	static MObject aMaxSeparation;
	static MObject aSeparationDecay;
	static MObject aMaxTime;
	static MObject aTimeDecay;
	

	
	static	MTypeId		id;
	
protected:
		
		virtual	MStatus		assess( MDataBlock& data, const poolData * pd, MVectorArray &assessment);
};

#endif

