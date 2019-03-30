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
#include "legacyTumbler.h"
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
#include <ellipsoidSensor.h>
#include <slotSensor.h>
#include <goalSensor.h>

#include <collisionSensor.h>
#include "multPP.h"
#include "addPP.h"
#include "aimPP.h"

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

// #include "curvePoints.h"
#include "hexapod.h"

#include "hungerState.h"
#include "addDoublePP.h"
#include "addVectorPP.h"
#include "vectorArrayToMulti.h"
#include "phiArrayToMulti.h"
#include "arrayToMulti.h"
#include "multiBlendVectorPP.h"

#include "averageVectorPP.h"

#include "averageDoublePP.h"



MStatus initializePlugin( MObject obj)
{

	MStatus st;

	MString method("initializePlugin");

	MFnPlugin plugin( obj, PLUGIN_VENDOR, PLUGIN_VERSION , MAYA_VERSION);
	st = plugin.registerNode( "flockShape", flockShape::id, flockShape::creator,
	                          flockShape::initialize, MPxNode::kLocatorNode); mser;

	st = plugin.registerNode( "prioritize", prioritize::id, prioritize::creator,
	                          prioritize::initialize, MPxNode::kLocatorNode); mser;
	st = plugin.registerNode( "steerRemap", steerRemap::id, steerRemap::creator,
	                          steerRemap::initialize ); mser;
	st = plugin.registerNode( "roller", rollerPP::id, rollerPP::creator,
	                          rollerPP::initialize); msert;
	st = plugin.registerNode( "tumbler", tumbler::id, tumbler::creator, tumbler::initialize );
	mser;

	st = plugin.registerNode( "fishDeform", fishDeform::id, fishDeform::creator,
	                          fishDeform::initialize); mser;



	st = plugin.registerNode( "legacyTumbler", legacyTumbler::id, legacyTumbler::creator,
	                          legacyTumbler::initialize); mser;

	st = plugin.registerNode( "articulate", articulate::id, articulate::creator,
	                          articulate::initialize); mser;

	st = plugin.registerData( "poolData", poolData::id, poolData::creator ); mser;
	st = plugin.registerNode( "pool", pool::id, pool::creator, pool::initialize ); mser;
	st = plugin.registerNode( "sensor", sensor::id, sensor::creator, sensor::initialize );
	mser;
	st = plugin.registerNode( "agentSensor", agentSensor::id, agentSensor::creator,
	                          agentSensor::initialize ); mser;
	st = plugin.registerNode( "alignment", alignment::id, alignment::creator,
	                          alignment::initialize ); mser;
	st = plugin.registerNode( "separation", separation::id, separation::creator,
	                          separation::initialize ); mser;
	st = plugin.registerNode( "approach", approach::id, approach::creator,
	                          approach::initialize ); mser;
	st = plugin.registerNode( "cohesion", cohesion::id, cohesion::creator,
	                          cohesion::initialize ); mser;
	st = plugin.registerNode( "exposure", exposure::id, exposure::creator,
	                          exposure::initialize ); mser;
	st = plugin.registerNode( "ellipsoidSensor", ellipsoidSensor::id,
	                          ellipsoidSensor::creator, ellipsoidSensor::initialize ); mser;
	st = plugin.registerNode( "slotSensor", slotSensor::id, slotSensor::creator,
	                          slotSensor::initialize ); mser;
	st = plugin.registerNode( "goalSensor", goalSensor::id, goalSensor::creator,
	                          goalSensor::initialize ); mser;

	st = plugin.registerNode( "collisionSensor", collisionSensor::id,
	                          collisionSensor::creator, collisionSensor::initialize ); mser;




	st = plugin.registerData( "splinePoolData", splinePoolData::id, splinePoolData::creator );
	mser;
	st = plugin.registerNode( "splinePool", splinePool::id, splinePool::creator,
	                          splinePool::initialize ); mser;
	st = plugin.registerNode( "splineSensor", splineSensor::id, splineSensor::creator,
	                          splineSensor::initialize ); mser;
	st = plugin.registerNode( "geometrySensor", geometrySensor::id, geometrySensor::creator,
	                          geometrySensor::initialize ); mser;
	st = plugin.registerNode( "axisSensor", axisSensor::id, axisSensor::creator,
	                          axisSensor::initialize ); mser;
	st = plugin.registerNode( "jerkSensor", jerkSensor::id, jerkSensor::creator,
	                          jerkSensor::initialize ); mser;
	st = plugin.registerNode( "multPP", multPP::id, multPP::creator, multPP::initialize);
	mser;
	st = plugin.registerNode( "aimPP", aimPP::id, aimPP::creator, aimPP::initialize); mser;
	st = plugin.registerNode( "lookupPP", lookupPP::id, lookupPP::creator,
	                          lookupPP::initialize); mser;
	st = plugin.registerNode( "rampLookupPP", rampLookupPP::id, rampLookupPP::creator,
	                          rampLookupPP::initialize); mser;
	st = plugin.registerNode( "extractElements", extractElements::id,
	                          extractElements::creator, extractElements::initialize); mser;
	st = plugin.registerNode( "addPP", addPP::id, addPP::creator, addPP::initialize); mser;
	st = plugin.registerNode( "addDoublePP", addDoublePP::id, addDoublePP::creator,
	                          addDoublePP::initialize); mser;
	st = plugin.registerNode( "addDoublePP", addDoublePP::id, addDoublePP::creator,
	                          addDoublePP::initialize); mser;
	st = plugin.registerNode( "addVectorPP", addVectorPP::id, addVectorPP::creator,
	                          addVectorPP::initialize); mser;

	st = plugin.registerNode( "vectorComponentPP", vectorComponentPP::id,
	                          vectorComponentPP::creator, vectorComponentPP::initialize); mser;
	st = plugin.registerNode( "binormalPP", binormalPP::id, binormalPP::creator,
	                          binormalPP::initialize); mser;
	st = plugin.registerNode( "reversePP", reversePP::id, reversePP::creator,
	                          reversePP::initialize); mser;
	st = plugin.registerNode( "orthogonalPP", orthogonalPP::id, orthogonalPP::creator,
	                          orthogonalPP::initialize); mser;
	st = plugin.registerNode( "magNormPP", magNormPP::id, magNormPP::creator,
	                          magNormPP::initialize); mser;
	st = plugin.registerNode( "blendVectorPP", blendVectorPP::id, blendVectorPP::creator,
	                          blendVectorPP::initialize); mser;
	st = plugin.registerNode( "blendDoublePP", blendDoublePP::id, blendDoublePP::creator,
	                          blendDoublePP::initialize); mser;
	st = plugin.registerNode( "crossPP", crossPP::id, crossPP::creator, crossPP::initialize);
	mser;
	st = plugin.registerNode( "dotPP", dotPP::id, dotPP::creator, dotPP::initialize); mser;
	st = plugin.registerNode( "signPP", signPP::id, signPP::creator, signPP::initialize);
	mser;
	st = plugin.registerNode( "vectorRampLookupPP", vectorRampLookupPP::id,
	                          vectorRampLookupPP::creator, vectorRampLookupPP::initialize); mser;
	st = plugin.registerNode( "coordSysComponentPP", coordSysComponentPP::id,
	                          coordSysComponentPP::creator, coordSysComponentPP::initialize); mser;
	st = plugin.registerNode( "multVectorDoublePP", multVectorDoublePP::id,
	                          multVectorDoublePP::creator, multVectorDoublePP::initialize); mser;
	st = plugin.registerNode( "multVectorVectorPP", multVectorVectorPP::id,
	                          multVectorVectorPP::creator, multVectorVectorPP::initialize); mser;
	st = plugin.registerNode( "priorityClipPP", priorityClipPP::id, priorityClipPP::creator,
	                          priorityClipPP::initialize); mser;
	st = plugin.registerNode( "multDoubleDoublePP", multDoubleDoublePP::id,
	                          multDoubleDoublePP::creator, multDoubleDoublePP::initialize); mser;
	st = plugin.registerNode( "transitionPP", transitionPP::id, transitionPP::creator,
	                          transitionPP::initialize); mser;
	st = plugin.registerNode( "containmentPP", containmentPP::id, containmentPP::creator,
	                          containmentPP::initialize); mser;
	st = plugin.registerNode( "uvPP", uvPP::id, uvPP::creator, uvPP::initialize); mser;
	st = plugin.registerNode( "accumulatorPP", accumulatorPP::id, accumulatorPP::creator,
	                          accumulatorPP::initialize); mser;
	st = plugin.registerNode( "sinePP", sinePP::id, sinePP::creator, sinePP::initialize);
	mser;
	st = plugin.registerNode( "meshProximityPP", meshProximityPP::id,
	                          meshProximityPP::creator, meshProximityPP::initialize); mser;
	st = plugin.registerNode( "subVectorVectorPP", subVectorVectorPP::id,
	                          subVectorVectorPP::creator, subVectorVectorPP::initialize); mser;
	st = plugin.registerNode( "speedRampLookupPP", speedRampLookupPP::id,
	                          speedRampLookupPP::creator, speedRampLookupPP::initialize); mser;
	st = plugin.registerNode( "vectorArrayToMulti", vectorArrayToMulti::id,
	                          vectorArrayToMulti::creator, vectorArrayToMulti::initialize); mser;
	st = plugin.registerNode( "phiArrayToMulti", phiArrayToMulti::id,
	                          phiArrayToMulti::creator, phiArrayToMulti::initialize); mser;
	st = plugin.registerNode( "arrayToMulti", arrayToMulti::id, arrayToMulti::creator,
	                          arrayToMulti::initialize); mser;
	st = plugin.registerNode( "multiBlendVectorPP", multiBlendVectorPP::id,
	                          multiBlendVectorPP::creator, multiBlendVectorPP::initialize); mser;
	st = plugin.registerNode( "averageVectorPP", averageVectorPP::id,
	                          averageVectorPP::creator, averageVectorPP::initialize); mser;
	st = plugin.registerNode( "averageDoublePP", averageDoublePP::id,
	                          averageDoublePP::creator, averageDoublePP::initialize); mser;



	// st = plugin.registerNode( "curvePoints", curvePoints::id, curvePoints::creator, curvePoints::initialize); mser;

	st = plugin.registerNode( "hexapod", hexapod::id, hexapod::creator, hexapod::initialize,
	                          MPxNode::kLocatorNode); mser;
	// st = plugin.registerNode( "hexapodLocatorShape", hexapodLocator::id, hexapodLocator::creator,hexapodLocator::initialize, MPxNode::kLocatorNode  );mser;

	st = plugin.registerNode( "hungerState", hungerState::id, hungerState::creator,
	                          hungerState::initialize); mser;

	MGlobal::executePythonCommand("import flock;flock.load()");
	return st;

}

