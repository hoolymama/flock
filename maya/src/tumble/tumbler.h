/*
*  tumbler.h
*  jtools
*
*  Created by Julian Mann on 02/12/2006.
*  Copyright 2006 hooly|mama. All rights reserved.
*
*/
// tumbler - simulation of rolling particles
// torques are calculated for the particle based on sampling the gradient of forces near the particle.
// The resulting rotation is sent back to the particle object.

// Particles can also have a goal orientation, and will strive to 
// match that orientation by the shortest angular path according to goal weights 
//
#ifndef _tumbler
#define _tumbler


#include <maya/MVectorArray.h> 
#include <maya/MDoubleArray.h> 
#include <maya/MEulerRotation.h>
#include <maya/MTime.h> 
#include "mayaMath.h"
#include <maya/MTypeId.h> 

#define ROTATE_ORDER_XYZ	0
#define ROTATE_ORDER_YZX	1
#define ROTATE_ORDER_ZXY	2
#define ROTATE_ORDER_XZY	3
#define ROTATE_ORDER_YXZ	4
#define ROTATE_ORDER_ZYX	5
// #define OUTPUT_RADIANS	0
// #define OUTPUT_DEGREES	1

#define ROT_TO_DEG  57.295779524


class tumbler: public MPxNode
{

public:
	tumbler() ;

	virtual ~tumbler();

	static void		*creator();
	static MStatus	initialize();

	// will compute output force.
	virtual MStatus	compute( const MPlug& plug, MDataBlock& data );

	static MTypeId	id;

	// inputs
	static MObject	aPosition;
	static MObject	aVelocity;
	static MObject	aMass;
	static MObject  aImpulse;
	static MObject  aLocalImpulse;

	// static MObject  aCollisionImpulse;
	// static MObject  aCollisionGoalRange ;
	// static MObject  aCollisionGoalWeightMap ;
	// static MObject  aImpulseGoalRange ;
	// static MObject  aImpulseGoalWeightMap;
	// static MObject  aVelocityGoalRange ;
	// static MObject  aVelocityGoalWeightMap;
	
	static MObject aFrontAxis;	
	static MObject aUpAxis;		


	static MObject aGoalFront;	
	static MObject aGoalUp;
	static MObject aGoalWeight;	
	static MObject aGoalWeightPP;	// pp goalweight

	static MObject	aPhi; // current rotation
	static MObject	aOmega;	// angular velocity


	//static MObject  aSphereSize;	// force collection
	//static MObject	aTorqueMag; // mult
	
	static MObject	aTorqueConserve; 
	static MObject	aRotateOrder;
	static MObject	aOutputUnit;

	static MObject	aOutPhi; 	
	static MObject	aOutPoints;
	static MObject	aOutOmega; 	
	static MObject	aOutFront;
	static MObject	aOutUp;		
	static MObject	aOutRotation;

	static MObject  aCurrentTime;

	static MObject	aOutRotationData;


private:
	
	MTime m_lastTimeIEvaluated;
	
	MStatus getAppliedForces(
		MDataBlock& block,
		const MVectorArray &positions,
		const MVectorArray &velocities,
		const MDoubleArray &densities,
		const MTime &dT,
		MVectorArray &appliedForce
		);

	MStatus simStep(
		double dt,
		MDataBlock &data, 
		const MVectorArray &phi,
		const MVectorArray &omega,
		const MVectorArray &goalFront,
		const MVectorArray &goalUp,
		const MDoubleArray &goalWeightPP,
		const MVectorArray &impulse,
		// const MVectorArray &collisionImpulse,
		const MVectorArray &points,
		const MVectorArray &velocities,
		MVectorArray &outPhi,
		MVectorArray &outOmega,
		MVectorArray &outFront,
		MVectorArray &outUp,
		MVectorArray &outRotation
		);
		


};


// inline MVector calcRotation(const MVector &phi,MEulerRotation::RotationOrder ord, short outUnit){
// 	double len = phi.length();
// 	MQuaternion q;
// 	q.setAxisAngle(phi, len);
// 	MEulerRotation euler(MVector::zero, ord);
// 	euler = q;
// 	if (outUnit == OUTPUT_DEGREES){
// 		return (euler.asVector() * ROT_TO_DEG);
// 	}
// 	return euler.asVector() ;
// }

#endif
