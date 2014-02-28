#ifndef clonerStack_H
#define clonerStack_H


#include <map>
#include <maya/MObject.h>
#include "meshCloner.h"

typedef std::map< int, meshCloner * > meshClonerMap ;


class clonerStack{
	public:
		
		clonerStack();
		
		~clonerStack();
		
		MStatus conform(const MObject &node, MArrayDataHandle & ah, unsigned schoolSize);
		
		void pushPoints();
			
		void prepareOutGeometry();
		
		void doOutput(MArrayDataBuilder & b) ;
		
	private:

		meshClonerMap m_clonerMap;	
		
		unsigned m_schoolSize;
		bool m_schoolSizeChanged;
};


#endif

