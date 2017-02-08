
#ifndef _hexapod
#define _hexapod


#include <maya/MVectorArray.h> 
#include <maya/MDoubleArray.h> 
#include <maya/MEulerRotation.h>
#include <maya/MTime.h> 
#include "mayaMath.h"
#include <maya/MTypeId.h> 
#include <maya/MPxLocatorNode.h>

#include "HexapodColony.h"

#define LEAD_COLOR        18  // green
#define ACTIVE_COLOR      15  // white
#define ACTIVE_AFFECTED_COLOR  8  // purple
#define DORMANT_COLOR      4  // blue
#define HILITE_COLOR      17  // pale blue
#define DORMANT_VERTEX_COLOR  8  // purple
#define ACTIVE_VERTEX_COLOR    16  // yellow

#define POINT_SIZE        2.0  
#define ROT_TO_DEG  57.295779524


class hexapod:  public MPxLocatorNode
{

public:
	hexapod() ;

	virtual ~hexapod();

	static void		*creator();
	static MStatus	initialize();

	// will compute output force.
	virtual MStatus	compute( const MPlug& plug, MDataBlock& data );

	virtual void draw( M3dView & view, const MDagPath & path,
		M3dView::DisplayStyle style,
		M3dView::DisplayStatus status );
	
	virtual bool            isBounded() const;
	virtual MBoundingBox    boundingBox() const; 


	static MTypeId	id;

	static MObject	aParticleId;
	static MObject	aSortedId;
	static MObject	aIdIndex;


	static MObject	aPosition;	
	static MObject	aPhi;
	static MObject	aVelocity;	
	static MObject	aOmega;	
	static MObject	aScale;
	static MObject	aMesh;
	static MObject	aCurrentTime; 
	static MObject	aStartTime;
	static MObject  aRankA;
	static MObject  aHomeAX;
	static MObject  aHomeAZ;
	static MObject	aHomeA; 
	static MObject	aRadiusMinA; 
	static MObject	aRadiusMaxA;  
	static MObject	aRadiusA; 
	static MObject	aStepIncrementRampA; 
	static MObject	aSlideProfileRampA;

	static MObject  aRankB;
	static MObject  aHomeBX;
	static MObject  aHomeBZ;
	static MObject	aHomeB; 
	static MObject	aRadiusMinB; 
	static MObject	aRadiusMaxB;  
	static MObject	aRadiusB; 
	static MObject	aStepIncrementRampB; 
	static MObject	aSlideProfileRampB;

	static MObject  aRankC;
	static MObject  aHomeCX;
	static MObject  aHomeCZ;
	static MObject	aHomeC; 
	static MObject	aRadiusMinC; 
	static MObject	aRadiusMaxC;  
	static MObject	aRadiusC; 
	static MObject	aStepIncrementRampC; 
	static MObject	aSlideProfileRampC;

	static MObject	aOutLeftA;
	static MObject	aOutLeftB;
	static MObject	aOutLeftC;
	static MObject	aOutRightA;
	static MObject	aOutRightB;
	static MObject	aOutRightC;

private:
	
	MTime m_lastTimeIEvaluated;
	
	HexapodColony * m_colony;

	MStatus  checkArrayLength(unsigned alen, unsigned len, const MString &name);
	MStatus checkArrayLength(const MVectorArray &arr, unsigned len, const MString &name);
	MStatus checkArrayLength(const MDoubleArray &arr, unsigned len, const MString &name);
	MStatus checkArrayLength(const MIntArray &arr, unsigned len, const MString &name);
	MTime  timeValue( MDataBlock& data, MObject & attribute );
};


inline MStatus hexapod::checkArrayLength(unsigned alen, unsigned len, const MString &name) {

	if (alen != len) {

		MString msg("Array length doesn't match: ");
		msg +=name;
		msg +=" length: ";
		msg += alen;
		msg +=" should be: " ;
		msg += len;

		MGlobal::displayError(msg);
		return MS::kFailure;
	}
	return MS::kSuccess;

}

inline  MStatus hexapod::checkArrayLength(const MDoubleArray &arr, unsigned len, const MString &name) {
	unsigned alen = arr.length();
	return checkArrayLength(alen, len, name);
}

inline  MStatus hexapod::checkArrayLength(const MVectorArray &arr, unsigned len, const MString &name) {
	unsigned alen = arr.length();
	return checkArrayLength(alen, len, name);
}
inline  MStatus hexapod::checkArrayLength(const MIntArray &arr, unsigned len, const MString &name) {
	unsigned alen = arr.length();
	return checkArrayLength(alen, len, name);
}

inline MTime hexapod::timeValue( MDataBlock& data, MObject & attribute )
{
	MStatus status;
	MDataHandle hValue = data.inputValue( attribute, &status );
	
	MTime value(0.0);
	if( status == MS::kSuccess )
		value = hValue.asTime();
	
	return( value );
}

#endif
