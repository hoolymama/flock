/*
*  sensor.cpp
*  animal
*mser
*  Created by Julian Mann on 29/07/2006.
*  Copyright 2006 hooly|mama. All rights reserved.
*
*/


#include <maya/MFnTypedAttribute.h>
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>

#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <poolData.h>

#include <errorMacros.h>
#include <sensor.h>
#include "jMayaIds.h"



MTypeId     sensor::id( k_sensor );


MObject     sensor::aPositionPP;
MObject 	sensor::aVelocityPP;
MObject 	sensor::aViewVectorPP;

MObject     sensor::aMaxDistancePP;
MObject     sensor::aUseMaxDistancePP;
MObject 	sensor::aFov;
MObject 	sensor::aDropOff;
MObject     sensor::aAcuity;
MObject     sensor::aDecay;
MObject     sensor::aUseRampAttenuation;
MObject     sensor::aFovProfile;
MObject     sensor::aDecayProfile;




MObject     sensor::aMaxDistance;

MObject     sensor::aAssessmentMultX;
MObject     sensor::aAssessmentMultY;
MObject     sensor::aAssessmentMultZ;


MObject     sensor::aAssessment;


sensor::sensor() {
}

sensor::~sensor() {
}


void *sensor::creator()
{
	return new sensor();
}


void sensor::multAssessment(MDataBlock &data, MVectorArray &assessment)
{

	double mx = data.inputValue(aAssessmentMultX).asDouble();
	double my = data.inputValue(aAssessmentMultY).asDouble();
	double mz = data.inputValue(aAssessmentMultZ).asDouble();
	unsigned len = assessment.length();
	if (mx != 1.0) {
		if (mx == 0.0) {
			for (unsigned i = 0; i < len; i++ ) { assessment[i].x = 0.0; }
		}
		else {
			for (unsigned i = 0; i < len; i++ ) { assessment[i].x *= mx; }
		}
	}

	if (my != 1.0) {
		if (my == 0.0) {
			for (unsigned i = 0; i < len; i++ ) { assessment[i].y = 0.0; }
		}
		else {
			for (unsigned i = 0; i < len; i++ ) { assessment[i].y *= my; }
		}
	}

	if (mz != 1.0) {
		if (mz == 0.0) {
			for (unsigned i = 0; i < len; i++ ) { assessment[i].z = 0.0; }
		}
		else {
			for (unsigned i = 0; i < len; i++ ) { assessment[i].z *= mz; }
		}
	}
}


MStatus sensor::outputAssessment(MDataBlock &data, MVectorArray &assessment)
{
	MStatus st;
	multAssessment(data, assessment);

	MDataHandle hOut = data.outputValue(sensor::aAssessment);
	MFnVectorArrayData fnOut;
	MObject dOut = fnOut.create(assessment);
	hOut.set(dOut);
	st = data.setClean( sensor::aAssessment);
	return ( st );

}




MStatus sensor::assess(MDataBlock &data, MVectorArray &assessment)
{
	return MS::kSuccess;
}


MStatus sensor::compute( const MPlug &plug, MDataBlock &data )
{

	MStatus st;

	if (plug != aAssessment) 	{ return MS::kUnknownParameter; }

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


	// always output an assessment array the same length as points input
	unsigned len = MFnVectorArrayData(data.inputValue(sensor::aPositionPP).data()).length();
	MVectorArray assessment(len, MVector::zero);
	st = assess(data, assessment);
	st = outputAssessment(data, assessment);
	return st;

}


MStatus sensor::initialize()

//
{
	MStatus st;


	MFnTypedAttribute tAttr;
	MFnUnitAttribute uAttr;
	MFnNumericAttribute nAttr;
	MFnEnumAttribute eAttr;
	MRampAttribute rAttr;


	aPositionPP = tAttr.create("positionPP", "posp", MFnData::kVectorArray);
	tAttr.setStorable(false);
	tAttr.setConnectable(true);
	st = addAttribute(aPositionPP); mser;

	aVelocityPP = tAttr.create("velocityPP", "velp", MFnData::kVectorArray);
	tAttr.setStorable(false);
	tAttr.setConnectable(true);
	st = addAttribute(aVelocityPP); mser;

	aViewVectorPP = tAttr.create("viewVectorPP", "vvp", MFnData::kVectorArray);
	tAttr.setStorable(false);
	tAttr.setConnectable(true);
	st = addAttribute(aViewVectorPP); mser;

	aMaxDistancePP	= tAttr.create("maxDistancePP", "mxdp", MFnData::kDoubleArray);
	tAttr.setStorable(false);
	tAttr.setConnectable(true);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	st = addAttribute(aMaxDistancePP); mser;

	aUseMaxDistancePP = nAttr.create( "useMaxDistancePP", "uxdp", MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(false);
	addAttribute( aUseMaxDistancePP );

	aFov	= nAttr.create("fieldOfView", "fov", MFnNumericData::kDouble, 1.0, &st); mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aFov); mser;

	aDropOff = nAttr.create("dropOff", "drp", MFnNumericData::kDouble , 1.0, &st); mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aDropOff); mser;

	aAcuity	= nAttr.create("acuity", "acu", MFnNumericData::kDouble , 1.0, &st  ); mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);

	st = addAttribute(aAcuity); mser;

	aDecay	= nAttr.create("decay", "dec", MFnNumericData::kDouble, 1.0, &st    ); mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aDecay); mser;

	aUseRampAttenuation	= nAttr.create("useRampAttenuation", "ura", MFnNumericData::kBoolean,
	                                   false, &st    ); mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aUseRampAttenuation); mser;


	aFovProfile = rAttr.createCurveRamp("fovProfile", "fpr", &st); mser;
	st = addAttribute(aFovProfile ); mser;

	aDecayProfile = rAttr.createCurveRamp("decayProfile", "dpr", &st); mser;
	st = addAttribute(aDecayProfile ); mser;


	aMaxDistance	= nAttr.create("maxDistance", "mxd", MFnNumericData::kDouble, 1.0, &st );
	mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aMaxDistance); mser;


	aAssessmentMultX = nAttr.create ("multX", "mx", MFnNumericData::kDouble);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(1.0);

	aAssessmentMultY = nAttr.create ("multY", "my", MFnNumericData::kDouble);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(1.0);

	aAssessmentMultZ = nAttr.create ("multZ", "mz", MFnNumericData::kDouble);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(1.0);

	st = addAttribute( aAssessmentMultX 	); mser;
	st = addAttribute( aAssessmentMultY 	); mser;
	st = addAttribute( aAssessmentMultZ 	); mser;

	aAssessment = tAttr.create("assessment", "ass", MFnData::kVectorArray);
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(true);
	st = addAttribute( aAssessment 	); mser;

	attributeAffects( aPositionPP	   	, aAssessment	);
	attributeAffects( aVelocityPP	  , aAssessment   );
	attributeAffects( aViewVectorPP 	  , aAssessment );

	return MS::kSuccess;
}


