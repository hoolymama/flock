/*
 *  collisionSensor.h
 *  animal
 *
 *  Created by Julian Mann on 30/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */


#ifndef _collisionSensor
#define _collisionSensor

#include <agentSensor.h>

class collisionSensor : public agentSensor
{
public:
	static	MStatus		initialize();
	static	void*		creator();
	
	virtual	bool		isAbstractClass() const {return false;}
	
	// Attributes
	static MObject aRadius ;
	static MObject aBounce ;
  static MObject aFriction ;
  static MObject aPushOut ;
 
  static MObject aBias ;
 
  static MObject aDeltaTime ;
	static MTypeId id;
	
protected:
		
		virtual	MStatus		assess( MDataBlock& data, const poolData * pd, MVectorArray &assessment);
};

#endif
