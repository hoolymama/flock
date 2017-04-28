/*
 *  separationSensor.h
 *  animal
 *
 *  Created by Julian Mann on 29/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */

// sensor.h: interface for the sensor class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _sensor
#define _sensor
// Copyright (C) 2000 hoolyMama 
// 
// File: eye.h
//
// Dependency Graph Node: eye
#include <math.h>
#include <maya/MVectorArray.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MRampAttribute.h>

#include <maya/MTime.h>
#include <maya/MPxNode.h>
#include <maya/MTypeId.h> 
#include <errorMacros.h> 

class sensor : public MPxNode
{
public:
	
	sensor();
	/// 
	virtual				~sensor(); 
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
	

		 
	static MObject aPositionPP;		   	// 
										// 
	static MObject aVelocityPP;		   	//
										// 
	static MObject aViewVectorPP;		//	
	
	//static MObject aFovPP;  		 	//
										// 
	//static MObject aDropOffPP;  	 	//
										// 
	//static MObject aAcuityPP;		 	//
										// 
	//static MObject aDecayPP;		 	//
										// 
	static MObject aMaxDistancePP;   	//	
	static MObject aUseMaxDistancePP;
	static MObject aFov;			   	//
	static MObject aDropOff;		   	//
	static MObject aAcuity;		   		// 
	static MObject aDecay;		   		//

	static MObject aUseRampAttenuation;		   		//
	static MObject aFovProfile;		   		//
	static MObject aDecayProfile;		   		//

//	static MObject aInvert;		   		//



	static MObject aMaxDistance;	   	//		
	static MObject aAssessmentMultX;
	static MObject aAssessmentMultY;
	static MObject aAssessmentMultZ;
	//static MObject aAssessmentMult;
			   	// 						   
	static MObject aAssessment;		   	// 						   

	static	MTypeId		id;
	

protected:
	virtual	MStatus		assess( MDataBlock& data, MVectorArray &assessment) ;
	
	void multAssessment(MDataBlock& data, MVectorArray &assessment);
	MStatus outputAssessment(MDataBlock& data, MVectorArray &assessment);

};


inline MStatus getRequiredSensorData(
	MDataBlock &data,  MVectorArray & points, 
	MVectorArray & velocities, 
	MVectorArray & viewVectors)  {
	unsigned plen = (MFnVectorArrayData(data.inputValue(sensor::aPositionPP).data()).length());
	unsigned vlen = (MFnVectorArrayData(data.inputValue(sensor::aVelocityPP).data()).length());
	if (!plen)  return MS::kSuccess; 
	if (plen != vlen) return MS::kUnknownParameter; 

	points = MFnVectorArrayData(data.inputValue(sensor::aPositionPP).data()).array();
	velocities = MFnVectorArrayData(data.inputValue(sensor::aVelocityPP).data()).array();
	
	unsigned viewlen = MFnVectorArrayData(data.inputValue(sensor::aViewVectorPP).data()).length();
	
	if (viewlen == plen) {
		viewVectors = MFnVectorArrayData(data.inputValue(sensor::aViewVectorPP).data()).array();
	} else {
		viewVectors = MFnVectorArrayData(data.inputValue(sensor::aVelocityPP).data()).array();
	}
	return MS::kSuccess;
}

inline double attenuateSense(
							 const MVector & pos,		// My Position
							 const MVector & view,		// My view vector (normalized)
							 const double &fov,			// My field of view (0 is thin cone, 0.5 is hemisphere, 1.0 is sphere)
							 const double &drp,			// My peripheral drop off
							 const double &dec,			// My distance decay
							 const MVector &otherPos,	// The other guy's position
							 const double &dist,		// The distance to the other guy, this was calculated earlier
							 const double &maxDist		// My max viewing distance, used to calculate decay
							 ) 
{
	
	double result = 1.0;
	const double fovN = (fov *-2.0) +1.;  // field of view mapped into dotprod space i.e. (0 -> 1) -to- (1 -> -1) i.e. (1=narrow cone -> -1=sphere)
	
	MVector diff = (otherPos - pos).normal(); // vector to other guy
 	double dot = diff * view; // viewVector should already be normalized
	if (dot < fovN) return 0.0; // outside my peripheral vision - return 0
	if (drp > 0.0) { // 
		double frac = 1.0 - (((dot -1.0) * -0.5) / fov);
		if (drp==1.0) {
			result = frac;
		} else if (drp==2.0) {
			result = frac * frac;
		} else {
			result = pow(frac, drp); 	
		}
	}

	if (dec > 0) { 									
		double closeness = (1.0 - (dist / maxDist));
		if (dec == 1.0) {//if linear
			result *= closeness;
		} else if (dec == 2.0) {
			result *= (closeness * closeness) ; // ^2
		} else {
			result *= pow(closeness, dec);
		}
	}
	return result;
}

inline double attenuateSense(
    const MVector & pos,		// My Position
    const MVector & view,		// My view vector (normalized)
    MRampAttribute & fovRamp,
    MRampAttribute & decRamp,
    const MVector &otherPos,	// The other guy's position
    const double &dist,		// The distance to the other guy, this was calculated earlier
    const double &maxDist		// My max viewing distance, used to calculate decay
    ) 
{
	MStatus st;
	// double result = 1.0;
	const double PI = 3.141592653;
	//MVector diff = (otherPos - pos); //.normal(); // vector to other guy
 	
 	double angle = (otherPos - pos).angle(view) / PI; 
	float lookupFov;
	fovRamp.getValueAtPosition( float(angle), lookupFov, &st ); er;

	double closeness = (dist / maxDist);
	if (closeness > 1.0) closeness =  1.0;
	float lookupDec;
	decRamp.getValueAtPosition( float(closeness), lookupDec, &st ); er;
	return double(lookupDec * lookupFov);

}
 


/*
inline double attenuate(const double &distance,const  double &maxDistance,const  double &decay) {
	if ( distance >=  maxDistance) return 0.0;

	double result = 1.0;
	if (decay > 0) { 									
		double closeness = (1.0 - (distance / maxDistance));
		if (decay == 1.0) {//if linear
			result *= closeness;
		} else if (decay == 2.0) {
			result *= (closeness * closeness) ; // ^2
		} else {
			result *= pow(closeness, decay);
		}
	}
	return result;
}
*/

#endif
