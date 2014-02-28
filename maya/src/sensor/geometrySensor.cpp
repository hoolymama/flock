/*
*  geometrySensor.cpp
	*  animal
	*
	*  Created by Julian Mann on 31/07/2006.
	*  Copyright 2006 hooly|mama. All rights reserved.
	*
*/



#include <sensor.h>
#include <geometrySensor.h>

#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnCompoundAttribute.h>

#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>

#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnPluginData.h>
#include <maya/MBoundingBox.h>


#include <maya/MQuaternion.h>
#include <maya/MFnMesh.h>
#include <maya/MFloatPoint.h>
#include <maya/MDagPath.h>
#include <maya/MMatrix.h>

#include <errorMacros.h>
#include <attrUtils.h>
#include <mayaMath.h>


#include "jMayaIds.h"

//
	MTypeId     geometrySensor::id( k_geometrySensor );

MObject     geometrySensor::aNumSamples ;
MObject     geometrySensor::aSeed ;
MObject     geometrySensor::aSurfaceTangent ;
MObject     geometrySensor::aSurfaceNormal ;
MObject     geometrySensor::aViewDirection  ;
MObject     geometrySensor::aInMesh ;

MObject geometrySensor::aBBMinX;
MObject geometrySensor::aBBMinY;
MObject geometrySensor::aBBMinZ;
MObject geometrySensor::aBBMin;
MObject geometrySensor::aBBMaxX;
MObject geometrySensor::aBBMaxY;
MObject geometrySensor::aBBMaxZ;
MObject geometrySensor::aBBMax;
MObject geometrySensor::aBBSizX;
MObject geometrySensor::aBBSizY;
MObject geometrySensor::aBBSizZ;
MObject geometrySensor::aBBSiz;
MObject geometrySensor::aBB;
MObject geometrySensor::aUseBB;



geometrySensor::geometrySensor() {
}

geometrySensor::~geometrySensor() {
}


void* geometrySensor::creator()
{
	return new geometrySensor();
}

MStatus geometrySensor::compute( const MPlug& plug, MDataBlock& data )
{
	MStatus st;
	MString method("geometrySensor::compute");
	if(plug != aAssessment)	return MS::kUnknownParameter;
	


	short int nodeState = data.inputValue( state).asShort();
	if (nodeState)  {
		// just clear any data on the output
		MVectorArray empty;
		MDataHandle hOut = data.outputValue(sensor::aAssessment);
		MFnVectorArrayData fnOut;
		MObject dOut = fnOut.create(empty);
		hOut.set(dOut);
		st = data.setClean( sensor::aAssessment);er;
		return( MS::kSuccess );
	}


	unsigned len = MFnVectorArrayData(data.inputValue(sensor::aPositionPP).data()).length();
	MVectorArray assessment(len,MVector::zero);
	st = assess(data, assessment);

	// always output an assessment array the same length as points input
	st = outputAssessment(data,assessment);
	return st;	

}


void geometrySensor::sphericalSpread(
	const MVector & dir,
	double spread,
	unsigned count,     
	MFloatVectorArray & rays,
	MFloatArray & strengths,
	long seedVal
) const {
	if (seedVal >=0) srand48(seedVal);
	const double  PI  = 3.1415927;
	const double  _2PI = 2.0 * PI;

	MQuaternion q(MVector::zAxis, dir);
	double vl = dir.length();
	double min = cos(spread*PI);
	double t, w, z,r;
	double range = 1.0 - min;
	MVector v;
	for (;count;count--) {
		r = drand48();
		z = (r * range) + min;

		t = drand48() * _2PI;
		w = (sqrt( 1.0 - z*z )) * vl;
		v.z = z * vl;
		v.x = w * cos(t);
		v.y = w * sin(t);
		rays.append(MFloatVector(v.rotateBy(q)));
		strengths.append(r);
	}
}


