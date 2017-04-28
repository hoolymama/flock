
#include "hexapodAgent.h"
#include "HexapodFoot.h"


#include <maya/MEulerRotation.h>

#include "hexUtil.h"


const double epsilon = 0.00000001;


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
	const MVector &bodyOffset,
	double floorThickness,
	double bodyFootAverageBias

	): 
m_particleId(particleId), 
m_phi(phi), 
m_velocity(vel), 
m_omega(omega), 
m_scale(scale),
m_bodyOffset(bodyOffset),
m_bodyFootAverageBias(bodyFootAverageBias),
m_leftFeed(MVector::zero),
m_rightFeed(MVector::zero),
m_feedBlend(0)
{

	buildMatrices(pos,phi,vel,omega, scale, dt, floorThickness);
	/* build the feet. Give them a pointer to this object 
	so they can use matrices to maintain the foot positions in world space
	*/
	m_footLA = HexapodFoot(rankA.homeX, rankA.homeZ, rankA.radiusMin,  
		rankA.radiusMax,  this , hexUtil::kAnterior, hexUtil::kLeft);
	
	m_footLB = HexapodFoot(rankB.homeX, rankB.homeZ, rankB.radiusMin,  
		rankB.radiusMax,  this , hexUtil::kMedial, hexUtil::kLeft);
	
	m_footLC = HexapodFoot(rankC.homeX, rankC.homeZ, rankC.radiusMin,  
		rankC.radiusMax,  this , hexUtil::kPosterior, hexUtil::kLeft);
	
	m_footRA = HexapodFoot(rankA.homeX, -rankA.homeZ, rankA.radiusMin, 
		rankA.radiusMax, this, hexUtil::kAnterior, hexUtil::kRight);
	
	m_footRB = HexapodFoot(rankB.homeX, -rankB.homeZ, rankB.radiusMin, 
		rankB.radiusMax, this, hexUtil::kMedial, hexUtil::kRight);
	
	m_footRC = HexapodFoot(rankC.homeX, -rankC.homeZ, rankC.radiusMin, 
		rankC.radiusMax, this, hexUtil::kPosterior, hexUtil::kRight);


}

HexapodAgent::~HexapodAgent(){ }


void HexapodAgent::buildMatrices(
	const MVector &pos,
	const MVector &phi,
	const MVector &vel,
	const MVector &omega,
	double scale, double dt, 	double floorThickness) 
{

	MVector offset = (MVector::yNegAxis * floorThickness);
	MVector p = pos;
	if (! phi.isEquivalent(MVector::zero)) {
		double theta = phi.length();
		MVector n = phi.normal();
		MQuaternion q(theta,n);
		offset = offset.rotateBy(q);
	}  
	m_position = pos + offset;
	m_matrix = mayaMath::matFromPhi(m_position, phi, MVector(scale,scale,scale) );
	m_matrixInverse = m_matrix.inverse();
	m_matrixNext = mayaMath::matFromPhi(pos+(vel*dt), phi+(omega*dt), MVector(scale,scale,scale));
}









// void HexapodAgent::applyActuator (
// 	const HexapodFoot &leftFoot, 
// 	const HexapodFoot &rightFoot, 
// 	 actuator &actuator)
// {
// 	float inputValue;
// 	MVector v = leftFoot.actuatorValue(actuator);
// 	v += rightFoot.actuatorValue(actuator);

// 	hexUtil::ActuatorChannel channel = actuator.channel();
// 	if (channel ==  hexUtil::kTX ||  channel  ==  hexUtil::kTY ||  channel  ==  hexUtil::kTZ ) {
// 		m_body_position += v;
// 	} else {
// 		m_body_phi += v;
// 	}
// }




void HexapodAgent::applyActuator (
	const HexapodFoot &leftFoot, 
	const HexapodFoot &rightFoot,
	actuator &actuator)
{

	MVector v = leftFoot.actuatorValue(actuator);
	v += rightFoot.actuatorValue(actuator);


	hexUtil::ActuatorChannel channel = actuator.channel();
	if (
		channel ==  hexUtil::kTX ||  
		channel  ==  hexUtil::kTY ||  
		channel  ==  hexUtil::kTZ ) 
	{
		m_body_position += v;
	} else {
		m_body_rotation += v;
	}
}



