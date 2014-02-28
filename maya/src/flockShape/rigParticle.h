#ifndef rigParticle_H
#define rigParticle_H


#include <list>

#include <maya/MFloatArray.h>
#include <maya/MFloatMatrix.h>
#include <maya/MFloatVector.h>
#include <maya/MVector.h>
#include <maya/MString.h>
#include <maya/MPlug.h>

#include "errorMacros.h"

#include "mayaMath.h"

class rigParticle{
	public:
		rigParticle(
			unsigned particleId,
			const MVector & t,
			const MVector & a,
			const MVector & u,
			const MVector & s,
			const MFloatArray & fa,
			const MFloatVectorArray & fva,
			mayaMath::axis aimFrontAxis=mayaMath::xAxis,
			mayaMath::axis aimUpAxis=mayaMath::yAxis
			);

		rigParticle(
			unsigned particleId,
			const MVector & t,
			const MVector & phi,
			const MVector & s,
			const MFloatArray & fa,
			const MFloatVectorArray & fva
			);

		rigParticle(unsigned nUserValues, unsigned nUserVectorValues);
		
		rigParticle();	
			
		~rigParticle();


		const int & id() const;

		const MFloatMatrix & transform() const;

		const MFloatArray & values() const;
		const MFloatVectorArray & vectorValues() const;
		
		const rigParticle * future() const;
		
		MStatus setFuture(rigParticle *) ;
		
		MStatus setPlugValues(
			MPlug &tx,
			MPlug &ty,
			MPlug &tz,
			MPlug &rx,
			MPlug &ry,
			MPlug &rz,
			MPlug &sx,
			MPlug &sy,
			MPlug &sz,
			MPlugArray &userPlugs,
			MPlugArray &userVectorPlugs,
			MTransformationMatrix::RotationOrder order=MTransformationMatrix::kXYZ
			) const;

	private:

		int m_id;
		
		MFloatMatrix  m_transform;
				
		MFloatArray m_values;
		MFloatVectorArray m_vector_values;

		// pointer to the equivalent particle in the future
		rigParticle * m_future;
};

typedef std::list < rigParticle * > particleStack;

#endif

