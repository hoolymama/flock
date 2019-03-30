/*
*  tumbler.cpp
*  jtools
*
*  Created by Julian Mann on 02/12/2006.
*  Copyright 2006 hooly|mama. All rights reserved.
*
*/


#include <math.h>
#include <vector>
#include <maya/MGlobal.h>
#include <maya/MAnimControl.h>
#include <maya/MTime.h>
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MPointArray.h>

#include <maya/MFnDependencyNode.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MRampAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MEulerRotation.h>
#include <maya/MVector.h>
#include <maya/MPoint.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MMatrix.h>
#include <maya/MGlobal.h>
#include <maya/MPlugArray.h>
#include <maya/MQuaternion.h>
#include <maya/MDataBlock.h>
#include <maya/MPxNode.h>
#include <maya/MFnPluginData.h>


#include "tumbler.h"
#include "errorMacros.h"
#include "attrUtils.h"
#include "jMayaIds.h"



MObject tumbler::aPosition;
MObject tumbler::aVelocity;
MObject tumbler::aMass;


MObject tumbler::aFrontAxis;
MObject tumbler::aUpAxis;

MObject tumbler::aGoalFront;
MObject tumbler::aGoalUp;
MObject tumbler::aGoalWeightPP;

MObject tumbler::aImpulse ;

MObject tumbler::aLocalImpulse;


MObject tumbler::aPhi;
MObject tumbler::aOmega;

MObject tumbler::aGoalWeight;


MObject tumbler::aTorqueConserve;
MObject tumbler::aRotateOrder;
MObject tumbler::aOutputUnit;

MObject tumbler::aOutPoints;
MObject tumbler::aOutPhi;
MObject tumbler::aOutOmega;
MObject tumbler::aOutFront;
MObject tumbler::aOutUp;
MObject tumbler::aOutRotation;
MObject tumbler::aOutRotationData;
MObject tumbler::aCurrentTime;



MTypeId tumbler::id( k_tumbler );

tumbler::tumbler() {

	m_lastTimeIEvaluated = MAnimControl::currentTime();
}

tumbler::~tumbler() {}

void *tumbler::creator()
{
	return new tumbler;
}


