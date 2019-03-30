

#include <string.h>

#include <sstream>

#include <vector>
#include <algorithm>
#include <math.h>


#include <maya/MObject.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnIntArrayData.h>

#include <maya/MRampAttribute.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>

#include <maya/MColor.h>
#include <maya/MIntArray.h>
#include <maya/MVectorArray.h>


#include "mayaMath.h"
#include "errorMacros.h"
#include "HexapodFoot.h"
#include "hexapodAgent.h"
#include "actuator.h"



const double  PI  = 3.141592653;
const double  TAU = 2.0 * PI;
const double  epsilon = 0.000001;

static int circleVertexCount = 16;
const float gap = TAU / circleVertexCount;

static float circle[][4] = {
	{sin(gap * 0), 0.0f, cos(gap * 0), 1.0f},
	{sin(gap * 1), 0.0f, cos(gap * 1), 1.0f},
	{sin(gap * 2), 0.0f, cos(gap * 2), 1.0f},
	{sin(gap * 3), 0.0f, cos(gap * 3), 1.0f},
	{sin(gap * 4), 0.0f, cos(gap * 4), 1.0f},
	{sin(gap * 5), 0.0f, cos(gap * 5), 1.0f},
	{sin(gap * 6), 0.0f, cos(gap * 6), 1.0f},
	{sin(gap * 7), 0.0f, cos(gap * 7), 1.0f},
	{sin(gap * 8), 0.0f, cos(gap * 8), 1.0f},
	{sin(gap * 9), 0.0f, cos(gap * 9), 1.0f},
	{sin(gap * 10), 0.0f, cos(gap * 10), 1.0f},
	{sin(gap * 11), 0.0f, cos(gap * 11), 1.0f},
	{sin(gap * 12), 0.0f, cos(gap * 12), 1.0f},
	{sin(gap * 13), 0.0f, cos(gap * 13), 1.0f},
	{sin(gap * 14), 0.0f, cos(gap * 14), 1.0f},
	{sin(gap * 15), 0.0f, cos(gap * 15), 1.0f}
};

HexapodFoot::HexapodFoot() {}

HexapodFoot::HexapodFoot(
  double homeX,
  double homeZ,
  double minRadius,
  double maxRadius,
  const HexapodAgent *pAgent,
  hexUtil::Rank rank,
  hexUtil::Side side

):
	m_homeX(homeX),
	m_homeZ(homeZ),
	m_minRadius(minRadius),
	m_maxRadius(maxRadius),
	m_speed(0),
	m_pAgent(pAgent),
	m_rank(rank),
	m_side(side)
{

	/*
	We arbitrarily start with:
		Both plants and the foot together
		StepParam of 1.0 which means planted.
		Radius halfway between min and max.
	*/


	m_footPosition = MPoint(homeX, 0, homeZ) * pAgent->matrix();
	m_lastPlant = m_footPosition;
	m_nextPlant = m_footPosition;
	m_stepParam = 1.0;
	m_radius = (maxRadius + minRadius) / 2.0;

	m_footPositionLocal = MPoint::origin;
}


HexapodFoot::HexapodFoot(
  const rankData &rankData,
  const HexapodAgent *pAgent,
  hexUtil::Rank rank,
  hexUtil::Side side
):
	m_speed(0),
	m_pAgent(pAgent),
	m_rank(rank),
	m_side(side)
{

	/*
	We arbitrarily start with:
		Both plants and the foot together
		StepParam of 1.0 which means planted.
		Radius halfway between min and max.
	*/
	m_homeX = rankData.homeX;
	m_homeZ = (m_side == hexUtil::kLeft) ? rankData.homeZ : -rankData.homeZ;
	m_minRadius = rankData.radiusMin;
	m_maxRadius = rankData.radiusMax;

	m_footPosition = MPoint(m_homeX, 0, m_homeZ) * pAgent->matrix();
	m_lastPlant = m_footPosition;
	m_nextPlant = m_footPosition;
	m_stepParam = 1.0;
	m_radius = (m_maxRadius + m_minRadius) / 2.0;

	m_footPositionLocal = MPoint::origin;
}



HexapodFoot::~HexapodFoot() {}

