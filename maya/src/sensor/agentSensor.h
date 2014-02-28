/*
 *  agentSensor.h
 *  animal
 *
 *  Created by Julian Mann on 29/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */

//////////////////////////////////////////////////////////////////////
#ifndef _agentSensor
#define _agentSensor

#include <poolData.h> 
#include <sensor.h> 


class agentSensor : public sensor  
{
public:

	
	agentSensor();
	/// 
	virtual				~agentSensor(); 
	/// 
	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );
	///
	virtual	bool		isAbstractClass() const {return true;}
	/// 
	static  void*		creator();
	/// 
	static  MStatus		initialize();	
	/// 	
	
	
	//////////////////////////////////////////
	
	static MObject aPool;				// 

	static MObject aMaxNeighbors;	   	//
										// 
					   
	static	MTypeId		id;
protected:

		// Protected implementation
		virtual	MStatus		assess( MDataBlock& data, const poolData * pd , MVectorArray &assessment);
};

#endif
