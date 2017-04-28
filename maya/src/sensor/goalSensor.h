/*
 *  goalSensor.h
 *  animal
 *
 *  Created by Julian Mann on 31/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */

//////////////////////////////////////////////////////////////////////
#ifndef _goalSensor
#define _goalSensor

#include <sensor.h> 


class goalSensor : public sensor  
{
public:
	
	
	goalSensor();
	/// 
	virtual				~goalSensor(); 
	/// 
	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );
	///
	virtual	bool		isAbstractClass() const {return false;}
	/// 
	static  void*		creator();
	/// 
	static  MStatus		initialize();	
	/// 	
	
	
	//////////////////////////////////////////
 
 	static MObject aGoalPositions;
 	static MObject aGoalWeights;
 	
	static MObject aDecayRampMax;	
 
	
	

	static	MTypeId		id;
protected:
		
		// Protected implementation
		virtual	MStatus		assess( MDataBlock& data,  MVectorArray &assessment);
};

#endif
