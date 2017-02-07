#ifndef HexapodAgent_H
#define HexapodAgent_H

#include <maya/MObject.h>


 #include "HexapodFoot.h"


// typedef std::map< int, meshCloner * > meshClonerMap ;


class HexapodAgent{
public:

	HexapodAgent(const MVector &pos, const MVector & phi, double scale);

	~HexapodAgent();

		// MStatus simulate(const MObject &node, MArrayDataHandle & ah, unsigned schoolSize);


	MStatus createFootLA(
		const MVector & position,
		const MVector & homePosition,
		const MVector & lastPlant,
		const MVector & nextPlant,
		float stepTime,
		float radiusMin,
		float radiusMax
		);

private:

	MMatrix m_matrix;
	MMatrix m_matrixInverse;

	MVector m_position;
	MVector m_homePosit;
	MVector m_lastPlant;
	MVector m_nextPlant;
	float m_radiusMin;
	float m_radiusMax;

		// meshClonerMap m_clonerMap;	

		// unsigned m_schoolSize;
		// bool m_schoolSizeChanged;
};


#endif

