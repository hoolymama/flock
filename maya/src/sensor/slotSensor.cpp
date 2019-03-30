/*
 *  slotSensor.cpp
 *  animal
 *
 *  Created by Julian Mann on 30/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */
#include <set>

#include <maya/MFnPluginData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>

#include <maya/MGlobal.h>

#include <errorMacros.h>
#include <attrUtils.h>
#include <poolData.h>
#include <agentSensor.h>
#include <slotSensor.h>


#include "jMayaIds.h"
//////////////////////////////////////////////////////////////////////////////
// Initialisation and destruction

struct
	PrioritySlot {
	double priority;
	agent *pAgent;
	bool operator< (const PrioritySlot &other) const {return priority > other.priority;}
}  ;





MObject slotSensor::aSlotRadius ;
// MObject slotSensor::aSlotPositionPP;
MObject slotSensor::aSlotImportancePP;
MObject slotSensor::aImportanceMargin ;
MObject slotSensor::aMaxFreeSlots ;

MTypeId     slotSensor::id( k_slotSensor );

MStatus
slotSensor::initialize()
{
	inheritAttributesFrom("agentSensor");

	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;
	MStatus st;

	aSlotRadius	= nAttr.create("slotRadius", "srd", MFnNumericData::kDouble, 1, &st); mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aSlotRadius); mser;

	aSlotImportancePP = tAttr.create("slotImportancePP", "sipp", MFnData::kDoubleArray, &st );
	mser;
	tAttr.setStorable(false);
	tAttr.setConnectable(true);
	st = addAttribute(aSlotImportancePP); mser;

	// aSlotPositionPP = tAttr.create("slotPositionPP", "sppp", MFnData::kVectorArray, &st ); mser;
	// tAttr.setStorable(false);
	// tAttr.setConnectable(true);
	// st = addAttribute(aSlotPositionPP); mser;

	aImportanceMargin	= nAttr.create("importanceMargin", "imm", MFnNumericData::kDouble, 0.1,
	                                 &st); mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aImportanceMargin); mser;

	aMaxFreeSlots	= nAttr.create("maxFreeSlots", "mfs", MFnNumericData::kInt, 10, &st); mser;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aMaxFreeSlots); mser;

	//--------------

	return MS::kSuccess;
}

void *slotSensor::creator()
{
	return new slotSensor;
}

//////////////////////////////////////////////////////////////////////////////
// Implementation

