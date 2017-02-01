/*
 *  legacySteerRemap.cpp
 *  jtools
 *
 *  Created by Julian Mann on 29/12/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */
#include <maya/MIOStream.h>

#include <legacySteerRemap.h>
// using namespace std;

#include <math.h>

#include <maya/MFnAnimCurve.h>
#include <maya/MAnimControl.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>

#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MQuaternion.h>
#include <attrUtils.h>
#include <lookup.h>
#include "jMayaIds.h"


// #include <attributeInlines.h>

const double ONE_EIGHTY_OVER_PI = 57.29578;
const float  ONE_EIGHTY_OVER_PIf = 57.29578f;

MObject legacySteerRemap::aRotMethod;    
MObject legacySteerRemap::aMagMethod;    
MObject legacySteerRemap::aReference;    
MObject legacySteerRemap::aGoal;  

MObject legacySteerRemap::aRotCurve;   
MObject legacySteerRemap::aMagCurve;   
MObject legacySteerRemap::aOutput;            
          


MTypeId legacySteerRemap::id( k_legacySteerRemap );

legacySteerRemap::legacySteerRemap() {
}

legacySteerRemap::~legacySteerRemap() {
}

void *legacySteerRemap::creator()
{
    return new legacySteerRemap;
}



MStatus legacySteerRemap::initialize()

{
  MStatus st;
  MString method("legacySteerRemap::initialize");
  
  MFnTypedAttribute   tAttr;
  MFnMessageAttribute   mAttr;
  MFnNumericAttribute   nAttr;
  MFnEnumAttribute    eAttr;
  MFnUnitAttribute    uAttr;
  
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
  
  aRotMethod = eAttr.create( "calculateRotation", "clr", legacySteerRemap::kMult, &st ); ert;
  eAttr.addField( "remap",    legacySteerRemap::kRemap);
  eAttr.addField( "multiply",   legacySteerRemap::kMult);
  eAttr.setKeyable(true);
  eAttr.setStorable(true);
  st = addAttribute(aRotMethod);
  
  aMagMethod = eAttr.create( "calculateMagnitude", "clm", legacySteerRemap::kMult, &st ); ert;
  eAttr.addField( "remap",    legacySteerRemap::kRemap);
  eAttr.addField( "multiply",   legacySteerRemap::kMult);
  eAttr.setKeyable(true);
  eAttr.setStorable(true);
  st = addAttribute(aMagMethod);
  
  
  
  aMagCurve = nAttr.create("magnitudeLookup","mlu", MFnNumericData::kFloat);
  nAttr.setStorable(true);
  nAttr.setWritable(true);
  nAttr.setKeyable(true);
  st = addAttribute( aMagCurve ); ert;
  
  
  aRotCurve = nAttr.create("rotationLookup","rlu", MFnNumericData::kFloat);
  nAttr.setStorable(true);
  nAttr.setWritable(true);
  nAttr.setKeyable(true);
  st = addAttribute( aRotCurve ); ert;
  
  aOutput = tAttr.create("output","o", MFnData::kVectorArray);
  tAttr.setStorable(false);
  tAttr.setReadable(true);
  st = addAttribute( aOutput ); ert;

  st = attributeAffects( aReference,    aOutput );ert;
  st = attributeAffects( aGoal,       aOutput );ert;
  
  st = attributeAffects( aRotMethod,    aOutput );ert;
  st = attributeAffects( aMagMethod,    aOutput );ert;

  st = attributeAffects( aRotCurve,     aOutput);ert;
  st = attributeAffects( aMagCurve,     aOutput);ert;
  
  
  
  return( MS::kSuccess );
}


