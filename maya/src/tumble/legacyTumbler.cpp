/*
*  legacyTumbler.cpp
*  jtools
*
*  Created by Julian Mann on 02/12/2006.
*  Copyright 2006 hooly|mama. All rights reserved.
*
*/

//#include <boost/shared_ptr.hpp>

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
//#include <maya/MFnPlugin.h>
#include <maya/MPxNode.h>
#include <maya/MFnPluginData.h>


#include <legacyTumbler.h>
#include <errorMacros.h>

#include "jMayaIds.h"

//#include <attributeInlines.h>
//#define OPENMAYA_EXPORT





MObject legacyTumbler::aPosition;
MObject legacyTumbler::aVelocity;
MObject legacyTumbler::aMass;


MObject legacyTumbler::aFrontAxis;  
MObject legacyTumbler::aUpAxis;   
MObject legacyTumbler::aGoalFront;  
MObject legacyTumbler::aGoalUp;
MObject legacyTumbler::aGoalWeightPP; 

MObject legacyTumbler::aImpulse ;
MObject legacyTumbler::aCollisionImpulse;


MObject legacyTumbler::aCollisionGoalRange ;
MObject legacyTumbler::aCollisionGoalWeightMap ;

MObject legacyTumbler::aImpulseGoalRange ;
MObject legacyTumbler::aImpulseGoalWeightMap;

MObject legacyTumbler::aVelocityGoalRange ;
MObject legacyTumbler::aVelocityGoalWeightMap;




MObject legacyTumbler::aPhi;
MObject legacyTumbler::aOmega;

MObject legacyTumbler::aGoalWeight;

MObject legacyTumbler::aLocalImpulse;
  
MObject legacyTumbler::aTorqueConserve; 
MObject legacyTumbler::aRotateOrder;
MObject legacyTumbler::aOutputUnit;

MObject legacyTumbler::aOutPoints;  
MObject legacyTumbler::aOutPhi; 
MObject legacyTumbler::aOutOmega;   
MObject legacyTumbler::aOutFront;   
MObject legacyTumbler::aOutUp;  
MObject legacyTumbler::aOutRotation;
MObject legacyTumbler::aOutRotationData;
MObject legacyTumbler::aCurrentTime;



MTypeId legacyTumbler::id( k_legacyTumbler );

legacyTumbler::legacyTumbler(){

  m_lastTimeIEvaluated = MAnimControl::currentTime();
}

legacyTumbler::~legacyTumbler(){}

void *legacyTumbler::creator()
{ 
  return new legacyTumbler;
}


