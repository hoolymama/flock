#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnTypedAttribute.h>

#include <maya/MFnNumericAttribute.h>
#include <maya/MVectorArray.h> 
#include "errorMacros.h"
#include "crossPP.h"
#include "jMayaIds.h"


MTypeId crossPP::id(k_crossPP);
MObject crossPP::aInput1;
MObject crossPP::aInput2;
MObject crossPP::aNormalizeInputs;
MObject crossPP::aOutput;



void * crossPP::creator () {
	return new crossPP;
}

/// Post constructor
void
	crossPP::postConstructor()
{
	MPxNode::postConstructor();

	setExistWithoutInConnections(true);
	setExistWithoutOutConnections(true);
}

MStatus crossPP::initialize () {

	MStatus st;


	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;



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

	aNormalizeInputs = nAttr.create( "normalizeInputs", "ni",MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);
	addAttribute( aNormalizeInputs ); 


	aOutput = tAttr.create ("output", "out",MFnData::kVectorArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);



	// add attributes and setup dependencies
	addAttribute(aInput1);
	addAttribute(aInput2);
	addAttribute (aOutput);

	attributeAffects (aInput1, aOutput);
	attributeAffects (aInput2, aOutput);
	attributeAffects (aNormalizeInputs, aOutput);




	return MS::kSuccess;
}

crossPP::crossPP () {}
crossPP::~crossPP () {}

MStatus crossPP::compute(const MPlug& plug, MDataBlock& data) {

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

	bool norm = data.inputValue(aNormalizeInputs).asBool();
	
	int len = in1.length();
	if (len != in2.length())	return MS::kUnknownParameter; 

	MVectorArray out(len);	
	if (norm){
		for (int i = 0;i<len;i++) {
			out[i] = (in1[i].normal()^in2[i].normal());
		}
	} else {
		for (int i = 0;i<len;i++) {
			out[i] = (in1[i]^in2[i]);
		}
	} 

	MDataHandle hOut = data.outputValue(aOutput);
	MFnVectorArrayData fnOut;
	MObject objOut = fnOut.create(out);		
	hOut.set(objOut);
	data.setClean(plug);				
	return MS::kSuccess;


}
