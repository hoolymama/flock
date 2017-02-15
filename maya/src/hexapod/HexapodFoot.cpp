

#include <string.h>

#include <sstream>

#include <vector>
#include <algorithm>
#include <math.h> 


#include <maya/MObject.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnIntArrayData.h>

#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>

#include <maya/MColor.h>
#include <maya/MIntArray.h>
#include <maya/MVectorArray.h>


#include "mayaMath.h"

#include "HexapodFoot.h"

const double  PI  = 3.141592653;
const double  TAU = 2.0 * PI;
const double  nearlyOne = 1.0 - 0.000001;

static int circleVertexCount = 16;
const float gap = TAU / circleVertexCount;

static float circle[][4] = { 
	{sin(gap * 0), 0.0f, cos(gap * 0),1.0f},
	{sin(gap * 1), 0.0f, cos(gap * 1),1.0f},
	{sin(gap * 2), 0.0f, cos(gap * 2),1.0f},
	{sin(gap * 3), 0.0f, cos(gap * 3),1.0f},
	{sin(gap * 4), 0.0f, cos(gap * 4),1.0f},
	{sin(gap * 5), 0.0f, cos(gap * 5),1.0f},
	{sin(gap * 6), 0.0f, cos(gap * 6),1.0f},
	{sin(gap * 7), 0.0f, cos(gap * 7),1.0f},
	{sin(gap * 8), 0.0f, cos(gap * 8),1.0f},
	{sin(gap * 9), 0.0f, cos(gap * 9),1.0f},
	{sin(gap * 10), 0.0f, cos(gap * 10),1.0f},
	{sin(gap * 11), 0.0f, cos(gap * 11),1.0f},
	{sin(gap * 12), 0.0f, cos(gap * 12),1.0f},
	{sin(gap * 13), 0.0f, cos(gap * 13),1.0f},
	{sin(gap * 14), 0.0f, cos(gap * 14),1.0f},
	{sin(gap * 15), 0.0f, cos(gap * 15),1.0f}
};

HexapodFoot::HexapodFoot(){}

HexapodFoot::HexapodFoot(	
	double homeX, 
	double homeZ, 
	double minRadius, 
	double maxRadius,
	const MMatrix & agentMatrix 
	):  
m_homeX(homeX),
m_homeZ(homeZ),
m_minRadius(minRadius),
m_maxRadius(maxRadius)
{

	m_footPosition = MPoint(homeX, 0, homeZ) * agentMatrix;
	m_lastPlant = m_footPosition;
	m_nextPlant = m_footPosition;



	m_stepParam = 1.0;
	m_radius = (maxRadius+minRadius)/2.0; 
// 	cerr << "CTOR: "   << endl;
// 	cerr << "m_minRadius: " << m_minRadius << endl;
// cerr << "m_maxRadius: " << m_maxRadius << endl;
// 	cerr << "m_radius: " << m_radius << endl;


}

HexapodFoot::~HexapodFoot(){}


bool HexapodFoot::footIsOutside(const MMatrix &agentMatrix, MPoint & nextPlant) const {

	double dmat[4][4] = {
		{m_radius,0.0,0.0,0.0},
		{0.0,m_radius,0.0,0.0},
		{0.0,0.0,m_radius,0.0},
		{ m_homeX,0.0,m_homeZ,1.0}
	};

	MMatrix homeRadiusMatrix = MMatrix(dmat) * agentMatrix;
	MPoint localFoot = m_footPosition  * homeRadiusMatrix.inverse();

 	/*
	pick a point on the radius in the direction opposite to where we went out.
 	*/
	double dist = sqrt(localFoot.x*localFoot.x)+(localFoot.z*localFoot.z);

	if  (dist >= 1) {
		MPoint localTarget =  localFoot / (-dist);
		nextPlant = localTarget * homeRadiusMatrix;
		return true;
	}
	return false;
}



