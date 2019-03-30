/*
 *  goalSensor.cpp
 *  animal
 *
 *  Created by Julian Mann on 31/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
*/



#include <sensor.h>
#include <goalSensor.h>

#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>

#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnPluginData.h>

#include <errorMacros.h>
#include <attrUtils.h>

#include "jMayaIds.h"

//
MTypeId     goalSensor::id( k_goalSensor );

MObject     goalSensor::aGoalPositions;
MObject     goalSensor::aGoalWeights;
MObject     goalSensor::aDecayRampMax;


goalSensor::goalSensor() {
}

goalSensor::~goalSensor() {
}


void *goalSensor::creator()
{
	return new goalSensor();
}

MStatus goalSensor::compute( const MPlug &plug, MDataBlock &data )
{
	MStatus st;
	MString method("goalSensor::compute");
	if (plug != aAssessment)	{ return MS::kUnknownParameter; }



	short int nodeState = data.inputValue( state).asShort();
	if (nodeState)  {
		// just clear any data on the output
		MVectorArray empty;
		MDataHandle hOut = data.outputValue(sensor::aAssessment);
		MFnVectorArrayData fnOut;
		MObject dOut = fnOut.create(empty);
		hOut.set(dOut);
		st = data.setClean( sensor::aAssessment); mser;
		return ( MS::kSuccess );
	}


	unsigned len = MFnVectorArrayData(data.inputValue(sensor::aPositionPP).data()).length();
	MVectorArray assessment(len, MVector::zero);

	st = assess(data, assessment);
	// always output an assessment array the same length as points input
	st = outputAssessment(data, assessment);
	return st;


}

MStatus goalSensor::assess( MDataBlock &data,  MVectorArray &assessment )
{
	MStatus st;
	MString method(" goalSensor::assess");
	const double epsilon = 1e-6;
	// get the 3 main required vector arrays
	////////////////////////////////////////////////////
	MVectorArray  points;
	MVectorArray  velocities;
	MVectorArray  viewVectors;
	st = getRequiredSensorData(data, points, velocities, viewVectors);
	if (st.error()) { return MS::kUnknownParameter; }



	MDoubleArray maxDistPP = MFnDoubleArrayData(data.inputValue(
	                           sensor::aMaxDistancePP).data()).array();
	bool doMaxDistPP = false;
	unsigned len = points.length();
	if ((maxDistPP.length() == len) && data.inputValue(aUseMaxDistancePP).asBool())	 { doMaxDistPP = true; }
	////////////////////////////////////////////////////


	MVectorArray goalPositions = MFnVectorArrayData(data.inputValue(
	                               aGoalPositions).data()).array();
	MDoubleArray goalWeights = MFnDoubleArrayData(data.inputValue(
	                             aGoalWeights).data()).array();

	// get double arrays from the other sensor attributes
	////////////////////////////////////////////////////
	////////////////////////////////////////////////////
	double fov = data.inputValue(aFov).asDouble();
	double drp = data.inputValue(aDropOff).asDouble();
	double acuity = data.inputValue(aAcuity).asDouble();
	double dec = data.inputValue(aDecay).asDouble();
	double radius = data.inputValue(aMaxDistance).asDouble();

	double decayRampMax = data.inputValue(aDecayRampMax).asDouble();

	bool   useRamps = data.inputValue(aUseRampAttenuation).asBool();

	if (acuity == 0.0) { return MS::kSuccess; }


	unsigned i;

	MRampAttribute fovRamp( thisMObject() , aFovProfile, &st ); mser;
	MRampAttribute decRamp( thisMObject() , aDecayProfile, &st ); mser;

	unsigned glen = goalPositions.length();
	if (glen < 1 ) { return MS::kSuccess; }
	if (glen != len ) { return MS::kSuccess; }
	bool doGwPP = (goalWeights.length() == glen);
	double goalWeight = 1;


	for (i = 0; i < len; i++ ) {
		// unsigned i = i%glen;
		if (doGwPP) { goalWeight = goalWeights[i]; }

		if (goalWeight < epsilon) { continue; }
		////////////////////////////////
		MVector signal(MVector::zero);
		const MVector viewVectorN = viewVectors[i].normal();
		////////////////////////////////
		if (doMaxDistPP) { radius = maxDistPP[i]; }
		const MVector &p =  points[i];
		const MVector &g = goalPositions[i];

		MVector diff = g - p;
		double dist = diff.length();

		if (dist > radius) { continue; }


		double atten;
		if (useRamps) {
			atten = attenuateSense(
			          p, viewVectorN, fovRamp, decRamp,
			          g, dist, decayRampMax
			        );
		}
		else {
			atten = attenuateSense(
			          p, viewVectorN, fov, drp, dec,
			          g, dist, radius
			        );
		}

		if (fabs(atten) > epsilon) {
			signal = diff.normal() * atten * acuity * goalWeight ;
			assessment.set(signal, i);
		}
	}

	return st;


}



MStatus goalSensor::initialize()
{
	MStatus st;

	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;

	inheritAttributesFrom("sensor");




	aDecayRampMax	= nAttr.create("decayRampMax", "drmx", MFnNumericData::kDouble, 1, &st);
	mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aDecayRampMax); mser;

	aGoalPositions = tAttr.create("goalPositions", "gpos", MFnData::kVectorArray, &st ); mser;
	tAttr.setStorable(false);
	tAttr.setConnectable(true);
	st = addAttribute(aGoalPositions); mser;

	aGoalWeights = tAttr.create("goalWeights", "gwt", MFnData::kDoubleArray, &st ); mser;
	tAttr.setStorable(false);
	tAttr.setConnectable(true);
	st = addAttribute(aGoalWeights); mser;







	attributeAffects( aGoalWeights       , aAssessment	);
	attributeAffects( aDecayRampMax       , aAssessment	);
	attributeAffects( aGoalPositions	, aAssessment	);


	return MS::kSuccess;
}