MVector HexapodFoot::actuatorValue(actuator &actuator) const
{
	float inputValue;
	hexUtil::ActuatorAxis axis = actuator.axis();
	switch ( axis ) {
		case  hexUtil::kX:
			inputValue = float(m_footPositionLocal.x);
			break;

		case  hexUtil::kY:
			inputValue = float(m_footPositionLocal.y);
			break;

		case  hexUtil::kZ:
			inputValue = float(m_footPositionLocal.z);
			break;

		default: //hexUtil::kStepParam:
			inputValue = float(m_stepParam);
			break;
	}

	return actuator.lookup(inputValue, m_side);

}

float HexapodFoot::stepParam() const { return float(m_stepParam);}

MVector HexapodFoot::position(hexUtil::Space space) const
{
	if (space == hexUtil::kLocal) {
		return MVector(m_footPositionLocal);
	}

	return MVector(m_footPosition);
}

/*
Determine whether the foot needs to find a new plant.

We construct a matrix that represents the
local space of the constraining circle and we bring the foot into
that space and test if it is outside the circle. If so it maybe
that we need to plan a new plant.

However, if the home point is moving towards the planted foot,
then in theory we can just wait until it comes to us, and
therefore skip a re-plant.

We use a dot product between the foot and the velocity vector,
and really this is a bit of a rough heuristic.
To know whether the foot will actually be in the radius in the future
we should test the sweep of the circle along the velocity.
*/

bool HexapodFoot::needsNewPlant(const MVector &localVelocity) const {

	double dmat[4][4] = {
		{m_radius, 0.0, 0.0, 0.0},
		{0.0, m_radius, 0.0, 0.0},
		{0.0, 0.0, m_radius, 0.0},
		{ m_homeX, 0.0, m_homeZ, 1.0}
	};

	MMatrix homeRadiusMatrix = MMatrix(dmat) * m_pAgent->matrix();
	MPoint localFoot = m_footPosition  * homeRadiusMatrix.inverse();

	bool outside = ((localFoot.x * localFoot.x) + (localFoot.z * localFoot.z) >= 1);
	if (!outside) { return false; }
	return (( MVector(localFoot) * localVelocity ) < 0) ;
}

/*
We use the agents matrix and matrixNext to figure out where
this foot's home will be at dt in the future. We then calc
the velocity from that
*/
MVector HexapodFoot::getLocalVelocity(double dt) const {
	MStatus st;
	MPoint  localHome(m_homeX, 0, m_homeZ);
	MMatrix localNextMatrix = m_pAgent->matrixNext() *  m_pAgent->matrixInverse();
	MPoint localNext = localHome * localNextMatrix;
	localNext.y = 0;
	return MVector(localNext - localHome) / dt;
}

/*
We make a guess about where the home point will be in the future
at the time when the foot will land. Then, using the plant bias,
we set the plant to be either at the home, or on the far edge of the circle,
or at some point between. Plant bias is based on speed
*/
MPoint HexapodFoot::planNextPlant(
  double dt,
  float increment,
  const MVector localVelocity,
  float plantBias,
  Ground &ground) const
{

	// cerr << "planning plant" << endl;
	double timeToPlant = dt / increment; /* time-to-plant in seconds */
	MPoint result =  MPoint(m_homeX, 0,
	                        m_homeZ) +  (localVelocity * timeToPlant); /* local home at plant time*/
	if (plantBias > 0.0) {
		result += localVelocity.normal() * (m_radius *
		                                    plantBias); /* put plant on other side of radius*/
	}

	MMatrix agentMatrix =  m_pAgent->matrix();
	result = result * agentMatrix; /* put plant position in world space*/
	result = ground.project(result, agentMatrix);



	return result;
}

/*
Update the inner and outer radius.
Then set the actual circle radius at some point inbetween.
The circle radius is a function of the stepParams of
three adjacent feet. Specifically, the sum of the
output values from three lookup curves where the stepParams
are the inputs.

This mechanism, by adjusting the circle radius, has the effect
of changing the probability that the foot will want to plan a
new plant on this frame. If many of the other feet are in mid
step, this foot will almost definitely stay on the ground.
*/



