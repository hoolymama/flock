/*
 *  phiArrayToMulti.cpp
 *  animal
 *
 *  Created by Julian Mann on 01/08/2006.
 *  Copyright 2006 HoolyMama. All rights reserved.
 *
 */

#include <maya/MFnVectorArrayData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MVectorArray.h>
#include <maya/MEulerRotation.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnUnitAttribute.h>



#include "errorMacros.h"

#include "errorMacros.h"
#include "jMayaIds.h"
#include "mayaMath.h"

#include "phiArrayToMulti.h"


MTypeId phiArrayToMulti::id(k_phiArrayToMulti);
MObject phiArrayToMulti::aRotateOrder;
MObject phiArrayToMulti::aInput;

MObject phiArrayToMulti::aOutput;
MObject phiArrayToMulti::aOutputX;
MObject phiArrayToMulti::aOutputY;
MObject phiArrayToMulti::aOutputZ;



void * phiArrayToMulti::creator () {
	return new phiArrayToMulti;
}
/// Post constructor
void
phiArrayToMulti::postConstructor()
{
}

MStatus phiArrayToMulti::initialize () {

	MStatus st;

	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;
	MFnEnumAttribute eAttr;
	MFnUnitAttribute uAttr;


	aInput = tAttr.create("input", "in",MFnData::kVectorArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	addAttribute(aInput);

	aRotateOrder = eAttr.create( "rotateOrder", "ro", MEulerRotation::kXYZ);
	eAttr.addField("xyz",  MEulerRotation::kXYZ);
	eAttr.addField("yzx",  MEulerRotation::kYZX);
	eAttr.addField("zxy",  MEulerRotation::kZXY);
	eAttr.addField("xzy",  MEulerRotation::kXZY);
	eAttr.addField("yxz",  MEulerRotation::kYXZ);
	eAttr.addField("zyx",  MEulerRotation::kZYX);
	eAttr.setKeyable(true);
	eAttr.setHidden(false);
	st = addAttribute( aRotateOrder );er;


	// Joint Rotation output
	///////////////////////////////////////////////////////////////////////
	aOutputX = uAttr.create("outputX","orox", MFnUnitAttribute::kAngle);
	aOutputY = uAttr.create("outputY","oroy", MFnUnitAttribute::kAngle);
	aOutputZ = uAttr.create("outputZ","oroz", MFnUnitAttribute::kAngle);
	aOutput = nAttr.create("output","orot",aOutputX, aOutputY, aOutputZ);
	nAttr.setReadable( true );
	nAttr.setStorable( false );
	nAttr.setArray( true );
	nAttr.setUsesArrayDataBuilder( true );
	st = addAttribute( aOutput );er;
	


	// add attributes and setup dependencies


	attributeAffects (aInput, aOutput);
	attributeAffects (aInput, aOutputX);
	attributeAffects (aInput, aOutputY);
	attributeAffects (aInput, aOutputZ);

	attributeAffects (aRotateOrder, aOutput);
	attributeAffects (aRotateOrder, aOutputX);
	attributeAffects (aRotateOrder, aOutputY);
	attributeAffects (aRotateOrder, aOutputZ);
	
	
	return MS::kSuccess;
}

phiArrayToMulti::phiArrayToMulti () {}
phiArrayToMulti::~phiArrayToMulti () {}

MStatus phiArrayToMulti::compute (const MPlug& plug, MDataBlock& data) 
{
	if (! (plug == aOutput ) ) return MS::kUnknownParameter;	
	cerr << 	"phiArrayToMulti::compute: plug="	 << plug.name() << endl;
	MStatus st = MS::kSuccess;
	MDataHandle hIn = data.inputValue( aInput, &st );	ert;
	MObject objIn  = hIn.data();
	MVectorArray in = MFnVectorArrayData(objIn).array();
	const unsigned nLength = in.length();

	MArrayDataHandle  hOutput = data.outputArrayValue( aOutput, &st ); ert;
	MArrayDataBuilder bOutput = hOutput.builder();


	MEulerRotation::RotationOrder order = 
	MEulerRotation::RotationOrder(data.inputValue(aRotateOrder).asShort());

	for (unsigned i = 0;i< nLength;i++) {

		MEulerRotation euler = mayaMath::phiToEuler(in[i], order);

		MDataHandle h = bOutput.addElement(i);

		double3 &out = h.asDouble3();
		out[0] = euler.x;
		out[1] = euler.y;
		out[2] = euler.z;
	}
	hOutput.setAllClean();
	return st;
}