void HexapodAgent::applyActuator (actuator &actuator)
{
	switch( actuator.rank() ) {
		case hexUtil::kAnterior:
		//cerr << "hexUtil::kAnterior" << endl;

		applyActuator(m_footLA, m_footRA, actuator);
		break;
		case  hexUtil::kMedial:
		//cerr << "hexUtil::kMedial" << endl;

		applyActuator(m_footLB, m_footRB, actuator);
		break;
		case  hexUtil::kPosterior:
		//cerr << "hexUtil::kPosterior" << endl;
		applyActuator(m_footLC, m_footRC, actuator);
		break;
	}
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
	)
{
	m_position = pos;
	m_phi  = phi;
	m_velocity = vel;
	m_omega = omega;
	m_scale = scale;
	m_bodyOffset = bodyOffset;
	m_bodyFootAverageBias = bodyFootAverageBias;
	buildMatrices(pos,phi,vel,omega, scale, dt, floorThickness);

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

	m_footLA.update(dt, maxSpeed, rankA, plantSpeedBiasRamp, ground);
	m_footLB.update(dt, maxSpeed, rankB, plantSpeedBiasRamp, ground);
	m_footLC.update(dt, maxSpeed, rankC, plantSpeedBiasRamp, ground);
	m_footRA.update(dt, maxSpeed, rankA, plantSpeedBiasRamp, ground);
	m_footRB.update(dt, maxSpeed, rankB, plantSpeedBiasRamp, ground);
	m_footRC.update(dt, maxSpeed, rankC, plantSpeedBiasRamp, ground);

	MPoint avgPos(MPoint::origin);
	if (m_bodyFootAverageBias > 0) {
		avgPos  = MPoint((
			m_footLA.position() + 
			m_footLB.position() + 
			m_footLC.position() + 
			m_footRA.position() + 
			m_footRB.position() + 
			m_footRC.position() 
			) * 1.0/6.0);
		avgPos = avgPos * m_matrixInverse;
	}

	m_body_position  = (avgPos*m_bodyFootAverageBias) + m_bodyOffset;
	m_body_rotation = MVector::zero;

	m_leftFeed = leftFootFeed;
	m_rightFeed = rightFootFeed;
	// if (m_leftFeed.y < 0 ) m_leftFeed = MVector(m_leftFeed.x,0, m_leftFeed.z)  ;
	// if (m_rightFeed.y < 0 ) m_rightFeed = MVector(m_rightFeed.x,0, m_rightFeed.z) ;


	m_feedBlend = feedBlend;
}
void HexapodAgent::getOutputData(
	MVector& la, MVector& lb, MVector& lc, 
	MVector& ra, MVector& rb, MVector& rc, 
	MVector& position, MVector& phi, double &scale) const 
{

	MVector tmpLeft;
	MVector tmpRight;


 	la = m_footLA.position();
	ra = m_footRA.position();

	if (m_feedBlend > 0.00001) {
		MPoint leftFeed = m_leftFeed;
		MPoint rightFeed = m_rightFeed;
		if (leftFeed.y < 0) leftFeed.y = 0;
		if (rightFeed.y < 0) rightFeed.y = 0;
		
	  leftFeed = leftFeed * m_matrix;
		rightFeed = rightFeed * m_matrix;
		la = la * (1.0 - m_feedBlend) + (leftFeed * m_feedBlend);
		ra = ra * (1.0 - m_feedBlend) + (rightFeed * m_feedBlend);
	} 

	lb = m_footLB.position();
	lc = m_footLC.position();
	rb = m_footRB.position();
	rc = m_footRC.position();
// cerr << "-----------------------" << endl;
	//cerr << "m_body_rotation: " << m_body_rotation << endl;
		// buid a local matrix for the body
	// cerr << "m_body_rotation: " << m_body_rotation << endl;
	MMatrix mat = MEulerRotation(m_body_rotation).asMatrix();

	// mat[3][0] = m_body_position.x;
	// mat[3][1] = m_body_position.y;
	// mat[3][2] = m_body_position.z;

	// cerr << "m_phi: " << m_phi << endl;

	 	// multiply by agent rotation  matrix
	MMatrix unitAgentMat = mayaMath::matFromPhi(m_position, m_phi, MVector(1.0,1.0,1.0));
	// cerr << "unitAgentMat: " << unitAgentMat << endl;

	mat = (mat * unitAgentMat).homogenize();
	
	// cerr << "transformed mat: " << mat << endl;

		// MMatrix tmpMat = m_matrix
		// get phi
	MQuaternion q;
	q=mat;
		// MVector phi; 
	double theta;
	q.getAxisAngle (phi, theta);
	if (theta < epsilon) {
		phi = MVector::zero;
	} else {
		phi *= theta;
	}
 	// cerr << "phi: " << phi << " theta " << theta << endl;






	position = 	MVector(MPoint(m_body_position) * m_matrix);

 
	scale = m_scale;
}

// s


void HexapodAgent::draw(
	M3dView & view, 
	const DisplayMask & mask 
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

	MFloatVector pos(float(m_position.x), float(m_position.y),float(m_position.z));
	if (mask.displayId) {
		MString pid;
		pid += m_particleId;
		view.setDrawColor( MColor( MColor::kRGB, 0.0, 1.0,1.0 ) );
		view.drawText(pid, pos, M3dView::kLeft);		
	}

	if (mask.displayAgentMatrix) {
		MFloatVector x = MFloatPoint(MFloatVector::xAxis) * fmat;
		MFloatVector y = MFloatPoint(MFloatVector::yAxis) * fmat;
		MFloatVector z = MFloatPoint(MFloatVector::zAxis) * fmat;



		glBegin( GL_LINES );
		view.setDrawColor( MColor( MColor::kRGB, 1.0,0.0,0.0 ) );
		glVertex3f( pos.x , pos.y , pos.z );
		glVertex3f( x.x , x.y , x.z );

		view.setDrawColor( MColor( MColor::kRGB, 0.0,1.0,0.0 ) );
		glVertex3f( pos.x , pos.y , pos.z );
		glVertex3f( y.x , y.y , y.z );

		view.setDrawColor( MColor( MColor::kRGB, 0.0,0.0,1.0 ) );
		glVertex3f( pos.x , pos.y , pos.z );
		glVertex3f( z.x , z.y , z.z );

		glEnd();


	}

}


const MVector & HexapodAgent::position()  const {return m_position; } 
int HexapodAgent::id() const {return m_particleId; } 
const MMatrix & HexapodAgent::matrix() const {return m_matrix;}
const MMatrix & HexapodAgent::matrixInverse() const {return m_matrixInverse;}
const MMatrix & HexapodAgent::matrixNext() const {return m_matrixNext;}

