
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MDoubleArray.h> 

#include "errorMacros.h"
#include "multDoubleDoublePP.h"
#include "jMayaIds.h"


MTypeId multDoubleDoublePP::id(k_multDoubleDoublePP);
MObject multDoubleDoublePP::aInput1;
MObject multDoubleDoublePP::aInput2;
MObject multDoubleDoublePP::aDefault;
MObject multDoubleDoublePP::aOutput;



void * multDoubleDoublePP::creator () {
	return new multDoubleDoublePP;
}

/// Post constructor
void
	multDoubleDoublePP::postConstructor()
{
	MPxNode::postConstructor();

	setExistWithoutInConnections(true);
	setExistWithoutOutConnections(true);
}

MStatus multDoubleDoublePP::initialize () {

	MStatus st;


	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;




	aInput1 = tAttr.create("input1", "in1",MFnData::kDoubleArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);

	aInput2 = tAttr.create("input2", "in2",MFnData::kDoubleArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);


	aDefault = nAttr.create ("default", "def", MFnNumericData::kDouble);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(1.0);



	aOutput = tAttr.create ("output", "out",MFnData::kDoubleArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);



	// add attributes and setup dependencies
	addAttribute(aInput1);
	addAttribute(aInput2);
	addAttribute (aDefault);
	addAttribute (aOutput);

	attributeAffects (aInput1, aOutput);
	attributeAffects (aInput2, aOutput);
	attributeAffects (aDefault, aOutput);




	return MS::kSuccess;
}

multDoubleDoublePP::multDoubleDoublePP () {}
multDoubleDoublePP::~multDoubleDoublePP () {}

MStatus multDoubleDoublePP::compute(const MPlug& plug, MDataBlock& data) {

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

	double def = data.inputValue(aDefault).asDouble();

	int len = in1.length();
	MDoubleArray out;
	out.copy(in1);
	if (len == in2.length()) {
		for (int i = 0;i<len;i++) {
			out[i] = out[i] * in2[i];
		}
	}

	if (def != 1.0) {

		for (int i = 0;i<len;i++) {
			out[i] = out[i] * def;
		}
	}

	MDataHandle hOut = data.outputValue(aOutput);
	MFnDoubleArrayData fnOut;
	MObject objOut = fnOut.create(out);		
	hOut.set(objOut);
	data.setClean(plug);				
	return MS::kSuccess;


}
