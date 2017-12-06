#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnTypedAttribute.h>

#include <maya/MFnNumericAttribute.h>
#include <maya/MVectorArray.h> 
#include <maya/MDoubleArray.h> 

#include "errorMacros.h"
#include "multVectorDoublePP.h"
#include "jMayaIds.h"


MTypeId multVectorDoublePP::id(k_multVectorDoublePP);
MObject multVectorDoublePP::aInput1;
MObject multVectorDoublePP::aInput2;
MObject multVectorDoublePP::aScalarInput;
MObject multVectorDoublePP::aNormalizeVector;
MObject multVectorDoublePP::aOutput;



void * multVectorDoublePP::creator () {
	return new multVectorDoublePP;
}

/// Post constructor
void
multVectorDoublePP::postConstructor()
{
	MPxNode::postConstructor();

	setExistWithoutInConnections(true);
	setExistWithoutOutConnections(true);
}

MStatus multVectorDoublePP::initialize () {

	MStatus st;


	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;



	aInput1 = tAttr.create("inputVector", "inv",MFnData::kVectorArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	// tAttr.setCached(false);

	aInput2 = tAttr.create("inputDouble", "ind",MFnData::kDoubleArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	// tAttr.setCached(false);

	aNormalizeVector = nAttr.create( "normalizeVector", "nv",MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(false);

	aScalarInput = nAttr.create("scalarInput", "sin",MFnNumericData::k3Double);
	nAttr.setWritable(true);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(1.0, 1.0, 1.0);


	aOutput = tAttr.create ("output", "out",MFnData::kVectorArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);



	// add attributes and setup dependencies
	addAttribute( aNormalizeVector ); 
	addAttribute (aScalarInput);
	addAttribute(aInput1);
	addAttribute(aInput2);
	addAttribute (aOutput);

	attributeAffects (aInput1, aOutput);
	attributeAffects (aInput2, aOutput);
	attributeAffects (aNormalizeVector, aOutput);
	attributeAffects (aScalarInput, aOutput);




	return MS::kSuccess;
}

multVectorDoublePP::multVectorDoublePP () {}
multVectorDoublePP::~multVectorDoublePP () {}

MStatus multVectorDoublePP::compute(const MPlug& plug, MDataBlock& data) {

	if (!(plug == aOutput)) 	return MS::kUnknownParameter;			

	MStatus st;
	// MFn::Type type = MFn::kInvalid;

	const double3 &scalarValue = data.inputValue(aScalarInput).asDouble3();

		// Get inputs
	MDataHandle hIn1 = data.inputValue(aInput1);
	MObject objIn1 = hIn1.data();
	MVectorArray in1 = MFnVectorArrayData(objIn1).array();

	MDataHandle hIn2 = data.inputValue(aInput2);
	MObject objIn2 = hIn2.data();
	MDoubleArray in2 = MFnDoubleArrayData(objIn2).array();

	bool norm = data.inputValue(aNormalizeVector).asBool();
	
	int len = in1.length();

	MVectorArray out;
	out.copy(in1);

	// cerr << "out1: " << out << endl;

	if (norm){
		for (int i = 0;i<len;i++) {
			out[i] = out[i].normal() ;
		}
	}
	if (in2.length() == len){
		for (int i = 0;i<len;i++) {
			out[i] = out[i] * in2[i] ;
		}
	}

	MVector sv = MVector(scalarValue[0], scalarValue[1], scalarValue[2]);

	if (! sv.isEquivalent(MVector(1.0,1.0,1.0))) {
		for (int i = 0;i<len;i++) {
			out[i] = MVector(out[i].x*scalarValue[0] , out[i].y*scalarValue[1] , out[i].z*scalarValue[2] );
		}
	}

  // cerr << "out2: " << out << endl;


	MDataHandle hOut = data.outputValue(aOutput);
	MFnVectorArrayData fnOut;
	MObject objOut = fnOut.create(out);		
	hOut.set(objOut);
	data.setClean(plug);				
	return MS::kSuccess;


}
