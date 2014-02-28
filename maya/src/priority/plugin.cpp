#include <maya/MStatus.h>
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>

#include "errorMacros.h"
#include "prioritize.h"

MStatus initializePlugin( MObject obj)
{

	MStatus st;
	
	MString method("initializePlugin");

	MFnPlugin plugin( obj, PLUGIN_VENDOR, PLUGIN_VERSION , MAYA_VERSION);

	st = plugin.registerNode( "prioritize", prioritize::id, prioritize::creator, prioritize::initialize, MPxNode::kLocatorNode  );er;
	
	return st;

}

MStatus uninitializePlugin( MObject obj)
{
	MStatus st;

	MString method("uninitializePlugin");

	MFnPlugin plugin( obj );

	st = plugin.deregisterNode( prioritize::id );er;


	return st;
}


