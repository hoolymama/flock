/*
 *  splineSensor.cpp
 *  animal
 *
 *  Created by Julian Mann on 31/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
*/



#include <sensor.h>
#include <splineSensor.h>

#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>

#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnPluginData.h>

#include <errorMacros.h>
#include <attrUtils.h>
#include <lookup.h>
// #include <maya/MTime.h>
#include <splinePoolData.h>
#include "jMayaIds.h"

//
MTypeId     splineSensor::id( k_splineSensor );

MObject     splineSensor::aTangent;
MObject     splineSensor::aNormal;
MObject     splineSensor::aOrbital;
MObject     splineSensor::aTangentFalloff;
MObject     splineSensor::aNormalFalloff;
MObject     splineSensor::aOrbitalFalloff;
MObject     splineSensor::aTangentParam;
MObject     splineSensor::aNormalParam;
MObject     splineSensor::aOrbitalParam;

MObject     splineSensor::aSplinePool;				//

splineSensor::splineSensor() {
}

splineSensor::~splineSensor() {
}


void *splineSensor::creator()
{
	return new splineSensor();
}

MStatus splineSensor::compute( const MPlug &plug, MDataBlock &data )
{
	MStatus st;
	MString method("splineSensor::compute");
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

	// get the pool
	////////////////////////////////////////////////////
	splinePoolData *sd = 0;
	MDataHandle h = data.inputValue(splineSensor::aSplinePool, &st);
	if (! st.error()) {
		MObject d = h.data();
		MFnPluginData fn( d , &st);
		if (! st.error()) {
			sd =  (splinePoolData *)fn.data();
			if (sd->size() ) { st = assess(data, sd, assessment); }
		}
	}
	////////////////////////////////////////////////////

	// always output an assessment array the same length as points input
	st = outputAssessment(data, assessment);
	return st;


}

MStatus splineSensor::assess( MDataBlock &data, const splinePoolData *sd ,
                              MVectorArray &assessment )
{
	MStatus st;
	MString method(" splineSensor::assess");
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

	// get double arrays from the other sensor attributes
	////////////////////////////////////////////////////
	////////////////////////////////////////////////////
	double fov = data.inputValue(aFov).asDouble();
	double drp = data.inputValue(aDropOff).asDouble();
	double acuity = data.inputValue(aAcuity).asDouble();
	double dec = data.inputValue(aDecay).asDouble();
	double radius = data.inputValue(aMaxDistance).asDouble();

	double tan = data.inputValue(aTangent).asDouble();
	double nor = data.inputValue(aNormal).asDouble();
	double orb = data.inputValue(aOrbital).asDouble();
	bool   useRamps = data.inputValue(aUseRampAttenuation).asBool();


	////////////////////////////////////////////
	lookup tanLut;
	MFnAnimCurve tanAniFn;
	st = getAniCurveFn(thisMObject(), aTangentFalloff, tanAniFn);
	if (!(st.error())) {
		tanLut.create(tanAniFn, 15);
	}

	lookup norLut;
	MFnAnimCurve norAniFn;
	st = getAniCurveFn(thisMObject(), aNormalFalloff, norAniFn);
	if (!(st.error())) {
		norLut.create(norAniFn, 15);
	}
	lookup orbLut;
	MFnAnimCurve orbAniFn;
	st = getAniCurveFn(thisMObject(), aOrbitalFalloff, orbAniFn);
	if (!(st.error())) {
		orbLut.create(orbAniFn, 15);
	}
	////////////////////////////////////////////


	////////////////////////////////////////////
	lookup tanParamLut;
	MFnAnimCurve tanParamAniFn;
	st = getAniCurveFn(thisMObject(), aTangentParam, tanParamAniFn);
	if (!(st.error())) {
		tanParamLut.create(tanParamAniFn, 15);
	}

	lookup norParamLut;
	MFnAnimCurve norParamAniFn;
	st = getAniCurveFn(thisMObject(), aNormalParam, norParamAniFn);
	if (!(st.error())) {
		norParamLut.create(norParamAniFn, 15);
	}
	lookup orbParamLut;
	MFnAnimCurve orbParamAniFn;
	st = getAniCurveFn(thisMObject(), aOrbitalParam, orbParamAniFn);
	if (!(st.error())) {
		orbParamLut.create(orbParamAniFn, 15);
	}
	////////////////////////////////////////////




	// MVectorArray assessment(len);

	double tanMag, norMag, orbMag;

	unsigned i;

	MRampAttribute fovRamp( thisMObject() , aFovProfile, &st ); mser;
	MRampAttribute decRamp( thisMObject() , aDecayProfile, &st ); mser;

	for (i = 0; i < len; i++ ) {


		if (acuity == 0.0) { continue; }

		// do the search for this point
		////////////////////////////////

		////////////////////////////////
		MVector signal(MVector::zero);
		const MVector viewVectorN = viewVectors[i].normal();
		////////////////////////////////

		MPoint c = MPoint(points[i]);

		splineData tb;
		if (doMaxDistPP) { radius = maxDistPP[i]; }
		double  searchRadius = radius; // not ref - as it will shrink
		sd->tree()->closestLine( sd->tree()->root(), c , searchRadius,  tb);
		if (searchRadius < radius) {  // we found a line
			//	cerr << "U Coords " <<  tb.uCoords()  << endl;
			float dist;
			const MVector &vhit =  tb.cachedPoint();
			const double uVal = tb.calcU();
			// double atten = attenuateSense(points[i],viewVectorN,fov,drp,dec,vhit,searchRadius,radius);
			double atten;
			if (useRamps) {
				atten = attenuateSense(
				          points[i], viewVectorN, fovRamp, decRamp,
				          vhit, searchRadius, radius
				        );
			}
			else {
				atten = attenuateSense(
				          points[i], viewVectorN, fov, drp, dec,
				          vhit, searchRadius, radius
				        );
			}
			if (fabs(atten) > epsilon) {
				MVector vnorm = (c - vhit);
				dist = float(vnorm.length());
				vnorm.normalize();
				MVector vorbital = (tb.tangent()^vnorm).normal();

				// alternative that will act differently at the end of the line
				// MVector norm = circular^tan;

				tanMag = tan * tanLut.evaluate(dist) * tanParamLut.evaluate(uVal);
				norMag = nor * norLut.evaluate(dist) * norParamLut.evaluate(uVal);;
				orbMag = orb * orbLut.evaluate(dist) * orbParamLut.evaluate(uVal);;

				signal = (tb.tangent() * tanMag) + (vnorm * norMag) + (vorbital * orbMag);

				// now attenuate this vector
				signal = signal * atten * acuity ;

				assessment.set(signal, i);
			}
		}
	}

	// sensor::multAssessment(data, assessment);
	//
	// MDataHandle hOut = data.outputValue(sensor::aAssessment);
	// MFnVectorArrayData fnOut;
	// MObject dOut = fnOut.create(assessment);
	// hOut.set(dOut);
	// st = data.setClean( sensor::aAssessment);mser;

	return st;


}



