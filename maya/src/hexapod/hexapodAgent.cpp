
#include "HexapodAgent.h"
#include "HexapodFoot.h"


HexapodAgent::HexapodAgent(
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
	const MVector &bodyOffset

	): 
m_particleId(particleId), 
m_position(pos), 
m_phi(phi), 
m_velocity(vel), 
m_omega(omega), 
m_scale(scale),
m_bodyOffset(bodyOffset)
{

	buildMatrices(pos,phi,vel,omega, scale, dt);
	/* build the feet. Give them a pointer to this object 
	so they can use matrices to maintain the foot positions in world space
	*/
	m_footLA = HexapodFoot(rankA.homeX, rankA.homeZ, rankA.radiusMin,  rankA.radiusMax,  this);
	m_footLB = HexapodFoot(rankB.homeX, rankB.homeZ, rankB.radiusMin,  rankB.radiusMax,  this);
	m_footLC = HexapodFoot(rankC.homeX, rankC.homeZ, rankC.radiusMin,  rankC.radiusMax,  this);
	m_footRA = HexapodFoot(rankA.homeX, -rankA.homeZ, rankA.radiusMin,  rankA.radiusMax, this);
	m_footRB = HexapodFoot(rankB.homeX, -rankB.homeZ, rankB.radiusMin,  rankB.radiusMax, this);
	m_footRC = HexapodFoot(rankC.homeX, -rankC.homeZ, rankC.radiusMin,  rankC.radiusMax, this);

}

HexapodAgent::~HexapodAgent(){ }


void HexapodAgent::buildMatrices(
	const MVector &pos,
	const MVector &phi,
	const MVector &vel,
	const MVector &omega,
	double scale, double dt) 
{
	m_matrix = mayaMath::matFromPhi(pos, phi, MVector(scale,scale,scale));
	m_matrixInverse = m_matrix.inverse();
	m_matrixNext = mayaMath::matFromPhi(pos+(vel*dt), phi+(omega*dt), MVector(scale,scale,scale));
}

void HexapodAgent::update(
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
	MRampAttribute &plantSpeedBiasRamp,
	MRampAttribute &anteriorRadiusRamp,
	MRampAttribute &lateralRadiusRamp,
	MRampAttribute &posteriorRadiusRamp
	)
{
	m_position = pos;
	m_phi  = phi;
	m_velocity = vel;
	m_omega = omega;
	m_scale = scale;
	m_bodyOffset = bodyOffset;

	buildMatrices(pos,phi,vel,omega, scale, dt);

 	/* 
	A planted foot plans a new plant and transitions to a step when 
	it is found to be outside its constraining circle. 

	For any given foot, the size of the constraining circle is a 
	function of the stepParameter of the three neighboring feet:
	in-front: (Anterior)
	behind (Posterior)
	opposite side (lateral) 

	the sense of anterior and posterior wrap around. Meaning: 
	The foot in rank B has anterior=A posterior=C
	The foot in rank A has anterior=C posterior=B
	The foot in rank C has anterior=B posterior=A

	By modulating the constraining circles based on neighboring feet
	the agent tends to sync its steps in differnt phases front to back, 
	and it makes left and right alternate in normal walking.

	When the agent turns,stops, or moves backwards, the feet tend 
	to do what is necessary to move, while keeping most feet on
	the ground at any one time
 	*/
	m_footLA.updateHomeCircles(rankA, 
		m_footLC.stepParam(),m_footRA.stepParam(),m_footLB.stepParam(),
		anteriorRadiusRamp, lateralRadiusRamp, posteriorRadiusRamp);

	m_footLB.updateHomeCircles(rankB, 
		m_footLA.stepParam(),m_footRB.stepParam(),m_footLC.stepParam(),
		anteriorRadiusRamp, lateralRadiusRamp, posteriorRadiusRamp);

	m_footLC.updateHomeCircles(rankC, 
		m_footLB.stepParam(),m_footRC.stepParam(),m_footLA.stepParam(),
		anteriorRadiusRamp, lateralRadiusRamp, posteriorRadiusRamp);

	m_footRA.updateHomeCircles(rankA, 
		m_footRC.stepParam(),m_footLA.stepParam(),m_footRB.stepParam(),
		anteriorRadiusRamp, lateralRadiusRamp, posteriorRadiusRamp, true);

	m_footRB.updateHomeCircles(rankB, 
		m_footRA.stepParam(),m_footLB.stepParam(),m_footRC.stepParam(),
		anteriorRadiusRamp, lateralRadiusRamp, posteriorRadiusRamp, true);

	m_footRC.updateHomeCircles(rankC, 
		m_footRB.stepParam(),m_footLC.stepParam(),m_footRA.stepParam(),
		anteriorRadiusRamp, lateralRadiusRamp, posteriorRadiusRamp, true);

	m_footLA.update(dt, maxSpeed, rankA, plantSpeedBiasRamp);
	m_footLB.update(dt, maxSpeed, rankB, plantSpeedBiasRamp);
	m_footLC.update(dt, maxSpeed, rankC, plantSpeedBiasRamp);
	m_footRA.update(dt, maxSpeed, rankA, plantSpeedBiasRamp);
	m_footRB.update(dt, maxSpeed, rankB, plantSpeedBiasRamp);
	m_footRC.update(dt, maxSpeed, rankC, plantSpeedBiasRamp);
}
void HexapodAgent::getOutputData(
	MVector& la, MVector& lb, MVector& lc, 
	MVector& ra, MVector& rb, MVector& rc, 
	MVector& position, MVector& phi, double &scale) const 
{
	la = m_footLA.position();
	lb = m_footLB.position();
	lc = m_footLC.position();
	ra = m_footRA.position();
	rb = m_footRB.position();
	rc = m_footRC.position();
	position = worldBodyPosition();
	phi = m_phi;
	scale = m_scale;
}

MVector HexapodAgent::worldBodyPosition() const {
	return MVector(MPoint(m_bodyOffset) * m_matrix);
}


void HexapodAgent::draw(
	M3dView & view, const DisplayMask & mask 
	) const {
	double d44[4][4];
	m_matrix.get(d44);
	MFloatMatrix fmat(d44);

	m_footLA.draw(view, fmat, mask);
	m_footLB.draw(view, fmat, mask);
	m_footLC.draw(view, fmat, mask);
	m_footRA.draw(view, fmat, mask);
	m_footRB.draw(view, fmat, mask);
	m_footRC.draw(view, fmat, mask);

	if (mask.displayId) {
		MFloatVector pos(float(m_position.x), float(m_position.y),float(m_position.z));
		MString pid;
		pid += m_particleId;
		view.setDrawColor( MColor( MColor::kRGB, 0.0, 1.0,1.0 ) );
		view.drawText(pid, pos, M3dView::kLeft);		
	}
}


const MVector & HexapodAgent::position()  const {return m_position; } 
int HexapodAgent::id() const {return m_particleId; } 
const MMatrix & HexapodAgent::matrix() const {return m_matrix;}
const MMatrix & HexapodAgent::matrixInverse() const {return m_matrixInverse;}
const MMatrix & HexapodAgent::matrixNext() const {return m_matrixNext;}

