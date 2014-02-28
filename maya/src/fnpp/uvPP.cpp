#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MGlobal.h> 
#include <maya/MFnNumericAttribute.h>
#include <maya/MVectorArray.h> 
#include <maya/MDoubleArray.h> 

#include "errorMacros.h"
#include "uvPP.h"
#include "jMayaIds.h"


MTypeId uvPP::id(k_uvPP);
// MObject uvPP::aInput1;
// MObject uvPP::aInput2;
// MObject uvPP::aNormalizeInput1;	   
// MObject uvPP::aDefault;	
// MObject uvPP::aOutput;

MObject uvPP::aPosition;
MObject uvPP::aMinUV;	   
MObject uvPP::aMaxUV;	

MObject uvPP::aProjection;	   

MObject uvPP::aOutU;
MObject uvPP::aOutV;


void * uvPP::creator () {
	return new uvPP;
}

/// Post constructor
void
	uvPP::postConstructor()
{
	MPxNode::postConstructor();
	setExistWithoutInConnections(true);
	setExistWithoutOutConnections(true);
}

MStatus uvPP::initialize () {

	MStatus st;

	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;

	aPosition = tAttr.create("position", "pos",MFnData::kVectorArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);
	addAttribute(aPosition);

	aMinUV = nAttr.create ("minUV", "mn", MFnNumericData::k3Double);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(0.0, 0.0, 0.0);
	addAttribute (aMinUV);

	aMaxUV = nAttr.create ("maxUV", "mx", MFnNumericData::k3Double);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(1.0, 1.0, 1.0);
	addAttribute (aMaxUV);
	
	aOutU = tAttr.create ("outU", "ou",MFnData::kDoubleArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	addAttribute (aOutU);

	aOutV = tAttr.create ("outV", "ov",MFnData::kDoubleArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	addAttribute (aOutV);

	attributeAffects (aPosition, aOutU);
	attributeAffects (aMinUV, aOutU);
	attributeAffects (aMaxUV, aOutU);
	attributeAffects (aPosition, aOutV);
	attributeAffects (aMinUV, aOutV);
	attributeAffects (aMaxUV, aOutV);

	return MS::kSuccess;
}

uvPP::uvPP () {}
uvPP::~uvPP () {}

MStatus uvPP::compute(const MPlug& plug, MDataBlock& data) {

	if (!((plug == aOutU) || (plug == aOutV) )) 	return MS::kUnknownParameter;			

	MStatus st;
	MFn::Type type = MFn::kInvalid;

		// Get inputs
	MDataHandle hPos = data.inputValue(aPosition);
	MObject dPos = hPos.data();
	MVectorArray pos = MFnVectorArrayData(dPos).array();

	MVector minUV = data.inputValue(aMinUV).asVector();
	MVector maxUV = data.inputValue(aMaxUV).asVector();


	int len = pos.length();


	MDoubleArray outU(len);
	MDoubleArray outV(len);

	const MVector uvRange = maxUV - minUV;

	if ((uvRange.x == 0.0) || (uvRange.z == 0.0) ) {
		MGlobal::displayError("Divide By Zero in uvPP");
		return MS::kUnknownParameter;
	}
	double uRangeRecip = 1.0 / uvRange.x;
	double vRangeRecip = 1.0 / uvRange.z;

	for (int i = 0;i<len;i++) {
		outU[i] = (pos[i].x -  minUV.x) * uRangeRecip;
		outV[i] = (pos[i].z -  minUV.z) * vRangeRecip;
	}

	MDataHandle hOutU = data.outputValue(aOutU);
	MFnDoubleArrayData fnOutU;
	MObject dOutU = fnOutU.create(outU);		
	hOutU.set(dOutU);
	data.setClean(aOutU);

	MDataHandle hOutV = data.outputValue(aOutV);
	MFnDoubleArrayData fnOutV;
	MObject dOutV = fnOutV.create(outV);		
	hOutV.set(dOutV);
	data.setClean(aOutV);
							
	return MS::kSuccess;


}
