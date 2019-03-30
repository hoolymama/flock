/***************************************************************************
splinePoolData.cpp  -  description
-------------------
    begin                : Fri Mar 31 2006
    copyright            : (C) 2006 by Julian Mann
    email                : julian.mann@gmail.com

    this is the maya data object containing a splineKdTree so that it can be passed
    through the dependency graph.
    It also contains some methods to make calls to the splineKdTree methods
	***************************************************************************/



//
// Copyright (C) 2000 hoolyMama

//
// Author: Julian Mann
//


#include <maya/MDataHandle.h>
#include <splineKdTree.h>
#include <splineData.h>
#include <splinePoolData.h>
#include <errorMacros.h>
#include "jMayaIds.h"

#define EPSILON 0.0000001

const double bigNum = 99999999999.0  ;

MTypeId splinePoolData::id( k_splinePoolData );
MString splinePoolData::typeName( "splinePoolData" );

splinePoolData::splinePoolData()
{
	m_pTree = 0;

}

splinePoolData::~splinePoolData()
{
}

void *splinePoolData::creator()
{
	return new splinePoolData;
}

// the ptKdTree
splineKdTree	*splinePoolData::tree() const {return m_pTree;}

// clean up
void	splinePoolData::clear() {
	if (m_pTree) {delete m_pTree; m_pTree = 0;}
}


// create new tree
void splinePoolData::create(MDataHandle &h) {

	MStatus st = MS::kSuccess;
	MString method("splinePoolData::create" );

	// clean up old stuff
	clear();

	// make a new tree
	m_pTree = new splineKdTree();
	if (m_pTree)  {
		MObject  d = h.data();
		MFnDynSweptGeometryData fnG(d, &st);
		if (!(st.error())) {
			st = m_pTree->populate(fnG, 0);
			if (!(st.error())) {
				m_pTree->build();
			}
		}
	}
	// NOTE: there is now a splineKdTree which is either
	// null, has zero size, or has size
	// Any calling function should check the state of the tree
}


void splinePoolData::create(MArrayDataHandle &ah) {
	// create new tree from an array of curves

	MStatus st = MS::kSuccess;
	MString method("splinePoolData::create" );

	clear();

	unsigned n = ah.elementCount();

	// make a new tree
	m_pTree = new splineKdTree();
	if (m_pTree)  {

		for (unsigned i = 0; i < n; i++, ah.next()) {
			int el = ah.elementIndex(&st);
			MDataHandle h = ah.inputValue();
			MObject  d = h.data();
			MFnDynSweptGeometryData fnG(d, &st);
			if (!(st.error())) {
				m_pTree->populate(fnG, el);
			}
		}
		m_pTree->build();
	}
	// NOTE: there is now a splineKdTree which is either
	// null, has zero size, or has size
	// Any calling function should check the state of the tree
}

int splinePoolData::size() {
	if (m_pTree)
	{ return m_pTree->size(); }
	else
	{ return 0; }
}

