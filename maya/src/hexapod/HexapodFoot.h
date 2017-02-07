#ifndef HexapodFoot_H
#define HexapodFoot_H

#include <maya/MObject.h>
 
 #include <maya/MVector.h>

class HexapodFoot{
	public:
		
		HexapodFoot();
		
		~HexapodFoot();
		
		// MStatus simulate(const MObject &node, MArrayDataHandle & ah, unsigned schoolSize);
		
 
	private:


		MVector m_home;
		double m_minRadius;
		double m_maxRadius;

		MVector m_footPosition;
		MVector m_lastPlant;
		MVector m_nextPlant;

 
};


#endif

