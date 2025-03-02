/*
 *  collisionSensor.cpp
 *  animal
 *
 *  Created by Julian Mann on 30/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */

#include <maya/MFnPluginData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>

#include <maya/MFnUnitAttribute.h>
#include <maya/MMatrixArray.h>
#include <errorMacros.h>
#include <attrUtils.h>
#include <poolData.h>
#include <agentSensor.h>
#include <collisionSensor.h>

#include "mayaMath.h"
#include "jMayaIds.h"
//////////////////////////////////////////////////////////////////////////////
// Initialisation and destruction


MObject collisionSensor::aRadius;
MObject collisionSensor::aBounce;
MObject collisionSensor::aFriction;
MObject collisionSensor::aPushOut;
MObject collisionSensor::aPushOutPP;

MObject collisionSensor::aDeltaTime;

MObject collisionSensor::aBias;

MTypeId     collisionSensor::id( k_collisionSensor );


MStatus collisionSensor::initialize()
{
	inheritAttributesFrom("agentSensor");

	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;
	MFnUnitAttribute uAttr;
	MStatus st;

	aRadius = tAttr.create("radius", "rad", MFnData::kDoubleArray ); 
	tAttr.setStorable(false);
	tAttr.setConnectable(true);
	st = addAttribute(aRadius); mser;


	aPushOutPP = tAttr.create("pushOutMultPP", "pumpp", MFnData::kDoubleArray ); 
	tAttr.setStorable(false);
	tAttr.setConnectable(true);
	st = addAttribute(aPushOutPP); mser;


	aBounce = nAttr.create("bounce", "bnc", MFnNumericData::kDouble, 0.0);
	nAttr.setKeyable(true);
	nAttr.setHidden(false);
	st = addAttribute(aBounce); mser;

	aFriction = nAttr.create("friction", "frc", MFnNumericData::kDouble, 0.0);
	nAttr.setKeyable(true);
	nAttr.setHidden(false);
	st = addAttribute(aFriction); mser;

	aPushOut = nAttr.create("pushOut", "psh", MFnNumericData::kDouble, 0.0);
	nAttr.setKeyable(true);
	nAttr.setHidden(false);
	st = addAttribute(aPushOut); mser;

	aBias = nAttr.create("bias", "bias", MFnNumericData::kDouble, 0.0);
	nAttr.setKeyable(true);
	nAttr.setHidden(false);
	nAttr.setDefault(0.5);
	st = addAttribute(aBias); mser;


	aDeltaTime = uAttr.create( "deltaTime", "dt", MFnUnitAttribute::kTime );
	uAttr.setStorable( true );
	st = addAttribute(aDeltaTime); mser;


	return MS::kSuccess;
}

void *collisionSensor::creator()
{
	return new collisionSensor;
}

//////////////////////////////////////////////////////////////////////////////
// Implementation

