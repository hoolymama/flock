/*
 *  slotSensor.h
 *  animal
 *
 *  Created by Julian Mann
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */


#ifndef _slotSensor
#define _slotSensor

#include <agentSensor.h>

class slotSensor : public agentSensor
{
public:
	static	MStatus		initialize();
	static	void*		creator();
	
	virtual	bool		isAbstractClass() const {return false;}
	
	// Attributes
	static MObject aSlotRadius ;
	// static MObject aSlotPositionPP;
	static MObject aSlotImportancePP;
	static MObject aImportanceMargin ;	
		static MObject aMaxFreeSlots;
	static	MTypeId		id;
	
protected:
		
		virtual	MStatus		assess( MDataBlock& data, const poolData * pd, MVectorArray &assessment);
};

#endif
