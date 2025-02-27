/***************************************************************************
pool.cpp  -  description
-------------------
    begin                : Mon Apr 3 2006
    copyright            : (C) 2006 by Julian Mann
    email                : julian.mann@gmail.com

    A node to store an array of points in a kdTree and make the tree data available in the dependency graph
	***************************************************************************/


#include <maya/MFnTypedAttribute.h>
#include <maya/MFnPluginData.h>
#include <errorMacros.h>
#include <poolData.h>
#include <pool.h>
#include "jMayaIds.h"

MTypeId     pool::id( k_pool );

// Pool stuff
//////////////////////////////////////////
MObject     pool::aPosition;
MObject 	pool::aOutput;


pool::pool() {
	m_pd = new poolData;
}

pool::~pool() {
	delete m_pd;
	m_pd = 0;
}

void *pool::creator()
{
	return new pool();
}

MStatus pool::initialize()

{
	MStatus st;

	MFnTypedAttribute tAttr;


	aPosition = tAttr.create("position", "pos", MFnData::kVectorArray); 
	tAttr.setReadable(false);
	tAttr.setCached(false);
	st = addAttribute(aPosition ); mser;

	aOutput = tAttr.create("output", "out", poolData::id);
	tAttr.setReadable(true);
	tAttr.setCached(false);

	addAttribute(aOutput);

	st = attributeAffects(aPosition, aOutput ); mser;


	return MS::kSuccess;
}


MStatus pool::compute( const MPlug &plug, MDataBlock &data )

{
	if ( plug != aOutput) { return ( MS::kUnknownParameter ); }

	MStatus st;
	MDataHandle hp = data.inputValue(aPosition, &st);

	m_pd->create(hp);

	MDataHandle hOutput = data.outputValue(aOutput);
	MFnPluginData fnOut;
	MTypeId kdid(poolData::id);
	MObject dOut = fnOut.create(kdid , &st ); mser;
	poolData *outTree = (poolData *)fnOut.data(&st); mser;

	if (m_pd) {
		*outTree = (*m_pd);
	}

	MDataHandle outputHandle = data.outputValue(pool::aOutput, &st ); mser;
	st = outputHandle.set(outTree); mser;
	data.setClean( plug );

	return MS::kSuccess;
}

