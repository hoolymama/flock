/*
 *  aimPP.cpp
 *  animal
 *
 *  Created by Julian Mann on 01/08/2006.
 *  Copyright 2006 HoolyMama. All rights reserved.
 *
 */
#include <maya/MIOStream.h>

#include "math.h"


#include <maya/MFnTypedAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MVectorArray.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MMatrix.h>
#include <maya/MQuaternion.h>
#include <maya/MTransformationMatrix.h>

#include <maya/MGlobal.h>
#include "errorMacros.h"
#include "jMayaIds.h"
#include "aimPP.h"



#define ROTATE_ORDER_XYZ	0
#define ROTATE_ORDER_YZX	1
#define ROTATE_ORDER_ZXY	2
#define ROTATE_ORDER_XZY	3
#define ROTATE_ORDER_YXZ	4
#define ROTATE_ORDER_ZYX	5


#define X_AXIS	0
#define Y_AXIS	1
#define Z_AXIS	2


#define INVERT_NONE		0
#define INVERT_FRONT		1
#define INVERT_UP			2
#define INVERT_BOTH		3

#define OUTPUT_RADIANS	0
#define OUTPUT_DEGREES	1


#define OUTPUT_EULER	1
#define OUTPUT_PHI	1

const double rad_to_deg = (180 / 3.1415927);


MTypeId     aimPP::id(  k_aimPP);



MObject     aimPP::aFront;
MObject     aimPP::aUp;
MObject     aimPP::aFrontAxis;
MObject     aimPP::aUpAxis;

MObject     aimPP::aInversions;
MObject     aimPP::aRotateOrder;
MObject     aimPP::aOutputUnit;
MObject     aimPP::aOutputType;
MObject     aimPP::aOutX;
MObject     aimPP::aOutY;
MObject     aimPP::aOutZ;
MObject     aimPP::aOut;


aimPP::aimPP() {}
aimPP::~aimPP() {}

MStatus aimPP::compute( const MPlug &plug, MDataBlock &data )
//

