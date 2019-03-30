/*
*  alignment.cpp
*  animal
*
*  Created by Julian Mann on 29/07/2006.
*  Copyright 2006 hooly|mama. All rights reserved.
*
*/

#include <maya/MFnPluginData.h>
#include <maya/MFnEnumAttribute.h>

#include <maya/MFnNumericAttribute.h>
#include <errorMacros.h>
#include <attrUtils.h>
#include <poolData.h>
#include <agentSensor.h>
#include <alignment.h>

#include "jMayaIds.h"


//////////////////////////////////////////////////////////////////////////////
// Initialisation and destruction
MObject		alignment::aMeasureVelocity;
MObject		alignment::aBlend;
MTypeId     alignment::id( k_alignment );

MStatus
alignment::initialize()
{
	MStatus st;
	MString method(" alignment::initialize");

	inheritAttributesFrom("agentSensor");

	MFnEnumAttribute eAttr;
	MFnNumericAttribute nAttr;

	aMeasureVelocity = eAttr.create( "measureVelocity", "mvl", kAbsolute, &st ); mser;
	eAttr.addField( "absolute",		kAbsolute);
	eAttr.addField( "relative",		kRelative	);
	eAttr.addField( "blend",		kBlend	);
	eAttr.setKeyable(true);
	eAttr.setStorable(true);
	st = addAttribute(aMeasureVelocity); mser;



	aBlend	= nAttr.create("relativeBlend", "rlb", MFnNumericData::kDouble, 0.0, &st); mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aBlend); mser;

	return MS::kSuccess;
}

void *alignment::creator()
{
	return new alignment;
}

//////////////////////////////////////////////////////////////////////////////
// Implementation

MStatus alignment::assess( MDataBlock &data, const poolData *pd, MVectorArray &assessment)
{

	MStatus st;
	MString method(" alignment::assess");
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
	double blend = data.inputValue(aBlend).asDouble();
	bool   useRamps = data.inputValue(aUseRampAttenuation).asBool();

	// MVectorArray assessment(len);
	int num = data.inputValue(aMaxNeighbors).asShort();

	velAlgo algo = (velAlgo)data.inputValue(aMeasureVelocity).asShort();

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
			//const double &  radius = radius;
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
			if (algo == kRelative) {
				for (AGENT_VECTOR::const_iterator currAgent = neighbors->begin();
				     currAgent != neighbors->end(); currAgent++ ) {
					c = (*currAgent)->id();

					// getRelativeVelocity of neighbor
					///////////////////
					MVector dV = velocities[c] - velocities[i];
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
					}					dV = dV.normal() * atten;
					signal += dV;
					///////////////////
					iter++;
				}
			}
			else if (algo == kAbsolute) {  // absolute
				for (AGENT_VECTOR::const_iterator currAgent = neighbors->begin();
				     currAgent != neighbors->end(); currAgent++ ) {
					c = (*currAgent)->id();

					// getRelativeVelocity of neighbor
					///////////////////
					//MVector dV = velocities[c] - velocities[i];
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
					}					MVector dV =  velocities[c].normal() * atten;
					signal += dV;
					///////////////////
					iter++;
				}
			}
			else {   //blend
				for (AGENT_VECTOR::const_iterator currAgent = neighbors->begin();
				     currAgent != neighbors->end(); currAgent++ ) {
					c = (*currAgent)->id();

					///////////////////
					MVector dV = velocities[c] - (velocities[i] * blend);
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
					}					dV = dV.normal() * atten;
					signal += dV;
					///////////////////
					iter++;
				}
			}
			signal *= acuity  ;
			assessment.set(signal, i);
			if (neighbors) {
				delete neighbors;
				neighbors = 0;
			}

		}
		//	sensor::multAssessment(data, assessment);

	}

	// MDataHandle hOut = data.outputValue(sensor::aAssessment);
	// MFnVectorArrayData fnOut;
	// MObject dOut = fnOut.create(assessment);
	// hOut.set(dOut);
	// st = data.setClean( sensor::aAssessment);mser;

	return st;
}
