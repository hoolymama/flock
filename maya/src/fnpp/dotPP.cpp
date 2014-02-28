#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MVectorArray.h> 
#include <maya/MDoubleArray.h> 
#include "errorMacros.h"
#include "dotPP.h"
#include "jMayaIds.h"


MTypeId dotPP::id(k_dotPP);
MObject dotPP::aInput1;
MObject dotPP::aInput2;
MObject dotPP::aOutput;



void * dotPP::creator () {
	return new dotPP;
}

/// Post constructor
void
	dotPP::postConstructor()
{
	MPxNode::postConstructor();

	setExistWithoutInConnections(true);
	setExistWithoutOutConnections(true);
}

MStatus dotPP::initialize () {

	MStatus st;


	MFnTypedAttribute tAttr;


	aInput1 = tAttr.create("input1", "in1",MFnData::kVectorArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);

	aInput2 = tAttr.create("input2", "in2",MFnData::kVectorArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);

	aOutput = tAttr.create ("output", "out",MFnData::kDoubleArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);



	// add attributes and setup dependencies
	addAttribute(aInput1);
	addAttribute(aInput2);
	addAttribute (aOutput);

	attributeAffects (aInput1, aOutput);
	attributeAffects (aInput2, aOutput);




	return MS::kSuccess;
}

dotPP::dotPP () {}
dotPP::~dotPP () {}

MStatus dotPP::compute(const MPlug& plug, MDataBlock& data) {

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


	int len = in1.length();
	if (len != in2.length())	return MS::kUnknownParameter; 

	MDoubleArray out(len);	

	for (int i = 0;i<len;i++) {
		out[i] = (in1[i]*in2[i]);
	}

	MDataHandle hOut = data.outputValue(aOutput);
	MFnDoubleArrayData fnOut;
	MObject objOut = fnOut.create(out);		
	hOut.set(objOut);
	data.setClean(plug);				
	return MS::kSuccess;


}
