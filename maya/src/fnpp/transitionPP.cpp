
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MDoubleArray.h>

#include "errorMacros.h"
#include "transitionPP.h"
#include "jMayaIds.h"


MTypeId transitionPP::id(k_transitionPP);
MObject transitionPP::aInput;
MObject transitionPP::aChange;
MObject transitionPP::aMinMaxValue;
MObject transitionPP::aOutput;
// MObject transitionPP::aProfile;

void *transitionPP::creator () {
	return new transitionPP;
}

/// Post constructor
void
transitionPP::postConstructor()
{
	MPxNode::postConstructor();

	setExistWithoutInConnections(true);
	setExistWithoutOutConnections(true);
}

MStatus transitionPP::initialize () {

	MStatus st;

	MFnTypedAttribute tAttr;
	// MRampAttribute rAttr;
	MFnNumericAttribute nAttr;


	aInput = tAttr.create("input", "in", MFnData::kDoubleArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);
	addAttribute(aInput);

	aChange = tAttr.create("change", "chg", MFnData::kDoubleArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);
	addAttribute(aChange);

	aMinMaxValue = nAttr.create ("minMaxValue", "mmv", MFnNumericData::k2Double);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(0.0f, 1.0f);
	addAttribute(aMinMaxValue);

	aOutput = tAttr.create ("output", "out", MFnData::kDoubleArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	addAttribute (aOutput);


	// add attributes and setup dependencies

	attributeAffects( aInput, aOutput);
	attributeAffects( aChange, aOutput);
	attributeAffects( aMinMaxValue, aOutput);






	return MS::kSuccess;
}

transitionPP::transitionPP () {}
transitionPP::~transitionPP () {}

MStatus transitionPP::compute(const MPlug &plug, MDataBlock &data) {

	if (!(plug == aOutput)) 	{ return MS::kUnknownParameter; }

	MStatus st;
	MFn::Type type = MFn::kInvalid;

	// Get inputs
	MDataHandle hIn = data.inputValue(aInput);
	MObject dIn = hIn.data();
	MDoubleArray in = MFnDoubleArrayData(dIn).array();

	MDataHandle hChange = data.inputValue(aChange);
	MObject dChange = hChange.data();
	MDoubleArray change = MFnDoubleArrayData(dChange).array();

	double2 &minMax = data.inputValue( aMinMaxValue ).asDouble2();

	int len = in.length();
	if (len != change.length()) { return MS::kUnknownParameter; }

	MDoubleArray out(len);



	for (int i = 0; i < len; i++) {
		out[i] = in[i] + change[i];
		if (out[i] > minMax[1]) {
			out[i] = minMax[1];
		}
		else if (out[i] < minMax[0]) {
			out[i] = minMax[0];
		}
	}

	MDataHandle hOut = data.outputValue(aOutput);
	MFnDoubleArrayData fnOut;
	MObject objOut = fnOut.create(out);
	hOut.set(objOut);
	data.setClean(plug);
	return MS::kSuccess;


}
