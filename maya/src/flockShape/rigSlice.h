#ifndef rigSlice_H
#define rigSlice_H


#include <map>
#include "rigParticle.h"

#include <maya/MDataBlock.h>

#include "mayaMath.h"

class rigSlice{
	public:
		
		rigSlice();
		
		rigSlice( MObject & node, MDataBlock & data);
	
		~rigSlice();

		const particleStack & stack() const;
		
		int plugGaps(const rigSlice & src);
		
		void info() const;
	
		const unsigned & userValuesCount() const;
		const unsigned & userVectorValuesCount() const ;


		MStatus fortify(const rigSlice & target);	
	private:

		unsigned m_nValues;
		unsigned m_nVectorValues;
		particleStack m_stack;
		
};

typedef std::map< float, rigSlice * > rigSliceMap ;


#endif

