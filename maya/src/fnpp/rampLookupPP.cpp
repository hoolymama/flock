/*
 *  rampLookup.cpp
 *  jtools
 *
 *  Created by Julian Mann on 8/24/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */




#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnTypedAttribute.h>

#include <maya/MFnDoubleArrayData.h>
#include <maya/MDoubleArray.h>
#include <maya/MVectorArray.h>
#include <maya/MGlobal.h>
#include <maya/MRampAttribute.h>
#include <maya/MFnNumericAttribute.h>


#include <maya/MFloatArray.h>
#include "errorMacros.h"
#include "attrUtils.h"
#include "rampLookupPP.h"
#include "jMayaIds.h"

MTypeId rampLookupPP::id(k_rampLookupPP);

MObject rampLookupPP::aInputMin;
MObject rampLookupPP::aInputMax;
MObject rampLookupPP::aOutputMin;
MObject rampLookupPP::aOutputMax;


MObject rampLookupPP::aInput;
MObject rampLookupPP::aRamp;
MObject rampLookupPP::aOutput;


rampLookupPP::rampLookupPP () {}
rampLookupPP::~rampLookupPP () {}

void *rampLookupPP::creator () {
	return new rampLookupPP;
}
/// Post constructor
void
rampLookupPP::postConstructor()
{

}

MStatus rampLookupPP::initialize () {

	MStatus st;
	MFnTypedAttribute tAttr;

	MRampAttribute rAttr;
	MFnNumericAttribute nAttr;

	aInputMin = nAttr.create("inputMin", "imn", MFnNumericData::kFloat);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(0.0);
	st = addAttribute(aInputMin); mser;

	aInputMax = nAttr.create("inputMax", "imx", MFnNumericData::kFloat);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	st = addAttribute(aInputMax); mser;

	aOutputMin = nAttr.create("outputMin", "omn", MFnNumericData::kFloat);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(0.0);
	st = addAttribute(aOutputMin); mser;

	aOutputMax = nAttr.create("outputMax", "omx", MFnNumericData::kFloat);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	st = addAttribute(aOutputMax); mser;

	aRamp = rAttr.createCurveRamp("ramp", "rmp", &st);
	st = addAttribute(aRamp ); mser;

	aInput = tAttr.create("input", "in", MFnData::kDoubleArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	st = addAttribute(aInput); mser;

	aOutput = tAttr.create ("output", "out", MFnData::kDoubleArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	st = addAttribute (aOutput); mser;

	attributeAffects (aInputMin, aOutput);
	attributeAffects (aInputMax, aOutput);
	attributeAffects (aOutputMin, aOutput);
	attributeAffects (aOutputMax, aOutput);
	attributeAffects (aInput, aOutput);
	attributeAffects (aRamp, aOutput);

	return MS::kSuccess;
}


MStatus rampLookupPP::compute (const MPlug &plug, MDataBlock &data)
{

	if (!(plug == aOutput )) { return MS::kUnknownParameter; }

	MStatus st;



	//cerr << "about to create ramp attr in compute " << endl;
	//	MObject tob = thisMObject();
	//	MRampAttribute rampAttr(tob, aRamp,  &st);mser;
	//cerr << "created ramp attr in compute " << endl;
	/*
	MIntArray entries;
	MFloatArray positions;
	MFloatArray values;
	MIntArray interps;

	rampAttr.getEntries(entries,positions,values,interps);

	cerr << "entries " << entries << endl;
	cerr << "positions " << positions << endl;
	cerr << "values " << values << endl;
	cerr << "interps " << interps << endl;
	*/
	double imn = data.inputValue(aInputMin).asFloat();
	double imx = data.inputValue(aInputMax).asFloat();
	double omn = data.inputValue(aOutputMin).asFloat();
	double omx = data.inputValue(aOutputMax).asFloat();

	MDataHandle hIn = data.inputValue(aInput);
	MObject objIn = hIn.data();
	MDoubleArray vals =  MFnDoubleArrayData(objIn).array();
	unsigned nVals = vals.length();
	MDoubleArray out(nVals);



	//JPMDBG;
	doRampLookup(thisMObject(), aRamp, vals, out, float(imn), float(imx), float(omn),
	             float(omx) );
	//JPMDBG;


	//   //cerr << "before loop " << nVals << endl;
	//   for (unsigned i = 0;i<nVals;i++) {
	//   	float newVal;
	//   	float f = float(vals[i]);
	//   	rampAttr.getValueAtPosition(f,newVal, &st);mser;
	//   	out.append(double(newVal));
	//   }
	//   //cerr << "done loop" << endl;


	MDataHandle hOut = data.outputValue(aOutput);
	MFnDoubleArrayData fnOut;
	MObject objOut = fnOut.create(out);
	hOut.set(objOut);
	data.setClean(plug);
	//cerr << "done compute" << endl;

	return MS::kSuccess;
}
