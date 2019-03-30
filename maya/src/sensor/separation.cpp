/*
 *  separation.cpp
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
#include <separation.h>
#include "jMayaIds.h"
//////////////////////////////////////////////////////////////////////////////
// Initialisation and destruction

MTypeId     separation::id( k_separation );

MStatus
separation::initialize()
{
	inheritAttributesFrom("agentSensor");

	return MS::kSuccess;
}

void *separation::creator()
{
	return new separation;
}

//////////////////////////////////////////////////////////////////////////////
// Implementation

MStatus separation::assess(
  MDataBlock &data,
  const poolData *pd,
  MVectorArray &assessment)
{

	MStatus st;
	MString method(" separation::assess");

	// get the 3 main required vector arrays
	////////////////////////////////////////////////////
	MVectorArray  points;
	MVectorArray  velocities;
	MVectorArray  viewVectors;
	st = getRequiredSensorData(data, points, velocities, viewVectors); msert;

	MDoubleArray maxDistPP = MFnDoubleArrayData(data.inputValue(
	                           sensor::aMaxDistancePP).data()).array();
	bool doMaxDistPP = false;
	unsigned len = points.length();
	if ((maxDistPP.length() == len) && data.inputValue(aUseMaxDistancePP).asBool())	 { doMaxDistPP = true; }
	////////////////////////////////////////////////////
	double fov = data.inputValue(aFov).asDouble();
	double drp = data.inputValue(aDropOff).asDouble();
	double acuity = data.inputValue(aAcuity).asDouble();
	double dec = data.inputValue(aDecay).asDouble();
	double radius = data.inputValue(aMaxDistance).asDouble();
	bool   useRamps = data.inputValue(aUseRampAttenuation).asBool();

	int num = data.inputValue(aMaxNeighbors).asShort();

	if (num && (0.0 != acuity) && (0.0 != radius) && (0.0 < fov ) ) {
		const AGENT_VECTOR *allAgents = pd->tree()->perm();
		unsigned i;

		MRampAttribute fovRamp( thisMObject() , aFovProfile, &st ); mser;
		MRampAttribute decRamp( thisMObject() , aDecayProfile, &st ); mser;

		for (AGENT_VECTOR::const_iterator searchAgent = allAgents->begin();
		     searchAgent != allAgents->end(); searchAgent++ ) {
			i = (*searchAgent)->id();

			// do the search for this point
			////////////////////////////////
			MDoubleArray resultDists;
			AGENT_VECTOR *neighbors = new AGENT_VECTOR;
			if (doMaxDistPP) { radius = maxDistPP[i]; }
			pd->closestNPoints( *searchAgent , radius, num, neighbors, resultDists ) ;
			if (!(neighbors->size())) {
				delete neighbors;
				neighbors = 0;
				continue;
			}

			MVector signal(MVector::zero);
			const MVector viewVectorN = viewVectors[i].normal();

			unsigned c;
			unsigned iter = 0;

			for (AGENT_VECTOR::const_iterator currAgent = neighbors->begin();
			     currAgent != neighbors->end(); currAgent++ ) {
				c = (*currAgent)->id();

				// getRelativePosition of neighbor (vector points towards us)
				//////////////////
				MVector dP = points[i] - points[c] ;
				double atten;
				if (useRamps) {
					atten = attenuateSense(
					          points[i], viewVectorN, fovRamp, decRamp,
					          points[c], resultDists[iter], radius
					        );
				}
				else {
					atten = attenuateSense(
					          points[i], viewVectorN, fov, drp, dec,
					          points[c], resultDists[iter], radius
					        );
				}
				dP = dP.normal() * atten;
				signal += dP;
				iter++;
			}
			signal *= acuity  ;

			assessment.set(signal, i);
			if (neighbors) {
				delete neighbors;
				neighbors = 0;
			}
		}
	}
	return st;
}
