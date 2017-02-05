/*
 *  fishSkeleton.cpp
 *  jtools
 *
 *  Created by Julian Mann on 14/02/2007.
 *  Copyright 2007 hoolyMama. All rights reserved.
 *
 */

#include "fishSkeleton.h"

#include <iostream>

#include <math.h>

// constructor ///destructor///////////////////
fishSkeleton::fishSkeleton(unsigned numTailBones,float tailLength,float headLength) :m_isSmooth(false)
{

	if (tailLength < 0.00001f) tailLength = 0.00001f;
	if (headLength < 0.00001f) headLength = 0.00001f;
	
	if (numTailBones < 1) 
		numTailBones = 1;

	m_n_bones=numTailBones+1;

	m_bones.resize(m_n_bones) ;
	m_bindBones.resize(m_n_bones) ;
	
	// .___._._._._._

	m_spacing = tailLength/(m_n_bones - 1);
	m_nose = headLength;
	
	// bind bones are the inverse matrices
	m_bindBones[0] += JFloatVector(0,0,-m_nose);

	for(unsigned i=0; i<numTailBones; i++)
	{
		m_bindBones[(i+1)] += JFloatVector(0,0, (m_spacing*i));
	}
}

fishSkeleton::~fishSkeleton() {}
/////////////////////////////////////////////////////////

// a single bone /////////////////////////////////////////
JFloatMatrix & fishSkeleton::operator[](unsigned i)  {
	return m_bones[i];
}
/////////////////////////////////////////////////////////


// transform the whole skeleton ////////////////////////
void fishSkeleton::transformBy(const JFloatMatrix& mat) {
	
	for(unsigned i=0; i<m_n_bones; i++)
	{
		m_bones[i] *= mat ;
	}
}

// transform the whole skeleton ////////////////////////
void fishSkeleton::translateBy(const JFloatVector& offset) {
	
	for(unsigned i=0; i<m_n_bones; i++)
	{
		m_bones[i] += offset ;
	}
}

/////////////////////////////////////////////////////////


// transform a vertex by a given bone //////////////////
//void fishSkeleton::transformVertex(unsigned boneId, JFloatVector & vertex) const {
//	vertex = vertex*m_bones[boneId] ;
//}
/////////////////////////////////////////////////////////


// transform vertices by given bones ///////////////////
/*
void fishSkeleton::transformVertices(const JIntArray & boneIds , JFloatVectorArray & vertices) const {
	unsigned len = boneIds.size();
	if (boneIds.size() ==vertices.size() ) {
		for (unsigned i = 0;i<len;i++) {
			vertices[i]*=m_bones[boneIds[i]] ;
		}
	}
}
*/
/////////////////////////////////////////////////////////

/*
// vertex local positions for rigid bind ///////////////////
void fishSkeleton::calcSkinValues(  JIntArray & boneIds , JFloatVectorArray & vertices ) const {
	
	unsigned len = boneIds.size();
	if (boneIds.size() !=vertices.size() ) return;
	
	float lastBoneDist =  (m_spacing  * (m_n_bones - 2) );
	
	for (unsigned i = 0;i<len;i++) {
		JFloatVector & v = vertices[i];
		int & id = boneIds[i];
		
		if(v.z>0) {
			id = 0;
			v.z =v.z - m_nose; 
		} else if  ((-v.z) > lastBoneDist )  {
			id = m_n_bones-1;
			v.z = lastBoneDist + v.z ;
		} else {
			id =   int( -v.z / m_spacing ) +1;
			v.z = v.z + ((id - 1) * m_spacing);
		}
	}
	return;

}
/////////////////////////////////////////////////////////
*/

/*
// vertex local position for rigid bind ///////////////////
void fishSkeleton::calcSkinValue( int & id, JFloatVector& v ) const {
	
	if(v.z>0) {
		id = 0;
		v.z =v.z - m_nose; 
		return;
	} 
	
	
	float lastBoneDist =  (m_spacing  * (m_n_bones - 2) );
	if  ((-v.z) > lastBoneDist )  {
		id = m_n_bones-1;
		v.z = lastBoneDist + v.z ;
		return ;
	}

	id =   int( -v.z / m_spacing ) +1;
	v.z = v.z + ((id - 1) * m_spacing);
	return;

}
/////////////////////////////////////////////////////////
*/

/*
void fishSkeleton::bindTransformation(){
	// pose the bones according to the bind position
	// should do this at the time we deform them !!
	for(int i=0; i<m_n_bones; i++)
	{
		m_bones[i] =  m_bones[i] * m_bindBones[i] ;			
	}
}
*/

// transform vertices  ///////////////////
// this method does a smooth transform, however the interpolation is linear.
// it should be changed to SLERP soon
// a fast way to do this is to cache the axis and angle for each joint
// google: smooth skin deformation normals
void fishSkeleton::transform(JFloatVectorArray & vertices) const {
	unsigned len = vertices.size() ;
	
	if ( len == m_skinIds.size() ) {
		
	
		if ((m_isSmooth) &&  (len == m_skinWeights.size() )) 
		{ // smooth bind
			JFloatVector a, b;
			for (unsigned i = 0;i<len;i++) {
				JFloatVector &v = vertices[i];
				a = v; b = v;
				a = a * m_bones[m_skinIds[i]];
				if (m_skinWeights[i] < 1.0) {
					b = b * m_bones[(m_skinIds[i] + 1)];
					a = (a * m_skinWeights[i]) + (b * (1.0f - m_skinWeights[i]) );
				}
				v=a;
			}
		} else { // rigid bind
			for (unsigned i = 0;i<len;i++) {
				JFloatVector &v = vertices[i];
				v = v * m_bones[m_skinIds[i]];
			}
		}
	}
}

