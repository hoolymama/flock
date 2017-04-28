#ifndef HexapodAgent_H
#define HexapodAgent_H


#include <vector>

#include <maya/MVector.h>
#include <maya/MMatrix.h>
#include <maya/M3dView.h>
#include <maya/MString.h>

#include <maya/MRampAttribute.h>
#include "HexapodFoot.h"
#include "mayaMath.h"
#include "displayMask.h"
#include "rankData.h"

#include  "actuator.h"
#include  "Ground.h"

class HexapodFoot;

class HexapodAgent {
public:

	// HexapodAgent(	
	// 	double dt,
	// 	unsigned particleId,
	// 	const MVector &pos,
	// 	const MVector &phi,
	// 	const MVector &vel,
	// 	const MVector &omega,
	// 	double scale,
	// 	double homeAX, double homeAZ, double radiusMinA, double radiusMaxA,
	// 	double homeBX, double homeBZ, double radiusMinB, double radiusMaxB,
	// 	double homeCX, double homeCZ, double radiusMinC, double radiusMaxC
	// 	);

	HexapodAgent(	
		double dt,
		unsigned particleId,
		const MVector &pos,
		const MVector &phi,
		const MVector &vel,
		const MVector &omega,
		double scale,
		rankData &rankA,

		rankData &rankB,

		rankData &rankC,
		const MVector &bodyOffset,
		double floorThickness,
		double bodyFootAverageBias

		);

	~HexapodAgent();

	void  buildMatrices(
		const MVector &pos,
		const MVector &phi,
		const MVector &vel,
		const MVector &omega,
		double scale,
		double dt, 	double floorThickness);

	void  applyActuator(	
		const HexapodFoot &leftFoot, 
		const HexapodFoot &rightFoot, 
		actuator &actuator);

	void  applyActuator( actuator &actuator);

	// void applyActuators( std::vector<actuator*> & actuatorStack ) ;

	void update(	
		double dt, double maxSpeed,
		const MVector &pos,
		const MVector &phi,
		const MVector &vel,
		const MVector &omega,
		double scale,
		rankData &rankA,
		rankData &rankB,
		rankData &rankC,
		const MVector &bodyOffset,
		double floorThickness,
		double bodyFootAverageBias,
		MRampAttribute &plantSpeedBiasRamp,
		MRampAttribute &anteriorRadiusRamp,
		MRampAttribute &lateralRadiusRamp,
		MRampAttribute &posteriorRadiusRamp,
		const MVector &leftFootFeed, 
		const MVector &rightFootFeed,
		double feedBlend,
		Ground & ground
		);

	void draw( M3dView & view, const DisplayMask & mask) const;

	void getOutputData(
		MVector& la, MVector& lb, MVector& lc, 
		MVector& ra, MVector& rb, MVector& rc, 
		MVector& position, MVector& phi, double &scale) const;

	const MVector &  position()  const ;
	int  id()  const;
	const MMatrix & matrix() const;
	const MMatrix & matrixInverse() const;

	const MMatrix & matrixNext() const;

	// MVector worldBodyPosition() const;

private:

	/* input particle data */
	int m_particleId;
	MVector m_position;
	MVector m_phi;
	MVector m_velocity;
	MVector m_omega;
	double m_scale;

	/* calc matrices of input data*/
	MMatrix m_matrix;
	MMatrix m_matrixInverse;

	/* 
	Position of rig body transform in relation to particle
	*/
	MVector m_bodyOffset;
	double m_bodyFootAverageBias;
	/* m_matrixNext is where the agent is likely to be in one frame */
	MMatrix m_matrixNext;


	HexapodFoot m_footLA;
	HexapodFoot m_footLB;
	HexapodFoot m_footLC;
	HexapodFoot m_footRA;
	HexapodFoot m_footRB;
	HexapodFoot m_footRC;


	/* local position and rotation (phi) of body, calculated by actuators*/
	MVector m_body_position;
	MVector m_body_rotation;

	MVector m_leftFeed;
	MVector m_rightFeed;
	double m_feedBlend;

};



#endif