MStatus collisionSensor::assess(
  MDataBlock &data,
  const poolData *pd,
  MVectorArray &assessment)
{

	MStatus st;
	MString method(" collisionSensor::assess");

	// get the 3 main required vector arrays
	////////////////////////////////////////////////////
	MVectorArray  points;
	MVectorArray  velocities;
	MVectorArray  viewVectors;
	st = getRequiredSensorData(data, points, velocities, viewVectors); msert;

	MDoubleArray maxDistPP = MFnDoubleArrayData(data.inputValue(
	                           sensor::aMaxDistancePP).data()).array();
	bool doMaxDistPP = false;
	unsigned len = points.length();

	MDoubleArray radii = MFnDoubleArrayData(data.inputValue(aRadius).data()).array();
	if (radii.length() != len)  {
		radii = MDoubleArray(len, 1.0);
	}




	if ((maxDistPP.length() == len) && data.inputValue(aUseMaxDistancePP).asBool())	 { doMaxDistPP = true; }
	////////////////////////////////////////////////////
	double fov = data.inputValue(aFov).asDouble();
	double drp = data.inputValue(aDropOff).asDouble();
	double acuity = data.inputValue(aAcuity).asDouble();
	double dec = data.inputValue(aDecay).asDouble();
	double maxDist = data.inputValue(aMaxDistance).asDouble();
	bool   useRamps = data.inputValue(aUseRampAttenuation).asBool();

	double bounce = data.inputValue(aBounce).asDouble();
	double friction = data.inputValue(aFriction).asDouble();
	double push = data.inputValue(aPushOut).asDouble();

	MDoubleArray pupp = MFnDoubleArrayData(data.inputValue(aPushOutPP).data()).array();
	if (pupp.length() != len)  {
		pupp = MDoubleArray(len, push);
	}
	else {
		for (int i = 0; i < len; ++i)
		{
			pupp[i] = pupp[i] * push;
		}
	}



	double bias = data.inputValue(aBias).asDouble();

	double deltaTime =  data.inputValue( aDeltaTime).asTime().as( MTime::kSeconds );

	// cerr << "deltaTime: "<< deltaTime << endl;
	if (deltaTime < 0.0001) { deltaTime = 1.0 / 24; }

	int num = data.inputValue(aMaxNeighbors).asShort();

	if (num && (0.0 != acuity) && (0.0 != maxDist) && (0.0 < fov ) ) {
		const AGENT_VECTOR *allAgents = pd->tree()->perm();
		unsigned i;

		MRampAttribute fovRamp( thisMObject() , aFovProfile, &st ); mser;
		MRampAttribute decRamp( thisMObject() , aDecayProfile, &st ); mser;

		/*
		Strategy is, get the collision response for all collisions and average them.
		This is by no means perfect, but hey...
		*/

		for (AGENT_VECTOR::const_iterator searchAgent = allAgents->begin();
		     searchAgent != allAgents->end(); searchAgent++ ) {
			i = (*searchAgent)->id();

			////////////////////////////////
			MDoubleArray resultDists;
			AGENT_VECTOR *neighbors = new AGENT_VECTOR;
			if (doMaxDistPP) { maxDist = maxDistPP[i]; }
			pd->closestNPoints( *searchAgent , maxDist, num, neighbors, resultDists ) ;
			if (!(neighbors->size())) {
				delete neighbors;
				neighbors = 0;
				continue;
			}

			MVector signal(MVector::zero);
			const MVector viewVectorN = viewVectors[i].normal();

			unsigned c;


			mayaMath::Sphere me;
			me.center = points[i];
			me.radius = radii[i];
			me.change = velocities[i] * deltaTime;

			mayaMath::Sphere other;

			int hitCount = 0;
			unsigned iter = 0;

			for (AGENT_VECTOR::const_iterator currAgent = neighbors->begin();
			     currAgent != neighbors->end(); currAgent++ ) {
				c = (*currAgent)->id();

				other.center = points[c];
				other.radius = radii[c];
				other.change = velocities[c] * deltaTime;

				double t;



				bool hit =  mayaMath::testMovingSphereSphere(me, other, t);

				if (! hit) { continue; }


				hitCount++;
				MVector meChangeToImpact = me.change * t;
				MPoint meHitCenter = me.center + meChangeToImpact;

				MVector otherChangeToImpact = other.change * t;
				MPoint otherHitCenter = other.center + otherChangeToImpact;


				MVector diff =  (meHitCenter - otherHitCenter) ;
				MVector normal =  diff.normal();

				MVector acceleration ;



				MVector relativeVelocity = ( velocities[i]  -  velocities[c]   ) * 0.5;



				if (t < 0) {
					double dist = diff.length();
					MVector escapeVel = normal * (1.0 - (other.radius - dist)) / deltaTime;
					// escapeVel -= velocities[i];
					acceleration = (escapeVel * pupp[i] * 0.5) /  deltaTime;

				}
				else {

					// MVector relativeVelocity = -( velocities[i]  -  velocities[c]   ) *  0.5;


					MVector relativeVelocity = -(   (velocities[i] * bias)  -  (velocities[c] *
					                                (1.0 - bias))  ) ;

					MVector deltaVelocityN = (relativeVelocity * normal) * normal * bounce;

					MVector deltaVelocityT =  (normal ^ relativeVelocity)^normal * friction;

					acceleration = (deltaVelocityN + deltaVelocityT) / deltaTime;

				}

				double atten;


				if (useRamps) {
					atten = attenuateSense(
					          points[i], viewVectorN, fovRamp, decRamp,
					          points[c], resultDists[iter], maxDist
					        );
				}
				else {
					atten = attenuateSense(
					          points[i], viewVectorN, fov, drp, dec,
					          points[c], resultDists[iter], maxDist
					        );
				}
				signal += (acceleration * atten);

				iter++;
			}
			if (hitCount) {
				signal =	signal * (acuity / hitCount)    ;
			}

			assessment.set(signal, i);
			if (neighbors) {
				delete neighbors;
				neighbors = 0;
			}
		}
	}
	return st;
}
