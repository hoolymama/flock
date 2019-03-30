/*
*  articulate.cpp
*  jtools
*
*  Created by Julian Mann on 20/12/2006.
*  Copyright 2006 hooly|mama. All rights reserved.
*
*/



#include <math.h>
#include <vector>

#include <maya/MTime.h>
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MPointArray.h>

#include <maya/MFnDependencyNode.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MRampAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnEnumAttribute.h>

#include <maya/MFnTypedAttribute.h>
#include <maya/MFnVectorArrayData.h>

#include <maya/MFnDoubleArrayData.h>

#include <maya/MFnCompoundAttribute.h>

#include <maya/MVector.h>
#include <maya/MPoint.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MMatrix.h>
#include <maya/MGlobal.h>
#include <maya/MPlugArray.h>

#include <maya/MDataBlock.h>
#include <maya/MPxNode.h>

#include <articulate.h>
#include <lookup.h>
#include <attrUtils.h>

#include <errorMacros.h>
#include "jMayaIds.h"









MObject articulate::aViewPP;
MObject articulate::aDeltaViewPP;
MObject articulate::aUpVectorPP;

MObject articulate::aDoBend;
MObject articulate::aBendPP;
MObject articulate::aDeltaBendPP;
MObject articulate::aBendImpulsePP;
MObject articulate::aBendLookup;
MObject articulate::aBendMagnitude;
MObject articulate::aBendSpring;
MObject articulate::aConserveBend;
MObject articulate::aOutBendPP;

MObject articulate::aDoArch;
MObject articulate::aArchPP;
MObject articulate::aDeltaArchPP;
MObject articulate::aArchImpulsePP;
MObject articulate::aArchLookup;
MObject articulate::aArchMagnitude;
MObject articulate::aArchSpring;
MObject articulate::aConserveArch;
MObject articulate::aOutArchPP;


// MObject articulate::aArchRangeMin ;
// MObject articulate::aDoArch;
// MObject articulate::aArchMagnitude ;
// MObject articulate::aArchLookup;
// MObject articulate::aOutArchPP;


MObject articulate::aDoThrust;
MObject articulate::aThrustPP;
MObject articulate::aThrustMax;


MObject articulate::aThrustFacingStrategy;

MObject articulate::aThrustFacingLookup;

MObject articulate::aDoDrag;
MObject articulate::aVelocityPP;
MObject articulate::aVelocityMax;
MObject articulate::aDragLookup;


MObject articulate::aEffortAttack;
MObject articulate::aEffortDecay;
MObject articulate::aLastEffortPP;
MObject articulate::aEffortSpan;
MObject articulate::aOutEffortPP;


MObject articulate::aDoFlap;
MObject articulate::aFlapOffsetMult;
MObject articulate::aFlapOffsetLookup;
MObject articulate::aOutFlapOffsetPP;

MObject articulate::aFlapAmplitudeMult;
MObject articulate::aFlapAmplitudeLookup;
MObject articulate::aOutFlapAmplitudePP;


MObject articulate::aCurrentTime;


MTypeId articulate::id( k_articulate );

articulate::articulate() {
}

articulate::~articulate() {}

void *articulate::creator()
{
	return new articulate;
}


