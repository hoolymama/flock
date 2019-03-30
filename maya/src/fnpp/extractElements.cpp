/*
 *  extractDoubleElements.cpp
 *  jtools
 *
 *  Created by Julian Mann on 11/13/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MGlobal.h>

#include "errorMacros.h"
#include "jMayaIds.h"

#include "extractElements.h"

MTypeId extractElements::id(k_extractElements);
MObject extractElements::aInput;
MObject extractElements::aIndexPP;
//MObject extractElements::aDefault;
MObject extractElements::aOutputD;
MObject extractElements::aOutputV;


void *extractElements::creator () {
	return new extractElements;
}
/// Post constructor
void
extractElements::postConstructor()
{

	setExistWithoutInConnections(true);
	setExistWithoutOutConnections(true);
}

MStatus extractElements::initialize () {

	MStatus st;
	MFnGenericAttribute gAttr;
	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnEnumAttribute eAttr;

	aInput = gAttr.create("rInputPP", "ripp");
	gAttr.addAccept(MFnData::kDoubleArray);
	gAttr.addAccept(MFnData::kVectorArray);
	gAttr.setWritable(true);
	gAttr.setStorable(false);
	gAttr.setReadable(false);
	gAttr.setWritable(true);
	gAttr.setCached(false);


	aIndexPP = tAttr.create ("rIndexPP", "rinpp", MFnData::kDoubleArray);
	tAttr.setStorable (false);
	tAttr.setWritable (true);
	tAttr.setReadable (true);
	/*
	aDefault = nAttr.create ("default", "def", MFnNumericData::k3Double);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(0.0, 0.0, 0.0);
	*/

	aOutputD = tAttr.create ("outputDPP", "odpp", MFnData::kDoubleArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);

	aOutputV = tAttr.create ("outputVPP", "ovpp", MFnData::kVectorArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);

	// add attributes and setup dependencies
	addAttribute(aInput);
	addAttribute(aIndexPP);
	//addAttribute(aDefault);
	addAttribute (aOutputD);
	addAttribute (aOutputV);

	attributeAffects (aInput, aOutputD);
	attributeAffects (aInput, aOutputV);
	//attributeAffects (aDefault, aOutputV);
	//attributeAffects (aDefault, aOutputD);
	attributeAffects (aIndexPP, aOutputD);
	attributeAffects (aIndexPP, aOutputV);


	return MS::kSuccess;
}

extractElements::extractElements () {}
extractElements::~extractElements () {}

MStatus extractElements::compute (const MPlug &plug, MDataBlock &data) {

	if ((plug == aOutputD ) || (plug == aOutputV ))
	{
		MStatus st;

		// Get input
		MDataHandle hIn = data.inputValue( aInput, &st ); msert;
		MObject objIn  = hIn.data();

		// get index pp
		MDataHandle hInPP = data.inputValue( aIndexPP, &st ); msert;

		MObject objInPP  = hInPP.data();
		MDoubleArray indexPP = MFnDoubleArrayData(objInPP).array();

		// get default value
		//MVector def = data.inputValue(aDefault).asDouble3();

		// Negate vectors?
		if (plug == aOutputV)
		{
			if (!objIn.hasFn(MFn::kVectorArrayData))
			{MGlobal::displayError("outputV requested but no vector array connected to input"); return MS::kFailure;}

			// get vector array
			MVectorArray in = MFnVectorArrayData(objIn).array();
			const unsigned nLength = in.length();

			if (indexPP.length() != nLength)
			{MGlobal::displayError("index array should be same length as input"); return MS::kFailure;}
			// Prepare output
			MVectorArray outputV;

			// get the data
			for (unsigned i = 0; i < nLength; i++)
			{
				if (indexPP[i] > 0.5) { outputV.append(in[i]); }
			}

			// set the output data
			MObject oo;
			MFnVectorArrayData vfn;
			oo = vfn.create(outputV);
			MDataHandle oh = data.outputValue(aOutputV);
			oh.set(oo);
		}

		// doubles
		else
		{
			if (!objIn.hasFn(MFn::kDoubleArrayData))
			{MGlobal::displayError("outputD requested but no double array connected to input"); return MS::kFailure;}

			// get vector array
			MDoubleArray in = MFnDoubleArrayData(objIn).array();
			const unsigned nLength = in.length();


			if (indexPP.length() != nLength)
			{MGlobal::displayError("index array should be same length as input"); return MS::kFailure;}
			// Prepare output
			MDoubleArray outputD;

			// get the data
			for (unsigned i = 0; i < nLength; i++)
			{
				if (indexPP[i] > 0.5) { outputD.append(in[i]); }
			}

			// set the output data
			MObject oo;
			MFnDoubleArrayData dfn;
			oo = dfn.create(outputD);
			MDataHandle oh = data.outputValue(aOutputD);
			oh.set(oo);

		}

		data.setClean(plug);
		return MS::kSuccess;
	}

	return MS::kUnknownParameter;
}
