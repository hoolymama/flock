/*
 *  curvePoints.cpp
 *  feather
 *
 *  Created by Julian Mann on 26/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */



#include <math.h>

#include <maya/MDoubleArray.h>
#include <maya/MVectorArray.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MFnNurbsCurveData.h>
#include <maya/MString.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>

#include <maya/MFnCompoundAttribute.h>

#include <maya/MFnVectorArrayData.h>

#include <maya/MFnEnumAttribute.h> 

#include "curvePoints.h"
#include "jMayaIds.h"


#include "errorMacros.h"

#include "mayaMath.h"



// #define ONE_THIRD 0.333333333333


MObject curvePoints::aCurve;
MObject curvePoints::aNumPoints;
MObject curvePoints::aCurves;

MObject curvePoints::aSpacing;
MObject curvePoints::aOutPoints;


MTypeId curvePoints::id( k_curvePoints );

const double epsilon = 0.0001;

curvePoints::curvePoints() {}

curvePoints::~curvePoints() {}

void* curvePoints::creator(){
	return new curvePoints();
}

MStatus curvePoints::initialize()
{
	MStatus st;
	
	MString method("curvePoints::initialize");
	
	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnEnumAttribute eAttr;
	MFnCompoundAttribute cAttr;
	
	aCurve = tAttr.create( "curve", "crv", MFnNurbsCurveData::kNurbsCurve, &st); er;
	tAttr.setReadable(false);
	tAttr.setStorable(false);

	aNumPoints = nAttr.create("numPoints","np", MFnNumericData::kLong);
	nAttr.setHidden( false );
	nAttr.setKeyable( true );
	nAttr.setDefault( 10 );
		////////////////////////////////////
	aCurves = cAttr.create("curves", "crvs");
	cAttr.addChild(aCurve);
	cAttr.addChild(aNumPoints);
	cAttr.setArray( true );	
	cAttr.setDisconnectBehavior(MFnAttribute::kDelete);
	cAttr.setReadable(false);	
	st = addAttribute( aCurves );er;	
	
	aSpacing = eAttr.create("spacing", "spc");
	eAttr.addField("Parametric", kParametric);
	eAttr.addField("ArcLength", kArcLen);
	eAttr.setKeyable(true);
	eAttr.setWritable(true);
	addAttribute(aSpacing);

	aOutPoints = tAttr.create("outPoints", "opts", MFnData::kVectorArray, &st);er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	st = addAttribute(aOutPoints );er;

	st =attributeAffects(aCurve,aOutPoints );er;
	st =attributeAffects(aCurves,aOutPoints );er;
	st =attributeAffects(aNumPoints,aOutPoints );er;
	st =attributeAffects(aSpacing,aOutPoints );er;
	
	return( MS::kSuccess );   
}


MStatus curvePoints::compute( const MPlug& plug, MDataBlock& data )
{   
	MStatus st;
	MString method("curvePoints::compute");
	
	if(!(plug == aOutPoints)) return( MS::kUnknownParameter );
	

	MArrayDataHandle hCurves = data.inputArrayValue(aCurves, &st );
	MVectorArray outPoints;	
	
	Spac spacing = (Spac)data.inputValue(aSpacing).asShort();	

	unsigned n = hCurves.elementCount();
	for(unsigned i = 0;i < n; i++, hCurves.next()) {
		MDataHandle hComp = hCurves.inputValue(&st );
		if (st.error()) continue;

		int npts =  hComp.child(curvePoints::aNumPoints).asLong() ;
		MObject  dCurve =  hComp.child(curvePoints::aCurve).asNurbsCurveTransformed();
		MFnNurbsCurve curveFn(dCurve);


		MFnNurbsCurve::Form form = curveFn.form();


		MDoubleArray fractions(npts);
		if (npts == 1) {
			fractions.set(0.5,0);
		} else {
			if (curveFn.form() == MFnNurbsCurve::kOpen) {
				double recip = 1.0 / (npts -1);
				for (unsigned i = 0;i<npts;i++) {
					fractions.set((double(i) * recip),i);
				}
			} else {
				double recip = 1.0 / (npts);
				for (unsigned i = 0;i<npts;i++) {
					fractions.set((i * recip),i);
				}
			}
		}


		MPoint resultPoint;
		double param;
		if(spacing == kParametric)  {
			double startParam, endParam,rangeParam;
			curveFn.getKnotDomain(startParam, endParam);

			rangeParam =  endParam - startParam;
			for (unsigned i = 0;i<npts;i++) {
				param = (fractions[i] * rangeParam ) + startParam;
				st = curveFn.getPointAtParam(param,resultPoint, MSpace::kObject); 
				outPoints.append( MVector(resultPoint));
			}
		} else {
			double arclen = curveFn.length(epsilon, &st); er;
			for (unsigned i = 0;i<npts;i++) {
				double fracLen = fractions[i] * arclen;
				param = curveFn.findParamFromLength(fracLen, &st); er;
				st = curveFn.getPointAtParam(param,resultPoint, MSpace::kObject); 
				outPoints.append( MVector(resultPoint));

			}
		}
	}

	MDataHandle hOutPoints = data.outputValue(aOutPoints);
	MFnVectorArrayData fnOutPoints;
	MObject dOutPoints = fnOutPoints.create(outPoints);
	hOutPoints.set(dOutPoints);
	st = data.setClean( aOutPoints );er;

	return MS::kSuccess;
}