MStatus legacySteerRemap::compute(const MPlug& plug, MDataBlock& data)
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
    

    //////////////////////////////////////////////////////////////
    MFnAnimCurve rAniFn; 
    MFnAnimCurve mAniFn; 
    
    lookup rLut, mLut;
    
    st = getAniCurveFn(thisMObject(), aRotCurve, rAniFn);
    if (st.error()) {
      float constVal = data.inputValue(aRotCurve).asFloat();
      rLut = lookup(constVal);
    } else {
      rLut = lookup(rAniFn, 18);// divide the curve (0 - 180 into 10 degree increments)
    }
    
    st = getAniCurveFn(thisMObject(),aMagCurve, mAniFn);
    if (st.error()) {
      float constVal = data.inputValue(aMagCurve).asFloat();
      mLut = lookup(constVal);
    } else {
      mLut = lookup(mAniFn, 18); // divide the curve (0 - 180 into 10 degree increments)
    }
    
    // Get steering parameters
    RemapMethod rotMethod = RemapMethod(data.inputValue(aRotMethod).asShort());
    RemapMethod magMethod =  RemapMethod(data.inputValue(aMagMethod).asShort());
    
    
    MVectorArray result(nLength);
    
    MVector newVec ;
    float angleBetween, rotVal, magVal ;
    //cerr  << "here 16"<<endl;
    
    if ((rotMethod == legacySteerRemap::kMult) && (magMethod == legacySteerRemap::kMult)) { // multiply both
      
      for (unsigned i = 0;i< nLength;i++) {
        const MVector & r = ref[i];
        const MVector & g = goal[i];
        if (r.isEquivalent(MVector::zero) || g.isEquivalent(MVector::zero)  ) {
          result[i] = g;
          continue;
        }
        
        angleBetween = float(r.angle(g)) ;
        angleBetween *= ONE_EIGHTY_OVER_PIf; 
        
        
        rotVal = rLut.evaluate(angleBetween) ;
        magVal = mLut.evaluate(angleBetween) ;

        MQuaternion q(r , g , rotVal);
        newVec = r.rotateBy( q );
        newVec.normalize();
        
        result[i] = newVec * (magVal*g.length());
      }
    } else if ((rotMethod == legacySteerRemap::kRemap) && (magMethod == legacySteerRemap::kRemap)) {// remap both
                          //cerr  << "here 16_B"<<endl;
      for (unsigned i = 0;i<nLength;i++) {
        
        const MVector & r = ref[i];
        const MVector & g = goal[i];
        if (r.isEquivalent(MVector::zero) ||g.isEquivalent(MVector::zero)  ) {
          result[i] = g;
          continue;
        }
        
        angleBetween = float(r.angle(g)) ;
        angleBetween *= ONE_EIGHTY_OVER_PIf; 
        
        
        rotVal = rLut.evaluate(angleBetween) ;
        magVal = mLut.evaluate(angleBetween) ;
        
        double rotFraction =  rotVal / angleBetween ;
        
        MQuaternion q(r , g , rotFraction);
        newVec = r.rotateBy( q );
        newVec.normalize();
        
        result[i] = newVec * magVal;
      }
    } else if ((rotMethod == legacySteerRemap::kMult) && (magMethod == legacySteerRemap::kRemap)) {// remap mag, multiply rot
                         //cerr  << "here 16_C"<<endl;
      for (unsigned i = 0;i<nLength;i++) {
        
        const MVector & r = ref[i];
        const MVector & g = goal[i];
        if (r.isEquivalent(MVector::zero) ||g.isEquivalent(MVector::zero)  ) {
          result.set( g ,i);
          continue;
        }
        
        angleBetween = float(r.angle(g)) ;
        angleBetween *= ONE_EIGHTY_OVER_PIf; 
        
        
        rotVal = rLut.evaluate(angleBetween) ;
        magVal = mLut.evaluate(angleBetween) ;
        
        MQuaternion q(r , g , rotVal);
        newVec = r.rotateBy( q );
        newVec.normalize();
        
        result[i] = newVec * magVal;
      }
    }else if ((rotMethod == legacySteerRemap::kRemap) && (magMethod == legacySteerRemap::kMult))  {// remap  rot, multiply mag
                        //cerr  << "here 16_D"<<endl;
      for (unsigned i = 0;i<nLength;i++) {
        
        const MVector & r = ref[i];
        const MVector & g = goal[i];
        if (r.isEquivalent(MVector::zero) ||g.isEquivalent(MVector::zero)  ) {
          result[i] = g;
          continue;
        }
        
        angleBetween = float(r.angle(g)) ;
        angleBetween *= ONE_EIGHTY_OVER_PIf; 
        
        
        rotVal = rLut.evaluate(angleBetween) ;
        magVal = mLut.evaluate(angleBetween) ;
        
        double rotFraction =  rotVal / angleBetween ;
        
        MQuaternion q(r , g , rotFraction);
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

