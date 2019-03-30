/*
 *  arrayToMulti.cpp
 *  animal
 *
 *  Created by Julian Mann on 01/08/2006.
 *  Copyright 2006 HoolyMama. All rights reserved.
 *
 */

#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MDoubleArray.h>
#include "errorMacros.h"

#include "errorMacros.h"
#include "jMayaIds.h"

#include "arrayToMulti.h"

MTypeId arrayToMulti::id(k_arrayToMulti);
MObject arrayToMulti::aInput;
MObject arrayToMulti::aOutput;



void *arrayToMulti::creator () {
	return new arrayToMulti;
}
/// Post constructor
void
arrayToMulti::postConstructor()
{
}

MStatus arrayToMulti::initialize () {

	MStatus st;
	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;

	aInput = tAttr.create("input", "in", MFnData::kDoubleArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	addAttribute(aInput);


	aOutput = nAttr.create ("output", "out", MFnNumericData::kDouble);
	nAttr.setStorable (false);
	nAttr.setWritable (false);
	nAttr.setReadable (true);
	nAttr.setArray (true);
	nAttr.setUsesArrayDataBuilder(true);
	addAttribute (aOutput);


	// add attributes and setup dependencies
	attributeAffects (aInput, aOutput);



	return MS::kSuccess;
}

arrayToMulti::arrayToMulti () {}
arrayToMulti::~arrayToMulti () {}

MStatus arrayToMulti::compute (const MPlug &plug, MDataBlock &data)
{
	if (! (plug == aOutput )) { return MS::kUnknownParameter; }
	MStatus st = MS::kSuccess;
	MDataHandle hIn = data.inputValue( aInput, &st );	msert;
	MObject objIn  = hIn.data();
	MDoubleArray in = MFnDoubleArrayData(objIn).array();
	const unsigned nLength = in.length();

	MArrayDataHandle  hOutput = data.outputArrayValue( aOutput, &st ); msert;
	MArrayDataBuilder bOutput = hOutput.builder();

	for (unsigned i = 0; i < nLength; i++) {
		MDataHandle h = bOutput.addElement(i);
		double &out = h.asDouble();
		out  = in[i];
	}
	hOutput.setAllClean();
	return st;
}
