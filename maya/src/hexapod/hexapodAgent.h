#ifndef HexapodAgent_H
#define HexapodAgent_H

#include <maya/MVector.h>
#include <maya/MMatrix.h>
 

#include "HexapodFoot.h"
#include "mayaMath.h"

// typedef std::map< int, meshCloner * > meshClonerMap ;


class HexapodAgent{
public:

	HexapodAgent(	
		unsigned particleId,
		const MVector &pos,
		const MVector &phi,
		const MVector &vel,
		const MVector &omega,
		double scale);

	~HexapodAgent();
	
	void set(	
		const MVector &pos,
		const MVector &phi,
		const MVector &vel,
		const MVector &omega,
		double scale);

	const MVector &  position()  const ;
	int  id()  const;
	const MMatrix & matrix() const;

private:

	int m_particleId;
	MVector m_position;
	MVector m_phi;
	MVector m_velocity;
	MVector m_omega;
	double m_scale;

	MMatrix m_matrix;
	MMatrix m_matrixInverse;

};



#endif