MStatus slotSensor::assess(MDataBlock &data, const poolData *pd, MVectorArray &assessment)
{

	MStatus st;
	MString method(" slotSensor::assess");

	// get the 3 main required vector arrays
	////////////////////////////////////////////////////
	MVectorArray  points;
	MVectorArray  velocities;
	MVectorArray  viewVectors;
	st = getRequiredSensorData(data, points, velocities, viewVectors); msert;
	unsigned len = points.length();
	if (!len) {
		return MS::kSuccess;
	}

	// cerr << "points" << points << endl;
	// cerr << "velocities" << velocities << endl;

	MDoubleArray maxDistPP = MFnDoubleArrayData(data.inputValue(
	                           sensor::aMaxDistancePP).data()).array();
	const MDoubleArray &slotImportancePP = MFnDoubleArrayData(data.inputValue(
	    aSlotImportancePP).data()).array();
	// const MVectorArray & slotPositionPP = MFnVectorArrayData(data.inputValue(aSlotPositionPP).data()).array();
	double importanceMargin = data.inputValue(aImportanceMargin).asDouble();
	int maxFreeSlots = data.inputValue(aMaxFreeSlots).asInt();

	double slotRadius  = data.inputValue(aSlotRadius).asDouble();

	bool doMaxDistPP = false;
	if ((maxDistPP.length() == len) &&
	    data.inputValue(aUseMaxDistancePP).asBool()
	   )
	{
		doMaxDistPP = true;
	}


	////////////////////////////////////////////////////
	double fov = data.inputValue(aFov).asDouble();
	double drp = data.inputValue(aDropOff).asDouble();
	double acuity = data.inputValue(aAcuity).asDouble();
	double dec = data.inputValue(aDecay).asDouble();
	double radius = data.inputValue(aMaxDistance).asDouble();


	////////////////////////////////
	bool useRamps = data.inputValue(aUseRampAttenuation).asBool();
	int num = data.inputValue(aMaxNeighbors).asShort();





	bool doIt =  (num && (0.0 != acuity) && (0.0 != radius) && (0.0 != slotRadius)
	              && (0.0 < fov ));
	if (! doIt) { return MS::kSuccess; }

	const AGENT_VECTOR *allSlots = pd->tree()->perm();
	if (allSlots->size() == 0) {
		MGlobal::displayWarning("slotSensor no slots");
		return MS::kSuccess;
	}

	if (slotImportancePP.length() != allSlots->size()) {
		MGlobal::displayWarning("slotSensor slotImportancePP is the wrong size");
		return MS::kSuccess;
	}
	// if (slotPositionPP.length() != allSlots->size()) {
	// 	MGlobal::displayWarning("slotSensor slotPositionPP is the wrong size");
	// 	return MS::kSuccess;
	// }



	MRampAttribute fovRamp( thisMObject() , aFovProfile, &st ); mser;
	MRampAttribute decRamp( thisMObject() , aDecayProfile, &st ); mser;


	unsigned i;

	/* find subset of slots that are visible */

	/* create set with all slots in it*/
	std::set<PrioritySlot> availableSlots;

	AGENT_VECTOR::const_iterator slotIter = allSlots->begin();
	// cerr << "--------" << endl;
	while ( slotIter != allSlots->end() )
	{
		int id = (*slotIter)->id();
		PrioritySlot prioritySlot;
		prioritySlot.pAgent = *slotIter;
		prioritySlot.priority = slotImportancePP[id];
		availableSlots.insert(prioritySlot);
		// cerr << "ID: " << id <<  endl;
		slotIter++;
	}

	/* remove from available slots those slots that contain one or more points*/
	/* also record the slot id for contained points */
	// MIntArray desiredSlotIds(len, -1);
	std::vector<PrioritySlot>  desiredSlots(len);


	for (i = 0; i < len; i++) {
		MDoubleArray resultDists;
		AGENT_VECTOR *slots = new AGENT_VECTOR;
		pd->closestNPoints( points[i] , slotRadius, 1, slots, resultDists ) ;

		if (( slots->size() > 0)) {
			/*first and only slot*/
			int id = slots->front()->id();

			PrioritySlot unavailableSlot;
			unavailableSlot.pAgent = slots->front();
			unavailableSlot.priority = slotImportancePP[id];
			desiredSlots[i] = unavailableSlot;
			availableSlots.erase(unavailableSlot);

			// desiredSlotIds[i] = id;
			delete slots; slots = 0;
		}
		else {
			desiredSlots[i] = (PrioritySlot) { .pAgent = 0, .priority = 0};
			/* I'm not in a slot */
			delete slots; slots = 0;
			continue;
		}
	}



	/* now we know which slots are available,
	and points that are in a slot know the id of that slot
	cerr << "desiredSlotIds: " << desiredSlotIds << endl;
	cerr << "PrioritySlots" << endl;

	std::set<PrioritySlot>::const_iterator it;
	for (it=availableSlots.begin(); it!=availableSlots.end(); ++it)
	{
		cerr << "ID: " << it->pAgent->id() << " - " << it->priority << endl;
	}
	*/

	/* from the available slots,
		get the slots that are within importance margin of the top slot.
		We get an iterator to the first slot that is too low (or the end)
	*/
	std::set<PrioritySlot>::const_iterator pruneIter = availableSlots.begin();
	int count = 0;
	double topPriority;
	double bottomPriority = 0;
	while (  pruneIter != availableSlots.end()  && (count != maxFreeSlots))
	{
		bottomPriority = pruneIter->priority;
		// if ( pruneIter == availableSlots.begin()) {
		// 	topPriority = pruneIter->priority;
		// 	bottomPriority = topPriority - importanceMargin;
		// }

		// if (pruneIter->priority < bottomPriority ) {
		// 	break;
		// }
		pruneIter++;
		count++;
	}




	/**/
	for (i = 0; i < len; i++) {
		// cerr << "I: " <<  i << endl;
		// const int & slotId = desiredSlotIds[i];
		PrioritySlot slot = desiredSlots[i];
		MVector resultGoal(points[i]);
		double resultDist = 0.0;

		// if ( (slotId > -1)  && (slotImportancePP[slotId] > bottomPriority)  ){

		if (slot.pAgent  && (slot.priority > bottomPriority)  ) {
			/* 	go to the slot I'm on	*/
			resultGoal = slot.pAgent->position();
			resultDist = (resultGoal - points[i]).length();
			// if (i == 5) {
			// 	cerr << " p 5 IN SLOT - slotId: " <<  slot.pAgent->id() << endl;
			// 	cerr << "resultGoal: " << resultGoal << endl;
			// }

		}
		else {
			/* go to the closest from availableSlots */
			int chosenIndex = -1;
			std::set<PrioritySlot>::const_iterator iter = availableSlots.begin();
			while (  iter != pruneIter)
			{
				if (iter == availableSlots.begin()) {
					resultGoal = iter->pAgent->position();
					resultDist = (resultGoal - points[i]).length();
					chosenIndex = iter->pAgent->id();
				}
				else {
					double dist = ( iter->pAgent->position() - points[i]).length();
					if (dist < resultDist) {
						resultDist = dist;
						resultGoal = iter->pAgent->position() ;
						chosenIndex = iter->pAgent->id();
					}
				}
				iter++;
			}

			// if (i == 5) {
			// 	cerr << "USE AVAILABLE - slotId: " <<  chosenIndex << endl;
			// 	cerr << "resultGoal: " << resultGoal << endl;

			// }


		}

		const MVector viewVectorN = viewVectors[i].normal();

		double atten;
		if (useRamps) {
			atten = attenuateSense(
			          points[i], viewVectorN, fovRamp, decRamp,
			          resultGoal, resultDist, radius
			        );
		}
		else {
			atten = attenuateSense(
			          points[i], viewVectorN, fov, drp, dec,
			          resultGoal, resultDist, radius
			        );
		}

		MVector signal   = ((resultGoal - points[i]).normal()) * atten *  acuity ;
		assessment.set(signal, i);
	}
	// delete desiredSlots;
	// desiredSlots = 0;
	return st;
}
