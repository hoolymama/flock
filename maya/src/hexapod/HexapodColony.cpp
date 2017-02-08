

#include <string.h>

#include <sstream>

#include <list>
#include <maya/MObject.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnIntArrayData.h>

#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>

#include <maya/MDoubleArray.h>
#include <maya/MIntArray.h>
#include <maya/MVectorArray.h>


#include "mayaMath.h"

#include "HexapodColony.h"

HexapodColony::HexapodColony( ) :m_agents(), m_sortedIds()
{
}


void HexapodColony::clear(){
	std::list<HexapodAgent*>::iterator iter = m_agents.begin();
	while (iter != m_agents.end()) {
		delete *iter;
		*iter =0;
		iter++;
	}
	m_agents.clear();
	m_sortedIds.setLength(0);
}

MStatus HexapodColony::update( 
	const MIntArray & particleId, 
	const MIntArray & sortedIds, 
	const MIntArray & idIndex,
	const MVectorArray & pos,
	const MVectorArray & phi, 
	const MVectorArray & vel,
	const MVectorArray & omega,
	const MDoubleArray & scale
	){

	const MIntArray & left = m_sortedIds;
	const MIntArray & right = sortedIds;

	// int lPeg = 0;
	int rPeg = 0;

	std::list<HexapodAgent*>::iterator shadow;
	std::list<HexapodAgent*>::iterator agent = m_agents.begin();
  while (true) {
  	if (agent == m_agents.end() && rPeg == right.length()) {
  		// finished
  		break;
  	}
  	if (rPeg == right.length()) { // finished adding right stuff. delete the rest of left.
  		shadow = agent; 
 			agent++;
 			m_agents.erase(shadow);
 			// remove shadow;
  	} else if (agent == m_agents.end()) {
  			// insert new agent with id right[rPeg] before end
  		m_agents.push_back(new HexapodAgent( pos[idIndex[rPeg]]  , particleId[idIndex[rPeg]]));
  		rPeg++;
  	} else if (right[rPeg] < (*agent)->id()) {
  		m_agents.insert(agent, new HexapodAgent( pos[idIndex[rPeg]]  , particleId[idIndex[rPeg]]));
 			// insert new agent with id right[rPeg] before (*agent)
 			rPeg++;
 		} else if (right[rPeg]  == (*agent)->id()) {
 			(*agent)->update( pos[idIndex[rPeg]]);
 			// update (*agent)
 			rPeg++;
 			agent++;
 		} else { // right[rPeg] > (*agent)->id()
 			shadow = agent; 
 			agent++;
 			m_agents.erase(shadow);
 			// remove shadow;
 		}
	}


	return MS::kSuccess;
}


HexapodColony::~HexapodColony(){
	clear();
}

	// id mapping - 
	//////////////////////// IDS ////////////////////////
	// i         	:  0   1   2   3   4   5   6   7   8 //
	// particleId	: 14   6   2  12  10   5  13   8  11 // 

	// idIndex tells us the index in the id array 
	// sortedId  	:  2   5   6   8  10  11  12  13  14 // 
	// idIndex   	:  2   5   1   7   4   8   3   6   0 //




