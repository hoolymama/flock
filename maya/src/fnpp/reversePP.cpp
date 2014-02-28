
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MDoubleArray.h> 
#include "errorMacros.h"
#include "reversePP.h"
#include "jMayaIds.h"


MTypeId reversePP::id(k_reversePP);
MObject reversePP::aInput;
MObject reversePP::aOutput;



void * reversePP::creator () {
	return new reversePP;
}

/// Post constructor
void
	reversePP::postConstructor()
{
	MPxNode::postConstructor();

	setExistWithoutInConnections(true);
	setExistWithoutOutConnections(true);
}

MStatus reversePP::initialize () {

	MStatus st;


	MFnTypedAttribute tAttr;


	aInput = tAttr.create("input", "in",MFnData::kDoubleArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);

	aOutput = tAttr.create ("output", "out",MFnData::kDoubleArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);

	addAttribute(aInput);
	addAttribute (aOutput);
	attributeAffects (aInput, aOutput);




	return MS::kSuccess;
}

reversePP::reversePP () {}
reversePP::~reversePP () {}

MStatus reversePP::compute(const MPlug& plug, MDataBlock& data) {

	if (!(plug == aOutput)) 	return MS::kUnknownParameter;			

	MStatus st;
	MFn::Type type = MFn::kInvalid;

		// Get inputs
	MDataHandle hIn = data.inputValue(aInput);
	MObject objIn = hIn.data();
	MDoubleArray in = MFnDoubleArrayData(objIn).array();


	int len = in.length();

	MDoubleArray out(len);	

	for (int i = 0;i<len;i++) {
		out[i] = 1.0 - in[i];
	}

	MDataHandle hOut = data.outputValue(aOutput);
	MFnDoubleArrayData fnOut;
	MObject objOut = fnOut.create(out);		
	hOut.set(objOut);
	data.setClean(plug);				
	return MS::kSuccess;


}
