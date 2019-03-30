/*
 *  hungerState.cpp
 *  jtools
 *
 *  Created by Julian Mann on 18/04/2007.
 *  Copyright 2007 hoolyMama. All rights reserved.
 *
 */



#include <maya/MFnDependencyNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MRampAttribute.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MVector.h>
#include <maya/MFloatVector.h>
#include <maya/MObject.h>
#include <maya/MFloatArray.h>
#include <maya/MIntArray.h>
#include <maya/MPlug.h>
#include <maya/MTime.h>
#include <maya/MPlugArray.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MDoubleArray.h>
#include <maya/MVectorArray.h>

#include <errorMacros.h>
#include <jMayaIds.h>

#include <hungerState.h>



// other atts
MObject hungerState::aFeed;			//PPV food coming in
MObject hungerState::aSpend;			//PPV
MObject hungerState::aDefaultFeed;
MObject hungerState::aDefaultSpend;
MObject hungerState::aFoodLevel;			//PPD
MObject hungerState::aUpdatedFoodLevel;		//PPD
MObject hungerState::aHungry;		//PPI input (bool) open or closed
MObject hungerState::aUpdatedHungry;		//PPI	output energy value
MObject hungerState::aFullThreshold;		// float
MObject hungerState::aHungryFullRamp;
MObject hungerState::aHungryFullCycle;

MObject hungerState::aCurrentTime;


MTypeId hungerState::id( k_hungerState );


void *hungerState::creator()
{

	return new hungerState;
}


MStatus hungerState::initialize()
//
//	Descriptions:
//		Initialize the node, attributes.
//
{

	MStatus st;
	MString method("hungerState::initialize");

	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnUnitAttribute uAttr;


	aDefaultFeed = nAttr.create("defaultFeed", "dfd", MFnNumericData::kDouble, 1.0, &st );
	mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	addAttribute(aDefaultFeed);


	aDefaultSpend = nAttr.create("defaultSpend", "dsp", MFnNumericData::kDouble, 0.1, &st );
	mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	addAttribute(aDefaultSpend);

	aFeed = tAttr.create("feed", "fed", MFnData::kDoubleArray, &st ); mser;
	tAttr.setStorable(false);
	addAttribute(aFeed);

	aSpend = tAttr.create("spend", "spd", MFnData::kDoubleArray, &st ); mser;
	tAttr.setReadable(true);
	tAttr.setStorable(false);
	addAttribute(aSpend);

	aFoodLevel = tAttr.create("foodLevel", "fl", MFnData::kDoubleArray, &st ); mser;
	tAttr.setStorable(false);
	addAttribute(aFoodLevel);

	aUpdatedFoodLevel = tAttr.create("updatedFoodLevel", "ufl", MFnData::kDoubleArray, &st );
	mser;
	tAttr.setReadable(true);
	tAttr.setStorable(false);
	addAttribute(aUpdatedFoodLevel);

	aHungry = tAttr.create("hungry", "hg", MFnData::kDoubleArray, &st ); mser;
	tAttr.setStorable(false);
	addAttribute(aHungry);

	aUpdatedHungry = tAttr.create("updatedHungry", "uhg", MFnData::kDoubleArray, &st ); mser;
	tAttr.setReadable(true);
	tAttr.setStorable(false);
	addAttribute(aUpdatedHungry);

	aHungryFullCycle = tAttr.create("hungryFullCycle", "hfc", MFnData::kDoubleArray, &st );
	mser;
	tAttr.setReadable(true);
	tAttr.setStorable(false);
	addAttribute(aHungryFullCycle);



	aFullThreshold = nAttr.create("fullThreshold", "fth", MFnNumericData::kDouble, 10.0,
	                              &st ); mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	addAttribute(aFullThreshold);

	aHungryFullRamp = MRampAttribute::createCurveRamp("hungryFullRamp", "hfr");
	st = addAttribute( aHungryFullRamp ); mser;

	///


	aCurrentTime = uAttr.create( "currentTime", "ct", MFnUnitAttribute::kTime );
	uAttr.setStorable(true);
	st =  addAttribute(aCurrentTime);  mser;


	st = attributeAffects(aCurrentTime, aUpdatedFoodLevel); mser;
	st = attributeAffects(aCurrentTime, aUpdatedHungry); mser;
	st = attributeAffects(aCurrentTime, aHungryFullCycle); mser;

	// st = attributeAffects(aFeed, aUpdatedFoodLevel);mser;
	// st = attributeAffects(aFeed, aUpdatedHungry);mser;
	// st = attributeAffects(aSpend, aUpdatedFoodLevel);mser;
	// st = attributeAffects(aSpend, aUpdatedHungry);mser;
	// st = attributeAffects(aDefaultFeed, aUpdatedFoodLevel);mser;
	// st = attributeAffects(aDefaultFeed, aUpdatedHungry);mser;
	// st = attributeAffects(aDefaultSpend, aUpdatedFoodLevel);mser;
	// st = attributeAffects(aDefaultSpend, aUpdatedHungry);mser;

	return ( MS::kSuccess );
}


