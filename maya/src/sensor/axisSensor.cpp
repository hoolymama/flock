/*
 *  axisSensor.cpp
 *  animal
 *
 *  Created by Julian Mann on 30/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */

#include <maya/MFnPluginData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>

#include <errorMacros.h>
#include <attrUtils.h>
#include <poolData.h>
#include <agentSensor.h>
#include <axisSensor.h>
#include "jMayaIds.h"
//////////////////////////////////////////////////////////////////////////////
// Initialisation and destruction


MObject axisSensor::aAxisPP ;
MObject axisSensor::aPointNormal;
MObject axisSensor::aAxisNormal ;
MObject axisSensor::aAxisDirection;
MObject axisSensor::aAxisOrbit ;
MObject axisSensor::aEasyOrbit ;

MTypeId     axisSensor::id( k_axisSensor );

MStatus
axisSensor::initialize()
{
	inheritAttributesFrom("agentSensor");

	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;
	MStatus st;
	aAxisPP = tAttr.create("axisPP", "axpp", MFnData::kVectorArray, &st ); mser;
	tAttr.setStorable(false);
	tAttr.setConnectable(true);
	st = addAttribute(aAxisPP); mser;

	aPointNormal	= nAttr.create("pointNormal", "pno", MFnNumericData::kDouble, 1.0, &st);
	mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aPointNormal); mser;
	//--------------

	aAxisNormal	= nAttr.create("axisNormal", "ano", MFnNumericData::kDouble, 1.0, &st); mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aAxisNormal); mser;
	//--------------

	aAxisDirection	= nAttr.create("axisDirection", "adr", MFnNumericData::kDouble, 1.0, &st);
	mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aAxisDirection); mser;
	//--------------

	aAxisOrbit	= nAttr.create("axisOrbit", "aor", MFnNumericData::kDouble, 1.0, &st); mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aAxisOrbit); mser;
	//--------------

	aEasyOrbit =  nAttr.create( "easyOrbit", "eob", MFnNumericData::kBoolean);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(true);
	st =	addAttribute(aEasyOrbit);	mser;

	return MS::kSuccess;
}

void *axisSensor::creator()
{
	return new axisSensor;
}

//////////////////////////////////////////////////////////////////////////////
// Implementation

MStatus axisSensor::assess(MDataBlock &data, const poolData *pd, MVectorArray &assessment)
{

	MStatus st;
	MString method(" axisSensor::assess");

	// get the 3 main required vector arrays
	////////////////////////////////////////////////////
	MVectorArray  points;
	MVectorArray  velocities;
	MVectorArray  viewVectors;
	st = getRequiredSensorData(data, points, velocities, viewVectors); msert;

	MDoubleArray maxDistPP = MFnDoubleArrayData(data.inputValue(
	                           sensor::aMaxDistancePP).data()).array();
	MVectorArray axisPP = MFnVectorArrayData(data.inputValue(aAxisPP).data()).array();


	bool doEasyOrbit = data.inputValue(aEasyOrbit).asBool();

	bool doAxisPP = false;
	bool doMaxDistPP = false;

	unsigned len = points.length();
	if ((maxDistPP.length() == len) && data.inputValue(aUseMaxDistancePP).asBool())	 { doMaxDistPP = true; }
	if (axisPP.length() == len)	 { doAxisPP = true; }
	////////////////////////////////////////////////////
	double fov = data.inputValue(aFov).asDouble();
	double drp = data.inputValue(aDropOff).asDouble();
	double acuity = data.inputValue(aAcuity).asDouble();
	double dec = data.inputValue(aDecay).asDouble();
	double radius = data.inputValue(aMaxDistance).asDouble();

	double pointNormVal = data.inputValue(aPointNormal).asDouble();
	double axisNormVal =  data.inputValue(aAxisNormal).asDouble();
	double axisDirVal =  data.inputValue(aAxisDirection).asDouble();
	double axisOrbitVal = data.inputValue(aAxisOrbit).asDouble();

	////////////////////////////////
	bool   useRamps = data.inputValue(aUseRampAttenuation).asBool();

	//	MVectorArray assessment(len);
	int num = data.inputValue(aMaxNeighbors).asShort();

	if (num && (0.0 != acuity) && (0.0 != radius) && (0.0 < fov ) && (doAxisPP)) {
		const AGENT_VECTOR *allAgents = pd->tree()->perm();
		unsigned i;


		MRampAttribute fovRamp( thisMObject() , aFovProfile, &st ); mser;
		MRampAttribute decRamp( thisMObject() , aDecayProfile, &st ); mser;

		for (AGENT_VECTOR::const_iterator searchAgent = allAgents->begin();
		     searchAgent != allAgents->end(); searchAgent++ ) {
			i = (*searchAgent)->id();


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

				// getRelativePosition of neighbor (vector points towards us)
				///////////////////
				MVector vPointNorm = (points[i] - points[c]).normal() ;
				MVector vAxisOrbit = (vPointNorm ^ axisPP[i]).normal();
				MVector vAxisNorm = ( axisPP[i] ^ vAxisOrbit).normal();
				MVector vAxisDir = axisPP[i].normal();

				if (doEasyOrbit) {
					double dot = viewVectors[i] * vAxisOrbit;
					if ( dot < 0 )  { vAxisOrbit = -vAxisOrbit; }
				}
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

				signal += ( (vPointNorm * pointNormVal) + (vAxisOrbit * axisOrbitVal) +
				            (vAxisDir * axisDirVal) + (vAxisNorm * axisNormVal) ) * atten ;
				///////////////////

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