{

	MStatus st;
	MString method("aimPP::comppute");

	//
	if ( (plug == aOutX) ||
	     (plug == aOutY) ||
	     (plug == aOutZ) ||
	     (plug == aOut) )
	{
		unsigned i;

		unsigned fl = MFnVectorArrayData(data.inputValue(aFront).data()).length();
		unsigned ul = MFnVectorArrayData(data.inputValue(aUp).data()).length();

		if (fl !=  ul) { return MS::kFailure; }
		MVectorArray fronts = MFnVectorArrayData(data.inputValue(aFront).data()).array();
		MVectorArray ups = MFnVectorArrayData(data.inputValue(aUp).data()).array();


		short rotateOrder = data.inputValue(aRotateOrder).asShort();
		short inversion = data.inputValue(aInversions).asShort();
		short outUnit = data.inputValue(aOutputUnit).asShort();
		short outType = data.inputValue(aOutputType).asShort();

		short frontAxis = data.inputValue(aFrontAxis).asShort();
		short upAxis = data.inputValue(aUpAxis).asShort();
		//cerr << "upAxis: " << upAxis << endl;
		MTransformationMatrix::RotationOrder ord;
		switch ( rotateOrder ) {
			case ROTATE_ORDER_XYZ:
				ord = MTransformationMatrix::kXYZ; break;
			case ROTATE_ORDER_YZX:
				ord = MTransformationMatrix::kYZX; break;
			case ROTATE_ORDER_ZXY:
				ord = MTransformationMatrix::kZXY; break;
			case ROTATE_ORDER_XZY:
				ord = MTransformationMatrix::kXZY; break;
			case ROTATE_ORDER_YXZ:
				ord = MTransformationMatrix::kYXZ; break;
			case ROTATE_ORDER_ZYX:
				ord = MTransformationMatrix::kZYX; break;
			default:
				ord = MTransformationMatrix::kInvalid; break;
		}
		double mat[4][4] ;
		MVectorArray outRotations;
		double fMult = 1.0;
		double uMult = 1.0;

		switch ( inversion ) {
			case INVERT_NONE:
				break;
			case INVERT_FRONT:
				fMult = -1.0; break;
			case INVERT_UP:
				uMult = -1.0; break;
			case INVERT_BOTH:
				fMult = -1.0; uMult = -1.0; break;
			default:
				break;
		}

		// loop

		for (i = 0; i < fl; i++) {
			// make sure vectors are valid and orthogonalize
			MVector fAxis = fronts[i].normal();
			if (fAxis.isEquivalent(MVector::zero)) {
				fAxis = (MVector::xAxis);
			}
			MVector uAxis = ups[i];
			// if they are parallel then we have to resolve the situation
			// so - if we are on the real yAxis then use the real xAxis else use the real yAxis for up
			if  ( fAxis.isParallel(uAxis) ) { // gotta do something,
				if ( fAxis.isParallel(MVector::yAxis) ) {
					uAxis = (MVector::xAxis) ;
				}
				else {
					uAxis = (MVector::yAxis);
				}
			}
			fAxis *= fMult;// do inversions
			uAxis *= uMult;
			MVector sAxis = (fAxis ^ uAxis).normal(); // cross prod
			uAxis = (sAxis ^ fAxis).normal(); // re-align y to make orthonormal axes

			MVector xa, ya, za;


			if (frontAxis == X_AXIS)	{
				xa = fAxis;
				if (upAxis == Y_AXIS ) {
					ya = uAxis;
					za = sAxis;
				}
				else {
					za = uAxis;
					ya = -sAxis;
				}
			}
			else if (frontAxis == Y_AXIS)	{
				ya = fAxis;
				if (upAxis == Z_AXIS ) {
					za = uAxis;
					xa = sAxis;
				}
				else {
					xa = uAxis;
					za = -sAxis;
				}
			}
			else if (frontAxis == Z_AXIS)	{
				za = fAxis;
				if (upAxis == X_AXIS ) {
					xa = uAxis;
					ya = sAxis;
				}
				else {
					ya = uAxis;
					xa = -sAxis;
				}
			}


			// now plug them into the matrix
			mat[0][0] = xa.x;	mat[0][1] = xa.y;	mat[0][2] = xa.z;	mat[0][3] = 0;
			mat[1][0] = ya.x;	mat[1][1] = ya.y;	mat[1][2] = ya.z;	mat[1][3] = 0;
			mat[2][0] = za.x;	mat[2][1] = za.y;	mat[2][2] = za.z;	mat[2][3] = 0;
			mat[3][0] = 0;	mat[3][1] = 0;	mat[3][2] = 0;	mat[3][3] = 1.0;
			MMatrix mmat(mat);

			double rotValue[3];
			if (outType == OUTPUT_EULER) {
				MTransformationMatrix mtMat(mmat);
				mtMat.getRotation( rotValue, ord );
				if (outUnit == OUTPUT_DEGREES) {
					rotValue[0] *= rad_to_deg;
					rotValue[1] *= rad_to_deg;
					rotValue[2] *= rad_to_deg;
				}
			}
			else {
				MVector ax;
				double theta;
				MQuaternion q;
				q = mmat;
				q.getAxisAngle(ax, theta) ;
				rotValue[0] = ax.x * theta;
				rotValue[1] = ax.y * theta;
				rotValue[2] = ax.z * theta;

				if (outUnit == OUTPUT_DEGREES) {
					rotValue[0] *= rad_to_deg;
					rotValue[1] *= rad_to_deg;
					rotValue[2] *= rad_to_deg;
				}
			}

			outRotations.append(MVector(rotValue[0], rotValue[1], rotValue[2]));
		}

		MDataHandle hOut =  data.outputValue( aOut, &st); mser;
		MFnVectorArrayData fnOutput;
		MObject dOutput = fnOutput.create( outRotations, &st ); mser;
		hOut.set( dOutput);
		data.setClean( plug );
	}
	return MS::kSuccess;
}

