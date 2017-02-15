
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

void HexapodAgent::update(
	double dt,
	const MVector &pos,
	const MVector &phi,
	const MVector &vel,
	const MVector &omega,
	double scale,
	double homeAX, double  homeAZ,double  radiusMinA,double   radiusMaxA,
	double homeBX, double  homeBZ,double  radiusMinB,double   radiusMaxB,
	double homeCX, double  homeCZ,double  radiusMinC,double   radiusMaxC
	)
{
	m_position = pos;
	m_phi  = phi;
	m_velocity = vel;
	m_omega = omega;
	m_scale = scale;

	m_matrix = mayaMath::matFromPhi(pos, phi, MVector(scale,scale,scale));
	m_matrixInverse = m_matrix.inverse();

	m_footLA.update(dt, homeAX,  homeAZ, radiusMinA, radiusMaxA, m_matrix, m_matrixInverse);
	m_footLB.update(dt, homeBX,  homeBZ, radiusMinB, radiusMaxB, m_matrix, m_matrixInverse);
	m_footLC.update(dt, homeCX,  homeCZ, radiusMinC, radiusMaxC, m_matrix, m_matrixInverse);
	m_footRA.update(dt, homeAX, -homeAZ, radiusMinA, radiusMaxA, m_matrix, m_matrixInverse);
	m_footRB.update(dt, homeBX, -homeBZ, radiusMinB, radiusMaxB, m_matrix, m_matrixInverse);
	m_footRC.update(dt, homeCX, -homeCZ, radiusMinC, radiusMaxC, m_matrix, m_matrixInverse);
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
int   HexapodAgent::id()  const {return m_particleId; } 
const MMatrix & HexapodAgent::matrix() const {return m_matrix;}
