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
#include "speedRampLookup.h"
#include "jMayaIds.h"

MTypeId speedRampLookupPP::id(k_speedRampLookupPP);

MObject speedRampLookupPP::aInputMin;
MObject speedRampLookupPP::aInputMax;
MObject speedRampLookupPP::aOutputMin;
MObject speedRampLookupPP::aOutputMax;

MObject speedRampLookupPP::aInput1;
MObject speedRampLookupPP::aInput2;
MObject speedRampLookupPP::aRamp;
MObject speedRampLookupPP::aOutput;


speedRampLookupPP::speedRampLookupPP () {}
speedRampLookupPP::~speedRampLookupPP () {}

void *speedRampLookupPP::creator () {
	return new speedRampLookupPP;
}
/// Post constructor
void
speedRampLookupPP::postConstructor()
{

}

MStatus speedRampLookupPP::initialize () {

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

	aInput1 = tAttr.create("input1", "in1", MFnData::kVectorArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	st = addAttribute(aInput1); mser;

	aInput2 = tAttr.create("input2", "in2", MFnData::kVectorArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	st = addAttribute(aInput2); mser;

	aOutput = tAttr.create ("output", "out", MFnData::kDoubleArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	st = addAttribute (aOutput); mser;

	attributeAffects (aInputMin, aOutput);
	attributeAffects (aInputMax, aOutput);
	attributeAffects (aOutputMin, aOutput);
	attributeAffects (aOutputMax, aOutput);
	attributeAffects (aInput1, aOutput);
	attributeAffects (aInput2, aOutput);
	attributeAffects (aRamp, aOutput);

	return MS::kSuccess;
}


MStatus speedRampLookupPP::compute (const MPlug &plug, MDataBlock &data)
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

	MDataHandle hIn1 = data.inputValue(aInput1);
	MObject objIn1 = hIn1.data();
	MVectorArray vals1 =  MFnVectorArrayData(objIn1).array();
	unsigned nVals = vals1.length();



	MDataHandle hIn2 = data.inputValue(aInput2);
	MObject objIn2 = hIn2.data();
	MVectorArray vals2 =  MFnVectorArrayData(objIn2).array();

	MDoubleArray vals(nVals);

	if (nVals ==  vals2.length()) {
		for (int i = 0; i < nVals; ++i)
		{
			vals[i] = (vals1[i] - vals2[i]).length();
		}
	}
	else {
		for (int i = 0; i < nVals; ++i)
		{
			vals[i] = vals1[i].length();
		}
	}
	MDoubleArray out;

	doRampLookup(thisMObject(), aRamp, vals, out, float(imn), float(imx), float(omn),
	             float(omx) );




	MDataHandle hOut = data.outputValue(aOutput);
	MFnDoubleArrayData fnOut;
	MObject objOut = fnOut.create(out);
	hOut.set(objOut);
	data.setClean(plug);
	//cerr << "done compute" << endl;

	return MS::kSuccess;
}