void fishSkeleton::transformAsNormals(JFloatVectorArray & normals) const {
	unsigned len = normals.size() ;
	
	if ( len == m_skinIds.size() ) {
		
	
		if ((m_isSmooth) &&  (len == m_skinWeights.size() )) 
		{ // smooth bind
			JFloatVector a, b;
			for (unsigned i = 0;i<len;i++) {
				JFloatVector &n = normals[i];
				a = n; b = n;
				a.transformAsNormal(m_bones[m_skinIds[i]]);
				if (m_skinWeights[i] < 1.0) {
					b.transformAsNormal(m_bones[(m_skinIds[i] + 1)]);
					a = (a * m_skinWeights[i]) + (b * (1.0f - m_skinWeights[i]) );
					a.normalize();
				}
				n=a;
			}
		} else { // rigid bind
			for (unsigned i = 0;i<len;i++) {
				JFloatVector &n = normals[i];
				n.transformAsNormal(m_bones[m_skinIds[i]]);
			}
		}
	}
}


/////////////////////////////////////////////////////////

void fishSkeleton::bindSkin( JFloatVectorArray & vertices, bool smoothBind ) {
	m_isSmooth = smoothBind;
	// smooth bind
	// set up the primary boneIds and weights
	// if the weight for a vertex is less than 1, the reciprocal weight is implicitly applied to the child bone
	
	unsigned len = vertices.size();
	m_skinIds.resize(len);
	m_skinWeights.resize(len);
	
	float lastBoneDist =  (m_spacing  * (m_n_bones - 2) );
	
	if (smoothBind) {
		for (unsigned i = 0;i<len;i++) {
			JFloatVector & v = vertices[i];
			if(v.z>0) {
				m_skinIds[i] = 0;
				m_skinWeights[i] = (v.z / m_nose);

			} else if ((-v.z) > lastBoneDist )  {
				m_skinIds[i] = m_n_bones-1;
				m_skinWeights[i] = 1.0f;
			} else {
				float boneParam = -v.z / m_spacing;
				m_skinIds[i] = int( boneParam ) +1;
				m_skinWeights[i] =  1.0f - (boneParam - int(boneParam)) ;
			}
		}		
	} else {
		for (unsigned i = 0;i<len;i++) {
			JFloatVector & v = vertices[i];
			if(v.z>0) {
				m_skinIds[i] = 0;
			} else if ((-v.z) > lastBoneDist )  {
				m_skinIds[i] = m_n_bones-1;
			} else {
				float boneParam = -v.z / m_spacing;
				m_skinIds[i] = int( boneParam ) +1;
			}
		}		
	}
}


// given a z position, find a bone id ///////////////////
/*
unsigned fishSkeleton::getBoneId(float zVal) const {
	
	if(zVal>0) return 0;
	
	float lastBoneDist =  (m_spacing  * (m_n_bones - 2) );
	
	if  ((-zVal) > lastBoneDist )  return  m_n_bones-1;
	
	return  int( -zVal / m_spacing ) +1;

}
*/
/////////////////////////////////////////////////////////

// pose skeleton according to AI values ///////////////////
void fishSkeleton::pose(
	const float &  amplitude,
	const float &  offset,
	const float &  bend,
	const float &  wavelength,
	const float &  dropoff,
	const float &  start
)
{
	
	
	// deform the joint positions, nose first, then body to tip of tail
	//////////////////////////////////////////////////////////////
	JFloatVectorArray joints(m_n_bones + 1);
	
	// XZ * joints = new XZ[(m_n_bones + 1)]; 
	
	 joints[0].z = m_nose; 
	
	for (unsigned i=2;i<(m_n_bones + 1);i++) {
		joints[i].z = joints[i-1].z - m_spacing;
	}
	
	for (unsigned i=0;i<(m_n_bones + 1);i++) {

		JFloatVector & pt =  joints[i];
		
		// so sine deformation
		if (wavelength && amplitude) {
			if (pt.z < start) {
				float mult;
				if (dropoff == 0) {
					mult = 1.0;
				} else if (dropoff ==1) {
					mult = start - pt.z;
				} else if (dropoff == 2) {
					mult = start - pt.z;
					mult = mult * mult;
				} else {
					mult = start - pt.z;
					mult = pow(mult,dropoff);
				}
				 pt.x += (sin((pt.z / wavelength) + offset)) * amplitude * mult ;					
			}
		}

		//  do bend deformation
		if (bend) {
			float radius = 1.0f / float(bend);

			float zz=pt.z;
			float absRad = radius - fabs(pt.x);
				
			pt.z = (sin(zz * bend) )  * absRad;
	
			pt.x = ((cos(zz * bend) -1)  * absRad) +pt.x;
		}
	}
	//////////////////////////////////////////////////////////////
	
	// Now set position and alignment of bone matrices in WS 

	JFloatVector direction;	
	 for(unsigned i=0; i<m_n_bones; i++)
	{
		int next = i+1;
		direction.x = 	 joints[i].x - joints[next].x;
		direction.z = 	 joints[i].z - joints[next].z;
		m_bones[i].setToIdentity();
		m_bones[i].orientZ(direction, JFloatVector::yAxis  );
		m_bones[i] +=  joints[i];
		
		// transform bone by inverse of bind pose
		m_bones[i] = m_bindBones[i] * m_bones[i] ;	
		
	}
}
/////////////////////////////////////////////////////////

		
