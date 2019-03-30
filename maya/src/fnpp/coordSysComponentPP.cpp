#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnTypedAttribute.h>

#include <maya/MFnNumericAttribute.h>
#include <maya/MVectorArray.h>

#include "errorMacros.h"
#include "coordSysComponentPP.h"
#include "jMayaIds.h"


MTypeId coordSysComponentPP::id(k_coordSysComponentPP);

MObject coordSysComponentPP::aFront;
MObject coordSysComponentPP::aUp;
MObject coordSysComponentPP::aInput;
MObject coordSysComponentPP::aFrontComponent;
MObject coordSysComponentPP::aUpComponent;
MObject coordSysComponentPP::aSideComponent;
MObject coordSysComponentPP::aOutput;

void *coordSysComponentPP::creator () {
	return new coordSysComponentPP;
}

/// Post constructor
void
coordSysComponentPP::postConstructor()
{
	MPxNode::postConstructor();

	setExistWithoutInConnections(true);
	setExistWithoutOutConnections(true);
}

MStatus coordSysComponentPP::initialize () {

	MStatus st;


	MFnTypedAttribute tAttr;

	MFnNumericAttribute nAttr;

	aFront = tAttr.create("front", "f", MFnData::kVectorArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);
	addAttribute(aFront);

	aUp = tAttr.create("up", "u", MFnData::kVectorArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);
	addAttribute(aUp);

	aInput = tAttr.create("input", "in", MFnData::kVectorArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);
	addAttribute(aInput);



	aFrontComponent = nAttr.create("frontComponent", "fcom", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	st = addAttribute(aFrontComponent); mser;


	aUpComponent = nAttr.create("upComponent", "ucom", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	st = addAttribute(aUpComponent); mser;


	aSideComponent = nAttr.create("sideComponent", "scom", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	st = addAttribute(aSideComponent); mser;

	aOutput = tAttr.create ("output", "out", MFnData::kVectorArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);


	// add attributes and setup dependencies
	addAttribute (aOutput);

	attributeAffects (aFront, aOutput);
	attributeAffects (aUp, aOutput);
	attributeAffects (aInput, aOutput);
	attributeAffects (aFrontComponent, aOutput);
	attributeAffects (aUpComponent, aOutput);
	attributeAffects (aSideComponent, aOutput);




	return MS::kSuccess;
}

coordSysComponentPP::coordSysComponentPP () {}
coordSysComponentPP::~coordSysComponentPP () {}

MStatus coordSysComponentPP::compute(const MPlug &plug, MDataBlock &data) {

	if (!(plug == aOutput)) 	{ return MS::kUnknownParameter; }

	MStatus st;
	MFn::Type type = MFn::kInvalid;

	// Get inputs
	MDataHandle hFront = data.inputValue(aFront);
	MObject dFront = hFront.data();
	MVectorArray front = MFnVectorArrayData(dFront).array();

	MDataHandle hInput = data.inputValue(aInput);
	MObject dInput = hInput.data();
	MVectorArray input = MFnVectorArrayData(dInput).array();

	MDataHandle hUp = data.inputValue(aUp);
	MObject dUp = hUp.data();
	MVectorArray up = MFnVectorArrayData(dUp).array();


	double frontComponent = data.inputValue(aFrontComponent	).asDouble();
	double upComponent	   = data.inputValue(aUpComponent	).asDouble();
	double sideComponent  = data.inputValue(aSideComponent	).asDouble();


	int len = front.length();
	if (len != up.length())	{ return MS::kUnknownParameter; }
	if (len != input.length())	{ return MS::kUnknownParameter; }

	MVectorArray out(len);
	MVector frontVec, sideVec, upVec;
	if ((frontComponent == 1.0) && (upComponent == 1.0) && (sideComponent == 1.0) ) {
		out = input;
	}
	else {
		for (int i = 0; i < len; i++) {
			frontVec = front[i].normal();
			sideVec = front[i] ^ up[i].normal();
			upVec = (sideVec ^ frontVec);

			out[i] = (  (frontVec * ((input[i] * frontVec) * frontComponent)) +
			            (sideVec * ((input[i] * sideVec) * sideComponent)) +
			            (upVec * ((input[i] * upVec) * upComponent)) );
		}
	}




	MDataHandle hOut = data.outputValue(aOutput);
	MFnVectorArrayData fnOut;
	MObject objOut = fnOut.create(out);
	hOut.set(objOut);
	data.setClean(plug);
	return MS::kSuccess;


}