void HexapodFoot::updateHomeCircles(
  const rankData &rank,
  float anteriorStepParam,
  float lateralStepParam,
  float posteriorStepParam,
  MRampAttribute &anteriorRadiusRamp,
  MRampAttribute &lateralRadiusRamp,
  MRampAttribute &posteriorRadiusRamp,
  bool negateRank
)
{
	MStatus st;

	m_homeX = rank.homeX;
	m_homeZ = negateRank ? -(rank.homeZ) : rank.homeZ;
	m_minRadius = rank.radiusMin;
	m_maxRadius = rank.radiusMax;

	float anterior, lateral, posterior;
	anteriorRadiusRamp.getValueAtPosition(  anteriorStepParam, anterior, &st ); mser;
	lateralRadiusRamp.getValueAtPosition(  lateralStepParam, lateral, &st ); mser;
	posteriorRadiusRamp.getValueAtPosition(  posteriorStepParam, posterior, &st ); mser;

	float total = anterior + lateral + posterior;
	m_radius = m_minRadius + (double(total) * (m_maxRadius - m_minRadius));
}


/*
run the update - effectively a sim step
If the stepParam is less than 1.0, then the foot is in mid step.
In that case we add some increment to the stepParam, and move the
foot accrordingly towards the planned plant position.

If step param is at 1.0, then the foot is stationary on the ground.
We check to see if it needs to be repositioned, and if so we plan
a new plant and launch into a step. If not, then it stays where
it is for another frame.
// */
// void HexapodFoot::update(
// 	double dt, double maxSpeed,
// 	MRampAttribute &incRamp,
// 	MRampAttribute &plantSpeedBiasRamp
// 	)
// {

// 	MStatus st;

// 	/* prep velocity, increment, speed */
// 	MVector localVelocity = getLocalVelocity(dt);
// 	m_speed = localVelocity.length();
// 	float normalizedSpeed = float(m_speed / maxSpeed);
// 	float increment;
// 	incRamp.getValueAtPosition( normalizedSpeed, increment, &st ); mser;


// 		/* is foot in a step */
// 	if (m_stepParam < (1.0-epsilon)) {
// 		/* advance the foot a bit*/
// 		m_stepParam = std::min((m_stepParam + increment), 1.0);
// 		m_footPosition = (m_lastPlant*(1.0 - m_stepParam)) +(m_nextPlant * m_stepParam);
// 	} else {
// 		/* not in a step - so check if a plant is needed */
// 		if (needsNewPlant(localVelocity)) {
// 			float  plantBias;
// 			plantSpeedBiasRamp.getValueAtPosition( normalizedSpeed, plantBias, &st ); mser;
// 			m_lastPlant = m_footPosition;
// 			m_nextPlant = planNextPlant(dt, increment, localVelocity, plantBias);
// 			m_stepParam = 0;
// 		}
// 		// else nothing - leave it where it is
// 	}
// }

/*

*/
MPoint HexapodFoot::calcFootPosition(rankData &rank) const {
	MStatus st;
	float height;
	rank.liftProfileRamp.getValueAtPosition( float(m_stepParam), height, &st ); mser;

	float slide;
	rank.slideProfileRamp.getValueAtPosition( float(m_stepParam), slide, &st ); mser;

	MVector yOffset = MVector(0, height, 0) * m_pAgent->matrix();

	return MPoint((m_lastPlant * (1.0 - slide)) + (m_nextPlant * slide)) + yOffset;
}




void HexapodFoot::cacheLocalFootPosition() {

	double homeMat[4][4] = {
		{1, 0.0, 0.0, 0.0},
		{0.0, 1, 0.0, 0.0},
		{0.0, 0.0, 1, 0.0},
		{ m_homeX, 0.0, m_homeZ, 1.0}
	};

	MMatrix home = MMatrix(homeMat) * m_pAgent->matrix();

	m_footPositionLocal = m_footPosition * home.inverse();

}

