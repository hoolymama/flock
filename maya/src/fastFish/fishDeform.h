/*
 *  fishDeform.h
 *  
 *
 *  Created by Julian Mann on 12/02/2007.
 *  Copyright 2007 hoolyMama. All rights reserved.
 *
 */


#ifndef fishDeform_H
#define fishDeform_H

#include <maya/MTime.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MPoint.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MFnMeshData.h>
#include <maya/MVectorArray.h>
#include <maya/MIntArray.h>
#include <maya/MFloatArray.h> 
#include <maya/MPxNode.h>
#include <maya/MFnMesh.h>
#include <maya/MFloatPointArray.h>



class fishDeform :  public MPxNode

{
public:
	fishDeform();
	virtual ~fishDeform();
	
	virtual MStatus	compute( const MPlug&, MDataBlock& );
	
	static  void *		creator();
	static  MStatus	initialize();
	static	MTypeId		id;
	
	
private:
	enum Algorithm { kVertices, kBones};


	static  MObject		aPositions;
	static  MObject		aDirections;
	static	MObject		aUps;
	static	MObject		aScales;
	static	MObject		aActive;
	static	MObject		aPivot;
	static	MObject		aPivotOffsetMult;
	
	static	MObject		aInMesh;
	static	MObject		aOutMesh;
	
	static	MObject		aOffset;
	static	MObject		aBend;
	static	MObject		aAmplitude;

//	static	MObject		aOffsetMult;
//	static	MObject		aBendMult;
//	static	MObject		aAmplitudeMult;
//	static	MObject		aScaleMult;
	
	static	MObject		aWavelength;
	static	MObject		aStart;
	static	MObject		aDropoff;

	static	MObject		aNumBones;
	static	MObject		aBoneStart;
	static	MObject		aBoneEnd;
	static	MObject		aSmoothBind;
	
	static	MObject		aOutPoints ;
	static	MObject		aOutDirections;
	
	static	MObject		aCullList;
	
	bool updateSrcObject(MObject & dMesh);
	
	void transformOnly(
		unsigned numActive,
		const MVectorArray &pos, 
		const MVectorArray &dir, 
		const MVectorArray &up, 
		const MDoubleArray &scales, 
		const MDoubleArray &actives, 
		const MDoubleArray &pivot, 
		float pivotMlt, 
		MFloatPointArray & verts
	) ;

	void boneDeformation(	   
		unsigned numActive,  
		 const MVectorArray &poss, 
			const MVectorArray &dirs, 
			const MVectorArray &ups, 
			const MDoubleArray &scales, 
			const MDoubleArray &actives, 
			const MDoubleArray &pivot, 
			const MDoubleArray &bends, 
			const MDoubleArray &offsets, 
			const MDoubleArray &amps, 
			float headLength, 
			float tailLength, 
			short numTailBones,
			float wavelength, 
			float dropoff, 
			float start, 
		   //float offMlt, 
		   //float ampMlt, 
		   //float bendMlt, 
			float pivotMlt, 
			bool smooth,
			MFloatPointArray & verts,
			MVectorArray &outPts,
			MVectorArray &outDirs
	) ;
	
	
	
	MFnMesh m_outMeshFn;
	MFnMeshData m_outMeshCreator;
	MObject m_outGeom;	
	
	// polygons for one fish
	MFloatPointArray m_points;
	MIntArray m_counts;
	MIntArray m_connects;
	
	// UV mapping for one fish
	MFloatArray m_Us;
	MFloatArray m_Vs;
	MIntArray m_uvCounts;
	MIntArray m_uvIds;
	
	
	unsigned m_schoolSize;
	

	
};

#endif

