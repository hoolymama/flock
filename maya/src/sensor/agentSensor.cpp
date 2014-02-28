/*
 *  agentSensor.cpp
 *  animal
 *
 *  Created by Julian Mann on 29/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */

#include <agentSensor.h>



#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>

#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnPluginData.h>

#include <errorMacros.h>
#include <maya/MTime.h>
#include <poolData.h>
#include "jMayaIds.h"

//
MTypeId     agentSensor::id( k_agentSensor );
		

MObject     agentSensor::aPool;				// 
MObject     agentSensor::aMaxNeighbors;

agentSensor::agentSensor() {
}

agentSensor::~agentSensor() {
}


void* agentSensor::creator()
{
	return new agentSensor();
}

MStatus agentSensor::compute( const MPlug& plug, MDataBlock& data )

{
	
	MStatus st;
	MString method("agentSensor::compute");
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
	
	
	
	// always output an assessment array the same length as points input
	unsigned len = MFnVectorArrayData(data.inputValue(sensor::aPositionPP).data()).length();
	MVectorArray assessment(len,MVector::zero);		
	
	
	// get the pool
	////////////////////////////////////////////////////
	poolData * pd = 0;
	MDataHandle h = data.inputValue(agentSensor::aPool, &st);
	if (! st.error()) {
		MObject d = h.data();
		MFnPluginData fn( d , &st); 
		if (! st.error()) {
			pd =  (poolData*)fn.data();
			if ( pd->size() ) st = assess(data, pd, assessment);
		}
	}
	////////////////////////////////////////////////////
	st = sensor::outputAssessment(data,assessment);
	return st;

}

MStatus agentSensor::assess( MDataBlock& data, const poolData * pd , MVectorArray &assessment)
{
	return MS::kSuccess;
}

MStatus agentSensor::initialize()

//		
{
	MStatus st;

	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;
	
	//IN//////////////////////////////////////
	inheritAttributesFrom("sensor");
	
	aPool = tAttr.create("pool", "pl", poolData::id ) ; er;
	tAttr.setStorable(false);
	tAttr.setCached(false);
	st = addAttribute( aPool ); er;
	
	aMaxNeighbors	= nAttr.create("maxNeighbors","mxn", MFnNumericData::kShort, 5, &st ); er;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aMaxNeighbors); er;


	attributeAffects( aPool				, aAssessment	);  
	attributeAffects( aMaxNeighbors 	, aAssessment	);  

	return MS::kSuccess;
}


