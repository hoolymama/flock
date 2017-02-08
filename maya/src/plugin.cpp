#include <maya/MStatus.h>
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>

#include "errorMacros.h"

#include "flockShape.h"
#include "fishDeform.h"

#include "prioritize.h"

#include "steer_remap.h"


// #include "legacySteerRemap.h"
#include "rollerPP.h"
#include "tumbler.h"
// #include "legacyTumbler.h"
#include "articulate.h"

#include <poolData.h>
#include <pool.h>
#include <splinePoolData.h>
#include <splinePool.h>
#include <sensor.h>
#include <agentSensor.h>
#include <alignment.h>
#include <separation.h>
#include <approach.h>
#include <cohesion.h>
#include <exposure.h>
#include <splineSensor.h>
#include <geometrySensor.h>
#include <axisSensor.h>
#include <jerkSensor.h>

#include "multPP.h"
#include "addPP.h"
#include "aimPP.h"
#include "arrayToMulti.h"
#include "lookupPP.h"
#include "rampLookupPP.h"
#include "vectorRampLookupPP.h"
#include "extractElements.h"
#include "vectorComponentPP.h"
#include "binormalPP.h"
#include "reversePP.h"
#include "magNormPP.h"
#include "orthogonalPP.h"
#include "blendVectorPP.h"
#include "blendDoublePP.h"
#include "crossPP.h"
#include "dotPP.h"
#include "signPP.h"
#include "coordSysComponentPP.h"
#include "multVectorDoublePP.h" 
#include "multVectorVectorPP.h" 
#include "priorityClipPP.h" 
#include "multDoubleDoublePP.h" 
#include "transitionPP.h" 
#include "containmentPP.h" 
#include "uvPP.h" 
#include "accumulatorPP.h" 
#include "sinePP.h" 
#include "meshProximityPP.h" 
#include "subVectorVectorPP.h" 
#include "speedRampLookup.h"

#include "hexapod.h"
// #include "hexapodLocator.h"

#include "hungerState.h"


