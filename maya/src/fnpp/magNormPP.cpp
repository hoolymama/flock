#include <maya/MFnVectorArrayData.h>
#include <maya/MVectorArray.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MDoubleArray.h> 
#include "errorMacros.h"
#include "magNormPP.h"
#include "jMayaIds.h"


MTypeId magNormPP::id(k_magNormPP);
MObject magNormPP::aInput;
MObject magNormPP::aOutputMag;
MObject magNormPP::aOutputNormal;



void * magNormPP::creator () {
	return new magNormPP;
}

/// Post constructor
void
	magNormPP::postConstructor()
{
	MPxNode::postConstructor();

	setExistWithoutInConnections(true);
	setExistWithoutOutConnections(true);
}

MStatus magNormPP::initialize () {

	MStatus st;


	MFnTypedAttribute tAttr;


	aInput = tAttr.create("input", "in",MFnData::kVectorArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);
	addAttribute(aInput);
	
	aOutputMag = tAttr.create ("outputMagnitude", "omag",MFnData::kDoubleArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	addAttribute(aOutputMag);

	aOutputNormal = tAttr.create ("outputNormal", "onrm",MFnData::kVectorArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	addAttribute(aOutputNormal);
	
	attributeAffects (aInput, aOutputNormal);
	attributeAffects (aInput, aOutputMag);

	return MS::kSuccess;
}

magNormPP::magNormPP () {}
magNormPP::~magNormPP () {}

MStatus magNormPP::compute(const MPlug& plug, MDataBlock& data) {

	if (!(
	(plug == aOutputMag) ||
	(plug == aOutputNormal)
	)) 	return MS::kUnknownParameter;			

	MStatus st;
	MFn::Type type = MFn::kInvalid;

		// Get inputs
	MDataHandle hIn = data.inputValue(aInput);
	MObject objIn = hIn.data();
	MVectorArray in = MFnVectorArrayData(objIn).array();

	int len = in.length();

	MDoubleArray outMag(len);	
	MVectorArray outNorm(len);	

	for (int i = 0;i<len;i++) {
		outMag[i] = in[i].length();
		outNorm[i] = in[i].normal();
	}


	MDataHandle hOutMag = data.outputValue(aOutputMag);
	MFnDoubleArrayData fnOutMag;
	MObject dOutMag = fnOutMag.create(outMag);		
	hOutMag.set(dOutMag);
	
	MDataHandle hOutNorm = data.outputValue(aOutputNormal);
	MFnVectorArrayData fnOutNorm;
	MObject dOutNorm = fnOutNorm.create(outNorm);		
	hOutNorm.set(dOutNorm);

	data.setClean(aOutputMag);	
	data.setClean(aOutputNormal);
			
	return MS::kSuccess;


}
