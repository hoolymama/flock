#ifndef HexapodFoot_H
#define HexapodFoot_H

#include <maya/MObject.h>
#include <maya/M3dView.h>
#include <maya/MFloatMatrix.h>

#include <maya/MVector.h>
#include "displayMask.h"
#include "rankData.h"
#include "hexUtil.h"
#include "actuator.h"
#include  "Ground.h"
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
		const HexapodAgent* pAgent,
		hexUtil::Rank rank,
		hexUtil::Side side
		);
	HexapodFoot(	
		const rankData &rankData,
		const HexapodAgent * pAgent,
		hexUtil::Rank rank,
		hexUtil::Side side
		);



	~HexapodFoot();

	bool needsNewPlant(const MVector &localVelocity) const;

	MVector getLocalVelocity(double dt) const;

	MPoint planNextPlant( 
		double dt, 
		float increment, 
		const MVector localVelocity,
		float plantBias,
		 Ground & ground) const ;


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

	MVector actuatorValue(actuator& actuator) const;

	// void update(
	// 	double dt, double maxSpeed,
	// 	MRampAttribute &incRamp,
	// 	MRampAttribute &plantSpeedBiasRamp);

	void  cacheLocalFootPosition();

	MPoint calcFootPosition(rankData& rank) const;

	void update(
		double dt, double maxSpeed,
		rankData & rank,
		MRampAttribute &plantSpeedBiasRamp,
		 Ground & ground);


	void draw(M3dView & view, MFloatMatrix & matrix,  const DisplayMask & mask) const;

	void drawCircleAtHome(
		M3dView & view, 
		const MFloatMatrix & agentMatrix, 
		float radius,
		const MColor& color) const ;

	void  drawFootAndPlants( M3dView & view,  const DisplayMask & mask ) const ;

	void  drawDoubleValue(M3dView & view, double value) const;
	void  drawFootLocal(M3dView & view) const ;

	float stepParam() const;

	MVector position(hexUtil::Space space = hexUtil::kWorld) const;
private:
	
	double m_stepParam;

	/*
	position and plants are in world space
	*/
	MPoint m_footPosition;
	MPoint m_lastPlant;
	MPoint m_nextPlant;

	/* and we cache this for actuator calculations*/
	MPoint m_footPositionLocal;
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

	hexUtil::Rank m_rank;
	hexUtil::Side m_side;


};


#endif

