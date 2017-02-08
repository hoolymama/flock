#ifndef HexapodAgent_H
#define HexapodAgent_H

#include <maya/MObject.h>


#include "HexapodFoot.h"


// typedef std::map< int, meshCloner * > meshClonerMap ;


class HexapodAgent{
public:

	HexapodAgent(const MVector &pos, int particleId);

	~HexapodAgent();
void update(const MVector & position);

	const MVector &  position()  const ;
	int  id()  const;


private:


	int m_particleId;
	MVector m_position;

};



#endif

