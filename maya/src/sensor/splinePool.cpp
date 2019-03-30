/***************************************************************************
ptTreeNode.cpp  -  description
-------------------
    begin                : Mon Apr 3 2006
    copyright            : (C) 2006 by Julian Mann
    email                : julian.mann@gmail.com

    A node to store an array of points in a kdTree and make the tree data available in the dependency graph
	***************************************************************************/

#include <maya/MFnTypedAttribute.h>
#include <maya/MFnPluginData.h>

#include <errorMacros.h>
#include <splinePoolData.h>
#include <splinePool.h>


#include "jMayaIds.h"
MTypeId     splinePool::id( k_splinePool );

//  stuff
//////////////////////////////////////////
MObject     splinePool::aInput;
MObject 	splinePool::aOutput;


splinePool::splinePool() {
	m_pd = new splinePoolData;
}

splinePool::~splinePool() {
	delete m_pd;
	m_pd = 0;
}

void *splinePool::creator()
{
	return new splinePool();
}

MStatus splinePool::initialize()

//
{
	MStatus st;

	MFnTypedAttribute tAttr;

	// GEOMETRY
	aInput = tAttr.create("input", "in", MFnData::kDynSweptGeometry);
	tAttr.setReadable(false);
	tAttr.setArray(true);
	tAttr.setIndexMatters(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kDelete);
	tAttr.setCached(false);
	st = addAttribute( aInput ); mser


	aOutput = tAttr.create("output", "out", splinePoolData::id);
	tAttr.setReadable(true);
	tAttr.setStorable(false);
	tAttr.setCached(false);

	addAttribute(aOutput);

	st = attributeAffects(aInput, aOutput ); mser;

	return MS::kSuccess;
}


MStatus splinePool::compute( const MPlug &plug, MDataBlock &data )

{
	if ( plug != aOutput) { return ( MS::kUnknownParameter ); }

	MStatus st;

	MArrayDataHandle hIn = data.inputArrayValue( aInput );

	m_pd->create(hIn);

	////////////////////////////////////////////////////////////////

	MDataHandle hOutput = data.outputValue(aOutput);
	MFnPluginData fnOut;
	MTypeId kdid(splinePoolData::id);

	MObject dOut = fnOut.create(kdid , &st ); mser;
	splinePoolData *outTree = (splinePoolData *)fnOut.data(&st); mser;
	// cerr << "data is copied here in preparation for the output " << endl;
	if (m_pd) {
		*outTree = (*m_pd);
	}
	//// cerr << "just setting the output data now" << endl;
	MDataHandle outputHandle = data.outputValue(splinePool::aOutput, &st ); mser;
	st = outputHandle.set(outTree); mser;
	data.setClean( plug );

	return MS::kSuccess;
}



