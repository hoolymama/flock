
#ifndef _agent
#define _agent

#include <vector>
#include <maya/MDoubleArray.h>
#include <maya/MVectorArray.h>
#include <mayaMath.h>

// enum axis {xAxis, yAxis ,zAxis };
// typedef mayaMath::axis axis;
class agent ;

typedef std::vector<agent*> AGENT_VECTOR;

class agent {
public:
	
	/// 
	agent(
		  const MVector &p,
		  unsigned id
		  );

	~agent();
	
	const MVector & position()  const  ;
	

	const double position(mayaMath::axis a) const; 
	
	MVector  getPosition()  const ;
	
	
	const unsigned int & id() const;
	
private:
	MVector m_position;	// where the point is
	
	
	// this will be immplemented later
	// AGENT_VECTOR * m_cachedNeighbors;
	
	
	unsigned int  m_id;	// an id, just in case
};
#endif

