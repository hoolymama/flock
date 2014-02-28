
/*
 *  exposure.cpp
 *  animal
 *
 *  Created by Julian Mann on 30/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */

#include <maya/MFnPluginData.h>
#include <errorMacros.h>
#include <attrUtils.h>
#include <poolData.h>
#include <agentSensor.h>
#include <exposure.h>
#include "jMayaIds.h"
//////////////////////////////////////////////////////////////////////////////
// Initialisation and destruction

MTypeId     exposure::id( k_exposure );

MStatus
exposure::initialize()
{
	inheritAttributesFrom("agentSensor");
	
	return MS::kSuccess;
}

void* exposure::creator()
{
	return new exposure;
}

//////////////////////////////////////////////////////////////////////////////
// Implementation

MStatus exposure::assess(MDataBlock& data, const poolData * pd, MVectorArray &assessment)
{
	
	MStatus st;
	MString method(" exposure::assess");

	// get the 3 main required vector arrays
	////////////////////////////////////////////////////	
	MVectorArray  points;
	MVectorArray  velocities;
	MVectorArray  viewVectors;
	st = getRequiredSensorData(data, points, velocities, viewVectors); ert;
	
	MDoubleArray maxDistPP = MFnDoubleArrayData(data.inputValue(sensor::aMaxDistancePP).data()).array();
	bool doMaxDistPP = false;	
	unsigned len = points.length();
	if ((maxDistPP.length() == len) && data.inputValue(aUseMaxDistancePP).asBool())	 doMaxDistPP = true;
	////////////////////////////////////////////////////
	
	double fov = data.inputValue(aFov).asDouble();
	double drp = data.inputValue(aDropOff).asDouble();
	double acuity = data.inputValue(aAcuity).asDouble();
	double dec = data.inputValue(aDecay).asDouble();
	double radius = data.inputValue(aMaxDistance).asDouble();
 	bool   useRamps = data.inputValue(aUseRampAttenuation).asBool();

	//MVectorArray assessment(len);
	int num = data.inputValue(aMaxNeighbors).asShort();
	
	if (num && (0.0 != acuity) && (0.0 != radius) && (0.0 < fov ) ) {
		const AGENT_VECTOR * allAgents = pd->tree()->perm();
		unsigned i;

		MRampAttribute fovRamp( thisMObject() , aFovProfile, &st ); er;
		MRampAttribute decRamp( thisMObject() , aDecayProfile, &st ); er;

		for (AGENT_VECTOR::const_iterator searchAgent = allAgents->begin(); searchAgent!=allAgents->end(); searchAgent++ ) {
			i = (*searchAgent)->id();
			
			if (acuity == 0.0) continue;
			
			// do the search for this point
			////////////////////////////////
			//const double &  radius = radius; // 
			MDoubleArray resultDists;
			AGENT_VECTOR * neighbors = new AGENT_VECTOR;
			if (doMaxDistPP) radius = maxDistPP[i];
			pd->closestNPoints( *searchAgent , radius, num, neighbors, resultDists ) ;
			if (!(neighbors->size())) {
				delete neighbors;
				neighbors=0; 
				continue;	
			}
			////////////////////////////////
			
			////////////////////////////////
			MVector signal(MVector::zero);
			double accumulatedMags = 0;
			const MVector viewVectorN = viewVectors[i].normal(); 
			////////////////////////////////
			
			unsigned c;
			unsigned iter = 0;
			for (AGENT_VECTOR::const_iterator currAgent = neighbors->begin(); currAgent!=neighbors->end(); currAgent++ ) {
				c = (*currAgent)->id();
				
				// accumulate the differences and the magnitudes of differences
				///////////////////
				MVector dP =  points[c] - points[i] ; // vector away from me - towards points
 				double atten;
 				if (useRamps) {
 					atten = attenuateSense(
 						points[i],viewVectorN,fovRamp,decRamp,
 						points[c],resultDists[iter],radius
 						);
 				} else {
 					atten = attenuateSense(
 						points[i],viewVectorN,fov,drp,dec,
 						points[c],resultDists[iter],radius
 						);
 				}				
 				dP = dP.normal() * atten;
				accumulatedMags += atten;
				signal += dP;
				///////////////////
				
				
				iter++;
			}
			if (iter) {
				double exposureRatio = signal.length() / accumulatedMags;		
				signal = signal.normal() * acuity  * exposureRatio ;
				assessment.set(signal,i);
			}
			if (neighbors) {
				delete neighbors;
				neighbors=0; 
			}
		}

	}

	return st;
}
