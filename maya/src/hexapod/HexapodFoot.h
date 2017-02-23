#ifndef HexapodFoot_H
#define HexapodFoot_H

#include <maya/MObject.h>
#include <maya/M3dView.h>
#include <maya/MFloatMatrix.h>

#include <maya/MVector.h>
#include "displayMask.h"
#include "rankData.h"


// #include "HexapodAgent.h"

class HexapodAgent;

class HexapodFoot{
public:

	HexapodFoot();

	HexapodFoot(	
		double homeX, 
		double homeZ, 
		double minRadius, 
		double maxRadius,
		const HexapodAgent* pAgent
		);



	~HexapodFoot();

	bool needsNewPlant(const MVector &localVelocity) const;

	MVector getLocalVelocity(double dt) const;

	MPoint planNextPlant( 
		double dt, 
		float increment, 
		const MVector localVelocity,
		float plantBias) const ;


void  updateHomeCircles(	
 	const rankData & rank,
	float anteriorStepParam,
	float lateralStepParam,
	float posteriorStepParam,
	MRampAttribute &anteriorRadiusRamp,
	MRampAttribute &lateralRadiusRamp,
	MRampAttribute &posteriorRadiusRamp,
	bool negateRank=false
	);

 

	// void update(
	// 	double dt, double maxSpeed,
	// 	MRampAttribute &incRamp,
	// 	MRampAttribute &plantSpeedBiasRamp);

MPoint calcFootPosition(rankData& rank) const;

	void update(
		double dt, double maxSpeed,
	  rankData & rank,
		MRampAttribute &plantSpeedBiasRamp);


	void draw(M3dView & view, MFloatMatrix & matrix,  const DisplayMask & mask) const;

	void drawCircleAtHome(
		M3dView & view, 
		const MFloatMatrix & agentMatrix, 
		float radius,
		const MColor& color) const ;

	void  drawFootAndPlants( M3dView & view,  const DisplayMask & mask ) const ;

	void  drawDoubleValue(M3dView & view, double value) const;

	float stepParam() const;

	MVector position() const;
private:
	
	double m_stepParam;

	/*
	position and plants are in world space
	*/
	MPoint m_footPosition;
	MPoint m_lastPlant;
	MPoint m_nextPlant;

	/*
	home is in local space
	*/
	double m_homeX;
	double m_homeZ;
	double m_minRadius;
	double m_maxRadius;
	double m_radius;
	double m_speed;
	const HexapodAgent* m_pAgent;

};


#endif

