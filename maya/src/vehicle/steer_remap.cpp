/*
 *  steerRemap.cpp
 *  jtools
 *
 *  Created by Julian Mann on 29/12/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */
#include <maya/MIOStream.h>


// using namespace std;

#include <math.h>

#include <maya/MRampAttribute.h>
// #include <maya/MAnimControl.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnEnumAttribute.h>
// #include <maya/MFnUnitAttribute.h>
// #include <maya/MFnNumericAttribute.h>
// #include <maya/MFnMessageAttribute.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>

#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MQuaternion.h>
#include <attrUtils.h>
// #include <lookup.h>
#include "jMayaIds.h"
#include <steer_remap.h>

// #include <attributeInlines.h>

// const float  ONE_EIGHTY_OVER_PI = 57.29578f;
const float  one_over_pi = 0.31830988618;
// const float  pi = 3.14159265359;
MObject steerRemap::aRotMethod;    
MObject steerRemap::aMagMethod;    
MObject steerRemap::aReference;    
MObject steerRemap::aGoal;	

MObject steerRemap::aRotCurve;   
MObject steerRemap::aMagCurve;   
MObject steerRemap::aOutput; 					  
				  


MTypeId steerRemap::id( k_steerRemap );

steerRemap::steerRemap() {
}

steerRemap::~steerRemap() {
}

void *steerRemap::creator()
{
    return new steerRemap;
}



MStatus steerRemap::initialize()