MStatus tumbler::initialize()
//
//	Descriptions:
//		Initialize the node, attributes.
//
{
	MStatus st;
	MString method("tumbler::initialize");

	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnCompoundAttribute	cAttr;
	MFnUnitAttribute	uAttr;
	MFnEnumAttribute eAttr;


	aPosition = tAttr.create("position", "pos", MFnData::kVectorArray);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	st = addAttribute( aPosition ); mser;

	aVelocity = tAttr.create("velocity", "vel", MFnData::kVectorArray);
	tAttr.setReadable(false);
	st = addAttribute( aVelocity ); mser;

	aMass = tAttr.create("mass", "mas", MFnData::kDoubleArray);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	st = addAttribute( aMass ); mser;


	aFrontAxis = eAttr.create( "frontAxis", "fa", mayaMath::xAxis);
	eAttr.addField( "X",		mayaMath::xAxis);
	eAttr.addField( "Y",	 mayaMath::yAxis );
	eAttr.addField( "Z",	 mayaMath::zAxis );
	eAttr.addField( "negX",		mayaMath::xAxisNeg);
	eAttr.addField( "negY",	 mayaMath::yAxisNeg );
	eAttr.addField( "negZ",	 mayaMath::zAxisNeg );
	eAttr.setKeyable(true);
	eAttr.setHidden(false);
	st = addAttribute( aFrontAxis ); mser;

	aUpAxis = eAttr.create( "upAxis", "ua", mayaMath::yAxis);
	eAttr.addField( "X",		mayaMath::xAxis 	);
	eAttr.addField( "Y",	 	mayaMath::yAxis 	);
	eAttr.addField( "Z",	 	mayaMath::zAxis 	);
	eAttr.addField( "negX",		mayaMath::xAxisNeg	);
	eAttr.addField( "negY",	 	mayaMath::yAxisNeg 	);
	eAttr.addField( "negZ",	 	mayaMath::zAxisNeg 	);
	eAttr.setKeyable(true);
	eAttr.setHidden(false);
	st = addAttribute( aUpAxis ); mser;

	aGoalFront = tAttr.create("goalFrontVector", "gfnt", MFnData::kVectorArray , &st ); mser;
	tAttr.setStorable(false);
	st = addAttribute( aGoalFront ); mser;

	aGoalUp = tAttr.create("goalUpVector", "gup", MFnData::kVectorArray , &st ); mser;
	tAttr.setStorable(false);
	st = addAttribute( aGoalUp ); mser;

	aGoalWeightPP = tAttr.create("goalWeightPP", "gwpp", MFnData::kDoubleArray , &st ); mser;
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setStorable(false);
	st = addAttribute( aGoalWeightPP ); mser;

	aImpulse = tAttr.create("impulse", "ii", MFnData::kVectorArray, &st); mser;
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	st = addAttribute( aImpulse ); mser;

	////////////////////////////////////////////////////////////
	aLocalImpulse = nAttr.create( "localImpulse", "lim", MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);
	addAttribute( aLocalImpulse );


	aPhi = tAttr.create("phi", "ph", MFnData::kVectorArray);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	st = addAttribute(aPhi); mser;

	aOmega = tAttr.create("omega", "om", MFnData::kVectorArray);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	st = addAttribute(aOmega); mser;



	aGoalWeight = nAttr.create("goalWeight", "gwt", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	st = addAttribute(aGoalWeight); mser;


	aTorqueConserve = nAttr.create("conserveSpin", "csp", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	st = addAttribute(aTorqueConserve); mser;

	aRotateOrder = eAttr.create( "rotateOrder", "ro", ROTATE_ORDER_XYZ);
	eAttr.addField("xyz", ROTATE_ORDER_XYZ);
	eAttr.addField("yzx", ROTATE_ORDER_YZX);
	eAttr.addField("zxy", ROTATE_ORDER_ZXY);
	eAttr.addField("xzy", ROTATE_ORDER_XZY);
	eAttr.addField("yxz", ROTATE_ORDER_YXZ);
	eAttr.addField("zyx", ROTATE_ORDER_ZYX);
	eAttr.setKeyable(true);
	eAttr.setHidden(false);
	st = addAttribute( aRotateOrder ); mser;

	aOutputUnit = eAttr.create( "angularUnit", "ang", mayaMath::kDegrees);
	eAttr.addField("radians",  mayaMath::kRadians);
	eAttr.addField("degrees", mayaMath::kDegrees);
	uAttr.setKeyable(true);
	uAttr.setHidden(false);
	st = addAttribute( aOutputUnit ); mser;

	aOutPhi = tAttr.create("outPhi", "oph", MFnData::kVectorArray);
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);

	aOutOmega = tAttr.create("outOmega", "oom", MFnData::kVectorArray);
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);

	aOutFront = tAttr.create("outFront", "ofn", MFnData::kVectorArray);
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);

	aOutUp = tAttr.create("outUp", "oup", MFnData::kVectorArray);
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);

	aOutRotation = tAttr.create("outRotation", "orot", MFnData::kVectorArray , &st ); mser;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);

	aOutPoints = tAttr.create("outPoints", "opns", MFnData::kVectorArray , &st ); mser;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);

	aOutRotationData = cAttr.create("outRotationData", "ord", &st ); mser;
	cAttr.addChild(aOutPhi);
	cAttr.addChild(aOutOmega);
	cAttr.addChild(aOutFront);
	cAttr.addChild(aOutUp);
	cAttr.addChild(aOutRotation);
	cAttr.addChild(aOutPoints);
	cAttr.setWritable(false);
	st = addAttribute(aOutRotationData); mser;


	// Time
	///////////////////////////////////////////////////////////////////////
	aCurrentTime = uAttr.create( "currentTime", "ct", MFnUnitAttribute::kTime );
	uAttr.setStorable(true);
	st =  addAttribute(aCurrentTime);  mser;

	st = attributeAffects(aCurrentTime, aOutRotationData ); mser;


	return ( MS::kSuccess );
}


MStatus tumbler::compute(const MPlug &plug, MDataBlock &data)
//
//	Descriptions:
//		compute output force.
//

