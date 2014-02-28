/*
 *  rollerPP.cpp
 *  jtools
 *
 *  Created by Julian Mann on 02/12/2006.
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
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnDoubleArrayData.h>

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
#include <maya/MRampAttribute.h>
#include <maya/MPxNode.h>
#include "attrUtils.h"
#include "rollerPP.h"
#include "errorMacros.h"

#include "jMayaIds.h"
#include "mayaMath.h"

MObject rollerPP::aCurrentViewPP;	
MObject rollerPP::aLastViewPP;	
MObject rollerPP::aSpeedPP;
MObject rollerPP::aUpPP;		 	
MObject rollerPP::aSpeedInputMax;	
MObject rollerPP::aDoBanking;
MObject rollerPP::aBankingSpeedRamp;	
MObject rollerPP::aBankingStability;
MObject rollerPP::aBankingWorldUpStability;

MObject rollerPP::aDoRolling;	 	
MObject rollerPP::aRollingSpeedRamp;	
MObject rollerPP::aRollingStability;	
MObject rollerPP::aRollingWorldUpStability;
MObject rollerPP::aCurrentTime;
MObject rollerPP::aOutput;	


MTypeId rollerPP::id( k_rollerPP );

rollerPP::rollerPP(){
	//lastTimeIEvaluated = MAnimControl::currentTime();
	
}

rollerPP::~rollerPP(){}

void *rollerPP::creator()
{	
    return new rollerPP;
}


MStatus rollerPP::initialize()
//
//	Descriptions:
//		Initialize the node, attributes.
//
{
	MStatus st;
	MString method("rollerPP::initialize");
	
	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnCompoundAttribute	cAttr;
	MFnUnitAttribute	uAttr;


	aCurrentViewPP = tAttr.create("currentViewPP", "cvpp", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aCurrentViewPP ); er;

	aLastViewPP = tAttr.create("lastViewPP", "lvpp", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aLastViewPP ); er;
	
	aSpeedPP = tAttr.create("speedPP", "spp", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aSpeedPP ); er;
	
	aUpPP = tAttr.create("upPP", "upp", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aUpPP ); er;
	
	aSpeedInputMax = nAttr.create("speedInputMax", "sim", MFnNumericData::kDouble);	
	nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0);
	st = addAttribute(aSpeedInputMax);er;

	////////////////////////////////////////////////////////////
	aDoBanking = nAttr.create( "doBanking", "dbn",MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);
	addAttribute( aDoBanking ); 

	aBankingSpeedRamp = MRampAttribute::createCurveRamp("bankingSpeedRamp","bspr");
	st = addAttribute( aBankingSpeedRamp );er;
	
	aBankingStability = nAttr.create("bankingStability", "bst", MFnNumericData::kDouble);	
	nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0);
	st = addAttribute(aBankingStability);er;

	aBankingWorldUpStability = nAttr.create("bankingWorldStability", "bwst", MFnNumericData::kDouble);	
	nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0);
	st = addAttribute(aBankingWorldUpStability);er;


	aDoRolling = nAttr.create( "doRolling", "drl",MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);
	addAttribute( aDoRolling ); 

	aRollingSpeedRamp = MRampAttribute::createCurveRamp("rollingSpeedRamp","rspr");
	st = addAttribute( aRollingSpeedRamp );er;
	
	aRollingStability = nAttr.create("rollingStability", "rst", MFnNumericData::kDouble);	
	nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0);
	st = addAttribute(aRollingStability);er;
	
	aRollingWorldUpStability = nAttr.create("rollingWorldStability", "rwst", MFnNumericData::kDouble);	
	nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setDefault(1.0);
	st = addAttribute(aRollingWorldUpStability);er;



	///////////////////////////////////////////////////////////////////////
	aCurrentTime = uAttr.create( "currentTime", "ct", MFnUnitAttribute::kTime );
	uAttr.setStorable(true);
	st =  addAttribute(aCurrentTime);  er;

	aOutput = tAttr.create("output", "out", MFnData::kVectorArray);
    tAttr.setStorable(false);
    tAttr.setReadable(true);
    tAttr.setWritable(false);
	st = addAttribute(aOutput);er;

	st = attributeAffects(aCurrentTime, aOutput );er;

	return( MS::kSuccess );
}




MStatus rollerPP::compute(const MPlug& plug, MDataBlock& data)
//
//	Descriptions:
//		compute output force.
//

{
	MStatus st;
	//unsigned int counter = 0;
	MString method("rollerPP::compute");
	
	
	if(!( plug == aOutput ) ) return( MS::kUnknownParameter);

	MObject thisNode = thisMObject();
	
	
	

	MTime cT =  data.inputValue( aCurrentTime).asTime();


	MVectorArray view = MFnVectorArrayData(data.inputValue(aCurrentViewPP).data()).array();
	MVectorArray lastView = MFnVectorArrayData(data.inputValue(aLastViewPP).data()).array();
	MVectorArray up = MFnVectorArrayData(data.inputValue(aUpPP).data()).array();
	MDoubleArray speed = MFnDoubleArrayData(data.inputValue(aSpeedPP).data()).array();

	unsigned vwl = view.length();
	unsigned lvl = lastView.length();
	unsigned spl = speed.length();
	unsigned upl = up.length();

	if ( 
	(vwl !=  upl) || 
	(lvl !=  upl) || 
	(spl !=  upl) ) return MS::kFailure;	
	

	bool doBank = data.inputValue(aDoBanking).asBool();
	bool doRoll = data.inputValue(aDoRolling).asBool();
	
	float speedMax = data.inputValue(aSpeedInputMax).asFloat();
	double bankingStability = data.inputValue(aBankingStability).asDouble();
	double rollingStability = data.inputValue(aRollingStability).asDouble();
	double bankingWorldStability = data.inputValue(aBankingWorldUpStability).asDouble();
	double rollingWorldStability = data.inputValue(aRollingWorldUpStability).asDouble();
		

	MVectorArray  output(vwl);

	if (speedMax == 0.0) speedMax = 1.0;
	
	MDoubleArray bankSpeedLookup;
	if (doBank) doRampLookup(thisNode, aBankingSpeedRamp, speed, bankSpeedLookup,0.0f,speedMax,0.0f, 1.0f); 	

	MDoubleArray rollSpeedLookup;
	if (doRoll) st = doRampLookup(thisNode, aRollingSpeedRamp, speed, rollSpeedLookup,0.0f,speedMax,0.0f, 1.0f); 


	if (doRoll || doBank) {

		// get the view and last view projected onto the plane perpendicular to the up vector 
		for (unsigned i=0;i<vwl;i++) {
			MVector &out = 	output[i] ;
			MVector rollResult = MVector::zero ;
			MVector bankResult = MVector::zero ;
		
			if (doRoll) {
				MVector viewCross = (view[i]^lastView[i]).normal();
				double dot = viewCross*up[i];	
				double angle =  view[i].angle(lastView[i]);
				rollResult = (viewCross * dot * angle * rollSpeedLookup[i]) + (up[i] * rollingStability) + (MVector::yAxis * rollingWorldStability);
				rollResult.normalize();
			}
		
			if (doBank) {
				MVector deltaView = view[i] - lastView[i];
				bankResult = (deltaView * bankSpeedLookup[i]) + (up[i] * bankingStability) + (MVector::yAxis * bankingWorldStability);
				bankResult.normalize();
			}

			out = (rollResult + bankResult);
			
			if (doBank && doRoll ) out.normalize();
		}
	} else {
		output.copy(up); // pass through
	}

	MDataHandle hOutput = data.outputValue(aOutput );
	MFnVectorArrayData fnOutput ;
	MObject dOutput  = fnOutput .create( output, &st );er;
	hOutput.set( dOutput  );
	data.setClean( aOutput );
	
	return( MS::kSuccess );
}

