
#include "HexapodFoot.h"
#include "HexapodAgent.h"

HexapodAgent::HexapodAgent(
	unsigned particleId,
	const MVector &pos,
	const MVector &phi,
	const MVector &vel,
	const MVector &omega,
	double scale,
	double homeAX, double homeAZ, double radiusMinA, double radiusMaxA,
	double homeBX, double homeBZ, double radiusMinB, double radiusMaxB,
	double homeCX, double homeCZ, double radiusMinC, double radiusMaxC

	): m_particleId(particleId), 
		m_position(pos), 
		m_phi(phi), 
		m_velocity(vel), 
		m_omega(omega), 
		m_scale(scale)

{
	m_matrix = mayaMath::matFromPhi(pos, phi, MVector(scale,scale,scale));
	m_matrixInverse = m_matrix.inverse();

	m_footLA = HexapodFoot(homeAX, homeAZ, radiusMinA,  radiusMaxA, m_matrix);
	m_footLB = HexapodFoot(homeBX, homeBZ, radiusMinB,  radiusMaxB, m_matrix);
	m_footLC = HexapodFoot(homeCX, homeCZ, radiusMinC,  radiusMaxC, m_matrix);
	m_footRA = HexapodFoot(homeAX, -homeAZ, radiusMinA,  radiusMaxA, m_matrix);
	m_footRB = HexapodFoot(homeBX, -homeBZ, radiusMinB,  radiusMaxB, m_matrix);
	m_footRC = HexapodFoot(homeCX, -homeCZ, radiusMinC,  radiusMaxC, m_matrix);

}

HexapodAgent::~HexapodAgent(){ }

void HexapodAgent::set(
	const MVector &pos,
	const MVector &phi,
	const MVector &vel,
	const MVector &omega,
	double scale,
	double homeAX,double  homeAZ,double  radiusMinA,double   radiusMaxA,
	double homeBX,double  homeBZ,double  radiusMinB,double   radiusMaxB,
	double homeCX,double  homeCZ,double  radiusMinC,double   radiusMaxC
	)
{
	m_position = pos;
	m_phi  = phi;
	m_velocity = vel;
	m_omega = omega;
	m_scale = scale;

	m_matrix = mayaMath::matFromPhi(pos, phi, MVector(scale,scale,scale));
	m_matrixInverse = m_matrix.inverse();

	m_footLA.setHome(homeAX, homeAZ, radiusMinA,  radiusMaxA);
	m_footLB.setHome(homeBX, homeBZ, radiusMinB,  radiusMaxB);
	m_footLC.setHome(homeCX, homeCZ, radiusMinC,  radiusMaxC);
	m_footRA.setHome(homeAX, -homeAZ, radiusMinA,  radiusMaxA);
	m_footRB.setHome(homeBX, -homeBZ, radiusMinB,  radiusMaxB);
	m_footRC.setHome(homeCX, -homeCZ, radiusMinC,  radiusMaxC);


}

void HexapodAgent::draw(M3dView & view) const {
	double d44[4][4];
	m_matrix.get(d44);
	MFloatMatrix fmat(d44);
	m_footLA.draw(view, fmat);
	m_footLB.draw(view, fmat);
	m_footLC.draw(view, fmat);
	m_footRA.draw(view, fmat);
	m_footRB.draw(view, fmat);
	m_footRC.draw(view, fmat);
}

const MVector & HexapodAgent::position()  const {return m_position; } 
int   HexapodAgent::id()  const {return m_particleId; } 
const MMatrix & HexapodAgent::matrix() const {return m_matrix;}
