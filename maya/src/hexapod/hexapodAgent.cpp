


#include "HexapodAgent.h"

HexapodAgent::HexapodAgent(
	unsigned particleId,
	const MVector &pos,
	const MVector &phi,
	const MVector &vel,
	const MVector &omega,
	double scale
	): 
		m_particleId(particleId), 
		m_position(pos), 
		m_phi(phi), 
		m_velocity(vel), 
		m_omega(omega), 
		m_scale(scale)
{
 	m_matrix = mayaMath::matFromPhi(pos, phi, MVector(scale,scale,scale));
 	m_matrixInverse = m_matrix.inverse();
}

HexapodAgent::~HexapodAgent(){ }

void HexapodAgent::set(
		const MVector &pos,
		const MVector &phi,
		const MVector &vel,
		const MVector &omega,
		double scale)
{
	m_position = pos;
}

 
const MVector & HexapodAgent::position()  const {return m_position; } 
int   HexapodAgent::id()  const {return m_particleId; } 
const MMatrix & HexapodAgent::matrix() const {return m_matrix;}
