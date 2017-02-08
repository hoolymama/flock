#ifndef HexapodColony_H
#define HexapodColony_H
#include <list>

#include <maya/MObject.h>

#include "HexapodAgent.h"

// typedef std::vector<HexapodAgent*> AGENT_VECTOR;


class HexapodColony{
public:

	HexapodColony();

	~HexapodColony();

	void clear();
	MStatus update(
		const MIntArray & particleId, 
		const MIntArray & sortedId, 
		const MIntArray & idIndex,
		const MVectorArray & pos,
		const MVectorArray & phi, 
		const MVectorArray & vel,
		const MVectorArray & omega,
		const MDoubleArray & scale
		);

private:
	
	std::list<HexapodAgent*> m_agents;
	MIntArray m_sortedIds ;

};



#endif