MStatus splineSensor::initialize()
{
	MStatus st;

	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;

	inheritAttributesFrom("sensor");

	aTangent	= nAttr.create("tangent", "tan", MFnNumericData::kDouble, 1.0, &st); mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aTangent); mser;

	aNormal	= nAttr.create("normal", "nor", MFnNumericData::kDouble, 0.0, &st); mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aNormal); mser;

	aOrbital	= nAttr.create("orbital", "orb", MFnNumericData::kDouble, 0.0, &st); mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aOrbital); mser;

	aTangentFalloff	= nAttr.create("tangentFalloff", "tanf", MFnNumericData::kDouble, 1.0,
	                               &st); mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aTangentFalloff); mser;

	aNormalFalloff	= nAttr.create("normalFalloff", "norf", MFnNumericData::kDouble, 0.0, &st);
	mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aNormalFalloff); mser;

	aOrbitalFalloff	= nAttr.create("orbitalFalloff", "orbf", MFnNumericData::kDouble, 0.0,
	                               &st); mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aOrbitalFalloff); mser;




	aTangentParam	= nAttr.create("tangentParam", "tanp", MFnNumericData::kDouble, 1.0, &st);
	mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aTangentParam); mser;

	aNormalParam	= nAttr.create("normalParam", "norp", MFnNumericData::kDouble, 0.0, &st);
	mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aNormalParam); mser;

	aOrbitalParam	= nAttr.create("orbitalParam", "orbp", MFnNumericData::kDouble, 0.0, &st);
	mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aOrbitalParam); mser;

	aSplinePool = tAttr.create("splinePool", "spl", splinePoolData::id ) ; mser;
	tAttr.setStorable(false);
	tAttr.setCached(false);
	st = addAttribute( aSplinePool ); mser;

	attributeAffects( aSplinePool, aAssessment	);
	attributeAffects( aTangent, aAssessment	);
	attributeAffects( aNormal, aAssessment	);
	attributeAffects( aOrbital, aAssessment	);
	attributeAffects( aTangentFalloff, aAssessment	);
	attributeAffects( aNormalFalloff, aAssessment	);
	attributeAffects( aOrbitalFalloff, aAssessment	);

	return MS::kSuccess;
}