void HexapodFoot::update(
	double dt,
	double homeX, 
	double homeZ, 
	double radiusMin, 
	double radiusMax,
	const MMatrix & agentMatrix,
	const MMatrix & agentMatrixInverse) 
{
	m_homeX = homeX;
	m_homeZ = homeZ;
	m_minRadius = radiusMin;
	m_maxRadius = radiusMax;

	m_radius = (m_maxRadius + m_minRadius) * 0.5;
	cerr << "m_radius: " << m_radius << endl;




		/* is foot in a step */
	if (m_stepParam < 1.0) {
		double tmp = m_stepParam + 0.25;
		m_stepParam = std::min(tmp, 1.0);
			/* advance the foot a bit*/
		m_footPosition = (m_lastPlant*(1.0 - m_stepParam)) +(m_nextPlant * m_stepParam);
	} else {
				/*
					not in step - so check if foot is outside circle (projected into ground plane)
				*/
		MPoint next;
		if (footIsOutside(agentMatrix, next)) {
			m_lastPlant = m_footPosition;
			m_nextPlant = next;
			m_stepParam = 0;
		}
	}



	  // m_footPosition = MPoint(m_homeX, 0, m_homeZ, 1.0) * agentMatrix;

 	// m_radius = (radiusMax+radiusMin)/2.0;
// 	cerr << "SET HOME: "   << endl;
//  		cerr << "m_minRadius: " << m_minRadius << endl;
// cerr << "m_maxRadius: " << m_maxRadius << endl;
// 	cerr << "m_radius: " << m_radius << endl;

}


void HexapodFoot::drawCircleAtHome(
	M3dView & view, 
	const MFloatMatrix & agentMatrix, 
	float radius,
	const MColor& color) const {

	float fmat[4][4] = {
		{radius,0.0f,0.0f,0.0f},
		{0.0f,radius,0.0f,0.0f},
		{0.0f,0.0f,radius,0.0f},
		{float(m_homeX),0.0f,float(m_homeZ),1.0f}
	};

	MFloatMatrix circleMatrix(fmat);
	circleMatrix = circleMatrix * agentMatrix;

	MFloatPointArray c(circleVertexCount);
	for (int i = 0; i < circleVertexCount; ++i) { 
		c[i] = MFloatPoint(circle[i]) * circleMatrix;
	}

	view.setDrawColor(color);
	glBegin( GL_LINES );
	for (int i = 0; i < circleVertexCount; ++i)
	{
		int next = (i+1)%circleVertexCount;
		glVertex3f( c[i].x , c[i].y , c[i].z );
		glVertex3f( c[next].x , c[next].y , c[next].z );
	}
	glEnd();
};


void HexapodFoot::drawFootAndPlants( M3dView & view,  const DisplayMask & mask ) const {

	glPushAttrib(GL_CURRENT_BIT);

	glPointSize(4);

	glBegin( GL_POINTS );
	if (mask.displayPlants){
		view.setDrawColor( MColor( MColor::kRGB, 0.0, 0.0, 0.0 ) );
		glVertex3d( m_lastPlant.x , m_lastPlant.y ,m_lastPlant.z );
		view.setDrawColor( MColor( MColor::kRGB, 1.0, 1.0, 1.0 ) );
		glVertex3d( m_nextPlant.x , m_nextPlant.y ,m_nextPlant.z );
	}
	if (mask.displayFootPosition){

		view.setDrawColor( MColor( MColor::kRGB, 0.0, 1.0,0.0 ) );
		glVertex3d( m_footPosition.x , m_footPosition.y ,m_footPosition.z );
	}
	glEnd();

	glPopAttrib();

}

void HexapodFoot::draw(M3dView & view, MFloatMatrix & agentMatrix,  const DisplayMask & mask) const {
	
	if (mask.displayHome){
		HexapodFoot::drawCircleAtHome(view, 
			agentMatrix, 
			float(m_minRadius), 
			MColor(0.0, 0.0, 1.0)
			);
		
		HexapodFoot::drawCircleAtHome(view, 
			agentMatrix, 
			float(m_maxRadius), 
			MColor(1.0, 0.0, 0.0)
			);

		
		HexapodFoot::drawCircleAtHome(view, 
			agentMatrix, 
			float(m_radius), 
			MColor(1.0, 1.0, 1.0)
			);
	}
		/* world space */
	HexapodFoot::drawFootAndPlants(view, mask);




};


