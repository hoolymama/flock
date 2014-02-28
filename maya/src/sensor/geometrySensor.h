/*
*  geometrySensor.h
	*  animal
	*
	*  Created by Julian Mann on 31/07/2006.
	*  Copyright 2006 hooly|mama. All rights reserved.
	*
*/

//////////////////////////////////////////////////////////////////////
#ifndef _geometrySensor
#define _geometrySensor

#include <sensor.h> 
#include <maya/MFloatVectorArray.h> 
#include <maya/MFloatArray.h> 

	class geometrySensor : public sensor  
{
public:


	geometrySensor();
	/// 
	virtual				~geometrySensor(); 
	/// 
	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );
	///
	virtual	bool		isAbstractClass() const {return false;}
	/// 
	static  void*		creator();
	/// 
	static  MStatus		initialize();	
	/// 	


	//////////////////////////////////////////


	static MObject aNumSamples ;
	static MObject aSeed ;
	static MObject aSurfaceTangent ;
	static MObject aSurfaceNormal ;
	static MObject aViewDirection  ;
	static MObject aInMesh ;
	
	static  MObject         aBBMinX;
	static  MObject         aBBMinY;
	static  MObject         aBBMinZ;
	static  MObject         aBBMin;
	static  MObject         aBBMaxX;
	static  MObject         aBBMaxY;
	static  MObject         aBBMaxZ;
	static  MObject         aBBMax;
	static  MObject         aBBSizX;
	static  MObject         aBBSizY;
	static  MObject         aBBSizZ;
	static  MObject         aBBSiz;
	static  MObject         aBB;
	static  MObject         aUseBB;
	static	MTypeId		id;
protected:

	void sphericalSpread(
		const MVector & dir,
		double spread,
		unsigned count,     
		MFloatVectorArray & rays,
		MFloatArray & strengths,
		long seedVal=-1
		) const;
		

	virtual	MStatus		assess( MDataBlock& data , MVectorArray &assessment );
};

#endif
