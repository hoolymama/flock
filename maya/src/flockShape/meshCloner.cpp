#include <string.h>
#include <sstream>
#include <vector>
#include <maya/MObject.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnIntArrayData.h>

#include <maya/MDoubleArray.h>
#include <maya/MIntArray.h>
#include <maya/MVectorArray.h>

#include <maya/MArrayDataHandle.h>

#include "mayaMath.h"
#include "flockShape.h"

#include "meshCloner.h"
/**/
meshCloner::meshCloner()
	:m_needsRebuild(false),
	m_outGeom(MObject::kNullObj),
	m_numVertices(0),
	m_vertices(0,MFloatPoint::origin),
	m_counts(0,0),
	m_connects(0,0),
	m_Us(0,0.0f),
	m_Vs(0,0.0f),
	m_uvCounts(0,0),
	m_uvIds(0,0){};


meshCloner::meshCloner(const MPlug &plug)
	:m_needsRebuild(false),
	m_outGeom(MObject::kNullObj),
	m_numVertices(0),
	m_vertices(0,MFloatPoint::origin),
	m_counts(0,0),
	m_connects(0,0),
	m_Us(0,0.0f),
	m_Vs(0,0.0f),
	m_uvCounts(0,0),
	m_uvIds(0,0)
{
	m_plug = plug;
	//cerr << "meshCloner::meshCloner plug " <<  m_plug.name() << endl;
}


meshCloner::~meshCloner(){}

const MObject& meshCloner::geometry() const {
	return m_outGeom;
}

// if we find that the input mesh has changed, then we definitely need to rebuild the output school -
// even if it didnt change, if the school size changed we also need to rebuild the school 
void meshCloner::update(const MObject &d, bool schoolSizeChanged){
	MStatus st;
	MFnMesh meshFn(d);
	m_vertices.clear();
	m_needsRebuild = false;

	if (meshFn.numVertices() != m_numVertices) {
		m_needsRebuild=true;
		m_numVertices = meshFn.numVertices();	
	}

	unsigned prevNumCounts = m_counts.length();
	unsigned prevNumConnects = m_connects.length();
	meshFn.getVertices(m_counts,m_connects);
	if ((m_counts.length() != prevNumCounts) || (m_connects.length() != prevNumConnects) ) m_needsRebuild=true;

	if (m_needsRebuild) {	
		m_Us.clear();
		m_Vs.clear();
		m_uvCounts.clear();
		m_uvIds.clear();
		if (meshFn.numUVs() > 0){
			st = meshFn.getUVs( m_Us,  m_Vs) ;er;
			st = meshFn.getAssignedUVs( m_uvCounts, m_uvIds) ;er;
		}
	}
	
	if (schoolSizeChanged) 	m_needsRebuild=true;
}


// if we are not rebuilding from scratch, then we just add the points to the points array - like a deformer
// If we are rebuilding then we have to do the whole lot.
void meshCloner::pushPoints(){	
	MStatus st;
	MObject d;
//	cerr << "meshCloner::pushPoints plug " <<  m_plug.name() << endl;
	st = m_plug.getValue(d);er;

	MFnMesh fnM(d);
	// get points, counts and connects
	//////////////////////////////////////
	MFloatPointArray points;		
	st = fnM.getPoints(points, MSpace::kWorld); er;
	for (unsigned j=0;j<m_numVertices;j++) {
		m_vertices.append(points[j]);
	}
}

MStatus meshCloner::prepOutGeom(unsigned schoolSize){
	MStatus st;
  //  if (! m_vertices.length()) {
  //  	m_outGeom = MObject::kNullObj;
  //  	return MS::kUnknownParameter;
  //  }
	
	MFnMesh fnM;
	if (! m_needsRebuild) {
		st = fnM.setObject(m_outGeom);ert;
		fnM.setPoints(m_vertices);	
	} else {

		MIntArray schoolFaceCounts;
		MIntArray schoolConnectivity;
		MIntArray schoolUVCounts;
		MIntArray schoolUVIds;

		unsigned index = 0;
		
		
		////////////////// COUNTS //////////////////////
		unsigned nCounts = ( m_counts.length() );	
		schoolFaceCounts.setLength(nCounts * schoolSize);
		for (unsigned i = 0;i<schoolSize;i++) {
			for (unsigned j = 0;j<nCounts;j++) {
				schoolFaceCounts.set(m_counts[j],index);
				index++;
			}
		}
		//////////////////////////////////////	
				
		////////////////// CONNECTS //////////////////////
		index = 0;		
		unsigned nConnects = (m_connects.length() );
		schoolConnectivity.setLength(nConnects * schoolSize);
		for (unsigned i = 0;i<schoolSize;i++) {
			unsigned startIndex = i*m_numVertices;
			for (unsigned j = 0;j<nConnects;j++) {
				schoolConnectivity.set( ( startIndex + m_connects[j] ) ,index);
				index++;
			}
		}
		//////////////////////////////////////	
 		MFnMeshData fnC;
		m_outGeom = fnC.create(&st);er;
		fnM.create( m_vertices.length(), 
			schoolFaceCounts.length(), 
			m_vertices, schoolFaceCounts, schoolConnectivity, 
			m_outGeom, &st );  er;
		//m_outMeshFn.create( vertices.length(), faceCounts.length(), vertices, faceCounts, connectivity, m_Us, m_Vs, m_outGeom, &st );  er;	
	
		unsigned nUVCounts = m_uvCounts.length();
		if (nUVCounts == nCounts) {

			for (unsigned i = 0;i<schoolSize;i++) {// number of fish
				for (unsigned j = 0;j<nUVCounts;j++) { // number of polygons in one fish
					schoolUVCounts.append(m_uvCounts[j]);
				}
			}

			unsigned nUVIds = (m_uvIds.length() );
			for (unsigned i = 0;i<schoolSize;i++) {
				for (unsigned j = 0;j<nUVIds;j++) {
					schoolUVIds.append(m_uvIds[j]);
				}
			}
			st = fnM.setUVs(m_Us, m_Vs);	er;	
			st = fnM.assignUVs ( schoolUVCounts, schoolUVIds );		er;		

		} else {
			cerr << "nUVCounts counts don't match nCounts: " <<  nUVCounts << " " <<  nCounts << endl;
		}
	}
	

	return MS::kSuccess;
	
}


