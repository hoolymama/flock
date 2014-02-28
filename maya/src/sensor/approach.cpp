/*
 *  approach.cpp
 *  animal
 *
 *  Created by Julian Mann on 30/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */

#include <approach.h>

/*
 *  approach.cpp
 *  animal
 *
 *  Created by Julian Mann on 29/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */
#include <maya/MStatus.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnPluginData.h>
#include <errorMacros.h>
#include <attrUtils.h>
#include <poolData.h>
#include <agentSensor.h>
#include <approach.h>
#include "jMayaIds.h"

//////////////////////////////////////////////////////////////////////////////
// Initialisation and destruction

MObject approach::aMaxSeparation;
MObject approach::aSeparationDecay;
MObject approach::aMaxTime;
MObject approach::aTimeDecay;



MTypeId     approach::id( k_approach );

MStatus
approach::initialize()
{
	MStatus st;
	MString method("approach::initialize");
	
	
	inheritAttributesFrom("agentSensor");
	
	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;
	

	
	aMaxSeparation	= nAttr.create("maxSeparation","mxs", MFnNumericData::kDouble, 1.0, &st); er;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aMaxSeparation); er;
	//--------------
	

	
	aSeparationDecay	= nAttr.create("separationDecay","sdc", MFnNumericData::kDouble, 1.0, &st); er;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aSeparationDecay); er;
	//--------------
	//--------------

	
	aMaxTime	= nAttr.create("maxTime","mxt", MFnNumericData::kDouble, 1.0, &st); er;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aMaxTime); er;
	//--------------

	
	aTimeDecay	= nAttr.create("timeDecay","tdc", MFnNumericData::kDouble, 1.0, &st); er;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aTimeDecay); er;
	//--------------
	

	attributeAffects( 	 aMaxSeparation  	, aAssessment	);  

	attributeAffects( 	 aSeparationDecay  	, aAssessment	);  
 
	attributeAffects( 	 aMaxTime  	, aAssessment	);  

	attributeAffects( 	 aTimeDecay  	, aAssessment	);  
	return MS::kSuccess;
}

void* approach::creator()
{
	return new approach;
}

//////////////////////////////////////////////////////////////////////////////
// Implementation

MStatus approach::assess( MDataBlock& data, const poolData * pd, MVectorArray &assessment)
{
	
	MStatus st;
	MString method(" approach::assess");
	const double epsilon = 1e-6;
	// get the 3 main required vector arrays
	////////////////////////////////////////////////////	
	MVectorArray  points;
	MVectorArray  velocities;
	MVectorArray  viewVectors;
	st = getRequiredSensorData(data, points, velocities, viewVectors); ert;
	
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
	
	double maxSep = data.inputValue(aMaxSeparation).asDouble();
	double sepDec =  data.inputValue(aSeparationDecay).asDouble();
	double maxTim =  data.inputValue(aMaxTime).asDouble();
	double timDec = data.inputValue(aTimeDecay).asDouble();
	
 	bool   useRamps = data.inputValue(aUseRampAttenuation).asBool();

	////////////////////////////////	

	
	
//	MVectorArray assessment(len);
	int num = data.inputValue(aMaxNeighbors).asShort();
	
	if (num && (0.0 != acuity) && (0.0 != radius) && (0.0 < fov ) ) {
		
		
		const AGENT_VECTOR * allAgents = pd->tree()->perm();
		//cerr << "allAgents size " << allAgents->size() << endl;;
		unsigned i;
		MRampAttribute fovRamp( thisMObject() , aFovProfile, &st ); er;
		MRampAttribute decRamp( thisMObject() , aDecayProfile, &st ); er;

		for (AGENT_VECTOR::const_iterator searchAgent = allAgents->begin(); searchAgent!=allAgents->end(); searchAgent++ ) {
			i = (*searchAgent)->id();
			
			if (acuity == 0.0) continue;
			
			// do the search for this point
			////////////////////////////////
			//const double &  radius = radius;
			MDoubleArray resultDists;
			AGENT_VECTOR * neighbors = new AGENT_VECTOR;
			if (doMaxDistPP) radius = maxDistPP[i];
			pd->closestNPoints( *searchAgent , radius, num, neighbors, resultDists ) ;
			if (!(neighbors->size())) {
				delete neighbors;
				neighbors=0; 
				continue;	
			}
			////////////////////////////////
			
			////////////////////////////////
			MVector signal(MVector::zero);

			const MVector viewVectorN = viewVectors[i].normal(); 
			////////////////////////////////
	
			unsigned c;
			unsigned iter = 0;
			for (AGENT_VECTOR::const_iterator currAgent = neighbors->begin(); currAgent!=neighbors->end(); currAgent++ ) {
				c = (*currAgent)->id();
				//cerr << "found nearby agents" << endl;
				// closest approach time calculations
				////////////////////////////////
				MVector dV = velocities[i] - velocities[c];
				MVector dP = points[i] - points[c] ;
				double dV2 = dV*dV;
				double cat = 0.0;  
				if (dV2 > epsilon) cat = -(dP*dV) / dV2;  // cat is closest approach time
				if (cat < 0.0) continue; // closest approach time is in the past
				
				MVector r1 = points[i] + (cat * velocities[i]);
				MVector r2 = points[c] + (cat * velocities[c]);
				
				MVector diff =  (r1 - r2); // the difference between the closest approach points
				double sep = diff.length();
				
				double timeAtten = mayaMath::attenuate(cat, maxTim, timDec);
				double sepAtten = mayaMath::attenuate(sep, maxSep, sepDec);
				////////////////////////////////
				
				
				// standard sensor attenuation factor
				double atten;
				if (useRamps) {
					atten = attenuateSense(
						points[i],viewVectorN,fovRamp,decRamp,
						points[c],resultDists[iter],radius
					);
				} else {
					atten = attenuateSense(
						points[i],viewVectorN,fov,drp,dec,
						points[c],resultDists[iter],radius
					);
				}
				signal += diff.normal() * atten * timeAtten * sepAtten;
				
				iter++;
			}
			signal *= acuity  ;
			
			assessment.set(signal,i);
			if (neighbors) {
				delete neighbors;
				neighbors=0; 
			}	
		}
		
	//	sensor::multAssessment(data, assessment);
			
	}
	
   //MDataHandle hOut = data.outputValue(sensor::aAssessment);
   //MFnVectorArrayData fnOut;
   //MObject dOut = fnOut.create(assessment);
   //hOut.set(dOut);
   //st = data.setClean( sensor::aAssessment);er;
	
	return st;
}
