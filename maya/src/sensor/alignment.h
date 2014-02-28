/*
 *  alignment.h
 *  animal
 *
 *  Created by Julian Mann on 29/07/2006.
 *  Copyright 2006 hoolyMama. All rights reserved.
 *
 */

#ifndef _alignment
#define _alignment

#include <agentSensor.h>

class alignment : public agentSensor
{
public:
	static	MStatus		initialize();
	static	void*		creator();
	
	virtual	bool		isAbstractClass() const {return false;}
	
	// Attributes
	static MObject aMeasureVelocity;
	static MObject 	aBlend;
	static	MTypeId		id;
	
protected:

		
		enum velAlgo { kAbsolute, kRelative, kBlend};

		virtual	MStatus		assess( MDataBlock& data, const poolData * pd, MVectorArray &assessment);
};

#endif