void *aimPP::creator()
{
	return new aimPP();
}

MStatus aimPP::initialize()


{


	MStatus st;
	MString method("aimPP::initialize");

	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnUnitAttribute uAttr;
	MFnEnumAttribute eAttr;

	aFront = tAttr.create("frontVector", "fnt", MFnData::kVectorArray);
	tAttr.setStorable(false);
	addAttribute( aFront ); 

	aUp = tAttr.create("up", "upVector", MFnData::kVectorArray);
	tAttr.setStorable(false);
	addAttribute( aUp );

	aRotateOrder = eAttr.create( "rotateOrder", "ro", ROTATE_ORDER_XYZ);
	eAttr.addField("xyz", ROTATE_ORDER_XYZ);
	eAttr.addField("yzx", ROTATE_ORDER_YZX);
	eAttr.addField("zxy", ROTATE_ORDER_ZXY);
	eAttr.addField("xzy", ROTATE_ORDER_XZY);
	eAttr.addField("yxz", ROTATE_ORDER_YXZ);
	eAttr.addField("zyx", ROTATE_ORDER_ZYX);
	eAttr.setKeyable(true);
	eAttr.setHidden(false);
	addAttribute( aRotateOrder );

	aFrontAxis = eAttr.create( "frontAxis", "fa", X_AXIS);
	eAttr.addField("x", X_AXIS);
	eAttr.addField("y", Y_AXIS);
	eAttr.addField("z", Z_AXIS);
	eAttr.setKeyable(true);
	eAttr.setHidden(false);
	st = addAttribute( aFrontAxis ); mser;

	aUpAxis = eAttr.create( "upAxis", "ua", Y_AXIS);
	eAttr.addField("x", X_AXIS);
	eAttr.addField("y", Y_AXIS);
	eAttr.addField("z", Z_AXIS);
	eAttr.setKeyable(true);
	eAttr.setHidden(false);
	st = addAttribute( aUpAxis ); mser;




	aInversions = eAttr.create( "inversions", "ivs", INVERT_NONE);
	eAttr.addField("none", INVERT_NONE);
	eAttr.addField("front", INVERT_FRONT);
	eAttr.addField("up", INVERT_UP)	;
	eAttr.addField("both", INVERT_BOTH)	;
	eAttr.setKeyable(true);
	eAttr.setHidden(false);
	st = addAttribute( aInversions ); mser;

	aOutputUnit = eAttr.create( "angularUnit", "ang", OUTPUT_DEGREES);
	eAttr.addField("radians", OUTPUT_RADIANS);
	eAttr.addField("degrees", OUTPUT_DEGREES);
	eAttr.setKeyable(true);
	eAttr.setHidden(false);
	st = addAttribute( aOutputUnit ); mser;

	aOutputType = eAttr.create( "angularType", "atp", OUTPUT_EULER);
	eAttr.addField("euler", OUTPUT_EULER);
	eAttr.addField("phi", OUTPUT_PHI);
	eAttr.setKeyable(true);
	eAttr.setHidden(false);
	st = addAttribute( aOutputType ); mser;


	aOut = tAttr.create("out", "o", MFnData::kVectorArray ); 
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	st = addAttribute( aOut ); mser;

	attributeAffects(aFront, aOut);
	attributeAffects(aUp, aOut);
	attributeAffects(aFrontAxis, aOut);
	attributeAffects(aUpAxis, aOut);
	attributeAffects(aRotateOrder, aOut);
	attributeAffects(aInversions, aOut);
	attributeAffects(aOutputUnit, aOut);
	attributeAffects(aOutputType, aOut);


	return MS::kSuccess;

}

