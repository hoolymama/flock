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
	const MFloatVector & color,
	const MFloatVector & lastPlant,
	const MFloatVector & nextPlant,
	const MFloatVector & footPos,
	const MFloatVector & homeOffset,
	const MVector & rotation,
	float innerRadius,
	float outerRadius,
	float actualRadius,
	const MFloatVectorArray & circle,
	bool drawRotationAxes,
	float axesScale,
	bool drawMinMaxRadius,
	bool drawActualRadius,
	bool drawHomeDots,
	bool drawhexapod,
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

  	static MObject	aNextPlant1; 
 	static MObject	aNextPlant2; 
 	static MObject	aLastPlant1; 
 	static MObject	aLastPlant2; 
 	static MObject	aFootPos1;
 	static MObject	aFootPos2;
 	static MObject	aScale; // home and radius values are scaled

 	static MObject	aHomePos1;
 	static MObject	aHomePos2;


 	// static MObject	aHomeOffset1X;
 	// static MObject	aHomeOffset1Y;
 	// static MObject	aHomeOffset1Z;
 	// static MObject	aHomeOffset1;

 	// static MObject	aHomeOffset2X;
 	// static MObject	aHomeOffset2Y;
 	// static MObject	aHomeOffset2Z;
 	// static MObject	aHomeOffset2;


 	static MObject	aRadius1Min; 
 	static MObject	aRadius1Max;  
 	static MObject	aRadius1;  

 	static MObject	aRadius2Min; 
 	static MObject	aRadius2Max; 
 	static MObject	aRadius2; 


	static MObject	aActualRadius1;  
	static MObject	aActualRadius2;  

	static MObject	aRotation1;  
	static MObject	aRotation2; 

 	static MObject	aColor1R;
 	static MObject	aColor1G;
 	static MObject	aColor1B;
 	static MObject	aColor1;

 	static MObject	aColor2R;
 	static MObject	aColor2G;
 	static MObject	aColor2B;
 	static MObject	aColor2;

 	static MObject	aDrawAxes;
	static MObject	aAxesScale;

	static MObject	aDrawMinMaxRadius;
	static MObject	aDrawActualRadius;

	static MObject	aDrawHomeDots;
	static MObject	aDrawhexapod;
	static MObject	aDrawFootDots;
	static MObject	aFootDotSize;

 	static MObject	aDrawRotationAxes;








};
#endif