MStatus legacyTumbler::initialize()
//
//  Descriptions:
//    Initialize the node, attributes.
//
{
  MStatus st;
  MString method("legacyTumbler::initialize");

  MFnNumericAttribute nAttr;
  MFnTypedAttribute tAttr;
  MFnCompoundAttribute  cAttr;
  MFnUnitAttribute  uAttr;
  MFnEnumAttribute eAttr;

// data from particle object
  aPosition = tAttr.create("position", "pos", MFnData::kVectorArray);
  tAttr.setStorable(false);
  tAttr.setReadable(false);
  st = addAttribute( aPosition );er;
  
  aVelocity = tAttr.create("velocity", "vel", MFnData::kVectorArray);
  tAttr.setReadable(false);
  st = addAttribute( aVelocity );er;

  aMass = tAttr.create("mass", "mas", MFnData::kDoubleArray);
  tAttr.setStorable(false);
  tAttr.setReadable(false);
  st = addAttribute( aMass );er;




 //   aDeltaTime = uAttr.create( "deltaTime", "dt", MFnUnitAttribute::kTime );
 //   uAttr.setStorable( true );


  aFrontAxis = eAttr.create( "frontAxis", "fa", mayaMath::xAxis);
  eAttr.addField( "X",    mayaMath::xAxis);
  eAttr.addField( "Y",   mayaMath::yAxis );
  eAttr.addField( "Z",   mayaMath::zAxis );
  eAttr.addField( "negX",   mayaMath::xAxisNeg);
  eAttr.addField( "negY",  mayaMath::yAxisNeg );
  eAttr.addField( "negZ",  mayaMath::zAxisNeg );
  eAttr.setKeyable(true);
  eAttr.setHidden(false);
  st = addAttribute( aFrontAxis );er;

  aUpAxis = eAttr.create( "upAxis", "ua", mayaMath::yAxis);
  eAttr.addField( "X",    mayaMath::xAxis   );
  eAttr.addField( "Y",    mayaMath::yAxis   );
  eAttr.addField( "Z",    mayaMath::zAxis   );
  eAttr.addField( "negX",   mayaMath::xAxisNeg  );
  eAttr.addField( "negY",   mayaMath::yAxisNeg  );
  eAttr.addField( "negZ",   mayaMath::zAxisNeg  );
  eAttr.setKeyable(true);
  eAttr.setHidden(false);
  st = addAttribute( aUpAxis );er;

  aGoalFront = tAttr.create("goalFrontVector", "gfnt", MFnData::kVectorArray , &st ); er;
  tAttr.setStorable(false);
  st = addAttribute( aGoalFront ); er;

  aGoalUp = tAttr.create("goalUpVector", "gup", MFnData::kVectorArray , &st ); er;
  tAttr.setStorable(false);
  st = addAttribute( aGoalUp ); er;

  aGoalWeightPP = tAttr.create("goalWeightPP", "gwpp", MFnData::kDoubleArray , &st ); er;
  tAttr.setDisconnectBehavior(MFnAttribute::kReset);
  tAttr.setStorable(false);
  st = addAttribute( aGoalWeightPP ); er;

  aImpulse = tAttr.create("impulse", "ii", MFnData::kVectorArray, &st);er;
  tAttr.setStorable(false);
  tAttr.setReadable(false);
  st = addAttribute( aImpulse ); er;
  
  aCollisionImpulse = tAttr.create("collisionImpulse", "cim", MFnData::kVectorArray, &st);er;
  tAttr.setStorable(false);
  tAttr.setReadable(false);
  st = addAttribute( aCollisionImpulse ); er;
  

  aCollisionGoalRange = nAttr.create("collisionGoalRange", "cgr", MFnNumericData::kDouble); 
  nAttr.setKeyable(true);
  nAttr.setStorable(true);
  nAttr.setDefault(1.0);
  st = addAttribute(aCollisionGoalRange);er;

  aCollisionGoalWeightMap = MRampAttribute::createCurveRamp("collisionGoalWeightMap","cgwt");
  st = addAttribute( aCollisionGoalWeightMap );er;
  

  
  aImpulseGoalRange = nAttr.create("impulseGoalRange", "igr", MFnNumericData::kDouble); 
  nAttr.setKeyable(true);
  nAttr.setStorable(true);
  nAttr.setDefault(1.0);
  st = addAttribute(aImpulseGoalRange);er;
  
  aImpulseGoalWeightMap = MRampAttribute::createCurveRamp("impulseGoalWeightMap","igwt");
  st = addAttribute( aImpulseGoalWeightMap );er;
  
  
  aVelocityGoalRange = nAttr.create("velocityGoalRange", "vgr", MFnNumericData::kDouble); 
  nAttr.setKeyable(true);
  nAttr.setStorable(true);
  nAttr.setDefault(1.0);
  st = addAttribute(aVelocityGoalRange);er;
  
  aVelocityGoalWeightMap = MRampAttribute::createCurveRamp("velocityGoalWeightMap","vgwt");
  st = addAttribute( aVelocityGoalWeightMap );er;


  
  
  ///////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  aLocalImpulse = nAttr.create( "localImpulse", "lim",MFnNumericData::kBoolean);
  nAttr.setStorable(true);
  nAttr.setKeyable(true);
  nAttr.setDefault(true);
  addAttribute( aLocalImpulse );


  aPhi = tAttr.create("phi", "ph", MFnData::kVectorArray);
  tAttr.setStorable(false);
  tAttr.setReadable(false);
  st = addAttribute(aPhi);er;

  aOmega= tAttr.create("omega", "om", MFnData::kVectorArray);
  tAttr.setStorable(false);
  tAttr.setReadable(false);
  st = addAttribute(aOmega);er;



  aGoalWeight = nAttr.create("goalWeight", "gwt", MFnNumericData::kDouble); 
  nAttr.setKeyable(true);
  nAttr.setStorable(true);
  nAttr.setDefault(1.0);
  st = addAttribute(aGoalWeight);er;

 
  aTorqueConserve = nAttr.create("conserveSpin", "csp", MFnNumericData::kDouble); 
  nAttr.setKeyable(true);
  nAttr.setStorable(true);
  nAttr.setDefault(1.0);
  st = addAttribute(aTorqueConserve);er;

  aRotateOrder = eAttr.create( "rotateOrder", "ro", ROTATE_ORDER_XYZ);
  eAttr.addField("xyz", ROTATE_ORDER_XYZ);
  eAttr.addField("yzx", ROTATE_ORDER_YZX);
  eAttr.addField("zxy", ROTATE_ORDER_ZXY);
  eAttr.addField("xzy", ROTATE_ORDER_XZY);
  eAttr.addField("yxz", ROTATE_ORDER_YXZ);
  eAttr.addField("zyx", ROTATE_ORDER_ZYX);
  eAttr.setKeyable(true);
  eAttr.setHidden(false);
  st = addAttribute( aRotateOrder );er;

  aOutputUnit = eAttr.create( "angularUnit", "ang", mayaMath::kDegrees);
  eAttr.addField("radians",  mayaMath::kRadians);
  eAttr.addField("degrees", mayaMath::kDegrees);
  uAttr.setKeyable(true);
  uAttr.setHidden(false);
  st = addAttribute( aOutputUnit );er;

  aOutPhi = tAttr.create("outPhi", "oph", MFnData::kVectorArray);
  tAttr.setStorable(false);
  tAttr.setReadable(true);
  tAttr.setWritable(false);

  aOutOmega= tAttr.create("outOmega", "oom", MFnData::kVectorArray);
  tAttr.setStorable(false);
  tAttr.setReadable(true);
  tAttr.setWritable(false);

  aOutFront= tAttr.create("outFront", "ofn", MFnData::kVectorArray);
  tAttr.setStorable(false);
  tAttr.setReadable(true);
  tAttr.setWritable(false);

  aOutUp= tAttr.create("outUp", "oup", MFnData::kVectorArray);
  tAttr.setStorable(false);
  tAttr.setReadable(true);
  tAttr.setWritable(false);

  aOutRotation = tAttr.create("outRotation", "orot", MFnData::kVectorArray , &st ); er;
  tAttr.setStorable(false);
  tAttr.setReadable(true);
  tAttr.setWritable(false);

  aOutPoints = tAttr.create("outPoints", "opns", MFnData::kVectorArray , &st ); er;
  tAttr.setStorable(false);
  tAttr.setReadable(true);
  tAttr.setWritable(false);
  
  aOutRotationData = cAttr.create("outRotationData","ord", &st ); er;
  cAttr.addChild(aOutPhi);
  cAttr.addChild(aOutOmega);
  cAttr.addChild(aOutFront);
  cAttr.addChild(aOutUp);
  cAttr.addChild(aOutRotation);
    cAttr.addChild(aOutPoints);
  cAttr.setWritable(false);
  st = addAttribute(aOutRotationData);er;


  // Time
  ///////////////////////////////////////////////////////////////////////
  aCurrentTime = uAttr.create( "currentTime", "ct", MFnUnitAttribute::kTime );
  uAttr.setStorable(true);
  st =  addAttribute(aCurrentTime);  er;

  st = attributeAffects(aCurrentTime, aOutRotationData );er;


  return( MS::kSuccess );
}


