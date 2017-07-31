#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnTypedAttribute.h>

#include <maya/MFnNumericAttribute.h>
#include <maya/MVectorArray.h> 
#include <maya/MDoubleArray.h> 

#include "errorMacros.h"
#include "blendVectorPP.h"
#include "jMayaIds.h"


MTypeId blendVectorPP::id(k_blendVectorPP);
MObject blendVectorPP::aInput1;
MObject blendVectorPP::aInput2;	   
MObject blendVectorPP::aWeight;	
MObject blendVectorPP::aDefaultWeight;
MObject blendVectorPP::aOutput;


void * blendVectorPP::creator () {
	return new blendVectorPP;
}

/// Post constructor
void
	blendVectorPP::postConstructor()
{
	MPxNode::postConstructor();

	setExistWithoutInConnections(true);
	setExistWithoutOutConnections(true);
}

MStatus blendVectorPP::initialize () {

	MStatus st;

	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;
	aInput1 = tAttr.create("input1", "in1",MFnData::kVectorArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);
	addAttribute(aInput1);
	
	aInput2 = tAttr.create("input2", "in2",MFnData::kVectorArray);
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

 
	aDefaultWeight = nAttr.create("defaultWeight", "dwt",MFnNumericData::kDouble);
	nAttr.setWritable(true);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(1.0);
	addAttribute (aDefaultWeight);

	aOutput = tAttr.create ("output", "out",MFnData::kVectorArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	addAttribute (aOutput);


	attributeAffects (aInput1, aOutput);
	attributeAffects (aInput2, aOutput);
	attributeAffects (aWeight, aOutput);
	attributeAffects (aDefaultWeight, aOutput);

	return MS::kSuccess;
}

blendVectorPP::blendVectorPP () {}
blendVectorPP::~blendVectorPP () {}

MStatus blendVectorPP::compute(const MPlug& plug, MDataBlock& data) {

	if (!(plug == aOutput)) 	return MS::kUnknownParameter;			

	MStatus st;
	MFn::Type type = MFn::kInvalid;

	// Get inputs
	MDataHandle hIn1 = data.inputValue(aInput1);
	MObject objIn1 = hIn1.data();
	MVectorArray in1 = MFnVectorArrayData(objIn1).array();

	MDataHandle hIn2 = data.inputValue(aInput2);
	MObject objIn2 = hIn2.data();
	MVectorArray in2 = MFnVectorArrayData(objIn2).array();

	MDataHandle hWt = data.inputValue(aWeight);
	MObject objInWt = hWt.data();
	MDoubleArray wt = MFnDoubleArrayData(objInWt).array();

	double defaultWeight = data.inputValue(aDefaultWeight).asDouble(); 

	int len = in1.length();

	if (len != in2.length())	return MS::kUnknownParameter; 
	if (len != wt.length())	{
		wt=MDoubleArray(len, defaultWeight);
	} else {
		for (unsigned i=0;i<len;i++) {
			wt[i] = wt[i]* defaultWeight;
		}
	}

	MVectorArray out(len);
	
	for (int i = 0;i<len;i++) {
		out[i] =( (in1[i] * wt[i]) + (in2[i] * (1.0 - wt[i])));
	}

	MDataHandle hOut = data.outputValue(aOutput);
	MFnVectorArrayData fnOut;
	MObject objOut = fnOut.create(out);		
	hOut.set(objOut);
	data.setClean(plug);				
	return MS::kSuccess;


}
