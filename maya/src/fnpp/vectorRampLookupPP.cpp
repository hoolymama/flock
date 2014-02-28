/*
 *  rampLookup.cpp
 *  jtools
 *
 *  Created by Julian Mann on 8/24/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */




#include <maya/MFnTypedAttribute.h>

#include <maya/MFnVectorArrayData.h>
#include <maya/MDoubleArray.h>
#include <maya/MVectorArray.h>
#include <maya/MGlobal.h>
#include <maya/MRampAttribute.h>
#include <maya/MFnNumericAttribute.h>


#include "errorMacros.h"
#include "attrUtils.h"
#include "vectorRampLookupPP.h"
#include "jMayaIds.h"

MTypeId vectorRampLookupPP::id(k_vectorRampLookupPP);

MObject vectorRampLookupPP::aInputMin;
MObject vectorRampLookupPP::aInputMax;
MObject vectorRampLookupPP::aOutputMin;
MObject vectorRampLookupPP::aOutputMax;


MObject vectorRampLookupPP::aInput;
MObject vectorRampLookupPP::aRamp;
MObject vectorRampLookupPP::aOutput;


vectorRampLookupPP::vectorRampLookupPP () {}
vectorRampLookupPP::~vectorRampLookupPP () {}

void * vectorRampLookupPP::creator () {
	return new vectorRampLookupPP;
}
/// Post constructor
void
vectorRampLookupPP::postConstructor()
{
	
}

MStatus vectorRampLookupPP::initialize () {
	
	MStatus st;
	MFnTypedAttribute tAttr;
	
	MRampAttribute rAttr;
	MFnNumericAttribute nAttr;
	
	aInputMin = nAttr.create("inputMin", "imn", MFnNumericData::kFloat);	
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(0.0);
	st = addAttribute(aInputMin);er;
	
	aInputMax = nAttr.create("inputMax", "imx", MFnNumericData::kFloat);	
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	st = addAttribute(aInputMax);er;
	
	aOutputMin = nAttr.create("outputMin", "omn", MFnNumericData::kFloat);	
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(0.0);
	st = addAttribute(aOutputMin);er;
	
	aOutputMax = nAttr.create("outputMax", "omx", MFnNumericData::kFloat);	
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	st = addAttribute(aOutputMax);er;

	aRamp = rAttr.createCurveRamp("ramp","rmp",&st);
	st = addAttribute(aRamp );er;
	
	aInput = tAttr.create("input", "in",MFnData::kVectorArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	st = addAttribute(aInput);er;

	aOutput = tAttr.create ("output", "out",MFnData::kVectorArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	st = addAttribute (aOutput);er;

	attributeAffects (aInputMin, aOutput);
	attributeAffects (aInputMax, aOutput);
	attributeAffects (aOutputMin, aOutput);
	attributeAffects (aOutputMax, aOutput);
	attributeAffects (aInput, aOutput);
	attributeAffects (aRamp, aOutput);

	return MS::kSuccess;
}


MStatus vectorRampLookupPP::compute (const MPlug& plug, MDataBlock& data)
{

	if(!(plug == aOutput )) return MS::kUnknownParameter;
	
	MStatus st;

	double imn = data.inputValue(aInputMin).asFloat();
	double imx = data.inputValue(aInputMax).asFloat();
	double omn = data.inputValue(aOutputMin).asFloat();
	double omx = data.inputValue(aOutputMax).asFloat();

	MDataHandle hIn = data.inputValue(aInput);
	MObject objIn = hIn.data();
	MVectorArray vals =  MFnVectorArrayData(objIn).array();
	unsigned nVals = vals.length();
	
	MDoubleArray lengths(nVals);
	for(unsigned i = 0; i < nVals; ++i)
	{
		lengths[i]=vals[i].length();
	}

	MDoubleArray result(nVals);

	doRampLookup(thisMObject(), aRamp, lengths, result, float(imn),float(imx),float(omn),float(omx) );

	MVectorArray out(nVals);
	for(unsigned i = 0; i < nVals; ++i)
	{
		out[i]=vals[i].normal() * result[i];
	}


	MDataHandle hOut = data.outputValue(aOutput);
	MFnVectorArrayData fnOut;
	MObject objOut = fnOut.create(out);
	hOut.set(objOut);
	data.setClean(plug);
	//cerr << "done compute" << endl;

	return MS::kSuccess;															
}