{
	MStatus st;
	MString method("steerRemap::initialize");
	
	MFnTypedAttribute		tAttr;
	// MFnMessageAttribute		mAttr;
	// MFnNumericAttribute		nAttr;
	MFnEnumAttribute 		eAttr;
	// MFnUnitAttribute 		uAttr;
	MRampAttribute 			rAttr;
		
	aReference = tAttr.create("reference", "ref", MFnData::kVectorArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setWritable(true);
	tAttr.setCached(false);
	st = addAttribute( aReference); ert;
	
	aGoal = tAttr.create("goal", "goa", MFnData::kVectorArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setWritable(true);
	tAttr.setCached(false);
	st = addAttribute( aGoal); ert;
	
	aRotMethod = eAttr.create( "calculateRotation", "clr", steerRemap::kMult, &st ); ert;
	eAttr.addField( "remap",		steerRemap::kRemap);
	eAttr.addField( "multiply",		steerRemap::kMult);
	eAttr.setKeyable(true);
	eAttr.setStorable(true);
	st = addAttribute(aRotMethod);
	
	aMagMethod = eAttr.create( "calculateMagnitude", "clm", steerRemap::kMult, &st ); ert;
	eAttr.addField( "remap",		steerRemap::kRemap);
	eAttr.addField( "multiply",		steerRemap::kMult);
	eAttr.setKeyable(true);
	eAttr.setStorable(true);
	st = addAttribute(aMagMethod);
	
	aMagCurve = rAttr.createCurveRamp("magnitude","mag",&st);
	st = addAttribute(aMagCurve );er;
	
	aRotCurve = rAttr.createCurveRamp("rotation","rot",&st);
	st = addAttribute(aRotCurve );er;
	
	aOutput = tAttr.create("output","o", MFnData::kVectorArray);
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	st = addAttribute( aOutput ); ert;

	st = attributeAffects( aReference, 		aOutput );ert;
	st = attributeAffects( aGoal, 			aOutput );ert;
	
	st = attributeAffects( aRotMethod, 		aOutput );ert;
	st = attributeAffects( aMagMethod, 		aOutput );ert;

	st = attributeAffects( aRotCurve, 		aOutput);ert;
	st = attributeAffects( aMagCurve, 		aOutput);ert;
	
	
	
	return( MS::kSuccess );
}


MStatus steerRemap::compute(const MPlug& plug, MDataBlock& data)
{
	
	MStatus st;
	if (plug == aOutput) {
		

		MDataHandle hRef = data.inputValue(aReference, &st); 
		MObject dRef = hRef.data();
		MFnVectorArrayData fnRef(dRef, &st);
		const MVectorArray & ref = fnRef.array(&st);
		
		MDataHandle hGoal = data.inputValue(aGoal, &st); 
		MObject dGoal = hGoal.data();
		MFnVectorArrayData fnGoal(dGoal, &st);
		const MVectorArray & goal = fnGoal.array(&st);
		
		unsigned nLength = ref.length();
		if (goal.length() != nLength) return MS::kUnknownParameter;
		

		MRampAttribute rotRamp( thisMObject(), aRotCurve, &st ); ert;
		MRampAttribute magRamp( thisMObject(), aMagCurve, &st ); ert;
		//////////////////////////////////////////////////////////////
		// MFnAnimCurve rAniFn; 
		// MFnAnimCurve mAniFn; 
		
		// lookup rLut, mLut;
		
		// st = getAniCurveFn(thisMObject(), aRotCurve, rAniFn);
		// if (st.error()) {
		// 	float constVal = data.inputValue(aRotCurve).asFloat();
		// 	rLut = lookup(constVal);
		// } else {
		// 	rLut = lookup(rAniFn, 18);// divide the curve (0 - 180 into 10 degree increments)
		// }
		
		// st = getAniCurveFn(thisMObject(),aMagCurve, mAniFn);
		// if (st.error()) {
		// 	float constVal = data.inputValue(aMagCurve).asFloat();
		// 	mLut = lookup(constVal);
		// } else {
		// 	mLut = lookup(mAniFn, 18); // divide the curve (0 - 180 into 10 degree increments)
		// }
		
		// Get steering parameters
		RemapMethod rotMethod = RemapMethod(data.inputValue(aRotMethod).asShort());
		RemapMethod magMethod =  RemapMethod(data.inputValue(aMagMethod).asShort());
		
		
		MVectorArray result(nLength);
		
		MVector newVec ;
		float angleBetween, rotVal, magVal ;

		if ((rotMethod == steerRemap::kMult) && (magMethod == steerRemap::kMult)) { // multiply both
			
			for (unsigned i = 0;i< nLength;i++) {
				const MVector & r = ref[i];
				const MVector & g = goal[i];
				if ( r.isEquivalent(MVector::zero) || 
					g.isEquivalent(MVector::zero)  || 
					r.isParallel(g)) {
					result[i] = g;
					continue;
				}
				
				angleBetween = float(r.angle(g)) ;
				angleBetween *= one_over_pi; // 0 - 1				
				rotRamp.getValueAtPosition( angleBetween, rotVal, &st ); er;
				magRamp.getValueAtPosition( angleBetween, magVal, &st ); er;

				MQuaternion q(r ,g ,rotVal);
				newVec = r.rotateBy(q);
				newVec.normalize();
				
				result[i] = (newVec * magVal * g.length());
			}
		} else if ((rotMethod == steerRemap::kRemap) && (magMethod == steerRemap::kRemap)) {// remap both
												  //cerr  << "here 16_B"<<endl;
			for (unsigned i = 0;i<nLength;i++) {
				
				const MVector & r = ref[i];
				const MVector & g = goal[i];
				if ( r.isEquivalent(MVector::zero) || 
					g.isEquivalent(MVector::zero)  || 
					r.isParallel(g)) {
					result[i] = g;
					continue;
				}
				
				
				angleBetween = float(r.angle(g)) ;
				angleBetween *= one_over_pi; // 0 - 1
				rotRamp.getValueAtPosition( angleBetween, rotVal, &st ); er;
				magRamp.getValueAtPosition( angleBetween, magVal, &st ); er;
				
				rotVal =  rotVal / angleBetween ;
				MQuaternion q(r , g , rotVal);
				newVec = r.rotateBy( q );

				newVec.normalize();
				result[i] = newVec * magVal;
			}
		} else if ((rotMethod == steerRemap::kMult) && (magMethod == steerRemap::kRemap)) {// remap mag, multiply rot
											   //cerr  << "here 16_C"<<endl;
			for (unsigned i = 0;i<nLength;i++) {
				
				const MVector & r = ref[i];
				const MVector & g = goal[i];
				if ( r.isEquivalent(MVector::zero) || 
					g.isEquivalent(MVector::zero)  || 
					r.isParallel(g)) {
					result[i] = g;
					continue;
				}
				

				angleBetween = float(r.angle(g)) ;
				angleBetween *= one_over_pi; // 0 - 1
				rotRamp.getValueAtPosition( angleBetween, rotVal, &st ); er;
				magRamp.getValueAtPosition( angleBetween, magVal, &st ); er;
				
				MQuaternion q(r , g , rotVal);
				newVec = r.rotateBy( q );

				newVec.normalize();
				result[i] = newVec * magVal;
			}
		}else if ((rotMethod == steerRemap::kRemap) && (magMethod == steerRemap::kMult))  {// remap  rot, multiply mag
											  //cerr  << "here 16_D"<<endl;
			for (unsigned i = 0;i<nLength;i++) {
				
				const MVector & r = ref[i];
				const MVector & g = goal[i];
				if ( r.isEquivalent(MVector::zero) || 
					g.isEquivalent(MVector::zero)  || 
					r.isParallel(g)) {
					result[i] = g;
					continue;
				}
				
				
				angleBetween = float(r.angle(g)) ;
				angleBetween *= one_over_pi; // 0 - 1
				rotRamp.getValueAtPosition( angleBetween, rotVal, &st ); er;
				magRamp.getValueAtPosition( angleBetween, magVal, &st ); er;
				
				rotVal=  rotVal / angleBetween ;
				
				MQuaternion q(r , g , rotVal);
				newVec = r.rotateBy( q );
				newVec.normalize();
				
				result[i] = newVec * (magVal*g.length());
			}
		}
		
		
		MDataHandle hOut = data.outputValue(aOutput);
		MFnVectorArrayData fnOut;
		MObject dOut = fnOut.create(result);
		hOut.set(dOut);
		st = data.setClean( aOutput);er;
		
		
		
	}
	return MS::kSuccess;
}

