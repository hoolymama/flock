#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnTypedAttribute.h>

#include <maya/MFnNumericAttribute.h>
#include <maya/MVectorArray.h> 
#include <maya/MDoubleArray.h> 

#include "errorMacros.h"
#include "subVectorVectorPP.h"
#include "jMayaIds.h"


MTypeId subVectorVectorPP::id(k_subVectorVectorPP);
MObject subVectorVectorPP::aInput1;
MObject subVectorVectorPP::aInput2;
MObject subVectorVectorPP::aMultiplier;	   
// MObject subVectorVectorPP::aDefault;	
MObject subVectorVectorPP::aOutput;



void * subVectorVectorPP::creator () {
	return new subVectorVectorPP;
}

/// Post constructor
void
	subVectorVectorPP::postConstructor()
{
	MPxNode::postConstructor();

	setExistWithoutInConnections(true);
	setExistWithoutOutConnections(true);
}

MStatus subVectorVectorPP::initialize () {

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

	// aNormalizeInput1 = nAttr.create( "normalizeInput1", "ni",MFnNumericData::kBoolean);
	// nAttr.setStorable(true);
	// nAttr.setKeyable(true);
	// nAttr.setDefault(false);
	// addAttribute( aNormalizeInput1 ); 

	aMultiplier = nAttr.create ("default", "def", MFnNumericData::kDouble);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(1.0);

	aOutput = tAttr.create ("output", "out",MFnData::kVectorArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);



	// add attributes and setup dependencies
	addAttribute(aInput1);
	addAttribute(aInput2);
	addAttribute (aMultiplier);
	addAttribute (aOutput);

	attributeAffects (aInput1, aOutput);
	attributeAffects (aInput2, aOutput);
	attributeAffects (aMultiplier, aOutput);
	// attributeAffects (aDefault, aOutput);





	return MS::kSuccess;
}

subVectorVectorPP::subVectorVectorPP () {}
subVectorVectorPP::~subVectorVectorPP () {}

MStatus subVectorVectorPP::compute(const MPlug& plug, MDataBlock& data) {

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

	// MVector vD = data.inputValue(aDefault).asVector();

	bool mult = data.inputValue(aMultiplier).asDouble();
	
	MVectorArray out;
	int len = in1.length();
	if (len != in2.length()) return MS::kUnknownParameter;			

	out.setLength(len);



	for (int i = 0;i<len;i++) {
		out[i] = in1[i] - in2[i];
	}
	if (mult != 1.0) {
		for (int i = 0;i<len;i++) {
			out[i] *= mult;
		}
	}

	MDataHandle hOut = data.outputValue(aOutput);
	MFnVectorArrayData fnOut;
	MObject objOut = fnOut.create(out);		
	hOut.set(objOut);
	data.setClean(plug);				
	return MS::kSuccess;


}