MStatus geometrySensor::assess( MDataBlock& data, MVectorArray &assessment )
{
	MStatus st;
	MString method(" geometrySensor::assess");
	const double epsilon = 1e-6;
	// get the 3 main required vector arrays
	////////////////////////////////////////////////////	
	MVectorArray  points;
	MVectorArray  velocities;
	MVectorArray  viewVectors;
	st = getRequiredSensorData(data, points, velocities, viewVectors); 
	if (st.error()) return MS::kUnknownParameter;
	
	MDoubleArray maxDistPP = MFnDoubleArrayData(data.inputValue(sensor::aMaxDistancePP).data()).array();
	bool doMaxDistPP = false;	
	unsigned len = points.length();
	if ((maxDistPP.length() == len) && data.inputValue(aUseMaxDistancePP).asBool())	 doMaxDistPP = true;
	////////////////////////////////////////////////////

	// get double arrays from the other sensor attributes
	////////////////////////////////////////////////////	
	double fov = data.inputValue(aFov).asDouble();
	double drp = data.inputValue(aDropOff).asDouble();
	double acuity = data.inputValue(aAcuity).asDouble();
	double dec = data.inputValue(aDecay).asDouble();
	double radius = data.inputValue(aMaxDistance).asDouble();

	short samples = data.inputValue(aNumSamples).asShort();
	int seed = data.inputValue(aSeed).asLong();
	double stan = data.inputValue(aSurfaceTangent).asDouble();
	double snor = data.inputValue(aSurfaceNormal).asDouble();
	double vdir = data.inputValue(aViewDirection).asDouble();
	////////////////////////////////////////////////////


	MObject inMesh =  data.inputValue(aInMesh).asMeshTransformed();
	MFnMesh mFnIn(inMesh,&st );er;
	MMeshIsectAccelParams ap = mFnIn.autoUniformGridParams();

	double bbminx = data.inputValue(aBBMinX ).asDouble();
	double bbminy = data.inputValue(aBBMinY ).asDouble();
	double bbminz = data.inputValue(aBBMinZ ).asDouble();
	double bbmaxx = data.inputValue(aBBMaxX ).asDouble();
	double bbmaxy = data.inputValue(aBBMaxY ).asDouble();
	double bbmaxz = data.inputValue(aBBMaxZ ).asDouble();
	
	MPoint p1(bbminx, bbminy, bbminz);
	MPoint p2(bbmaxx, bbmaxy,bbmaxz);
	
	MBoundingBox gBox = MBoundingBox(p1,p2);
	
	bool useBB = data.inputValue(aUseBB).asBool();	

// 	MVectorArray assessment(len);



	// double tanMag, norMag, orbMag;
	if (acuity != 0.0) {

		unsigned i;
		for (i=0; i<len; i++ ) {
			
		  if (doMaxDistPP) radius = maxDistPP[i];
			
		   MPoint ac1(points[i] - MVector(radius,radius,radius));
		   MPoint ac2(points[i] + MVector(radius,radius,radius));
		   MBoundingBox aBox(ac1, ac2);
		   if (useBB && (!(mayaMath::boxBoxIntersection( gBox, aBox) ))) 	continue;

			////////////////////////////////
			MVector signal(MVector::zero);
			const MVector viewVectorN = viewVectors[i].normal(); 
			////////////////////////////////

			MFloatVectorArray rays;
			MFloatArray strengths;

			sphericalSpread( viewVectorN,fov,samples, rays,strengths,seed);

			MFloatPoint  fpSource ;
			fpSource.setCast (points[i])	;

			for (int j = 0;j<samples;j++) {
				MFloatVector & fvRayDir = rays[j];
				MFloatPoint hitPoint;
				int hitFace;
				float hitParam;

				bool hit = mFnIn.closestIntersection(
					fpSource, fvRayDir, NULL, NULL, false,
					MSpace::kWorld, radius,false, &ap, 
					hitPoint, &hitParam, &hitFace, NULL, NULL, NULL
				);
				
				if (hit) {
					MVector hitVector;
					if ((snor !=0 ) || (stan !=0)) {
						MVector faceNormal;
						mFnIn.getPolygonNormal( hitFace ,faceNormal,MSpace::kWorld	 );
						if (snor!=0.0) {
							hitVector += faceNormal*snor;
						}
						if (stan!=0.0) {
							MVector binormal = viewVectorN^faceNormal;
							MVector tangent = faceNormal^binormal.normal();
							hitVector += tangent*stan;
						}

					}
					if (vdir!=0.0) {
						hitVector += fvRayDir*vdir;
					}


					double dropoffFactor = strengths[j];

					if (drp==0.0) {
						dropoffFactor=1.0;
					} else if (drp==1.0) {
						; // dropoffFactor = dropoffFactor
					} else if (drp==2.0) {
						dropoffFactor=dropoffFactor * dropoffFactor;
					} else {
						dropoffFactor = pow(dropoffFactor, drp); 	
					}

					double decayFactor = 1.0 - (hitParam / radius);

					if (dec==0.0) {
						decayFactor=1.0;
					} else if (dec==1.0) {
						; // decayFactor = decayFactor
					} else if (dec==2.0) {
						decayFactor=decayFactor * decayFactor;
					} else {
						decayFactor = pow(decayFactor, dec); 	
					}

					hitVector *= (decayFactor * dropoffFactor);
					signal += hitVector;

				}

			}
			signal = signal *acuity ;
			assessment.set(signal,i);
		}

	}

	return st;

}