MStatus legacyTumbler::compute(const MPlug& plug, MDataBlock& data)
//
//  Descriptions:
//    compute output force.
//

{
  MStatus st;
  //unsigned int counter = 0;
  MString method("legacyTumbler::compute");

  // cerr << method << endl;
  if( ! (plug.parent() == aOutRotationData) ) return( MS::kUnknownParameter);


  //////////////////////////////////////////////////
  MPlug pPlug = plug.parent((&st));

  if (st.error()) return( MS::kFailure);


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
  MVectorArray phi = fnPhi.array( &st );er;

  MDataHandle hOmega =data.inputValue( aOmega );
  MObject dOmega = hOmega.data();
  MFnVectorArrayData fnOmega( dOmega );
  MVectorArray omega = fnOmega.array( &st );er;

  MDataHandle hGoalFront = data.inputValue( aGoalFront );
  MObject dGoalFront = hGoalFront.data();
  MFnVectorArrayData fnGoalFront( dGoalFront );
  MVectorArray goalFront = fnGoalFront.array( &st );

  MDataHandle hGoalUp =data.inputValue( aGoalUp );
  MObject dGoalUp = hGoalUp.data();
  MFnVectorArrayData fnGoalUp( dGoalUp );
  MVectorArray goalUp = fnGoalUp.array( &st );

  MDataHandle hGoalPP = data.inputValue( aGoalWeightPP );
  MObject dGoalPP = hGoalPP.data();
  MFnDoubleArrayData fnGoalPP( dGoalPP );
  MDoubleArray goalWeightPP = fnGoalPP.array( &st );

  MDataHandle hImpulse =data.inputValue( aImpulse );
  MObject dImpulse = hImpulse.data();
  MFnVectorArrayData fnImpulse( dImpulse );
  MVectorArray impulse = fnImpulse.array( &st );


  MDataHandle hCollisionImpulse =data.inputValue( aCollisionImpulse );
  MObject dCollisionImpulse = hCollisionImpulse.data();
  MFnVectorArrayData fnCollisionImpulse( dCollisionImpulse );
  MVectorArray collisionImpulse = fnCollisionImpulse.array( &st );

  unsigned len = phi.length();



  MVectorArray  outPhi(len);
  MVectorArray  outOmega(len);
  MVectorArray  outFront(len);
  MVectorArray  outUp(len);
  MVectorArray  outRotation(len);
  MVectorArray  outPoints;
   outPoints.copy(points);
  
  
  if (dt > 0.0) {

    //cerr << "about to simstep" << endl;
    st = simStep(   dt,
      data, 
      phi,
      omega,
      goalFront,
      goalUp,
      goalWeightPP,
      impulse,
      collisionImpulse,
      points,
      velocities,
      outPhi,
      outOmega,
      outFront,
      outUp,          
      outRotation
      );
  }
//  cerr << "done  simstep" << endl;


  MDataHandle hOutFront = data.outputValue(aOutFront);
  MFnVectorArrayData fnOutFront;
  MObject dOutFront = fnOutFront.create( outFront, &st );er;
  hOutFront.set( dOutFront );
//  data.setClean( aOutFront);

  MDataHandle hOutUp = data.outputValue(aOutUp);
  MFnVectorArrayData fnOutUp;
  MObject dOutUp = fnOutUp.create( outUp, &st );er;
  hOutUp.set( dOutUp );
//  data.setClean( aOutUp);

  MDataHandle hOutPhi = data.outputValue(aOutPhi);
  MFnVectorArrayData fnOutPhi;
  MObject dOutPhi = fnOutPhi.create( outPhi, &st );er;
  hOutPhi.set( dOutPhi );
//  data.setClean( aOutPhi);

  MDataHandle hOutOmega = data.outputValue(aOutOmega);
  MFnVectorArrayData fnOutOmega;
  MObject dOutOmega = fnOutOmega.create( outOmega, &st );er;
  hOutOmega.set( dOutOmega );
//  data.setClean( aOutOmega);

  MDataHandle hOutRotation = data.outputValue(aOutRotation);
  MFnVectorArrayData fnOutRotation;
  MObject dOutRotation = fnOutRotation.create( outRotation, &st );er;
  hOutRotation.set( dOutRotation );
//  data.setClean( aOutRotation);''
  
    MDataHandle hOutPoints = data.outputValue(aOutPoints);
    MFnVectorArrayData fnOutPoints;
    MObject dOutPoints = fnOutPoints.create( points, &st );er;
    hOutPoints.set( dOutPoints );
  //  data.setClean( aOutPoints);

  data.setClean( aOutRotationData);

//  cerr << "done set plugs Clean" << endl;
  return( MS::kSuccess );
}


