/*
 *  hungerState.h
 *  jtools
 *
 *  Created by Julian Mann on 18/04/2007.
 *  Copyright 2007 hoolyMama. All rights reserved.
 *
 */


#include <maya/MIOStream.h>
#include <math.h>

#include <maya/MQuaternion.h>
#include <maya/MTime.h>
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MIntArray.h>
#include <maya/MAnimControl.h>


#include <maya/MDataBlock.h>
#include <maya/MPxNode.h>
#include <errorMacros.h>


//#define PI 3.1415927
//#define ONE_EIGHTY_OVER_PI 57.29578
//#define RANGE 180
//#define RES 90




class hungerState: public MPxNode
{
public:
	hungerState() {

	};
	virtual ~hungerState() {};

	static void		*creator();
	static MStatus	initialize();

	// will compute output force.
	virtual MStatus	compute( const MPlug& plug, MDataBlock& block );

	static MTypeId	id;
	


	// other atts
	static MObject aFeed;			//PPV food coming in
	static MObject aSpend;			//PPV 	
	
	static MObject aDefaultFeed;		
	static MObject aDefaultSpend;			

	static MObject aFoodLevel;			//PPD	
	static MObject aUpdatedFoodLevel;		//PPD	

	static MObject aHungry;		//PPI input (bool) open or closed 
	static MObject aUpdatedHungry;		//PPI	output energy value
	
	
	static MObject aHungryFullRamp;	
	static MObject aHungryFullCycle;	
	
	
	static MObject aFullThreshold;		// float

	static MObject aCurrentTime;
	// static MObject aChargeRate;		
	// static MObject aTrickleCharge;
	// static MObject aReleaseRate;		
	// static MObject aTrickleRelease;
	// static MObject aMagnitude;
	
private:



};
