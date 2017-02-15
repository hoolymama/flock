#ifndef HexapodFoot_H
#define HexapodFoot_H

#include <maya/MObject.h>
#include <maya/M3dView.h>
#include <maya/MFloatMatrix.h>

#include <maya/MVector.h>
#include "displayMask.h"
class HexapodFoot{
public:

	HexapodFoot();

	HexapodFoot(	
		double homeX, 
		double homeZ, 
		double minRadius, 
		double maxRadius,
		const MMatrix & agentMatrix 
		);



	~HexapodFoot();
 
	bool  footIsOutside(const MMatrix &agentMatrix, MPoint & nextPlant) const ;


	void update(
		double dt, 
		double homeX, 
		double homeZ, 
		double radiusMin, 
		double radiusMax,
		const MMatrix & agentMatrix,
		const MMatrix & agentMatrixInverse);

	void draw(M3dView & view, MFloatMatrix & matrix,  const DisplayMask & mask) const;

	void drawCircleAtHome(
		M3dView & view, 
		const MFloatMatrix & agentMatrix, 
		float radius,
		const MColor& color) const ;

	void  drawFootAndPlants( M3dView & view,  const DisplayMask & mask ) const ;
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



};


#endif