/*
MStatus splinePoolData::closestPointOnLine(
										 const MVector &searchPoint,
										 MVector &resultPoint,
										 double &resultU,
										 double &resultParam
										 ) const {

	MStatus st = MS::kSuccess;
	MString method("splinePoolData::closestPointOnLine" );

	MPoint c = MPoint(searchPoint);
	double searchRad = bigNum;
	splineData tb;
	m_pTree->closestLine( m_pTree->root(), c ,searchRad,  tb);
  	if (searchRad < bigNum) {  // we found a point
 		resultU = tb.calcU();
		resultPoint =  tb.cachedPoint() ;
		resultParam = tb.cachedParam() ;
		st = MS::kSuccess;
 	} else { // we never found a closest point, so something is seriously wrong
		st = MS::kFailure; mser;
	}
	return st;
										 }

MStatus splinePoolData::closestPointOnLine(
										 const MVector &searchPoint,
										 MVector &resultPoint
										 ) const {
	MStatus st = MS::kSuccess;
	MString method("splinePoolData::closestPointOnLine" );

	MPoint c = MPoint(searchPoint);
	double searchRad = bigNum;
	splineData tb;
	m_pTree->closestLine( m_pTree->root(), c ,searchRad,  tb);
  	if (searchRad < bigNum) {  // we found a point
		resultPoint =  tb.cachedPoint() ;
		st = MS::kSuccess;
 	} else { // we never found a closest point, so something is seriously wrong
		st = MS::kFailure; mser;
	}
	return st;
										 }

MStatus splinePoolData::closestPointsOnLine(
										  const MVectorArray &searchPoints,
										  MVectorArray &resultPoints,
										  MDoubleArray &resultUs,
										  //MIntArray &resultIds,
										  MDoubleArray &resultParams
										  ) const {
	MStatus st = MS::kSuccess;
	MString method("splinePoolData::closestPointsOnLine" );

	unsigned len = searchPoints.length();
	resultPoints.setLength(len) ;
	//resultIds.setLength(len) ;
	resultUs.setLength(len) ;
 	resultParams.setLength(len) ;


	MPoint c   ; // the center of the search sphere
	for (unsigned i = 0; i<len; i++ ) {
		c = MPoint(searchPoints[i]);
		double searchRad = bigNum;
		splineData tb;
		m_pTree->closestLine( m_pTree->root(), c ,searchRad,  tb);
		if (searchRad < bigNum) {  // we found a point
			resultPoints.set( tb.cachedPoint() , i);
 			resultUs.set(tb.calcU(),i);
 			//resultIds.set(tb.id() ,i);
			resultParams.set( tb.cachedParam() , i);
			st = MS::kSuccess;
    	} else { // we never found a closest point, so something is seriously wrong
			st = MS::kFailure; mser;
		}
	}
	return st;
										  }

MStatus splinePoolData::closestPointsOnLine(
										  const MVectorArray &searchPoints,
										  MVectorArray &resultPoints
										  ) const {
	MStatus st = MS::kSuccess;
	MString method("splinePoolData::closestPointsOnTri" );

	unsigned len = searchPoints.length();
	resultPoints.setLength(len) ;

	MPoint c   ; // the center of the search sphere
	for (unsigned i = 0; i<len; i++ ) {
		c = MPoint(searchPoints[i]);
		double searchRad = bigNum;
		splineData tb;
		m_pTree->closestLine( m_pTree->root(), c ,searchRad,  tb);
  		if (searchRad < bigNum) {  // we found a point
			resultPoints.set( tb.cachedPoint() , i);
			st = MS::kSuccess;
    	} else { // we never found a closest point, so something is seriously wrong
			st = MS::kFailure; mser;
		}
	}
	return st;
 }



MStatus splinePoolData::blendClosestTangentsOnLine(
											const MVectorArray &searchPoints,
											double distance,
											double attenuation,
											MVectorArray &resultTangents
											) const {
	MStatus st = MS::kSuccess;
	MString method("splinePoolData::blendClosestTangentsOnLine" );

	unsigned len = searchPoints.length();
	resultTangents.setLength(len) ;

	MPoint c   ; // the center of the search sphere
	for (unsigned i = 0; i<len; i++ ) {
		c = MPoint(searchPoints[i]);
		double searchRad = distance;
		splineData tb;
		double blendVal;
		m_pTree->closestLine( m_pTree->root(), c ,searchRad,  tb);
  		if (searchRad < distance) {  // we found a point
			if (attenuation == 0.0) {
				resultTangents.set( tb.tangent() , i);
			} else if (attenuation == 1.0) {
				blendVal = 1.0 - (searchRad / distance);
				MVector newVec = (tb.tangent() *blendVal) + (resultTangents[i] * (1.0 - blendVal));
				resultTangents.set( newVec , i);
			} else {
				blendVal = pow(1.0 - (searchRad / distance), attenuation);
				MVector newVec = (tb.tangent() *blendVal) + (resultTangents[i] * (1.0 - blendVal));
				resultTangents.set( newVec , i);
			}
    	}
	}
	return st;
}

MStatus splinePoolData::blendClosestTangentsOnLine(
												 const MVectorArray &searchPoints,
												 const MVectorArray &forces,
												 double distance,
												 double attenuation,
												 MVectorArray &resultTangents
												 ) const {
	MStatus st = MS::kSuccess;
	MString method("splinePoolData::blendClosestTangentsOnLine" );

	unsigned len = searchPoints.length();
	unsigned forcesLen = forces.length();
	//cerr << "forcesLen" << forcesLen << endl;
	//cerr << forces << endl;
	resultTangents.setLength(len) ;

	MPoint c   ; // the center of the search sphere


	for (unsigned i = 0; i<len; i++ ) {
		c = MPoint(searchPoints[i]);
		double searchRad = distance;
		splineData tb;
		double blendVal;
		MVector v;
		m_pTree->closestLine( m_pTree->root(), c ,searchRad,  tb);
  		if (searchRad < distance) {  // we found a point
			// get the force components
			//cerr << "id " << tb.id() << endl;
			v = MVector::xAxis;
			if (tb.id() < forcesLen) {
				v = forces[tb.id()];
			}
			const MVector &tan= tb.tangent();
			MVector theVector = tan;
			if (!(v.isEquivalent(MVector::xAxis))) { // if anything but tangent only

				const MVector &hit =  tb.cachedPoint();

				MVector norm = (c - hit);
				norm.normalize();
				MVector circular = (tan^(c-hit)).normal();

				// alternative that will act differently at the end of the line
				// MVector norm = circular^tan;

				theVector = ((tan*v.x) + (norm*v.y) + (circular*v.z)).normal();

			}
			if (attenuation == 0.0) {
				resultTangents.set( theVector, i);
			} else if (attenuation == 1.0) {
				blendVal = 1.0 - (searchRad / distance);
				MVector newVec = (theVector *blendVal) + (resultTangents[i] * (1.0 - blendVal));
				resultTangents.set( newVec , i);
			} else {
				blendVal = pow(1.0 - (searchRad / distance), attenuation);
				MVector newVec = (theVector *blendVal) + (resultTangents[i] * (1.0 - blendVal));
				resultTangents.set( newVec , i);
			}
    	}
	}
	return st;
}






MStatus splinePoolData::closestTangentsOnLine(
			  const MVectorArray &searchPoints,
			  const MVectorArray &forces,
			  MVectorArray &resultTangents
		) const {
	MStatus st = MS::kSuccess;
	MString method("splinePoolData::closestTangentsOnLine" );

	unsigned len = searchPoints.length();
	unsigned forcesLen = forces.length();
	resultTangents.setLength(len) ;

	MPoint c   ; // the center of the search sphere
	for (unsigned i = 0; i<len; i++ ) {
		c = MPoint(searchPoints[i]);
		double searchRad = bigNum;
		splineData tb;
		MVector v;
		m_pTree->closestLine( m_pTree->root(), c ,searchRad,  tb);
  		if (searchRad < bigNum) {  // we found a point
			v = MVector::xAxis;
			if (tb.id() < forcesLen) {
				v = forces[tb.id()];
			}
			const MVector &tan= tb.tangent();
			MVector theVector = tan;
			if (!(v.isEquivalent(MVector::xAxis))) { // if anything but tangent only

				const MVector &hit =  tb.cachedPoint();

				MVector norm = (c - hit);
				norm.normalize();
				MVector circular = (tan^(c-hit)).normal();

				// alternative that will act differently at the end of the line
				// MVector norm = circular^tan;

				theVector = ((tan*v.x) + (norm*v.y) + (circular*v.z)).normal();

			}

			resultTangents.set(theVector , i);
			st = MS::kSuccess;
    	} else { // we never found a closest point, so something is seriously wrong
			st = MS::kFailure; mser;
		}
	}
	return st;
 }


MStatus splinePoolData::closestTangentsOnLine(
	const MVectorArray &searchPoints,
	MVectorArray &resultTangents
	) const {
		MStatus st = MS::kSuccess;
		MString method("splinePoolData::closestTangentsOnLine" );

		unsigned len = searchPoints.length();
		resultTangents.setLength(len) ;

		MPoint c   ; // the center of the search sphere
		for (unsigned i = 0; i<len; i++ ) {
			c = MPoint(searchPoints[i]);
			double searchRad = bigNum;
			splineData tb;
			m_pTree->closestLine( m_pTree->root(), c ,searchRad,  tb);
			if (searchRad < bigNum) {  // we found a point
				resultTangents.set( tb.tangent() , i);
				st = MS::kSuccess;
			} else { // we never found a closest point, so something is seriously wrong
				st = MS::kFailure; mser;
			}
		}
		return st;
	}
*/

void splinePoolData::copy(const MPxData &otherData)
{
	m_pTree = ((const splinePoolData &)otherData).tree();
}


splinePoolData &splinePoolData::operator=(const splinePoolData &otherData ) {
	if (this != &otherData ) {
		m_pTree = otherData.tree();
	}
	return *this;
}


