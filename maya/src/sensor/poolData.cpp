/***************************************************************************
                          poolData.cpp  -  description
                             -------------------
    begin                : Fri Mar 31 2006
    copyright            : (C) 2006 by Julian Mann
    email                : julian.mann@gmail.com

    this is the maya data object containing a agentKdTree so that it can be passed
    through the dependency graph.
    It also contains some methods to make calls to the agentKdTree methods
 ***************************************************************************/


//
// Author: Julian Mann
//

#include <maya/MDataHandle.h>
#include <maya/MFnVectorArrayData.h>
#include <poolData.h>
#include <errorMacros.h>
#include "jMayaIds.h"
#define EPSILON 0.0000001

const double bigNum = 99999999999.0  ;

MTypeId poolData::id( k_poolData );
MString poolData::typeName( "poolData" );

poolData::poolData()
{
	m_pTree = 0;
}

poolData::~poolData()
{
}

void* poolData::creator()
{
	return new poolData;
}

// the agentKdTree
 agentKdTree*	poolData::tree() const {return m_pTree;}

// clean up
void	poolData::clear() {
	if (m_pTree) {delete m_pTree;m_pTree = 0;}
}

// create new tree
void poolData::create(MDataHandle &hp) {
	
	MStatus st = MS::kSuccess;
	MString method("poolData::create" );
	
	// clean up old stuff
	clear();
	
	// make a new tree
	m_pTree = new agentKdTree();
	if (m_pTree)  {
		
     	MObject  d = hp.data();
     	MFnVectorArrayData fnV(d,&st);
		if (!(st.error())){
			const MVectorArray & points = fnV.array(&st);
			if (!(st.error())){
				// we have legal points
				unsigned len = points.length();
				
				if (len) {
					
		
					m_pTree->populate(points);
				
					m_pTree->build();
				}
			}
		}
 	}
	// NOTE: there is a agentKdTree which is either
	// null, has zero size, or has size
	// Any calling function should check the state of the tree
}


int poolData::size(){
	if (m_pTree)
		return m_pTree->size();
	else
		return 0;
}

void poolData::closestNPoints(const agent * searchAgent, double radius, int num, AGENT_VECTOR * neighbors, MDoubleArray &dists )  const {
    if (num > 0 ) {
		AGENT_QUEUE * q = new AGENT_QUEUE;
		for (int p = 0;p<num;p++) {
			agentDist k;
			k.dist = radius;
			k.pAgent=0;
			q->push(k);
		}
		
		m_pTree->closestNPts( m_pTree->root(),searchAgent, q);
		
		while (!q->empty()) {
			if (q->top().pAgent) {
				neighbors->push_back(q->top().pAgent);
				dists.append(q->top().dist);
			}
			q->pop();
		}
		delete q;
    } 
}
/*

void poolData::closestNPoints(const MVector &searchPoint, double radius, int num, AGENT_VECTOR * neighbors, MDoubleArray &dists )  const {
    AGENT_QUEUE * q;
    if (num > 0 ) {
		for (int p = 0;p<num;p++) {
			agentDist k;
			k.dist = radius;
			k.pAgent=0;
			q->push(k);
		}
		
		m_pTree->closestNPts( m_pTree->root(),searchPoint, q);
		
		while (!q->empty()) {
			if (q->top().pAgent) {
				neighbors->push_back(q->top().pAgent);
				dists.append(q->top().dist);
			}
			q->pop();
		}
    } 
}
*/

void poolData::copy(const MPxData& otherData)
{
	m_pTree = ((const poolData&)otherData).tree();
}

poolData& poolData::operator=(const poolData & otherData ) {
	if (this != &otherData ) {
		m_pTree = otherData.tree();
	}
	return *this;
}


