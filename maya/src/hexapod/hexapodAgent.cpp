

#include <string.h>

#include <sstream>

#include <vector>
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

#include "HexapodAgent.h"


HexapodAgent::HexapodAgent(const MVector &pos, const MVector & phi, double scale)
{
	m_matrix = mayaMath::matFromPhi(pos, phi, MVector(scale,scale,scale));
	m_matrixInverse = m_matrix.inverse();
}


// HexapodAgent::setHome( ){


// }

HexapodAgent::~HexapodAgent(){
	// meshClonerMap::iterator it  = m_clonerMap.begin();
	// for (;it !=m_clonerMap.end();it++) {
	// 	if (it->second) {
	// 		delete it->second;
	// 		it->second = 0;
	// 	}
	// 	m_clonerMap.erase(it);
	// }
	
}
 
