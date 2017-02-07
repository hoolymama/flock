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
		
 
	private:

		MMatrix m_matrix;
		MMatrix m_matrixInverse;

		;
		;
		;
		;
		;
		;


		// meshClonerMap m_clonerMap;	
		
		// unsigned m_schoolSize;
		// bool m_schoolSizeChanged;
};


#endif

