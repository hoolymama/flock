#ifndef _hexapodLocator
#define _hexapodLocator

#if defined(linux)
#ifndef LINUX
#define LINUX
#endif
#endif

#if defined(OSMac_MachO_)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <maya/MIOStream.h>


#include <maya/MFloatVectorArray.h>
#include <maya/MPxLocatorNode.h>
#include <maya/MColorArray.h>

#include "mayaMath.h"

class hexapodLocator : public MPxLocatorNode
{
public:
	hexapodLocator();
	virtual ~hexapodLocator();


    virtual MStatus   		compute( const MPlug& plug, MDataBlock& data );

	virtual void            draw( M3dView & view, const MDagPath & path,
								  M3dView::DisplayStyle style,
								  M3dView::DisplayStatus status );

	virtual bool            isBounded() const;

	static  void *          creator();
	static  MStatus         initialize();



	static	MTypeId		id;
	
	void drawFoot( 
	M3dView & view,
	const MFloatMatrix & fmat,
	const MColor & color,
	const MVector & footPos,
	const MVector & home,
	const MVector & lastPlant,
	const MVector & nextPlant,
	const float2 & radiusMinMax,
	float radius,
	const MFloatVectorArray & circle,
	bool drawMinMaxRadius,
	bool drawRadius,
	bool drawHomeDots,
	bool drawStride,
	bool drawFootDots,
	float footDotSize
);
	

	void drawCordSys(M3dView & view, const MFloatMatrix & fmat, float scale);

	// void drawMatrices( 
	// 	M3dView & view, 
	// 	const MColorArray &colors ,
	// 	const MVectorArray & positions,
	// 	const MVectorArray & axisAngles,
	// 	float lineMult
	// ) const;
	
	// void drawMatrices( 
	// 	M3dView & view, 
	// 	const MColorArray &colors ,
	// 	const MVectorArray & positions,
	// 	const MVectorArray & fronts,
	// 	const MVectorArray & ups,
	// 	mayaMath::axis frontAxis,
	// 	mayaMath::axis upAxis,
	// 	float lineMult
	// 	) const;
	
	// void drawMatrices( 
	// 	M3dView & view, 
	// 	const MColorArray &colors ,
	// 	const MFloatVectorArray p,
	// 	const MFloatVectorArray x,
	// 	const MFloatVectorArray y,
	// 	const MFloatVectorArray z
	// 	) const;
	
private:
	
 	static MObject	aPosition;	
  static MObject	aPhi;

	// Moving feet positions
	static MObject	aLeftA;
	static MObject	aLeftB;
	static MObject	aLeftC;
	static MObject	aRightA;
	static MObject	aRightB;
	static MObject	aRightC;

 	static MObject  aHomeLA;
 	static MObject  aHomeLB;
 	static MObject  aHomeLC;
 	static MObject  aHomeRA;
 	static MObject  aHomeRB;
 	static MObject  aHomeRC;

	static MObject	aLastPlantLA;
	static MObject	aLastPlantLB;
	static MObject	aLastPlantLC;
	static MObject	aLastPlantRA;
	static MObject	aLastPlantRB;
	static MObject	aLastPlantRC;

	static MObject	aNextPlantLA; 
	static MObject	aNextPlantLB; 
	static MObject	aNextPlantLC; 
	static MObject	aNextPlantRA; 
	static MObject	aNextPlantRB; 
	static MObject	aNextPlantRC; 

	static MObject	aScale;

	static MObject	aRadiusMinA; 
	static MObject	aRadiusMaxA;  
	static MObject	aRadiusA; 

	static MObject	aRadiusMinB; 
	static MObject	aRadiusMaxB;  
	static MObject	aRadiusB; 

	static MObject	aRadiusMinC; 
	static MObject	aRadiusMaxC;  
	static MObject	aRadiusC; 

  static MObject  aRadiusLA; 
 	static MObject  aRadiusLB;
 	static MObject  aRadiusLC;
 	static MObject  aRadiusRA;
 	static MObject  aRadiusRB;
 	static MObject  aRadiusRC;
 
 	static MObject	aColorAR;
 	static MObject	aColorAG;
 	static MObject	aColorAB;
 	static MObject	aColorA;

 	static MObject	aColorBR;
 	static MObject	aColorBG;
 	static MObject	aColorBB;
 	static MObject	aColorB;

 	static MObject	aColorCR;
 	static MObject	aColorCG;
 	static MObject	aColorCB;
 	static MObject	aColorC;

 	static MObject	aDrawAxes;
	static MObject	aAxesScale;

	static MObject	aDrawMinMaxRadius;
	static MObject	aDrawRadius;

	static MObject	aDrawHomeDots;
	static MObject	aDrawStride;
	static MObject	aDrawFootDots;
	static MObject	aFootDotSize;

 	// static MObject	aDrawRotationAxes;


};

 

#endif

