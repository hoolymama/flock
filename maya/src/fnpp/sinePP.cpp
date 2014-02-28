

#include <math.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MDoubleArray.h> 

#include "errorMacros.h"
#include "sinePP.h"
#include "jMayaIds.h"


MTypeId sinePP::id(k_sinePP);

MObject sinePP::aInput;
MObject sinePP::aAmplitude;	     
MObject sinePP::aOffsetY;	     
MObject sinePP::aOutput;



void * sinePP::creator () {
	return new sinePP;
}

/// Post constructor
void
	sinePP::postConstructor()
{
	MPxNode::postConstructor();

	setExistWithoutInConnections(true);
	setExistWithoutOutConnections(true);
}

MStatus sinePP::initialize () {

	MStatus st;


	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;



	aInput = tAttr.create("input", "in",MFnData::kDoubleArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);
	addAttribute(aInput);

	aAmplitude = tAttr.create("amplitude", "amp",MFnData::kDoubleArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);
	addAttribute(aAmplitude);

	aOffsetY = tAttr.create("offsetY", "off",MFnData::kDoubleArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);
	addAttribute(aOffsetY);


	aOutput = tAttr.create ("output", "out",MFnData::kDoubleArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	addAttribute(aOutput);


	attributeAffects (aInput, aOutput);
	attributeAffects (aAmplitude, aOutput);
	attributeAffects (aOffsetY, aOutput);



	return MS::kSuccess;
}

sinePP::sinePP () {}
sinePP::~sinePP () {}

MStatus sinePP::compute(const MPlug& plug, MDataBlock& data) {

	if (!(plug == aOutput)) 	return MS::kUnknownParameter;			

	MStatus st;
	MFn::Type type = MFn::kInvalid;

		// Get inputs
	MDataHandle hIn = data.inputValue(aInput);
	MObject dIn = hIn.data();
	MDoubleArray in = MFnDoubleArrayData(dIn).array();

	MDataHandle hAmp = data.inputValue(aAmplitude);
	MObject dAmp = hIn.data();
	MDoubleArray amp = MFnDoubleArrayData(dAmp).array();

	MDataHandle hOff = data.inputValue(aOffsetY);
	MObject dOff = hOff.data();
	MDoubleArray off = MFnDoubleArrayData(dOff).array();

	int len = in.length();
	MDoubleArray out(len);
	bool doAmp = ( amp.length() == len);
	bool doOff= ( off.length() == len);

	for (int i = 0;i<len;i++) {
		double v =  sin(in[i]);
		if (doAmp) v *= amp[i];
		if (doOff) v *= off[i];
		out[i] = v;

	}
	MDataHandle hOut = data.outputValue(aOutput);
	MFnDoubleArrayData fnOut;
	MObject objOut = fnOut.create(out);		
	hOut.set(objOut);
	data.setClean(plug);				
	return MS::kSuccess;


}
