#include <maya/MDataBlock.h>
#include <maya/MStatus.h>
#include <maya/MFnDependencyNode.h>
#include "errorMacros.h"

#include "Ground.h"



Ground::Ground()
	: m_useMesh(false),
	  m_mesh(),
	  m_meshFn(),
	  m_intersector()
{}

Ground::~Ground() {}

MStatus Ground::setMesh(const  MObject &node,  MDataBlock &data) {
	// cerr <<  "Ground::setMesh" << endl;

	MStatus st;
	MFnDependencyNode dn(node, &st); mser;

	MObject aSnapFeetToMesh = dn.attribute( "snapFeetToMesh", &st); mser;
	m_useMesh = data.inputValue(aSnapFeetToMesh).asBool();

	MObject aAnimatedFloor = dn.attribute	( "animatedFloor", &st); mser;
	m_isAnimated = data.inputValue(aAnimatedFloor).asBool();



	MObject aFloorMesh = dn.attribute	( "floorMesh", &st); mser;
	// cerr <<  "Ground::setMesh HERE 1" << endl;
	m_mesh = data.inputValue(aFloorMesh).asMeshTransformed();
	st = m_meshFn.setObject(m_mesh) ;
	if (st != MS::kSuccess) {
		m_meshFn.setObject(MObject::kNullObj);
		return MS::kUnknownParameter;
	}
	m_intersector = m_meshFn.autoUniformGridParams();

	return MS::kSuccess;
}

MPoint Ground::project(const MPoint &src, const MMatrix &mat)   {
	MStatus st;
	if (!valid()) {
		return src;
	}

	MFloatVector rayDirection = MFloatVector(MVector::yNegAxis * mat);
	MFloatPoint rayOrigin = MFloatPoint(src);

	MFloatPoint hitPoint;
	float hitParam ; //
	int hitFace ;
	int hitTriangle ;

	float meshDist = 1000000;

	bool didHit = m_meshFn.closestIntersection(
	                rayOrigin, rayDirection,
	                NULL, NULL, false,
	                MSpace::kWorld,  meshDist, true,
	                &m_intersector, hitPoint,
	                &hitParam, &hitFace, &hitTriangle, 0, 0, 0.00001f, &st
	              );
	mser;

	// cerr << "didHit" << didHit << endl;
	// cerr << "hitPoint" << hitPoint << endl;




	if ( st != MS::kSuccess || (!didHit)) {
		return src;
	}
	return MPoint(hitPoint);

}

bool Ground::valid() const {
	return m_mesh.hasFn(MFn::kMesh) && m_useMesh;
}

bool Ground::isAnimated() const {
	return  m_isAnimated;
}