MStatus legacyTumbler::simStep
  (
  double dt,
  MDataBlock &data, 
  const MVectorArray &phi,
  const MVectorArray &omega,
  const MVectorArray &goalFront,
  const MVectorArray &goalUp,
  const MDoubleArray &goalWeightPP,
  const MVectorArray &impulse,
  const MVectorArray &collisionImpulse,
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
  MString method("legacyTumbler::calculateRotation");

  if (dt == 0.0) {
    dt = 1.0;
  }
  double dtRecip2 = 1.0 / (dt*dt);

  ////////////////////////////////////////////////////
//  double torqueMagValue = data.inputValue(aTorqueMag).asDouble();

  double impulseGoalRange = data.inputValue(aImpulseGoalRange).asDouble();
  double collisionGoalRange = data.inputValue(aCollisionGoalRange).asDouble();
  double velocityGoalRange = data.inputValue(aVelocityGoalRange).asDouble();
  
  if (impulseGoalRange   == 0.0) impulseGoalRange = 1.0;
  if (collisionGoalRange == 0.0) collisionGoalRange = 1.0;
  
  double torqueConValue = data.inputValue(aTorqueConserve).asDouble();
  double goalWeightValue = data.inputValue(aGoalWeight).asDouble();
  short rotateOrder = data.inputValue(aRotateOrder).asShort();
  short outUnit =  data.inputValue(aOutputUnit).asShort();
//  double dist = data.inputValue(aSphereSize).asDouble();

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
  
  
  // cerr << "phi.length()"<< phi.length() << endl;
  // cerr << "omega.length()"<< omega.length() << endl;
  // cerr << "goalFront.length()"<< goalFront.length()<< endl;
  // cerr << "goalUp.length()"<< goalUp.length()<< endl;
  // cerr << "goalWeightPP.length()"<< goalWeightPP.length()<< endl;
  // cerr << "impulse.length()"<< impulse.length()<< endl;
  // cerr << "collisionImpulse.length() "<< collisionImpulse.length()<< endl;
  
  
  if (!len) return MS::kFailure;
  
  if (!(  (omega.length() == len)  
  &&  (goalFront.length() == len) 
  &&  (goalUp.length() == len)  
  )) {
    MGlobal::displayWarning("legacyTumbler array length mismatch - omega, goalFront and goalUp must be the same length");
    return MS::kFailure;
  }
  
  
  bool doLocalImpulse = data.inputValue(aLocalImpulse).asBool();

  // bool doAppiedForce = false;
  bool doImpulse = false;
  bool doCollisionImpulse = false;
  bool doGoal = false;
  bool doVelocityGoal= false;
  bool doGoalPP = false;
//  if  (forces.length() == len*4) doAppiedForce =true;
  if  (impulse.length() == len) doImpulse =true;
  if  (collisionImpulse.length() == len) doCollisionImpulse =true;
  if  (velocities.length() == len) doVelocityGoal =true;
  if  ((goalFront.length() == len) && (goalUp.length() == len)) doGoal = true;
  if  (goalWeightPP.length() == len) doGoalPP = true;
  
  
  
  //////////////////////////////////////////////////////////////
  MIntArray indexes ;
  MFloatArray positions ;
  MFloatArray values;
  MIntArray interps;
  bool doColRamp = false;
  bool doImpRamp = false;
  bool doVelRamp = false;
  unsigned vlen ;
  float val;
  

  // TODO - dry this up
  MRampAttribute colRampAttr( thisMObject(), aCollisionGoalWeightMap);  
  if (doCollisionImpulse){
    colRampAttr.getEntries(indexes, positions,values,interps);
    vlen = values.length();
    if (vlen){
      val = values[0];
      if (val == 1){
        for (unsigned i=1;i<indexes.length();i++) { 
          if (values[i] != val) {
            doColRamp = true;
            break;
          }
        }
      }  else {
        doColRamp = true;
      }
    }
  }
  
  MRampAttribute impRampAttr( thisMObject(), aImpulseGoalWeightMap);
  if (doImpulse){
    impRampAttr.getEntries(indexes, positions,values,interps);
    vlen = values.length();
    if (vlen){
      val = values[0];
      if (val == 1.0){
        for (unsigned i=1;i<indexes.length();i++) { 
          if (values[i] != val) {
            doImpRamp = true;
            break;
          }
        }
      } else {
        doImpRamp = true;
      }
    }
  }
  
  MRampAttribute velRampAttr( thisMObject(), aVelocityGoalWeightMap);
  if (doVelocityGoal){
  //  cerr << "doVelocityGoal is true" << endl;
    velRampAttr.getEntries(indexes, positions,values,interps);
    vlen = values.length();
    if (vlen){
      val = values[0];
      if (val == 1.0){
        for (unsigned i=1;i<indexes.length();i++) { 
          if (values[i] != val) {
            doVelRamp = true;
            break;
          }
        }
      } else {
        doVelRamp = true;
      }
    } else {
//      cerr << "doVelocityGoal is false" << endl;
    }
  }
  //////////////////////////////////////////////////////////////
//  cerr << "doVelRamp is " << doVelRamp << endl;
  
  
  

  // MMatrix mat;
  MMatrix outMat; 
  MMatrix goalMat;
  MMatrix phiMat;

  MVector goalTorque;
//  MVector appliedTorque;

  for (unsigned i=0;i<len;i++) {
    

    
    goalMat = mayaMath::matFromAim(MVector::zero, goalFront[i],goalUp[i], MVector(1.0,1.0,1.0),frontAxis,upAxis); 

    // do everything in the space of the goal
    
    MMatrix phiMat = mayaMath::matFromPhi(phi[i]);
    
    MMatrix localPhiMat = phiMat * goalMat.inverse(); 

    MVector localOmega = (omega[i] * goalMat.inverse());
  
    MVector localPhi = mayaMath::phiFromMat(localPhiMat);     

    //MVector localAppliedTorque = (appliedTorque * goalMat.inverse()); 
    
    double gw = -goalWeightValue *dtRecip2;
    
    if (doGoalPP)   gw *= goalWeightPP[i];
    
    
    if (doVelRamp) {
      float rampVal = 1.0;
      float in =  (velocities[i].length() / velocityGoalRange);
      velRampAttr.getValueAtPosition( in, rampVal, &st ); er;
      gw *= rampVal;
    //  cerr << "gw " << gw << endl; 
    }
    
    
      if (doImpulse) {
        if (doImpRamp) {
          float rampVal = 1.0;
          float in =  (impulse[i].length() / impulseGoalRange);
          impRampAttr.getValueAtPosition( in, rampVal, &st ); er;
          gw *= rampVal;
        } 
      }
      
      if (doCollisionImpulse) {
        if (doColRamp) {
          float rampVal = 1.0;
          float in =  (collisionImpulse[i].length() / collisionGoalRange);
          colRampAttr.getValueAtPosition( in, rampVal, &st ); er;
          gw *= rampVal;
        }
      }


    MVector localGoalTorque = localPhi * gw;    

    MVector localDeltaOmega = ( localGoalTorque  *dt);

    if (doImpulse) {
      if (doLocalImpulse) {
        localDeltaOmega += impulse[i];
      } else {
        localDeltaOmega += impulse[i] * goalMat.inverse();  
      }
    }

    if (doCollisionImpulse) {
      localDeltaOmega += collisionImpulse[i] * goalMat.inverse(); 
    }   
    
    
    
    MVector localOutOmega = (localOmega*torqueConValue) + localDeltaOmega;

    MVector localDeltaPhi = localOutOmega * dt;   

    MVector localOutPhi = localPhi +  localDeltaPhi;

    MMatrix localOutPhiMat = mayaMath::matFromPhi(localOutPhi); 

    MVector outOmegaVal = localOutOmega *  goalMat;

    MMatrix outPhiMat =localOutPhiMat * goalMat;  

    MVector outPhiVal = mayaMath::phiFromMat(outPhiMat);  

    outOmega.set(outOmegaVal,i);    

    outPhi.set(outPhiVal,i);
    MVector oFront = mayaMath::axisVector(frontAxis) * outPhiMat;
    MVector oUp = mayaMath::axisVector(upAxis) * outPhiMat;

    outFront.set(oFront,i);
    outUp.set(oUp,i);   

    MVector vOut =  mayaMath::phiToEuler(outPhiVal,ord,mayaMath::RotateUnit(outUnit));
    outRotation.set(vOut,i);

  }

  return MS::kSuccess;
}