{
	MStatus st;
	MString method("tumbler::compute");


	if ( !	(plug.parent() == aOutRotationData) ) { return ( MS::kUnknownParameter); }


	//////////////////////////////////////////////////
	MPlug pPlug = plug.parent((&st));

	if (st.error()) { return ( MS::kFailure); }


	MTime cT =  data.inputValue( aCurrentTime).asTime();
	MTime dT = cT - m_lastTimeIEvaluated;
	m_lastTimeIEvaluated = cT;
	double dt = dT.as( MTime::kSeconds );

	MDataHandle hPosition = data.inputValue( aPosition );
	MObject dPosition = hPosition.data();
	MFnVectorArrayData fnPosition( dPosition );
	MVectorArray points = fnPosition.array( &st );

	MDataHandle hVelocity = data.inputValue( aVelocity );
	MObject dVelocity = hVelocity.data();
	MFnVectorArrayData fnVelocity( dVelocity );
	MVectorArray velocities = fnVelocity.array( &st );

	MDataHandle hMass = data.inputValue( aMass );
	MObject dMass = hMass.data();
	MFnDoubleArrayData fnMass( dMass );
	MDoubleArray mass = fnMass.array( &st);

	MDataHandle hPhi = data.inputValue( aPhi );
	MObject dPhi = hPhi.data();
	MFnVectorArrayData fnPhi( dPhi );
	MVectorArray phi = fnPhi.array( &st ); mser;

	MDataHandle hOmega = data.inputValue( aOmega );
	MObject dOmega = hOmega.data();
	MFnVectorArrayData fnOmega( dOmega );
	MVectorArray omega = fnOmega.array( &st ); mser;

	MDataHandle hGoalFront = data.inputValue( aGoalFront );
	MObject dGoalFront = hGoalFront.data();
	MFnVectorArrayData fnGoalFront( dGoalFront );
	MVectorArray goalFront = fnGoalFront.array( &st );

	MDataHandle hGoalUp = data.inputValue( aGoalUp );
	MObject dGoalUp = hGoalUp.data();
	MFnVectorArrayData fnGoalUp( dGoalUp );
	MVectorArray goalUp = fnGoalUp.array( &st );

	MDataHandle hGoalPP = data.inputValue( aGoalWeightPP );
	MObject dGoalPP = hGoalPP.data();
	MFnDoubleArrayData fnGoalPP( dGoalPP );
	MDoubleArray goalWeightPP = fnGoalPP.array( &st );

	MDataHandle hImpulse = data.inputValue( aImpulse );
	MObject dImpulse = hImpulse.data();
	MFnVectorArrayData fnImpulse( dImpulse );
	MVectorArray impulse = fnImpulse.array( &st );



	unsigned len = phi.length();


	if (dt > 0.0) {
		// MVectorArray  outPoints;
		// outPoints.copy(points);

		MVectorArray  outPhi(len);
		MVectorArray  outOmega(len);
		MVectorArray  outFront(len);
		MVectorArray  outUp(len);
		MVectorArray  outRotation(len);

		//cerr << "about to simstep" << endl;
		st = simStep( 	dt,
		                data,
		                phi,
		                omega,
		                goalFront,
		                goalUp,
		                goalWeightPP,
		                impulse,
		                // collisionImpulse,
		                points,
		                velocities,
		                outPhi,
		                outOmega,
		                outFront,
		                outUp,
		                outRotation
		            );


		MDataHandle hOutFront = data.outputValue(aOutFront);
		MFnVectorArrayData fnOutFront;
		MObject dOutFront = fnOutFront.create( outFront, &st ); mser;
		hOutFront.set( dOutFront );
		//	data.setClean( aOutFront);

		MDataHandle hOutUp = data.outputValue(aOutUp);
		MFnVectorArrayData fnOutUp;
		MObject dOutUp = fnOutUp.create( outUp, &st ); mser;
		hOutUp.set( dOutUp );
		//	data.setClean( aOutUp);

		MDataHandle hOutPhi = data.outputValue(aOutPhi);
		MFnVectorArrayData fnOutPhi;
		MObject dOutPhi = fnOutPhi.create( outPhi, &st ); mser;
		hOutPhi.set( dOutPhi );
		//	data.setClean( aOutPhi);

		MDataHandle hOutOmega = data.outputValue(aOutOmega);
		MFnVectorArrayData fnOutOmega;
		MObject dOutOmega = fnOutOmega.create( outOmega, &st ); mser;
		hOutOmega.set( dOutOmega );
		//	data.setClean( aOutOmega);

		MDataHandle hOutRotation = data.outputValue(aOutRotation);
		MFnVectorArrayData fnOutRotation;
		MObject dOutRotation = fnOutRotation.create( outRotation, &st ); mser;
		hOutRotation.set( dOutRotation );
		//	data.setClean( aOutRotation);''

		MDataHandle hOutPoints = data.outputValue(aOutPoints);
		MFnVectorArrayData fnOutPoints;
		MObject dOutPoints = fnOutPoints.create( points, &st ); mser;
		hOutPoints.set( dOutPoints );

	}

	data.setClean( aOutRotationData);


	return ( MS::kSuccess );
}


