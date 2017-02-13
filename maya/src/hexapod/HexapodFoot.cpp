

#include <string.h>

#include <sstream>

#include <vector>
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

}

HexapodFoot::~HexapodFoot(){}

void HexapodFoot::setHome(
	double homeX, 
	double homeZ, 
	double radiusMin, 
	double radiusMax) 
{
 	m_homeX = homeX;
 	m_homeZ = homeZ;
 	m_minRadius = radiusMin;
 	m_maxRadius = radiusMax;
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

void HexapodFoot::draw(M3dView & view, MFloatMatrix & agentMatrix) const {
	
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
};