void HexapodFoot::update(
  double dt, double maxSpeed,
  rankData &rank,
  MRampAttribute &plantSpeedBiasRamp,
  Ground &ground
)
{

	MStatus st;

	/* prep velocity, increment, speed */
	MVector localVelocity = getLocalVelocity(dt);
	m_speed = localVelocity.length();
	float normalizedSpeed = float(m_speed / maxSpeed);
	float increment;

	// 	MRampAttribute stepIncrementRamp;
	// MRampAttribute slideProfileRamp;
	// MRampAttribute liftProfileRamp;

	rank.stepIncrementRamp.getValueAtPosition( normalizedSpeed, increment, &st ); mser;


	/* is foot in a step */
	if (m_stepParam < (1.0 - epsilon)) {
		/* advance the foot a bit*/
		m_stepParam = std::min((m_stepParam + increment), 1.0);
		m_footPosition = calcFootPosition(rank);
	}
	else {
		/* not in a step - so check if a plant is needed */
		if (needsNewPlant(localVelocity)) {
			float  plantBias;
			plantSpeedBiasRamp.getValueAtPosition( normalizedSpeed, plantBias, &st ); mser;
			m_lastPlant = m_footPosition;
			// m_lastPlant = m_nextPlant;
			m_nextPlant = planNextPlant(dt, increment, localVelocity, plantBias, ground);
			m_stepParam = 0;
		}



		/* Note - this will not give desired
		results when there is a mesh.

		It is only here for snapping the feet to the home plane.
		 */


		if (ground.isAnimated()) {
			MPoint localPos = m_footPosition * m_pAgent->matrixInverse();
			localPos.y = 0;
			m_footPosition = localPos * m_pAgent->matrix();
		}
		// snap to home rig
		// else nothing - leave it where it is

	}

	cacheLocalFootPosition();
	/* need to set the position of the foot in the space of its own home */





}

// draw functions
void HexapodFoot::drawCircleAtHome(
  M3dView &view,
  const MFloatMatrix &agentMatrix,
  float radius,
  const MColor &color) const {

	float fmat[4][4] = {
		{radius, 0.0f, 0.0f, 0.0f},
		{0.0f, radius, 0.0f, 0.0f},
		{0.0f, 0.0f, radius, 0.0f},
		{float(m_homeX), 0.0f, float(m_homeZ), 1.0f}
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
		int next = (i + 1) % circleVertexCount;
		glVertex3f( c[i].x , c[i].y , c[i].z );
		glVertex3f( c[next].x , c[next].y , c[next].z );
	}
	glEnd();
};


void HexapodFoot::drawFootAndPlants( M3dView &view,  const DisplayMask &mask ) const {

	glPushAttrib(GL_CURRENT_BIT);

	glPointSize(4);

	glBegin( GL_POINTS );
	if (mask.displayPlants) {
		view.setDrawColor( MColor( MColor::kRGB, 0.0, 0.0, 0.0 ) );
		glVertex3d( m_lastPlant.x , m_lastPlant.y , m_lastPlant.z );
		view.setDrawColor( MColor( MColor::kRGB, 1.0, 1.0, 1.0 ) );
		glVertex3d( m_nextPlant.x , m_nextPlant.y , m_nextPlant.z );
	}
	if (mask.displayFootPosition) {

		view.setDrawColor( MColor( MColor::kRGB, 0.0, 1.0, 0.0 ) );
		glVertex3d( m_footPosition.x , m_footPosition.y , m_footPosition.z );
	}
	glEnd();

	glPopAttrib();

}

void HexapodFoot::drawDoubleValue(M3dView &view, double value) const {
	MPoint dpos = MPoint(m_homeX, 0, m_homeZ) * m_pAgent->matrix();
	MFloatVector pos(float(dpos.x), float(dpos.y) , float(dpos.z));

	MString val;
	val.set (value, 4);
	view.setDrawColor( MColor( MColor::kRGB, 1.0, 0.5, 0.0 ) );
	view.drawText(val, pos, M3dView::kLeft);

}

void HexapodFoot::drawFootLocal(M3dView &view) const {
	MPoint dpos = MPoint(m_homeX, 0, m_homeZ) * m_pAgent->matrix();
	MFloatVector pos(float(dpos.x), float(dpos.y) , float(dpos.z));

	MString result("X:");

	MString val;
	val.set (m_footPositionLocal.x, 4); result += val;	result += "\nY:";
	val.set (m_footPositionLocal.y, 4); result += val;	result += "\nZ:";
	val.set (m_footPositionLocal.z, 4); result += val;	result += "\nP:";
	val.set (m_stepParam, 4); result += val;

	view.setDrawColor( MColor( MColor::kRGB, 1.0, 1.0, 1.0 ) );
	view.drawText(result, pos, M3dView::kLeft);

}




void HexapodFoot::draw(M3dView &view,
                       MFloatMatrix &agentMatrix,
                       const DisplayMask &mask) const {

	if (mask.displayHome) {
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

	if (mask.displaySpeed) {
		HexapodFoot::drawDoubleValue(view, m_speed);
	}

	if (mask.displayFootLocal) {
		HexapodFoot::drawFootLocal(view);
	}


};