MStatus geometrySensor::initialize()
{
	MStatus st;

	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;
	MFnCompoundAttribute cAttr;

	inheritAttributesFrom("sensor");


	aInMesh= tAttr.create("inMesh","im", MFnData::kMesh);
	tAttr.setReadable(false);
	st = addAttribute( aInMesh );er;

	aSurfaceTangent	= nAttr.create("surfaceTangent","stan", MFnNumericData::kDouble, 1.0, &st); er;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aSurfaceTangent); er;

	aSurfaceNormal	= nAttr.create("surfaceNormal","snor", MFnNumericData::kDouble, 1.0, &st); er;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aSurfaceNormal); er;

	aViewDirection	= nAttr.create("viewDirection","vdir", MFnNumericData::kDouble, 1.0, &st); er;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aViewDirection); er;

	aNumSamples	= nAttr.create("numSamples","nsm", MFnNumericData::kShort, 1.0, &st); er;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aNumSamples); er;

	aSeed	= nAttr.create("seed","sd", MFnNumericData::kLong, 1.0, &st); er;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aSeed); er;	


	aBBMinX = nAttr.create( "boxMinX", "bnx", MFnNumericData::kDouble);
	aBBMinY = nAttr.create( "boxMinY", "bny", MFnNumericData::kDouble);
	aBBMinZ = nAttr.create( "boxMinZ", "bnz", MFnNumericData::kDouble);
	aBBMin = nAttr.create( "boxMin", "bn", aBBMinX, aBBMinY, aBBMinZ );
	nAttr.setHidden(false);
	st = addAttribute(aBBMinX);
	st = addAttribute(aBBMinY);
	st = addAttribute(aBBMinZ);
	st = addAttribute(aBBMin);
	aBBMaxX = nAttr.create( "boxMaxX", "bxx", MFnNumericData::kDouble);
	aBBMaxY = nAttr.create( "boxMaxY", "bxy", MFnNumericData::kDouble);
	aBBMaxZ = nAttr.create( "boxMaxZ", "bxz", MFnNumericData::kDouble);
	aBBMax = nAttr.create( "boxMax", "bx", aBBMaxX,  aBBMaxY, aBBMaxZ );
	nAttr.setHidden(false);
	st = addAttribute(aBBMaxX);
	st = addAttribute(aBBMaxY);
	st = addAttribute(aBBMaxZ);
	st = addAttribute(aBBMax);
	aBBSizX = nAttr.create( "boxSizX", "bsx", MFnNumericData::kDouble);
	aBBSizY = nAttr.create( "boxSizY", "bsy", MFnNumericData::kDouble);
	aBBSizZ = nAttr.create( "boxSizZ", "bsz", MFnNumericData::kDouble);
	aBBSiz = nAttr.create( "boxSiz", "bs", aBBSizX,  aBBSizY, aBBSizZ );
	nAttr.setHidden(false);
	st = addAttribute(aBBSizX);
	st = addAttribute(aBBSizY);
	st = addAttribute(aBBSizZ);
	st = addAttribute(aBBSiz);
	aBB = cAttr.create("inBoundingBox", "ibb");
	cAttr.addChild(aBBMin);
	cAttr.addChild(aBBMax);
	cAttr.addChild(aBBSiz);
	cAttr.setStorable(true);
	
	cAttr.setHidden(false);
	
	addAttribute(aBB);
	
	aUseBB =  nAttr.create( "useBoundingBox", "ubb", MFnNumericData::kBoolean); 
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setDefault(false);
	st =	addAttribute(aUseBB);	er;



	attributeAffects( aInMesh , aAssessment	);  
	attributeAffects( aSurfaceTangent , aAssessment	);  
	attributeAffects( aSurfaceNormal , aAssessment	);  
	attributeAffects( aViewDirection , aAssessment	);  
	attributeAffects( aNumSamples , aAssessment	);  
	attributeAffects( aSeed , aAssessment	);  

	return MS::kSuccess;
}


