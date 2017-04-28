
#include "rigParticle.h"
#include "errorMacros.h"
#include <maya/MMatrix.h>
#include <maya/MPlugArray.h>
#include <maya/MTransformationMatrix.h>


// ctor creates transformation from aim and up vectors
rigParticle::rigParticle(
	unsigned particleId,
	const MVector & t,
	const MVector & a,
	const MVector & u,
	const MVector & s,
	const MFloatArray & fa,
	const MFloatVectorArray & fva,
	mayaMath::axis aimFrontAxis,
	mayaMath::axis aimUpAxis
	):	m_id(particleId),
	m_values(fa),
	m_vector_values(fva),
	m_future(0)
{
	// turn the trs stuff into a matrix


	MMatrix mat =  mayaMath::matFromAim(t,a,u,s,aimFrontAxis,aimUpAxis);
	m_transform = MFloatMatrix( mat.matrix );
}
// ctor creates transformation from phi (radians around axis)
rigParticle::rigParticle(
	unsigned particleId,
	const MVector & t,
	const MVector & phi,
	const MVector & s,
	const MFloatArray & fa,
	const MFloatVectorArray & fva
	):	m_id(particleId),
	m_values(fa),
	m_vector_values(fva),
	m_future(0)
{
	// cerr << "CTOR t: " << t << " phi: " <<phi<< " s: " << s << endl;
	// TODO  mat from phi is buggy - so refactor this or fix matFromPhi(t,p,s)
	MMatrix mat =  mayaMath::matFromPhi(t,phi,s);
		// cerr << "mat: " << mat << endl;
	m_transform = MFloatMatrix(mat.matrix);

}

rigParticle::rigParticle(unsigned nUserValues, unsigned nUserVectorValues)
:	m_id(-1),
	m_values(nUserValues,0.0f),
	m_vector_values(nUserVectorValues,MFloatVector::zero),
	m_future(0),
	m_transform(MFloatMatrix())
{
	m_transform.setToIdentity();
}

rigParticle::rigParticle()
:	m_id(-1),
	m_values(0,0.0f),
	m_vector_values(0,MFloatVector::zero),
	m_future(0),
	m_transform(MFloatMatrix())
{
	m_transform.setToIdentity();
}


rigParticle::~rigParticle(){}

const int & rigParticle::id() const {return m_id;}

const rigParticle * rigParticle::future() const{
	return m_future;
}

MStatus rigParticle::setFuture(rigParticle * p) {
	if (p->id() == m_id) {
		m_future = p;	
		return MS::kSuccess;	
	} 
	return MS::kUnknownParameter;
}


/*
MString rigParticle::info() const {
	Mstring result;



}
*/

const MFloatMatrix rigParticle::transform() const {return m_transform;}
const MFloatArray rigParticle::values() const {return m_values;}
const MFloatVectorArray rigParticle::vectorValues() const {return m_vector_values;}

 
/**
Set ouput plug values according to the values stored on this particle
	If there are more output plugs than values available, then we set the extras to 0
*/	

MStatus rigParticle::setPlugValues(
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
	MTransformationMatrix::RotationOrder order
	
) const {
	MStatus st = MS::kSuccess;	
	
	unsigned numPlugs =  userPlugs.length();
	unsigned numVectorPlugs =  userVectorPlugs.length();
	unsigned numValues =  m_values.length();	
	unsigned numVectorValues =  m_vector_values.length();	
	
	// cerr  << "numPlugs: " << numPlugs << " - numValues: " <<  numValues << endl; 

	// cerr  << "numVectorPlugs: " << numVectorPlugs << " - numVectorValues: " <<  numVectorValues << endl; 

	if (numPlugs != numValues)  return MS::kUnknownParameter;
	if (numVectorPlugs != numVectorValues)  return MS::kUnknownParameter;

	MMatrix mmat(m_transform.matrix);
	MTransformationMatrix mtmat(mmat);

	double rotateValue[3];
	double scaleValue[3];
	MVector translateValue;

	mtmat.getRotation( rotateValue, order );
	translateValue = mtmat.getTranslation(MSpace::kTransform);
	mtmat.getScale(scaleValue, MSpace::kTransform);

	tx.setValue(translateValue.x);er;
	ty.setValue(translateValue.y);er;	
	tz.setValue(translateValue.z);er;	
	rx.setValue(rotateValue[0]);	
	ry.setValue(rotateValue[1]);			
	rz.setValue(rotateValue[2]);			
	sx.setValue(scaleValue[0]);		
	sy.setValue(scaleValue[1]);	
	sz.setValue(scaleValue[2]);



	for(unsigned i = 0;i < numPlugs; i++) {
		MPlug::FreeToChangeState plugState = userPlugs[i].isFreeToChange(true,false);
		if (plugState == MPlug::kFreeToChange){
			st = (userPlugs[i]).setValue(m_values[i]);er;
		}
	}
	for(unsigned i = 0;i < numVectorPlugs; i++) {
		MPlug::FreeToChangeState plugState = userVectorPlugs[i].isFreeToChange(true,false);
		if (plugState == MPlug::kFreeToChange){
			MPlug xPlug = userVectorPlugs[i].child(0);
			MPlug yPlug = userVectorPlugs[i].child(1);
			MPlug zPlug = userVectorPlugs[i].child(2);
			st = xPlug.setValue(m_vector_values[i].x);er;
			st = yPlug.setValue(m_vector_values[i].y);er;
			st = zPlug.setValue(m_vector_values[i].z);er;
		}
	}

	return st;
}

// void rigParticle::setValues(const MFloatArray & array){ m_values.copy(array); }