MStatus initializePlugin( MObject obj)
{

	MStatus st;
	
	MString method("initializePlugin");

	MFnPlugin plugin( obj, PLUGIN_VENDOR, PLUGIN_VERSION , MAYA_VERSION);
	st = plugin.registerNode( "flockShape", flockShape::id, flockShape::creator, flockShape::initialize, MPxNode::kLocatorNode);er;

	st = plugin.registerNode( "prioritize", prioritize::id, prioritize::creator, prioritize::initialize, MPxNode::kLocatorNode);er;
	st = plugin.registerNode( "steerRemap", steerRemap::id, steerRemap::creator, steerRemap::initialize );er;
	st = plugin.registerNode( "roller", rollerPP::id, rollerPP::creator, rollerPP::initialize); ert;
	st = plugin.registerNode( "tumbler", tumbler::id, tumbler::creator, tumbler::initialize );er;

  st = plugin.registerNode( "fishDeform", fishDeform::id, fishDeform::creator, fishDeform::initialize); er;
 
	// st = plugin.registerNode( "legacySteerRemap", legacySteerRemap::id, legacySteerRemap::creator, legacySteerRemap::initialize );er;

	// st = plugin.registerNode( "legacyTumbler", legacyTumbler::id, legacyTumbler::creator, legacyTumbler::initialize );er;
	st = plugin.registerNode( "articulate", articulate::id, articulate::creator, articulate::initialize);er;

	st = plugin.registerData( "poolData", poolData::id, poolData::creator );er;
	st = plugin.registerNode( "pool", pool::id, pool::creator, pool::initialize );er;
	st = plugin.registerNode( "sensor", sensor::id, sensor::creator, sensor::initialize );er;
	st = plugin.registerNode( "agentSensor", agentSensor::id, agentSensor::creator, agentSensor::initialize );er;
	st = plugin.registerNode( "alignment", alignment::id, alignment::creator, alignment::initialize );er;
	st = plugin.registerNode( "separation", separation::id, separation::creator, separation::initialize );er;
	st = plugin.registerNode( "approach", approach::id, approach::creator, approach::initialize );er;
	st = plugin.registerNode( "cohesion", cohesion::id, cohesion::creator, cohesion::initialize );er;
	st = plugin.registerNode( "exposure", exposure::id, exposure::creator, exposure::initialize );er;
	st = plugin.registerData( "splinePoolData", splinePoolData::id, splinePoolData::creator );er;
	st = plugin.registerNode( "splinePool", splinePool::id, splinePool::creator, splinePool::initialize );er;
	st = plugin.registerNode( "splineSensor", splineSensor::id, splineSensor::creator, splineSensor::initialize );er;
	st = plugin.registerNode( "geometrySensor", geometrySensor::id, geometrySensor::creator, geometrySensor::initialize );er;
	st = plugin.registerNode( "axisSensor", axisSensor::id, axisSensor::creator, axisSensor::initialize );er;
	st = plugin.registerNode( "jerkSensor", jerkSensor::id, jerkSensor::creator, jerkSensor::initialize );er;
	st = plugin.registerNode( "multPP", multPP::id, multPP::creator, multPP::initialize); er;
	st = plugin.registerNode( "aimPP", aimPP::id, aimPP::creator, aimPP::initialize); er;
	st = plugin.registerNode( "arrayMulti", arrayToMulti::id, arrayToMulti::creator, arrayToMulti::initialize); er;
	st = plugin.registerNode( "lookupPP", lookupPP::id, lookupPP::creator, lookupPP::initialize); er;
	st = plugin.registerNode( "rampLookupPP", rampLookupPP::id, rampLookupPP::creator, rampLookupPP::initialize); er;
	st = plugin.registerNode( "extractElements", extractElements::id, extractElements::creator, extractElements::initialize); er;
	st = plugin.registerNode( "addPP", addPP::id, addPP::creator, addPP::initialize); er;
	st = plugin.registerNode( "vectorComponentPP", vectorComponentPP::id, vectorComponentPP::creator, vectorComponentPP::initialize); er;
	st = plugin.registerNode( "binormalPP", binormalPP::id, binormalPP::creator, binormalPP::initialize); er;
	st = plugin.registerNode( "reversePP", reversePP::id, reversePP::creator, reversePP::initialize); er;
	st = plugin.registerNode( "orthogonalPP", orthogonalPP::id, orthogonalPP::creator, orthogonalPP::initialize); er;
	st = plugin.registerNode( "magNormPP", magNormPP::id, magNormPP::creator, magNormPP::initialize); er;
	st = plugin.registerNode( "blendVectorPP", blendVectorPP::id, blendVectorPP::creator, blendVectorPP::initialize); er;
	st = plugin.registerNode( "blendDoublePP", blendDoublePP::id, blendDoublePP::creator, blendDoublePP::initialize); er;
	st = plugin.registerNode( "crossPP", crossPP::id, crossPP::creator, crossPP::initialize); er;
	st = plugin.registerNode( "dotPP", dotPP::id, dotPP::creator, dotPP::initialize); er;
	st = plugin.registerNode( "signPP", signPP::id, signPP::creator, signPP::initialize); er;
	st = plugin.registerNode( "vectorRampLookupPP", vectorRampLookupPP::id, vectorRampLookupPP::creator, vectorRampLookupPP::initialize); er;
	st = plugin.registerNode( "coordSysComponentPP", coordSysComponentPP::id, coordSysComponentPP::creator, coordSysComponentPP::initialize); er;
	st = plugin.registerNode( "multVectorDoublePP", multVectorDoublePP::id, multVectorDoublePP::creator, multVectorDoublePP::initialize); er;
	st = plugin.registerNode( "multVectorVectorPP", multVectorVectorPP::id, multVectorVectorPP::creator, multVectorVectorPP::initialize); er;
	st = plugin.registerNode( "priorityClipPP", priorityClipPP::id, priorityClipPP::creator, priorityClipPP::initialize); er;
	st = plugin.registerNode( "multDoubleDoublePP", multDoubleDoublePP::id, multDoubleDoublePP::creator, multDoubleDoublePP::initialize); er;
	st = plugin.registerNode( "transitionPP", transitionPP::id, transitionPP::creator, transitionPP::initialize); er;
	st = plugin.registerNode( "containmentPP", containmentPP::id, containmentPP::creator, containmentPP::initialize); er;
	st = plugin.registerNode( "uvPP", uvPP::id, uvPP::creator, uvPP::initialize); er;
	st = plugin.registerNode( "accumulatorPP", accumulatorPP::id, accumulatorPP::creator, accumulatorPP::initialize); er;
	st = plugin.registerNode( "sinePP", sinePP::id, sinePP::creator, sinePP::initialize); er;
	st = plugin.registerNode( "meshProximityPP", meshProximityPP::id, meshProximityPP::creator, meshProximityPP::initialize); er;
	st = plugin.registerNode( "subVectorVectorPP", subVectorVectorPP::id, subVectorVectorPP::creator, subVectorVectorPP::initialize); er;
	st = plugin.registerNode( "speedRampLookupPP", speedRampLookupPP::id, speedRampLookupPP::creator, speedRampLookupPP::initialize); er;
 
	st = plugin.registerNode( "hexapod", hexapod::id, hexapod::creator, hexapod::initialize, MPxNode::kLocatorNode); er;
 	// st = plugin.registerNode( "hexapodLocatorShape", hexapodLocator::id, hexapodLocator::creator,hexapodLocator::initialize, MPxNode::kLocatorNode  );er;
 
	st = plugin.registerNode( "hungerState", hungerState::id, hungerState::creator, hungerState::initialize); er;
 
	MGlobal::executePythonCommand("import flock;flock.load()");
	return st;

}

