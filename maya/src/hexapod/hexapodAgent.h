#ifndef HexapodAgent_H
#define HexapodAgent_H

#include <maya/MVector.h>
#include <maya/MMatrix.h>
#include <maya/M3dView.h>
#include <maya/MString.h>
#include "HexapodFoot.h"
#include "mayaMath.h"
#include "displayMask.h"
// typedef std::map< int, meshCloner * > meshClonerMap ;


class HexapodAgent{
public:

	HexapodAgent(	
		unsigned particleId,
		const MVector &pos,
		const MVector &phi,
		const MVector &vel,
		const MVector &omega,
		double scale,
		double homeAX, double homeAZ, double radiusMinA, double radiusMaxA,
		double homeBX, double homeBZ, double radiusMinB, double radiusMaxB,
		double homeCX, double homeCZ, double radiusMinC, double radiusMaxC
		);

	~HexapodAgent();
	
	void update(	
		double dt,
		const MVector &pos,
		const MVector &phi,
		const MVector &vel,
		const MVector &omega,
		double scale,
		double homeAX,double  homeAZ,double  radiusMinA,double   radiusMaxA,
		double homeBX,double  homeBZ,double  radiusMinB,double   radiusMaxB,
		double homeCX,double  homeCZ,double  radiusMinC,double   radiusMaxC
		);

	void draw( M3dView & view, const DisplayMask & mask) const;


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

	HexapodFoot m_footLA;
	HexapodFoot m_footLB;
	HexapodFoot m_footLC;
	HexapodFoot m_footRA;
	HexapodFoot m_footRB;
	HexapodFoot m_footRC;



};



#endif

