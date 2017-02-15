

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

#if defined(OSMac_MachO_)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif



HexapodColony::HexapodColony( ) 
:m_agents()
{}


void HexapodColony::clear(){
	// cerr << "HexapodColony::clear" << endl;
	std::list<HexapodAgent*>::iterator iter = m_agents.begin();
	while (iter != m_agents.end()) {
		delete *iter;
		*iter =0;
		iter++;
	}
	m_agents.clear();
}





MStatus HexapodColony::update( 
	double dt, double maxVelocity, 
	const MIntArray & particleId, 
	const MIntArray & sortedId, 
	const MIntArray & idIndex,
	const MVectorArray & pos,
	const MVectorArray & phi, 
	const MVectorArray & vel,
	const MVectorArray & omega,
	const MDoubleArray & scale,
	const double homeAX, 
	const double homeAZ, 
	const double homeBX, 
	const double homeBZ, 
	const double homeCX, 
	const double homeCZ,
	const double radiusMinA, 
	const double radiusMaxA, 
	const double radiusMinB, 
	const double radiusMaxB, 
	const double radiusMinC, 
	const double radiusMaxC,
	const	MRampAttribute &incRampA,
	const	MRampAttribute &incRampB,
	const	MRampAttribute &incRampC

	){



	const MIntArray & right = sortedId;

	// cerr << "-------------- HexapodColony::update --------------" << endl;
// int lPeg = 0;
	int rPeg = 0;

	std::list<HexapodAgent*>::iterator shadow;
	std::list<HexapodAgent*>::iterator agent = m_agents.begin();
	while (true) {
		if (agent == m_agents.end() && rPeg == right.length()) {
// finished
			// cerr << "m_agents ended && right ended - so break" << endl;
			break;
		}

if (rPeg == right.length()) { // finished adding right stuff. delete the rest of left.
	// cerr << "right ended only - so delete current left: id=" << (*agent)->id() << endl;

	shadow = agent; 
	agent++;
	delete *shadow;
	*shadow = 0;
	m_agents.erase(shadow);

// remove shadow;
} else if (agent == m_agents.end()) {
	unsigned index = idIndex[rPeg];
	// cerr << "left ended only - so push new right: id=" << particleId[index] << endl;
// insert new agent with id right[rPeg] before end
	m_agents.push_back(new HexapodAgent( 
		particleId[index],
		pos[index],
		phi[index], 
		vel[index],
		omega[index],
		scale[index],
		homeAX, homeAZ, radiusMinA,  radiusMaxA,
		homeBX, homeBZ, radiusMinB,  radiusMaxB,
		homeCX, homeCZ, radiusMinC,  radiusMaxC
		));
	rPeg++;
} else if (right[rPeg] < (*agent)->id()) {
	unsigned index = idIndex[rPeg];
	// cerr << "right position less than left so insert new right: id=" << particleId[idIndex[rPeg]] << endl;

	m_agents.insert(agent, new HexapodAgent(   			
		particleId[index],
		pos[index],
		phi[index], 
		vel[index],
		omega[index],
		scale[index],
		homeAX, homeAZ, radiusMinA,  radiusMaxA,
		homeBX, homeBZ, radiusMinB,  radiusMaxB,
		homeCX, homeCZ, radiusMinC,  radiusMaxC
		));

// insert new agent with id right[rPeg] before (*agent)
	rPeg++;
} else if (right[rPeg]  == (*agent)->id()) {
	unsigned index = idIndex[rPeg];
	// cerr << "right position equal to left so update left with right: id=" << particleId[idIndex[rPeg]] << endl;

	(*agent)->update( 
		dt, 
		pos[index],
		phi[index], 
		vel[index],
		omega[index],
		scale[index],
		homeAX, homeAZ, radiusMinA,  radiusMaxA,
		homeBX, homeBZ, radiusMinB,  radiusMaxB,
		homeCX, homeCZ, radiusMinC,  radiusMaxC
		);
// update (*agent)
	rPeg++;
	agent++;
} else { // right[rPeg] > (*agent)->id()
	// cerr << "right position greater than left so remove left: id=" << (*agent)->id() << endl;

	shadow = agent; 
	agent++;
	delete *shadow;
	*shadow = 0;
	m_agents.erase(shadow);
// remove shadow;
}
}
return MS::kSuccess;
}


HexapodColony::~HexapodColony(){
	clear();
}


void HexapodColony::draw(
	M3dView & view, const DisplayMask & mask	) const {

	view.beginGL(); 
	glPushAttrib(GL_CURRENT_BIT);

	std::list<HexapodAgent*>::const_iterator agent = m_agents.begin();
	double d44[4][4];
	while (agent != m_agents.end()) {
		(*agent)->draw(view,mask);
		agent++;
	}

	glPopAttrib();
	view.endGL();	
}



// id mapping - 
//////////////////////// IDS ////////////////////////
// i         	:  0   1   2   3   4   5   6   7   8 //
// particleId	: 14   6   2  12  10   5  13   8  11 // 

// idIndex tells us the index in the id array 
// sortedId  	:  2   5   6   8  10  11  12  13  14 // 
// idIndex   	:  2   5   1   7   4   8   3   6   0 //