MStatus hungerState::compute(const MPlug &plug, MDataBlock &data)
{
	MStatus st;
	MString method("hungerState::compute");

	if ( ! (
	       (  plug == aUpdatedFoodLevel  ) ||
	       (  plug == aUpdatedHungry  )  ||
	       (  plug == aHungryFullCycle  )
	     ) ) { return ( MS::kUnknownParameter); }

	MTime cT =  data.inputValue( aCurrentTime).asTime();

	MDoubleArray feed = MFnDoubleArrayData(data.inputValue(aFeed).data()).array();
	MDoubleArray spend = MFnDoubleArrayData(data.inputValue(aSpend).data()).array();

	MDoubleArray foodLevel = MFnDoubleArrayData(data.inputValue(aFoodLevel).data()).array();
	MDoubleArray hungry = MFnDoubleArrayData(data.inputValue(aHungry).data()).array();

	unsigned fl = foodLevel.length();
	if (! (hungry.length() == fl)) { return ( MS::kUnknownParameter); }


	double fullThreshold = data.inputValue(aFullThreshold).asDouble();
	if (fullThreshold == 0.0) { fullThreshold = 1.0; }
	const double defaultFeed = data.inputValue(aDefaultFeed).asDouble();
	const double defaultSpend = data.inputValue(aDefaultSpend).asDouble();


	if  ( fl != feed.length()) { feed = MDoubleArray(fl, defaultFeed); }
	if  ( fl != spend.length()) { spend = MDoubleArray(fl, defaultSpend); }

	MFnDependencyNode fnd(thisMObject());
	//	cerr << "NAME -------------------- " << fnd.name() << endl;
	//	cerr << "foodLevel.length();  " << fl << endl;

	MDoubleArray updatedFoodLevel(fl);
	MDoubleArray updatedHungry(fl);
	MDoubleArray hungryFullCycle(fl);

	//////////////////////////////////////////////////////////////
	MIntArray indexes ;
	MFloatArray positions ;
	MFloatArray values;
	MIntArray interps;
	bool doRamp = false;
	unsigned vlen ;


	MRampAttribute rampAttr( thisMObject(), aHungryFullRamp);

	rampAttr.getEntries(indexes, positions, values, interps);
	vlen = values.length();
	if (vlen) {
		if (values[0] == 1.0) {
			for (unsigned i = 1; i < indexes.length(); i++) {
				if (values[i] != values[0]) {
					doRamp = true;
					break;
				}
			}
		}
		else {
			doRamp = true;
		}
	}







	for (unsigned i = 0; i < fl; i++) {
		float rampSample;

		if (hungry[i]) { // 1 = open

			updatedFoodLevel[i] = foodLevel[i]  + feed[i] ; // eat
			//	cerr << " i " << i << " hungry + feed[i]" <<+ feed[i]<<  endl;
			if  (updatedFoodLevel[i]  > fullThreshold ) {
				updatedFoodLevel[i]  = fullThreshold;
				updatedHungry[i] = 0.0;
			}
			else {
				updatedHungry[i] = 1.0;
			}
			rampSample = (updatedFoodLevel[i] / fullThreshold) * 0.5;
		}
		else {   // not hungry, so spend a little
			//		cerr << " i " << i << "not hungry spend[i] " << spend[i] << endl;
			updatedFoodLevel[i] = foodLevel[i]  - spend[i] ; // do some exercise
			if  (updatedFoodLevel[i]  < 0.0 ) {
				updatedFoodLevel[i]  = 0.0;
				updatedHungry[i] = 1.0;
			}
			else {
				updatedHungry[i] = 0.0;
			}

			rampSample = 0.5 + ((1.0 - (updatedFoodLevel[i] / fullThreshold)) * 0.5);
		}
		float rampVal = 0.0;
		if (doRamp) {
			rampAttr.getValueAtPosition( rampSample, rampVal, &st ); mser;
		}
		else {
			rampVal = rampSample;
		}
		hungryFullCycle[i] = rampVal;
	}


	MDataHandle hHungryFullCycle = data.outputValue(hungerState::aHungryFullCycle, &st );
	mser;
	MFnDoubleArrayData fnHungryFullCycle;
	MObject dHungryFullCycle = fnHungryFullCycle.create( hungryFullCycle, &st ); mser;
	hHungryFullCycle.set( dHungryFullCycle );


	MDataHandle hUpdatedFoodLevel = data.outputValue(hungerState::aUpdatedFoodLevel, &st );
	mser;
	MFnDoubleArrayData fnUpdatedFoodLevel;
	MObject dUpdatedFoodLevel = fnUpdatedFoodLevel.create( updatedFoodLevel, &st ); mser;
	hUpdatedFoodLevel.set( dUpdatedFoodLevel );


	MDataHandle hUpdatedHungry = data.outputValue(hungerState::aUpdatedHungry, &st ); mser;
	MFnDoubleArrayData fnUpdatedHungry;
	MObject dUpdatedHungry = fnUpdatedHungry.create( updatedHungry, &st ); mser;
	hUpdatedHungry.set( dUpdatedHungry );

	data.setClean( aUpdatedFoodLevel );
	data.setClean( aUpdatedHungry );

	return ( MS::kSuccess );
}


