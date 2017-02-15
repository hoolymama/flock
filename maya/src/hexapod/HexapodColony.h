#ifndef HexapodColony_H
#define HexapodColony_H
#include <list>
#include <maya/MObject.h>
#include <maya/M3dView.h>
#include <maya/MRampAttribute.h>


#include "HexapodAgent.h"
#include "HexapodFoot.h"
#include "displayMask.h"

// typedef std::vector<HexapodAgent*> AGENT_VECTOR;


class HexapodColony{
public:

	HexapodColony();

	~HexapodColony();

	void clear();

	void draw( M3dView & view, const DisplayMask & mask ) const ;

	MStatus update(
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
		);

private:
	
	std::list<HexapodAgent*> m_agents;


};



#endif