MStatus articulate::initialize()
//
//	Descriptions:
//		Initialize the node, attributes.
//
{
	MStatus st;
	MString method("articulate::initialize");

	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnUnitAttribute uAttr;
	MRampAttribute	rAttr;
	MFnEnumAttribute eAttr;
	////////////////////////////////////////////////////////////
	aViewPP = tAttr.create("viewPP", "vwpp", MFnData::kVectorArray);
	tAttr.setStorable(false);
	st = addAttribute(aViewPP); mser;

	aDeltaViewPP = tAttr.create("deltaViewPP", "dvpp", MFnData::kVectorArray);
	tAttr.setStorable(false);
	st = addAttribute(aDeltaViewPP); mser;

	aUpVectorPP = tAttr.create("upVectorPP", "uvpp", MFnData::kVectorArray);
	tAttr.setStorable(false);
	st = addAttribute(aUpVectorPP); mser;
	////////////////////////////////////////////////////////////





	////////////////////////////////////////////////////////////
	aDoBend = nAttr.create( "doBend", "dbn", MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);
	addAttribute( aDoBend );

	aBendPP = tAttr.create("bendPP", "bnpp", MFnData::kDoubleArray);
	tAttr.setStorable(false);
	st = addAttribute(aBendPP); mser;

	aDeltaBendPP = tAttr.create("deltaBendPP", "dbpp", MFnData::kDoubleArray);
	tAttr.setStorable(false);
	st = addAttribute(aDeltaBendPP); mser;

	aBendImpulsePP = tAttr.create("bendImpulsePP", "bipp", MFnData::kDoubleArray);
	tAttr.setStorable(false);
	st = addAttribute(aBendImpulsePP); mser;

	aBendLookup = MRampAttribute::createCurveRamp("bendLookup", "blut");
	st = addAttribute( aBendLookup ); mser;

	aBendMagnitude = nAttr.create("bendMagnitude", "bmag", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	st = addAttribute(aBendMagnitude); mser;

	aBendSpring = nAttr.create("bendSpring", "bspr", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(0.0);
	st = addAttribute(aBendSpring); mser;

	aConserveBend = nAttr.create("conserveBend", "cbnd", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	st = addAttribute(aConserveBend); mser;

	aOutBendPP = tAttr.create("outBendPP", "obpp", MFnData::kDoubleArray , &st ); mser;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	st = addAttribute(aOutBendPP); mser;
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	aDoArch = nAttr.create( "doArch", "dar", MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);
	addAttribute( aDoArch );

	aArchPP = tAttr.create("archPP", "arpp", MFnData::kDoubleArray);
	tAttr.setStorable(false);
	st = addAttribute(aArchPP); mser;

	aDeltaArchPP = tAttr.create("deltaArchPP", "dapp", MFnData::kDoubleArray);
	tAttr.setStorable(false);
	st = addAttribute(aDeltaArchPP); mser;

	aArchImpulsePP = tAttr.create("archImpulsePP", "aipp", MFnData::kDoubleArray);
	tAttr.setStorable(false);
	st = addAttribute(aArchImpulsePP); mser;

	aArchLookup = MRampAttribute::createCurveRamp("archLookup", "alut");
	st = addAttribute( aArchLookup ); mser;

	aArchMagnitude = nAttr.create("archMagnitude", "armg", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	st = addAttribute(aArchMagnitude); mser;

	aArchSpring = nAttr.create("archSpring", "aspr", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(0.0);
	st = addAttribute(aArchSpring); mser;

	aConserveArch = nAttr.create("conserveArch", "car", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	st = addAttribute(aConserveArch); mser;

	aOutArchPP = tAttr.create("outArchPP", "oapp", MFnData::kDoubleArray , &st ); mser;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	st = addAttribute(aOutArchPP); mser;
	////////////////////////////////////////////////////////////



	//	////////////////////////////////////////////////////////////
	//	aDoArch = nAttr.create( "doArch", "dar",MFnNumericData::kBoolean);
	//	nAttr.setStorable(true);
	//	nAttr.setKeyable(true);
	//	nAttr.setDefault(true);
	//	addAttribute( aDoArch );
	//
	//	aArchLookup = MRampAttribute::createCurveRamp("archLookup","alut");
	//	st = addAttribute( aArchLookup );mser;
	//
	//	aArchMagnitude = nAttr.create("archMagnitude", "armg", MFnNumericData::kDouble);
	//	nAttr.setKeyable(true);
	//	nAttr.setStorable(true);
	//	nAttr.setDefault(1.0);
	//	st = addAttribute(aArchMagnitude);mser;
	//
	////  aArchRangeMax = nAttr.create("archRangeMax", "armx", MFnNumericData::kDouble);
	////  nAttr.setKeyable(true);
	////  nAttr.setStorable(true);
	////  nAttr.setDefault(1.0);
	////  st = addAttribute(aArchRangeMax);mser;
	//
	//
	//	aOutArchPP = tAttr.create("outArchPP", "oapp", MFnData::kDoubleArray , &st ); mser;
	//	tAttr.setStorable(false);
	//	tAttr.setReadable(true);
	//	st = addAttribute(aOutArchPP);mser;
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	aDoThrust = nAttr.create( "doThrust", "dth", MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);
	addAttribute( aDoThrust );

	aThrustPP = tAttr.create("thrustPP", "thpp", MFnData::kVectorArray);
	tAttr.setStorable(false);
	st = addAttribute(aThrustPP); mser;

	aThrustMax = nAttr.create("thrustMax", "thmx", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	st = addAttribute(aThrustMax); mser;



	aThrustFacingStrategy = eAttr.create( "thrustFacingStrategy", "tfs");
	eAttr.addField( "off",		articulate::kOff);
	eAttr.addField( "forward",	 articulate::kForward );
	eAttr.addField( "allAround",	 articulate::kAllAround );
	eAttr.setDefault(articulate::kForward );

	eAttr.setKeyable(true);
	eAttr.setHidden(false);
	st = addAttribute( aThrustFacingStrategy ); mser;


	aThrustFacingLookup = MRampAttribute::createCurveRamp("thrustFacingLookup", "thlt");
	st = addAttribute( aThrustFacingLookup ); mser;

	aDoDrag = nAttr.create( "doDrag", "ddg", MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);
	addAttribute( aDoDrag );

	aVelocityPP = tAttr.create("velocityPP", "vlpp", MFnData::kVectorArray);
	tAttr.setStorable(false);
	st = addAttribute(aVelocityPP); mser;

	aVelocityMax = nAttr.create("velocityMax", "vlmx", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	st = addAttribute(aVelocityMax); mser;

	aDragLookup = MRampAttribute::createCurveRamp("dragLookup", "dglt");
	st = addAttribute( aDragLookup ); mser;

	// aEffortPP = tAttr.create("effortPP", "efpp", MFnData::kDoubleArray);
	// tAttr.setStorable(false);
	// st = addAttribute(aEffortPP);mser;
	//
	// aConserveEffort = nAttr.create("conserveEffort", "ceff", MFnNumericData::kDouble);
	// nAttr.setKeyable(true);
	// nAttr.setStorable(true);
	// nAttr.setDefault(1.0);
	// st = addAttribute(aConserveEffort);mser;
	//  MFnEnumAttribute eAttr;
	//
	//  aSimulationType = eAttr.create ("simulationType", "smt");
	//  eAttr.addField( "potential",	 agentCollision::kPotential );
	//  eAttr.addField( "collision",		agentCollision::kCollision);
	//  eAttr.setDefault(	agentCollision::kPotential);
	//  eAttr.setHidden(false);
	//  addAttribute (aSimulationType);


	aEffortAttack = nAttr.create("effortAttack", "eat", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	st = addAttribute(aEffortAttack); mser;

	aEffortDecay = nAttr.create("effortDecay", "edc", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	st = addAttribute(aEffortDecay); mser;


	aLastEffortPP = tAttr.create("lastEffortPP", "lepp", MFnData::kDoubleArray);
	tAttr.setStorable(false);
	st = addAttribute(aLastEffortPP); mser;



	aEffortSpan = nAttr.create("effortSpan", "efsp", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	st = addAttribute(aEffortSpan); mser;

	aOutEffortPP = tAttr.create("outEffortPP", "oepp", MFnData::kDoubleArray , &st ); mser;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	st = addAttribute(aOutEffortPP); mser;
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	//aFlapOffsetPP = tAttr.create("flapOffsetPP", "fopp", MFnData::kDoubleArray);
	//tAttr.setStorable(false);
	//st = addAttribute(aFlapOffsetPP);mser;
	aDoFlap = nAttr.create( "doFlap", "dfl", MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);
	addAttribute( aDoFlap );

	aFlapOffsetMult = nAttr.create("flapOffsetMult", "foml", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	st = addAttribute(aFlapOffsetMult); mser;

	aFlapOffsetLookup = MRampAttribute::createCurveRamp("flapOffsetLookup", "folt");
	st = addAttribute( aFlapOffsetLookup ); mser;

	aOutFlapOffsetPP = tAttr.create("outFlapOffsetPP", "ofop", MFnData::kDoubleArray , &st );
	mser;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	st = addAttribute(aOutFlapOffsetPP); mser;
	////////////////////////////////////////////////////////////





	////////////////////////////////////////////////////////////
	aFlapAmplitudeMult = nAttr.create("flapAmplitudeMult", "faml", MFnNumericData::kDouble);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(1.0);
	st = addAttribute(aFlapAmplitudeMult); mser;

	aFlapAmplitudeLookup = MRampAttribute::createCurveRamp("flapAmplitudeLookup", "falt");
	st = addAttribute( aFlapAmplitudeLookup ); mser;

	aOutFlapAmplitudePP = tAttr.create("outFlapAmplitudePP", "ofap", MFnData::kDoubleArray ,
	                                   &st ); mser;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	st = addAttribute(aOutFlapAmplitudePP); mser;
	////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	aCurrentTime = uAttr.create( "currentTime", "ct", MFnUnitAttribute::kTime );
	uAttr.setStorable(true);
	st =  addAttribute(aCurrentTime);  mser;

	st = attributeAffects(aCurrentTime  , aOutBendPP			); mser;
	st = attributeAffects(aCurrentTime  , aOutArchPP 			); mser;
	st = attributeAffects(aCurrentTime  , aOutEffortPP 			); mser;
	st = attributeAffects(aCurrentTime  , aOutFlapOffsetPP 		); mser;
	st = attributeAffects(aCurrentTime  , aOutFlapAmplitudePP 	); mser;



	return ( MS::kSuccess );
}

MStatus articulate::compute(const MPlug &plug, MDataBlock &data)
//
//	Descriptions:
//		compute output force.
//

{
	MStatus st;
	//unsigned int counter = 0;
	MString method("articulate::compute");

	//	cerr <<  "articulate::compute"  << endl;



	if ( !(
	       (plug == aOutBendPP ) ||
	       (plug == aOutArchPP ) ||
	       (plug == aOutEffortPP ) ||
	       (plug == aOutFlapOffsetPP ) ||
	       (plug == aOutFlapAmplitudePP )
	     )) { return ( MS::kUnknownParameter); }


	//	cerr <<  "here 1"  << endl;
	MTime cT =  data.inputValue( aCurrentTime).asTime();

	//////////////////////////////////////////////////////////////////////////
	MDataHandle hViewPP = data.inputValue( aViewPP );
	MObject dViewPP = hViewPP.data();
	MFnVectorArrayData fnViewPP( dViewPP );
	MVectorArray viewPP = fnViewPP.array( &st ); //mser;

	MDataHandle hDeltaViewPP = data.inputValue( aDeltaViewPP );
	MObject dDeltaViewPP = hDeltaViewPP.data();
	MFnVectorArrayData fnDeltaViewPP( dDeltaViewPP );
	MVectorArray deltaViewPP = fnDeltaViewPP.array( &st ); //mser;

	MDataHandle hUpVectorPP = data.inputValue( aUpVectorPP );
	MObject dUpVectorPP = hUpVectorPP.data();
	MFnVectorArrayData fnUpVectorPP( dUpVectorPP );
	MVectorArray upVectorPP = fnUpVectorPP.array( &st ); //mser;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	MDataHandle hBendPP = data.inputValue( aBendPP );
	MObject dBendPP = hBendPP.data();
	MFnDoubleArrayData fnBendPP( dBendPP );
	MDoubleArray bendPP = fnBendPP.array( &st ); //mser;

	MDataHandle hDeltaBendPP = data.inputValue( aDeltaBendPP );
	MObject dDeltaBendPP = hDeltaBendPP.data();
	MFnDoubleArrayData fnDeltaBendPP( dDeltaBendPP );
	MDoubleArray deltaBendPP = fnDeltaBendPP.array( &st ); //mser;

	MDataHandle hBendImpulsePP = data.inputValue( aBendImpulsePP );
	MObject dBendImpulsePP = hBendImpulsePP.data();
	MFnDoubleArrayData fnBendImpulsePP( dBendImpulsePP );
	MDoubleArray bendImpulsePP = fnBendImpulsePP.array( &st ); // no error if missing
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	MDataHandle hArchPP = data.inputValue( aArchPP );
	MObject dArchPP = hArchPP.data();
	MFnDoubleArrayData fnArchPP( dArchPP );
	MDoubleArray archPP = fnArchPP.array( &st ); //mser;

	MDataHandle hDeltaArchPP = data.inputValue( aDeltaArchPP );
	MObject dDeltaArchPP = hDeltaArchPP.data();
	MFnDoubleArrayData fnDeltaArchPP( dDeltaArchPP );
	MDoubleArray deltaArchPP = fnDeltaArchPP.array( &st ); //mser;

	MDataHandle hArchImpulsePP = data.inputValue( aArchImpulsePP );
	MObject dArchImpulsePP = hArchImpulsePP.data();
	MFnDoubleArrayData fnArchImpulsePP( dArchImpulsePP );
	MDoubleArray archImpulsePP = fnArchImpulsePP.array( &st ); // no error if missing
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	MDataHandle hLastEffortPP = data.inputValue( aLastEffortPP );
	MObject dLastEffortPP = hLastEffortPP.data();
	MFnDoubleArrayData fnLastEffortPP( dLastEffortPP );
	MDoubleArray lastEffortPP = fnLastEffortPP.array( &st );

	MDataHandle hThrustPP = data.inputValue( aThrustPP );
	MObject dThrustPP = hThrustPP.data();
	MFnVectorArrayData fnThrustPP( dThrustPP );
	MVectorArray thrustPP = fnThrustPP.array( &st ); mser;

	MDataHandle hVelocityPP = data.inputValue( aVelocityPP );
	MObject dVelocityPP = hVelocityPP.data();
	MFnVectorArrayData fnVelocityPP( dVelocityPP );
	MVectorArray velocityPP = fnVelocityPP.array( &st ); mser;
	//////////////////////////////////////////////////////////////////////////




	//////////////////////////////////////////////////////////////////////////
	// MDataHandle hFlapOffsetPP = data.inputValue( aFlapOffsetPP );
	// MObject dFlapOffsetPP = hFlapOffsetPP.data();
	// MFnDoubleArrayData fnFlapOffsetPP( dFlapOffsetPP );
	// MDoubleArray flapOffsetPP = fnFlapOffsetPP.array( &st );mser;
	//////////////////////////////////////////////////////////////////////////

	//cerr << "here 0" << endl;
	double bendMagnitude = data.inputValue(aBendMagnitude).asDouble();
	double bendSpring = data.inputValue(aBendSpring).asDouble();
	double conserveBend = data.inputValue(aConserveBend).asDouble();

	//double archRangeMin = data.inputValue(aArchRangeMin).asDouble();
	double archMagnitude = data.inputValue(aArchMagnitude).asDouble();
	double archSpring = data.inputValue(aArchSpring).asDouble();
	double conserveArch = data.inputValue(aConserveArch).asDouble();

	double effortAttack = data.inputValue(aEffortAttack).asDouble();
	double effortDecay = data.inputValue(aEffortDecay).asDouble();

	double thrustMax = data.inputValue(aThrustMax).asDouble();
	double velocityMax = data.inputValue(aVelocityMax).asDouble();
	double effortSpan = data.inputValue(aEffortSpan).asDouble();

	double flapOffsetMult = data.inputValue(aFlapOffsetMult).asDouble();


	double flapAmplitudeMult = data.inputValue(aFlapAmplitudeMult).asDouble();


	bool doBend = data.inputValue(aDoBend).asBool();
	bool doArch = data.inputValue(aDoArch).asBool();
	bool doThrust = data.inputValue(aDoThrust).asBool();
	bool doDrag = data.inputValue(aDoDrag).asBool();
	bool doFlap = data.inputValue(aDoFlap).asBool();

	MObject thisNode = thisMObject();


	ThrustFacingStrategy thrustStrategy = ThrustFacingStrategy(data.inputValue(
	                                        aThrustFacingStrategy).asShort());



	MVectorArray viewNorm(viewPP);
	unsigned vl = viewPP.length();
	for (unsigned i = 0; i < viewPP.length(); ++i)
	{
		viewNorm[i] = viewPP[i].normal();
	}

	unsigned len = thrustPP.length();
	MDoubleArray  outEffortPP(len);
	st = calculateEffort(thrustPP, viewNorm, velocityPP, thrustMax, velocityMax, effortSpan,
	                     effortAttack,  effortDecay, lastEffortPP, outEffortPP, doThrust, doDrag, thrustStrategy);

	// now we have effort - calc flap offset
	MDoubleArray outFlapOffsetPP(len);
	MDoubleArray outFlapAmplitudePP(len);
	if (doFlap) {
		st = doRampLookup(thisNode, aFlapOffsetLookup, outEffortPP, outFlapOffsetPP, 0.0f, 1.0f,
		                  0.0f, float(flapOffsetMult)); 	mser;
		st = doRampLookup(thisNode, aFlapAmplitudeLookup, outEffortPP, outFlapAmplitudePP, 0.0f,
		                  1.0f, 0.0f, float(flapAmplitudeMult)); 	mser;
	}
	// we will assume the view and upVector are normal and orthogonal
	//cerr << "here 1" << endl;

	MDoubleArray  outBendPP(len);
	if (doBend) {
		st = calculateBend(viewNorm, deltaViewPP, upVectorPP, bendPP,   deltaBendPP,
		                   bendImpulsePP, outBendPP, bendMagnitude, conserveBend, bendSpring);
	}


	MDoubleArray  outArchPP(len);
	if (doArch) {
		st = calculateArch(viewNorm, deltaViewPP, upVectorPP, archPP,   deltaArchPP,
		                   archImpulsePP, outArchPP, archMagnitude, conserveArch, archSpring);
		// 	st = calculateArch(viewPP, deltaViewPP, upVectorPP ,outArchPP , archMagnitude); mser;
	}



	MDataHandle hOutEffortPP = data.outputValue(aOutEffortPP);
	MFnDoubleArrayData fnOutEffortPP;
	MObject dOutEffortPP = fnOutEffortPP.create( outEffortPP, &st ); mser;
	hOutEffortPP.set( dOutEffortPP );
	data.setClean( aOutEffortPP);


	MDataHandle hOutFlapOffsetPP = data.outputValue(aOutFlapOffsetPP);
	MFnDoubleArrayData fnOutFlapOffsetPP;
	MObject dOutFlapOffsetPP = fnOutFlapOffsetPP.create( outFlapOffsetPP, &st ); mser;
	hOutFlapOffsetPP.set( dOutFlapOffsetPP );
	data.setClean( aOutFlapOffsetPP);

	MDataHandle hOutFlapAmplitudePP = data.outputValue(aOutFlapAmplitudePP);
	MFnDoubleArrayData fnOutFlapAmplitudePP;
	MObject dOutFlapAmplitudePP = fnOutFlapAmplitudePP.create( outFlapAmplitudePP, &st );
	mser;
	hOutFlapAmplitudePP.set( dOutFlapAmplitudePP );
	data.setClean( aOutFlapAmplitudePP);


	MDataHandle hOutBendPP = data.outputValue(aOutBendPP);
	MFnDoubleArrayData fnOutBendPP;
	MObject dOutBendPP = fnOutBendPP.create( outBendPP, &st ); mser;
	hOutBendPP.set( dOutBendPP );
	data.setClean( aOutBendPP);


	MDataHandle hOutArchPP = data.outputValue(aOutArchPP);
	MFnDoubleArrayData fnOutArchPP;
	MObject dOutArchPP = fnOutArchPP.create( outArchPP, &st ); mser;
	hOutArchPP.set( dOutArchPP );
	data.setClean( aOutArchPP);


	return ( MS::kSuccess );
}



MStatus articulate::calculateArch(
  const MVectorArray &viewPP,
  const MVectorArray &deltaViewPP,
  const MVectorArray &upVectorPP,
  const MDoubleArray   &archPP ,
  const MDoubleArray   &deltaArchPP ,
  const MDoubleArray   &archImpulsePP,
  MDoubleArray   &outArchPP ,
  double archMagnitude,
  double conserveArch,
  double archSpring
) const
{
	MStatus st;
	unsigned len = viewPP.length();
	if ( deltaViewPP.length()  	!= len)  	{ return MS::kFailure; }
	if ( upVectorPP.length() 	!= len) 	{ return MS::kFailure; }
	if ( outArchPP.length()  	!= len)  	{ return MS::kFailure; }
	if ( archPP.length()  		!= len)  	{ return MS::kFailure; }
	if ( outArchPP.length()  	!= len)  	{ return MS::kFailure; }
	if ( deltaArchPP.length()  	!= len)  	{ return MS::kFailure; }

	bool archImpulse = false;
	if (archImpulsePP.length() == len) { archImpulse = true; }


	MDoubleArray angles(len);
	MVector  tmp;
	double c1, c2;

	for (unsigned i = 0; i < len; i++) {
		MVector oldView = viewPP[i] - deltaViewPP[i];
		const MVector &u = upVectorPP[i];

		c1 =  u.angle(oldView);
		c2 =  u.angle(viewPP[i]);
		angles[i] = c1 - c2;

	}
	MDoubleArray lutResult(len);
	st = doRampLookup(thisMObject(), articulate::aArchLookup, angles, lutResult,
	                  -1.5707963267948966,  1.5707963267948966, -archMagnitude, archMagnitude ); 	mser;

	double archForce, springForce , newDeltaArch;
	//cerr << "lookupVals " << lookupVals << endl;
	for (unsigned i = 0; i < len; i++) {
		double &newArch = outArchPP[i];

		archForce = lutResult[i];

		if (archImpulse) { archForce += archImpulsePP[i]; }
		double springForce = -archPP[i] * archSpring;
		newDeltaArch =  (deltaArchPP[i] * conserveArch) + springForce + archForce ;
		newArch = archPP[i] + newDeltaArch;
	}



	return MS::kSuccess;
}




MStatus articulate::calculateBend(
  const MVectorArray &viewPP,
  const MVectorArray &deltaViewPP,
  const MVectorArray &upVectorPP,
  const MDoubleArray   &bendPP ,
  const MDoubleArray   &deltaBendPP ,
  const MDoubleArray   &bendImpulsePP,
  MDoubleArray   &outBendPP ,
  double bendMagnitude,
  double conserveBend,
  double bendSpring
) const {
	// find how much the view has been rotated
	MStatus st;
	unsigned len = viewPP.length();
	if ( deltaViewPP.length()  	!= len)  	{ return MS::kFailure; }
	if ( upVectorPP.length() 	!= len) 	{ return MS::kFailure; }
	if ( bendPP.length()  		!= len)  	{ return MS::kFailure; }
	if ( outBendPP.length()  	!= len)  	{ return MS::kFailure; }
	if ( deltaBendPP.length()  	!= len)  	{ return MS::kFailure; }

	bool bendImpulse = false;
	if (bendImpulsePP.length() == len) {
		bendImpulse = true;
	}
	//	cerr << "bendImp true" << endl;
	//} else {
	//	cerr << "bendImp false" << endl;
	//	}


	// get the angle between
	MDoubleArray angles(len);
	MIntArray signs(len);
	MVector c1, c2, tmp;


	for (unsigned i = 0; i < len; i++) {
		MVector oldView = viewPP[i] - deltaViewPP[i];
		// cerr << "viewPP[i]: " << viewPP[i] << " viewPP[i] length: " << viewPP[i].length() << endl;
		// cerr << "deltaViewPP[i]: " << deltaViewPP[i] << " deltaViewPP[i] length: " << deltaViewPP[i].length() << endl;
		// cerr << "oldView: " << oldView << " oldView length: " << oldView.length() << endl;
		c1 =  upVectorPP[i] ^ oldView;
		c2 =  upVectorPP[i] ^ viewPP[i];
		angles[i] = c1.angle(c2);
		signs[i] = ( (c1 ^ c2) * upVectorPP[i] > 0.0) ?	-1 : 1 ;

		//	cerr << "angles[i] " << angles[i] << " signs[i]: " << signs[i] << endl;

	}

	// angles are now 0 to PI radians
	MDoubleArray lutResult(len);

	st = doRampLookup(thisMObject(), articulate::aBendLookup, angles, lutResult, 0,
	                  3.141592653, 0.0, float(bendMagnitude)); 	mser;
	//	cerr << "calculateBend  1" << endl;
	double bendForce, springForce , newDeltaBend;
	//cerr << "lookupVals " << lookupVals << endl;
	for (unsigned i = 0; i < len; i++) {
		double &newBend = outBendPP[i];
		bendForce = lutResult[i] * signs[i];
		//	cerr << "bendForce " << bendForce  << endl;

		if (bendImpulse) { bendForce += bendImpulsePP[i]; }


		double springForce = -bendPP[i] * bendSpring;

		//	cerr << "springForce " << springForce << endl;

		newDeltaBend =  (deltaBendPP[i] * conserveBend) + (springForce + bendForce );
		newBend = bendPP[i] + newDeltaBend;
		//	cerr << "newBend " << newBend << endl;
	}

	return MS::kSuccess;
}



/*
What we are trying to do here is create flapping motion that is appropriate for the way the fish moves through the water.
The fish is receiving thrust from somewhere - we assume from the AI network or fields. Thrust creates an acceleration for the fish, therefore it
would be caused by the flapping speed and amplitude.
It is also convenient to imagine the fish is affected by some drag in the water that is proportional to its velocity. Theoretically there would be
thrust to counter this drag anyway, but as we are not in a perfect world we will allow the artist to set a drag value that will cause the fishes velocity
to contribute to the flapping motion.

The thrust value can be interopreted as forward thrust by projecting to the view vector.
That value can be used to lookup a contribution based on angle of thrust to the fishes view direction.

The two components are added together to create an effort metric. The flap offset and amplitude are then derived as functions of effort.
*/

MStatus articulate::calculateEffort(
  const MVectorArray &thrustPP,
  const MVectorArray &viewPP,
  const MVectorArray &velocityPP,
  double thrustMax,
  double velocityMax,
  double effortSpan,
  double effortAttack,
  double effortDecay,
  MDoubleArray &lastEffortPP,
  MDoubleArray &outEffortPP,
  bool doThrust,
  bool doDrag,
  ThrustFacingStrategy thrustStrategy
) const
{
	MStatus st;

	unsigned len = thrustPP.length();
	if ( viewPP.length()      != len) { return MS::kFailure; }
	if ( velocityPP.length() != len) { return MS::kFailure; }
	if ( outEffortPP.length() != len) { return MS::kFailure; }
	if (thrustMax == 0) { thrustMax = 1.0; }

	double thrustMaxRecip = 1.0 / thrustMax;




	MDoubleArray thrustValues(len, 1.0);
	if (doThrust) {
		MDoubleArray thrustFacing(len);
		MDoubleArray thrustLengths(len);
		for (unsigned i = 0; i < len; i++) {
			thrustLengths[i] = thrustPP[i].length() * thrustMaxRecip;
			MVector n = thrustPP[i].normal();
			thrustFacing[i] = n * viewPP[i];
		}

		if (thrustStrategy != articulate::kOff) {
			float oldMin = (thrustStrategy = articulate::kForward)  ? 	0.0f : -1.0f ;
			st = doRampLookup(thisMObject(), articulate::aThrustFacingLookup, thrustFacing,
			                  thrustValues, oldMin, 1.0f, 0.0f, 1.0f); mser;
		}
		//cerr << "thrustLutResult " << thrustLutResult << endl;
		for (unsigned i = 0; i < len; i++) {
			thrustValues[i] = (thrustValues[i] * thrustLengths[i]) ;
		}
	}

	MDoubleArray dragValues(len);
	if (doDrag) {
		if (velocityMax == 0) { velocityMax = 1.0; }
		double velocityMaxRecip = 1.0 / velocityMax;
		MDoubleArray velocityLengths(len);
		for (unsigned i = 0; i < len; i++) {
			velocityLengths[i] = velocityPP[i].length() * velocityMaxRecip;
		}
		st = doRampLookup(thisMObject(), articulate::aDragLookup, velocityLengths, dragValues,
		                  0.0f, 1.0f, 0.0f, 1.0f); mser;
		//	cerr << "velocityLutResult " << thrustLutResult << endl;
	}

	if (effortSpan == 0) { effortSpan = 1.0; }
	double effortSpanRecip = 1.0 / effortSpan;


	// for (unsigned i=0;i<len;i++) {
	// 		cerr << "thrustValues[i] " << thrustValues[i] << endl;
	// 		cerr << "dragValues[i] " << dragValues[i] << endl;
	// }
	if (  (lastEffortPP.length() == len)  && ( (effortAttack != 1.0)
	      ||  (effortDecay != 1.0)  ) ) {
		for (unsigned i = 0; i < len; i++) {
			double tmp = (thrustValues[i] + dragValues[i]) * effortSpanRecip;
			double delta = tmp - lastEffortPP[i];
			if (delta > 0) {
				delta = delta * effortAttack;
			}
			else {
				delta = delta * effortDecay;
			}

			outEffortPP[i] = lastEffortPP[i] + delta;
		}
	}
	else {
		for (unsigned i = 0; i < len; i++) {
			outEffortPP[i] = (thrustValues[i] + dragValues[i]) * effortSpanRecip	;
		}
	}

	return MS::kSuccess;
}

/////////////////////////////////
/*
MStatus articulate::doRampLookup(
	const MObject& node,
	const MObject& attribute,
	const MDoubleArray& in,
	MDoubleArray& results,
	float oldMin,
	float oldMax,
	float newMin,
	float newMax ) const
{
	MStatus st;
	MRampAttribute rampAttr( node, attribute, &st ); msert;
	unsigned n = in.length();
	results.setLength(n);

	float oldRange = oldMax - oldMin;
	float oneOverOldRange = 1.0;
	if (oldRange != 0.0){
		oneOverOldRange = 1.0 / oldRange;
	}
	float newRange = newMax - newMin;

	for( unsigned i = 0; i < n; i++ )
	{
		float v = (float(in[i]) - oldMin ) * oneOverOldRange;
		float res;
		rampAttr.getValueAtPosition( v, res, &st ); mser;
		res = (res * newRange) + newMin;
		results[i] = double( res ) ;
	}

	return MS::kSuccess;
}
*/