MStatus uninitializePlugin( MObject obj)
{
	MStatus st;

	MString method("uninitializePlugin");

	MGlobal::executePythonCommand("import flock;flock.unload()");
 
	MFnPlugin plugin( obj );
 
	st = plugin.deregisterNode( hungerState::id );er;

	// st = plugin.deregisterNode( hexapodLocator::id );er;	
	st = plugin.deregisterNode( hexapod::id );er;
	st = plugin.deregisterNode( speedRampLookupPP::id );er;
 
 
	st = plugin.deregisterNode( subVectorVectorPP::id );er;
	st = plugin.deregisterNode( meshProximityPP::id );er;
	st = plugin.deregisterNode( sinePP::id );er;
	st = plugin.deregisterNode( accumulatorPP::id );er;
	st = plugin.deregisterNode( uvPP::id );er;
	st = plugin.deregisterNode( containmentPP::id );er;
	st = plugin.deregisterNode( transitionPP::id );er;
	st = plugin.deregisterNode( multDoubleDoublePP::id );er;
	st = plugin.deregisterNode( priorityClipPP::id );er;
	st = plugin.deregisterNode( multVectorVectorPP::id );er;
	st = plugin.deregisterNode( multVectorDoublePP::id );er;
	st = plugin.deregisterNode( coordSysComponentPP::id );er;
	st = plugin.deregisterNode( vectorRampLookupPP::id );er;
	st = plugin.deregisterNode( signPP::id );er;
	st = plugin.deregisterNode( dotPP::id );er;
	st = plugin.deregisterNode( crossPP::id );er;
	st = plugin.deregisterNode( blendDoublePP::id );er;
	st = plugin.deregisterNode( blendVectorPP::id );er;
	st = plugin.deregisterNode( magNormPP::id );er;
	st = plugin.deregisterNode( orthogonalPP::id );er;
	st = plugin.deregisterNode( reversePP::id );er;
	st = plugin.deregisterNode( binormalPP::id );er;
	st = plugin.deregisterNode( vectorComponentPP::id );er;
	st = plugin.deregisterNode( addPP::id );er;
	st = plugin.deregisterNode( extractElements::id );er;
	st = plugin.deregisterNode( rampLookupPP::id );er;
	st = plugin.deregisterNode( lookupPP::id );er;
	st = plugin.deregisterNode( arrayToMulti::id );er;
	st = plugin.deregisterNode( aimPP::id );er;
	st = plugin.deregisterNode( multPP::id );er;

	st = plugin.deregisterNode( jerkSensor::id );er;
	st = plugin.deregisterNode( axisSensor::id );er;
	st = plugin.deregisterNode( geometrySensor::id );er;
	st = plugin.deregisterNode( splineSensor::id );er;
	st = plugin.deregisterNode( splinePool::id );er;
	st = plugin.deregisterData( splinePoolData::id );er;
	st = plugin.deregisterNode( exposure::id );er;
	st = plugin.deregisterNode( cohesion::id );er;
	st = plugin.deregisterNode( approach::id );er;
	st = plugin.deregisterNode( separation::id );er;
	st = plugin.deregisterNode( alignment::id );er;
	st = plugin.deregisterNode( agentSensor::id );er;
	st = plugin.deregisterNode( sensor::id );er;
	st = plugin.deregisterNode( pool::id );er;
	st = plugin.deregisterData( poolData::id );er;

	st = plugin.deregisterNode( articulate::id );er;
	// st = plugin.deregisterNode( legacyTumbler::id );er;
	st = plugin.deregisterNode( tumbler::id );er;
	st = plugin.deregisterNode( rollerPP::id );er;

	// st = plugin.deregisterNode( legacySteerRemap::id );er;
	
	st = plugin.deregisterNode( steerRemap::id );er;
	
	st = plugin.deregisterNode( prioritize::id );er;
	st = plugin.deregisterNode( fishDeform::id );er;
	st = plugin.deregisterNode( flockShape::id );er;

	// deregister your plugin's nodes, commands, or whetever here.


	return st;
}


