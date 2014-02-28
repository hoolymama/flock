/*
 *  splineSensor.h
 *  animal
 *
 *  Created by Julian Mann on 31/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */

//////////////////////////////////////////////////////////////////////
#ifndef _splineSensor
#define _splineSensor

#include <splinePoolData.h> 
#include <sensor.h> 


class splineSensor : public sensor  
{
public:
	
	
	splineSensor();
	/// 
	virtual				~splineSensor(); 
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
	
	static MObject aSplinePool;				// 
				// 

			
	static MObject aTangent;
	static MObject aNormal;
	static MObject aOrbital;
	
	static MObject aTangentFalloff;
	static MObject aNormalFalloff;
	static MObject aOrbitalFalloff;
	
	static MObject aTangentParam;
	static MObject aNormalParam;
	static MObject aOrbitalParam;


	
	

	static	MTypeId		id;
protected:
		
		// Protected implementation
		virtual	MStatus		assess( MDataBlock& data,const splinePoolData * sd , MVectorArray &assessment);
};

#endif
