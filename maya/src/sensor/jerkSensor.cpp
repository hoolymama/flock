/*
 * jerkSensor.cpp
 * src
 *
 * Created by Julian Mann on 28/02/2010.
 *
 * Copyright (c) 2010 Julian Mann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the project's author nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */



#include <maya/MFnPluginData.h>
#include <maya/MFnEnumAttribute.h>

#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <errorMacros.h>
#include <attrUtils.h>
#include <poolData.h>
#include <agentSensor.h>
#include <jerkSensor.h>

#include "jMayaIds.h"


//////////////////////////////////////////////////////////////////////////////
// Initialisation and destruction

MObject		jerkSensor::aAcceleration;

MTypeId     jerkSensor::id( k_jerkSensor );

MStatus	jerkSensor::initialize()
{
	MStatus st;
	MString method(" jerkSensor::initialize");

	inheritAttributesFrom("agentSensor");

	MFnTypedAttribute tAttr;


	aAcceleration = tAttr.create("acceleration", "acc", MFnData::kVectorArray, &st ); mser;
	tAttr.setStorable(false);
	tAttr.setConnectable(true);
	st = addAttribute(aAcceleration); mser;
	return MS::kSuccess;
}

void *jerkSensor::creator()
{
	return new jerkSensor;
}

//////////////////////////////////////////////////////////////////////////////
// Implementation

MStatus jerkSensor::assess( MDataBlock &data, const poolData *pd,
                            MVectorArray &assessment)
{

	MStatus st;
	MString method(" jerkSensor::assess");
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
	if ((maxDistPP.length() == len) && data.inputValue(aUseMaxDistancePP).asBool())	 { doMaxDistPP = true; }

	MVectorArray acceleration = MFnVectorArrayData(data.inputValue(
	                              aAcceleration).data()).array();
	if (acceleration.length() != len)	{ return MS::kUnknownParameter; }
	////////////////////////////////////////////////////

	double fov = data.inputValue(aFov).asDouble();
	double drp = data.inputValue(aDropOff).asDouble();
	double acuity = data.inputValue(aAcuity).asDouble();
	double dec = data.inputValue(aDecay).asDouble();
	double radius = data.inputValue(aMaxDistance).asDouble();
	//double blend = data.inputValue(aBlend).asDouble();
	bool   useRamps = data.inputValue(aUseRampAttenuation).asBool();

	// MVectorArray assessment(len);
	int num = data.inputValue(aMaxNeighbors).asShort();

	//	velAlgo algo = (velAlgo)data.inputValue(aMeasureVelocity).asShort();

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
			//const double &  radius = radius;
			MDoubleArray resultDists;
			AGENT_VECTOR *neighbors = new AGENT_VECTOR;
			if (doMaxDistPP) { radius = maxDistPP[i]; }
			pd->closestNPoints( *searchAgent , radius, num, neighbors, resultDists ) ;
			if (!(neighbors->size())) {
				delete neighbors;
				neighbors = 0;
				continue;
			}
			////////////////////////////////

			////////////////////////////////
			MVector signal(MVector::zero);
			const MVector viewVectorN = viewVectors[i].normal();
			////////////////////////////////

			unsigned c;
			unsigned iter = 0;

			for (AGENT_VECTOR::const_iterator currAgent = neighbors->begin();
			     currAgent != neighbors->end(); currAgent++ ) {
				c = (*currAgent)->id();

				// acceleration of neighbor
				///////////////////
				MVector acc = acceleration[i];
				double atten;
				if (useRamps) {
					atten = attenuateSense(
					          points[i], viewVectorN, fovRamp, decRamp,
					          points[c], resultDists[iter], radius
					        );
				}
				else {
					atten = attenuateSense(
					          points[i], viewVectorN, fov, drp, dec,
					          points[c], resultDists[iter], radius
					        );
				}
				// dV = dV.normal() * atten;
				signal += acc * atten;
				///////////////////
				iter++;
			}


			signal *= acuity  ;
			assessment.set(signal, i);
			if (neighbors) {
				delete neighbors;
				neighbors = 0;
			}
		}
		//	sensor::multAssessment(data, assessment);

	}

	// MDataHandle hOut = data.outputValue(sensor::aAssessment);
	// MFnVectorArrayData fnOut;
	// MObject dOut = fnOut.create(assessment);
	// hOut.set(dOut);
	// st = data.setClean( sensor::aAssessment);mser;

	return st;
}