MStatus uninitializePlugin( MObject obj)
{
	MStatus st;

	MString method("uninitializePlugin");

	MGlobal::executePythonCommand("import flock;flock.unload()");

	MFnPlugin plugin( obj );

	st = plugin.deregisterNode( hungerState::id ); mser;

	// st = plugin.deregisterNode( hexapodLocator::id );mser;
	st = plugin.deregisterNode( hexapod::id ); mser;
	// st = plugin.deregisterNode( curvePoints::id );mser;
	st = plugin.deregisterNode( averageDoublePP::id ); mser;
	st = plugin.deregisterNode( averageVectorPP::id ); mser;

	st = plugin.deregisterNode( multiBlendVectorPP::id ); mser;

	st = plugin.deregisterNode( arrayToMulti::id ); mser;
	st = plugin.deregisterNode( phiArrayToMulti::id ); mser;


	st = plugin.deregisterNode( vectorArrayToMulti::id ); mser;

	st = plugin.deregisterNode( speedRampLookupPP::id ); mser;


	st = plugin.deregisterNode( subVectorVectorPP::id ); mser;
	st = plugin.deregisterNode( meshProximityPP::id ); mser;
	st = plugin.deregisterNode( sinePP::id ); mser;
	st = plugin.deregisterNode( accumulatorPP::id ); mser;
	st = plugin.deregisterNode( uvPP::id ); mser;
	st = plugin.deregisterNode( containmentPP::id ); mser;
	st = plugin.deregisterNode( transitionPP::id ); mser;
	st = plugin.deregisterNode( multDoubleDoublePP::id ); mser;
	st = plugin.deregisterNode( priorityClipPP::id ); mser;
	st = plugin.deregisterNode( multVectorVectorPP::id ); mser;
	st = plugin.deregisterNode( multVectorDoublePP::id ); mser;
	st = plugin.deregisterNode( coordSysComponentPP::id ); mser;
	st = plugin.deregisterNode( vectorRampLookupPP::id ); mser;
	st = plugin.deregisterNode( signPP::id ); mser;
	st = plugin.deregisterNode( dotPP::id ); mser;
	st = plugin.deregisterNode( crossPP::id ); mser;
	st = plugin.deregisterNode( blendDoublePP::id ); mser;
	st = plugin.deregisterNode( blendVectorPP::id ); mser;
	st = plugin.deregisterNode( magNormPP::id ); mser;
	st = plugin.deregisterNode( orthogonalPP::id ); mser;
	st = plugin.deregisterNode( reversePP::id ); mser;
	st = plugin.deregisterNode( binormalPP::id ); mser;
	st = plugin.deregisterNode( vectorComponentPP::id ); mser;
	st = plugin.deregisterNode( addVectorPP::id ); mser;
	st = plugin.deregisterNode( addDoublePP::id ); mser;
	st = plugin.deregisterNode( addPP::id ); mser;
	st = plugin.deregisterNode( extractElements::id ); mser;
	st = plugin.deregisterNode( rampLookupPP::id ); mser;
	st = plugin.deregisterNode( lookupPP::id ); mser;

	st = plugin.deregisterNode( aimPP::id ); mser;
	st = plugin.deregisterNode( multPP::id ); mser;

	st = plugin.deregisterNode( jerkSensor::id ); mser;
	st = plugin.deregisterNode( axisSensor::id ); mser;
	st = plugin.deregisterNode( geometrySensor::id ); mser;
	st = plugin.deregisterNode( splineSensor::id ); mser;
	st = plugin.deregisterNode( splinePool::id ); mser;
	st = plugin.deregisterData( splinePoolData::id ); mser;
	st = plugin.deregisterNode( collisionSensor::id ); mser;

	st = plugin.deregisterNode( goalSensor::id ); mser;
	st = plugin.deregisterNode( slotSensor::id ); mser;
	st = plugin.deregisterNode( ellipsoidSensor::id ); mser;
	st = plugin.deregisterNode( exposure::id ); mser;
	st = plugin.deregisterNode( cohesion::id ); mser;
	st = plugin.deregisterNode( approach::id ); mser;
	st = plugin.deregisterNode( separation::id ); mser;
	st = plugin.deregisterNode( alignment::id ); mser;
	st = plugin.deregisterNode( agentSensor::id ); mser;
	st = plugin.deregisterNode( sensor::id ); mser;
	st = plugin.deregisterNode( pool::id ); mser;
	st = plugin.deregisterData( poolData::id ); mser;

	st = plugin.deregisterNode( articulate::id ); mser;
	st = plugin.deregisterNode( legacyTumbler::id ); mser;
	st = plugin.deregisterNode( tumbler::id ); mser;
	st = plugin.deregisterNode( rollerPP::id ); mser;

	// st = plugin.deregisterNode( legacySteerRemap::id );mser;

	st = plugin.deregisterNode( steerRemap::id ); mser;

	st = plugin.deregisterNode( prioritize::id ); mser;
	st = plugin.deregisterNode( fishDeform::id ); mser;
	st = plugin.deregisterNode( flockShape::id ); mser;

	// deregister your plugin's nodes, commands, or whetever here.


	return st;
}


