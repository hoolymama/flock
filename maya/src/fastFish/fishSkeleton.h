/*
 *  fishSkeleton.h
 *  jtools
 *
 *  Created by Julian Mann on 14/02/2007.
 *  Copyright 2007 hoolyMama. All rights reserved.
 *
 */


#ifndef fishSkeleton_H
#define fishSkeleton_H

#include <vector>
#include <JFloatMatrix.h>
#include <JFloatVector.h>
 #include <jTypes.h>



class fishSkeleton
{
public:
	
	fishSkeleton(unsigned numTailBones,float tailLength,float headLength);
 
	~fishSkeleton();

	JFloatMatrix & operator[](unsigned i) ;
	/*
	void calcSkinValue( int & id, JFloatVector& v ) const;
	
	void calcSkinValues(  JIntArray & boneIds , JFloatVectorArray & vertices ) const;
	
	void transformVertex(unsigned boneId, JFloatVector & vertex) const;
	
	void transformVertices(const JIntArray & boneIds , JFloatVectorArray & vertices) const ;

	//unsigned getBoneId(float zVal) const;
	
	//float getSpacing() { return m_spacing; }
*/

	void transformBy(const JFloatMatrix& mat) ;

	void translateBy(const JFloatVector& offset);
// transform the whole skeleton ////////////////////////


	void transform(JFloatVectorArray & vertices) const;

	void transformAsNormals(JFloatVectorArray & normals) const ;

	void bindSkin( JFloatVectorArray & vertices, bool smoothBind=0 );
	
		
	
	void pose(
		const float &  amplitude,
		const float &  offset,
		const float &  bend,
		const float &  wavelength,
		const float &  dropoff,
		const float &  start
	);
	
	unsigned getBoneCount() { return m_n_bones; }


private:

	unsigned m_n_bones;
	
	float m_nose;
	
	float m_spacing;

	JFloatMatrixArray  m_bones;
	
	// bind bones are the inverse matrices of the bones - why? - 
	// because each point must be transformed from skin local space to bone local space before being taken to world.
	JFloatMatrixArray  m_bindBones;
	
	JIntArray m_skinIds;
	JFloatArray m_skinWeights;
	/*
	JFloatVectorArray m_skinVertices;
	JFloatVectorArray m_skinNormals;
	*/
	
	//JFloatArray m_weight1, m_weight2;
	bool m_isSmooth;
	
};

#endif /* fishSkeleton */
