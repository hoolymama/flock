/*
 *  cohesion.cpp
 *  animal
 *
 *  Created by Julian Mann on 30/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnPluginData.h>
#include <errorMacros.h>
#include <attrUtils.h>
#include <poolData.h>
#include <agentSensor.h>
#include <cohesion.h>
#include "jMayaIds.h"
//////////////////////////////////////////////////////////////////////////////
// Initialisation and destruction
MObject cohesion::aGoalDecay;
// MObject cohesion::aSwirlMag;
// MObject cohesion::aGoalMag;

MTypeId     cohesion::id( k_cohesion );

MStatus
cohesion::initialize()
{

	MStatus st;
	MString method("cohesion::initialize");
	inheritAttributesFrom("agentSensor");

	MFnNumericAttribute nAttr;


	// TODO replace with falloff curve
	aGoalDecay	= nAttr.create("goalDecay", "gdc", MFnNumericData::kDouble, 1.0, &st); mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aGoalDecay); mser;



	attributeAffects( 	 aGoalDecay  	, aAssessment	);

	return MS::kSuccess;
}

void *cohesion::creator()
{
	return new cohesion;
}

//////////////////////////////////////////////////////////////////////////////
// Implementation

MStatus cohesion::assess(MDataBlock &data, const poolData *pd, MVectorArray &assessment)
{

	MStatus st;
	MString method(" cohesion::assess");

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



	assessment  = MVectorArray(len, MVector::zero);
	int num = data.inputValue(aMaxNeighbors).asShort();
	double  goalDecay = data.inputValue(aGoalDecay).asDouble();
	// double  swirlMag = data.inputValue(aSwirlMag).asDouble();
	// double  goalMag = data.inputValue(aGoalMag).asDouble();
	bool   useRamps = data.inputValue(aUseRampAttenuation).asBool();



	if (num && (0.0 != acuity)  && (0.0 < fov ) ) {
		const AGENT_VECTOR *allAgents = pd->tree()->perm();
		unsigned i;
		MRampAttribute fovRamp( thisMObject() , aFovProfile, &st ); mser;
		MRampAttribute decRamp( thisMObject() , aDecayProfile, &st ); mser;

		for (AGENT_VECTOR::const_iterator searchAgent = allAgents->begin();
		     searchAgent != allAgents->end(); searchAgent++ ) {
			i = (*searchAgent)->id();

			if (acuity == 0.0) { continue; }

			// do the search for this point
			////////////////////////////////
			//const double &  radius = radius; //
			MDoubleArray resultDists;
			AGENT_VECTOR *neighbors = new AGENT_VECTOR;
			if (doMaxDistPP) { radius = maxDistPP[i]; }
			pd->closestNPoints( *searchAgent , radius, num, neighbors, resultDists ) ;
			if (!(neighbors->size())) {
				delete neighbors;
				neighbors = 0;
				continue;
			}
			////////////////////////////////

			////////////////////////////////
			MVector signal(MVector::zero);
			const MVector viewVectorN = viewVectors[i].normal();
			////////////////////////////////

			unsigned c;
			unsigned iter = 0;
			for (AGENT_VECTOR::const_iterator currAgent = neighbors->begin();
			     currAgent != neighbors->end(); currAgent++ ) {
				c = (*currAgent)->id();
				signal += points[c] ;
				iter++;
			}
			if (iter) {
				signal = signal / double(iter); // avg position
				// cerr << "avg pos" << signal << endl;

				MVector toAvgPos =  signal - points[i];
				// cerr << "toAvgPos" << toAvgPos << endl;

				double dist = toAvgPos.length();
				// cerr << "toAvgPos.length()" << dist << endl;

				// work out the swirl (like a ringroad around the group) - based on upvector, viewVector and center of group
				// MVector toAvgPosN = toAvgPos.normal();
				// MVector sideSignal  = toAvgPosN^MVector::yAxis;
				// MVector vCross  = viewVectorN^toAvgPosN;
				// double signDot = vCross*MVector::yAxis;
				// signDot = (signDot > 0) ? 1.0 : -1.0;
				// double facingDot = viewVectorN*toAvgPosN;
				// if (facingDot < 0 ) facingDot = 0.0;
				// sideSignal = sideSignal * signDot * facingDot;



				double atten;

				if (useRamps) {
					atten = attenuateSense(
					          points[i], viewVectorN, fovRamp, decRamp,
					          points[c], dist, radius
					        );
				}
				else {
					atten = attenuateSense(
					          points[i], viewVectorN, fov, drp, dec,
					          points[c], dist, radius
					        );
				}

				// if (atten > 1000000) {
				// 	cerr << "points[i]" << points[i] << endl;
				// 	cerr << "viewVectorN" << viewVectorN << endl;
				// 	cerr << "fov" << fov << endl;
				// 	cerr << "drp" << drp << endl;
				// 	cerr << "dec" << dec << endl;
				// 	cerr << "points[c]" << points[c] << endl;
				// 	cerr << "resultDists[iter]" << resultDists[iter] << endl;
				// 	cerr << "radius" << radius << endl;
				// 	cerr << "atten" << atten << endl;
				// }


				double goalToMaxDist =  radius - dist;
				// cerr << "goalToMaxDist" << goalToMaxDist << endl;

				double goalAtten = mayaMath::attenuate(goalToMaxDist, radius, goalDecay);
				// cerr << "goalAtten" << goalAtten << endl;

				if (! toAvgPos.isEquivalent(MVector::zero)) {
					signal = toAvgPos.normal() * acuity * atten * goalAtten;
				}
				else {
					signal = MVector::zero;
				}
				// cerr << "cohesion signal" << signal << endl;
				assessment.set(signal, i);
			}

			if (neighbors) {
				delete neighbors;
				neighbors = 0;
			}


		}
	}

	return st;
}
