#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnTypedAttribute.h>

#include <maya/MFnNumericAttribute.h>
#include <maya/MVectorArray.h> 
#include <maya/MDoubleArray.h> 

#include "errorMacros.h"
#include "multVectorVectorPP.h"
#include "jMayaIds.h"


MTypeId multVectorVectorPP::id(k_multVectorVectorPP);
MObject multVectorVectorPP::aInput1;
MObject multVectorVectorPP::aInput2;
MObject multVectorVectorPP::aNormalizeInput1;	   
MObject multVectorVectorPP::aDefault;	
MObject multVectorVectorPP::aOutput;



void * multVectorVectorPP::creator () {
	return new multVectorVectorPP;
}

/// Post constructor
void
	multVectorVectorPP::postConstructor()
{
	MPxNode::postConstructor();

	setExistWithoutInConnections(true);
	setExistWithoutOutConnections(true);
}

MStatus multVectorVectorPP::initialize () {

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

	aNormalizeInput1 = nAttr.create( "normalizeInput1", "ni",MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(false);
	addAttribute( aNormalizeInput1 ); 

	aDefault = nAttr.create ("default", "def", MFnNumericData::k3Double);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(1.0, 1.0, 1.0);

	aOutput = tAttr.create ("output", "out",MFnData::kVectorArray);
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
	attributeAffects (aNormalizeInput1, aOutput);
	attributeAffects (aDefault, aOutput);





	return MS::kSuccess;
}

multVectorVectorPP::multVectorVectorPP () {}
multVectorVectorPP::~multVectorVectorPP () {}

MStatus multVectorVectorPP::compute(const MPlug& plug, MDataBlock& data) {

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

	MVector vD = data.inputValue(aDefault).asVector();

	bool norm = data.inputValue(aNormalizeInput1).asBool();
	
	int len = in1.length();

	//cerr << "INPUT LENGTH:" << len << endl;
	MVectorArray out(in1);
	if (norm){
		for (int i = 0;i<len;i++) {
			out[i].normalize();
		}

	}
	if (! vD.isEquivalent(MVector(1.0,1.0,1.0))) {
		for (int i = 0;i<len;i++) {
			out[i] = MVector(out[i].x * vD.x,out[i].y * vD.y,out[i].z * vD.z);
		}
	}
	if (len == in2.length()) {
		for (int i = 0;i<len;i++) {
			out[i] = MVector(out[i].x * in2[i].x, out[i].y * in2[i].y, out[i].z * in2[i].z) ;
		}
	}

	MDataHandle hOut = data.outputValue(aOutput);
	MFnVectorArrayData fnOut;
	MObject objOut = fnOut.create(out);		
	hOut.set(objOut);
	data.setClean(plug);				
	return MS::kSuccess;


}