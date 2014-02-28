
#include <maya/MFnDoubleArrayData.h>

#include <maya/MFnTypedAttribute.h>

#include <maya/MDoubleArray.h> 

#include "errorMacros.h"
#include "blendDoublePP.h"
#include "jMayaIds.h"


MTypeId blendDoublePP::id(k_blendDoublePP);
MObject blendDoublePP::aInput1;
MObject blendDoublePP::aInput2;	   
MObject blendDoublePP::aWeight;	   
MObject blendDoublePP::aOutput;


void * blendDoublePP::creator () {
	return new blendDoublePP;
}

/// Post constructor
void
	blendDoublePP::postConstructor()
{
	MPxNode::postConstructor();

	setExistWithoutInConnections(true);
	setExistWithoutOutConnections(true);
}

MStatus blendDoublePP::initialize () {

	MStatus st;

	MFnTypedAttribute tAttr;

	aInput1 = tAttr.create("input1", "in1",MFnData::kDoubleArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);
	addAttribute(aInput1);
	
	aInput2 = tAttr.create("input2", "in2",MFnData::kDoubleArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);
	addAttribute(aInput2);
	
	aWeight = tAttr.create("weight", "wt",MFnData::kDoubleArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);
	addAttribute(aWeight);

	aOutput = tAttr.create ("output", "out",MFnData::kDoubleArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	addAttribute (aOutput);


	attributeAffects (aInput1, aOutput);
	attributeAffects (aInput2, aOutput);
	attributeAffects (aWeight, aOutput);

	return MS::kSuccess;
}

blendDoublePP::blendDoublePP () {}
blendDoublePP::~blendDoublePP () {}

MStatus blendDoublePP::compute(const MPlug& plug, MDataBlock& data) {

	if (!(plug == aOutput)) 	return MS::kUnknownParameter;			

	MStatus st;
	MFn::Type type = MFn::kInvalid;

	// Get inputs
	MDataHandle hIn1 = data.inputValue(aInput1);
	MObject objIn1 = hIn1.data();
	MDoubleArray in1 = MFnDoubleArrayData(objIn1).array();

	MDataHandle hIn2 = data.inputValue(aInput2);
	MObject objIn2 = hIn2.data();
	MDoubleArray in2 = MFnDoubleArrayData(objIn2).array();

	MDataHandle hWt = data.inputValue(aWeight);
	MObject objInWt = hWt.data();
	MDoubleArray wt = MFnDoubleArrayData(objInWt).array();



	int len = in1.length();

	if (len != in2.length())	return MS::kUnknownParameter; 
	if (len != wt.length())	return MS::kUnknownParameter; 

	MDoubleArray out(len);

	for (int i = 0;i<len;i++) {
		out[i] =( (in1[i] * wt[i]) + (in2[i] * (1.0 - wt[i])));
	}

	MDataHandle hOut = data.outputValue(aOutput);
	MFnDoubleArrayData fnOut;
	MObject objOut = fnOut.create(out);		
	hOut.set(objOut);
	data.setClean(plug);				
	return MS::kSuccess;


}
