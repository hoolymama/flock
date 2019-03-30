/*
 *  ellipsoidSensor.cpp
 *  animal
 *
 *  Created by Julian Mann on 30/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */

#include <maya/MFnPluginData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>

#include <maya/MMatrixArray.h>
#include <errorMacros.h>
#include <attrUtils.h>
#include <poolData.h>
#include <agentSensor.h>
#include <ellipsoidSensor.h>
#include "jMayaIds.h"
//////////////////////////////////////////////////////////////////////////////
// Initialisation and destruction
MObject ellipsoidSensor::aPhi ;
MObject ellipsoidSensor::aScale ;

MTypeId     ellipsoidSensor::id( k_ellipsoidSensor );


MStatus ellipsoidSensor::initialize()
{
	inheritAttributesFrom("agentSensor");

	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;
	MStatus st;

	aPhi = tAttr.create("phi", "phi", MFnData::kVectorArray, &st ); mser;
	tAttr.setStorable(false);
	tAttr.setConnectable(true);
	st = addAttribute(aPhi); mser;

	aScale = tAttr.create("scale", "scl", MFnData::kVectorArray, &st ); mser;
	tAttr.setStorable(false);
	tAttr.setConnectable(true);
	st = addAttribute(aScale); mser;

	return MS::kSuccess;
}

void *ellipsoidSensor::creator()
{
	return new ellipsoidSensor;
}

//////////////////////////////////////////////////////////////////////////////
// Implementation

MStatus ellipsoidSensor::assess(
  MDataBlock &data,
  const poolData *pd,
  MVectorArray &assessment)
{

	MStatus st;
	MString method(" ellipsoidSensor::assess");

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

	MVectorArray phi = MFnVectorArrayData(data.inputValue(aPhi).data()).array();
	MVectorArray scale = MFnVectorArrayData(data.inputValue(aScale).data()).array();
	if (!((phi.length() == len) && (scale.length() == len))) {
		return MS::kSuccess;
	}
	MMatrixArray mats(len);
	for (unsigned i = 0; i < len; i++) {
		mats[i] = mayaMath::matFromPhi(points[i], phi[i], scale[i]);
	}



	if ((maxDistPP.length() == len) && data.inputValue(aUseMaxDistancePP).asBool())	 { doMaxDistPP = true; }
	////////////////////////////////////////////////////
	double fov = data.inputValue(aFov).asDouble();
	double drp = data.inputValue(aDropOff).asDouble();
	double acuity = data.inputValue(aAcuity).asDouble();
	double dec = data.inputValue(aDecay).asDouble();
	double radius = data.inputValue(aMaxDistance).asDouble();
	bool   useRamps = data.inputValue(aUseRampAttenuation).asBool();



	int num = data.inputValue(aMaxNeighbors).asShort();

	if (num && (0.0 != acuity) && (0.0 != radius) && (0.0 < fov ) ) {
		const AGENT_VECTOR *allAgents = pd->tree()->perm();
		unsigned i;

		MRampAttribute fovRamp( thisMObject() , aFovProfile, &st ); mser;
		MRampAttribute decRamp( thisMObject() , aDecayProfile, &st ); mser;

		for (AGENT_VECTOR::const_iterator searchAgent = allAgents->begin();
		     searchAgent != allAgents->end(); searchAgent++ ) {
			i = (*searchAgent)->id();

			// do the search for this point
			////////////////////////////////
			MDoubleArray resultDists;
			AGENT_VECTOR *neighbors = new AGENT_VECTOR;
			if (doMaxDistPP) { radius = maxDistPP[i]; }
			pd->closestNPoints( *searchAgent , radius, num, neighbors, resultDists ) ;
			if (!(neighbors->size())) {
				delete neighbors;
				neighbors = 0;
				continue;
			}

			MVector signal(MVector::zero);
			const MVector viewVectorN = viewVectors[i].normal();

			unsigned c;
			unsigned iter = 0;

			for (AGENT_VECTOR::const_iterator currAgent = neighbors->begin();
			     currAgent != neighbors->end(); currAgent++ ) {
				c = (*currAgent)->id();

				// we want to get the intersection point on the ellipsoid;
				MMatrix &m = mats[c];
				MPoint meInNeighborEllipsoidSpace = MPoint(points[i]) * m.inverse();

				double localDist =  MVector(meInNeighborEllipsoidSpace).length();
				bool inside = localDist < 1.0;

				MPoint localEllipsoidSurfacePoint = MPoint(MVector(meInNeighborEllipsoidSpace).normal());
				MPoint wsIntersection = localEllipsoidSurfacePoint * m;


				// getRelativePosition of neighbor (vector points towards us)
				//////////////////
				MVector dP = points[i] - wsIntersection ;
				MVector dpNormal = inside ? -(dP.normal()) : dP.normal();
				double dist = inside ? 0 : dP.length();

				double atten;

				if (useRamps) {
					atten = attenuateSense(
					          points[i], viewVectorN, fovRamp, decRamp,
					          wsIntersection, dist, radius
					        );
				}
				else {
					atten = attenuateSense(
					          points[i], viewVectorN, fov, drp, dec,
					          wsIntersection, dist, radius
					        );
				}
				dP = dpNormal * atten;

				signal += dP;
				iter++;
			}
			signal *= acuity  ;

			assessment.set(signal, i);
			if (neighbors) {
				delete neighbors;
				neighbors = 0;
			}
		}
	}
	return st;
}