MStatus tumbler::simStep
(
  double dt,
  MDataBlock &data,
  const MVectorArray &phi,
  const MVectorArray &omega,
  const MVectorArray &goalFront,
  const MVectorArray &goalUp,
  const MDoubleArray &goalWeightPP,
  const MVectorArray &impulse,
  const MVectorArray &points,
  const MVectorArray &velocities,
  MVectorArray &outPhi,
  MVectorArray &outOmega,
  MVectorArray &outFront,
  MVectorArray &outUp,
  MVectorArray &outRotation
)
{

	MStatus st;

	if (dt == 0.0) {
		dt = 1.0;
	}
	double dtRecip2 = 1.0 / (dt * dt);


	double torqueConValue = data.inputValue(aTorqueConserve).asDouble();
	double goalWeightValue = data.inputValue(aGoalWeight).asDouble();
	short rotateOrder = data.inputValue(aRotateOrder).asShort();
	short outUnit =  data.inputValue(aOutputUnit).asShort();

	mayaMath::axis frontAxis = mayaMath::axis(data.inputValue(aFrontAxis).asShort());
	mayaMath::axis upAxis = mayaMath::axis(data.inputValue(aUpAxis).asShort());


	// rotate order
	////////////////////////////////////////////////////
	MEulerRotation::RotationOrder ord;
	switch ( rotateOrder ) {
		case ROTATE_ORDER_XYZ:
			ord = MEulerRotation::kXYZ; break;
		case ROTATE_ORDER_YZX:
			ord = MEulerRotation::kYZX; break;
		case ROTATE_ORDER_ZXY:
			ord = MEulerRotation::kZXY; break;
		case ROTATE_ORDER_XZY:
			ord = MEulerRotation::kXZY; break;
		case ROTATE_ORDER_YXZ:
			ord = MEulerRotation::kYXZ; break;
		case ROTATE_ORDER_ZYX:
			ord = MEulerRotation::kZYX; break;
		default:
			ord = MEulerRotation::kXYZ; break;
	}
	////////////////////////////////////////////////////

	unsigned len = phi.length();


	if (!len) { return MS::kFailure; }

	if (!( 	(omega.length() == len)
	        && 	(goalFront.length() == len)
	        && 	(goalUp.length() == len)
	     )) {
		MGlobal::displayWarning("tumbler array length mismatch - omega, goalFront and goalUp must be the same length");
		return MS::kFailure;
	}


	bool doLocalImpulse = data.inputValue(aLocalImpulse).asBool();

	bool doImpulse = false;
	bool doGoal = false;
	bool doGoalPP = false;
	if  (impulse.length() == len) { doImpulse = true; }

	if  ((goalFront.length() == len) && (goalUp.length() == len)) { doGoal = true; }
	if  (goalWeightPP.length() == len) { doGoalPP = true; }

	MIntArray indexes ;
	MFloatArray positions ;
	MFloatArray values;
	MIntArray interps;

	unsigned vlen ;
	float val;

	MMatrix outMat;
	MMatrix goalMat;
	MMatrix phiMat;

	MVector goalTorque;

	double gw = -goalWeightValue * dtRecip2;

	for (unsigned i = 0; i < len; i++) {



		goalMat = mayaMath::matFromAim(MVector::zero, goalFront[i], goalUp[i], MVector(1.0, 1.0,
		                               1.0), frontAxis, upAxis);


		MMatrix phiMat = mayaMath::matFromPhi(phi[i]);

		MMatrix localPhiMat	= phiMat * goalMat.inverse();

		MVector localOmega = (omega[i] * goalMat.inverse());

		MVector localPhi = mayaMath::phiFromMat(localPhiMat);

		if (doGoalPP) 	{ gw = -goalWeightPP[i]   * dtRecip2; };



		MVector localGoalTorque = localPhi * gw;

		MVector localDeltaOmega = ( localGoalTorque  * dt);

		if (doImpulse) {
			if (doLocalImpulse) {
				localDeltaOmega += impulse[i];
			}
			else {
				localDeltaOmega += impulse[i] * goalMat.inverse();
			}
		}



		MVector localOutOmega = (localOmega * torqueConValue) + localDeltaOmega;

		MVector localDeltaPhi = localOutOmega * dt;

		MVector localOutPhi = localPhi +  localDeltaPhi;

		MMatrix localOutPhiMat = mayaMath::matFromPhi(localOutPhi);

		MVector outOmegaVal = localOutOmega *  goalMat;

		MMatrix outPhiMat = localOutPhiMat * goalMat;

		MVector outPhiVal = mayaMath::phiFromMat(outPhiMat);

		outOmega.set(outOmegaVal, i);

		outPhi.set(outPhiVal, i);
		MVector oFront = mayaMath::axisVector(frontAxis) * outPhiMat;
		MVector oUp = mayaMath::axisVector(upAxis) * outPhiMat;

		outFront.set(oFront, i);
		outUp.set(oUp, i);

		MVector vOut =  mayaMath::phiToEuler(outPhiVal, ord, mayaMath::RotateUnit(outUnit));
		outRotation.set(vOut, i);

	}

	return MS::kSuccess;
}
