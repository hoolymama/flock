
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MDoubleArray.h>
#include "errorMacros.h"
#include "signPP.h"
#include "jMayaIds.h"


MTypeId signPP::id(k_signPP);
MObject signPP::aInput;
MObject signPP::aOutputIfPositive;
MObject signPP::aOutputIfNegative;
MObject signPP::aOutput;



void *signPP::creator () {
	return new signPP;
}

/// Post constructor
void
signPP::postConstructor()
{
	MPxNode::postConstructor();

	setExistWithoutInConnections(true);
	setExistWithoutOutConnections(true);
}

MStatus signPP::initialize () {

	MStatus st;


	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;


	aInput = tAttr.create("input", "in", MFnData::kDoubleArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);




	aOutputIfPositive = nAttr.create("outputIfPositive", "oip", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	st = addAttribute(aOutputIfPositive); mser;


	aOutputIfNegative = nAttr.create("outputIfNegative", "oin", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(-1.0);
	st = addAttribute(aOutputIfNegative); mser;


	aOutput = tAttr.create ("output", "out", MFnData::kDoubleArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);

	addAttribute(aInput);
	addAttribute (aOutput);
	attributeAffects (aInput, aOutput);




	return MS::kSuccess;
}

signPP::signPP () {}
signPP::~signPP () {}

MStatus signPP::compute(const MPlug &plug, MDataBlock &data) {

	if (!(plug == aOutput)) 	{ return MS::kUnknownParameter; }

	MStatus st;
	MFn::Type type = MFn::kInvalid;

	// Get inputs
	MDataHandle hIn = data.inputValue(aInput);
	MObject objIn = hIn.data();
	MDoubleArray in = MFnDoubleArrayData(objIn).array();

	double oPos = data.inputValue(aOutputIfPositive).asDouble();
	double oNeg = data.inputValue(aOutputIfNegative).asDouble();

	int len = in.length();

	MDoubleArray out(len);

	for (int i = 0; i < len; i++) { out[i] =  (in[i] > 0.0) ? oPos : oNeg ; }


	MDataHandle hOut = data.outputValue(aOutput);
	MFnDoubleArrayData fnOut;
	MObject objOut = fnOut.create(out);
	hOut.set(objOut);
	data.setClean(plug);
	return MS::kSuccess;


}
