
#ifndef _hexapod
#define _hexapod

/*
This is the main Maya locator node for hexapod.
*/
#include <maya/MVectorArray.h> 
#include <maya/MDoubleArray.h> 
#include <maya/MEulerRotation.h>
#include <maya/MTime.h> 
#include "mayaMath.h"
#include <maya/MTypeId.h> 
#include <maya/MPxLocatorNode.h>

#include <maya/MGlobal.h>
#include "hexapodColony.h"

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


	virtual MStatus	compute( const MPlug& plug, MDataBlock& data );

	virtual void draw( M3dView & view, const MDagPath & path,
		M3dView::DisplayStyle style,
		M3dView::DisplayStatus status );
	
	virtual bool            isBounded() const;
	virtual MBoundingBox    boundingBox() const; 


	static MTypeId	id;

	/*
	aParticleId, aSortedId, aIdIndex
	Int arrays to help us determine which agents are: 
	new born
	existing already
	dead
	*/
	static MObject	aParticleId;
	static MObject	aSortedId;
	static MObject	aIdIndex;

	/*
	attributes that make up the bugs matrix at
	the current time and in the future
	*/
	static MObject	aPosition;	
	static MObject	aPhi;
	static MObject	aVelocity;	
	static MObject	aOmega;	
	static MObject	aScale;

/* time attribs for the simulation*/
	static MObject	aCurrentTime; 
	static MObject	aStartTime;
	static MObject	aDefaultWhenDormant;

	/* maxSpeed is used to normalize the speed of 
	a foot or an agent to lookup a value from a ramp
	*/
	static MObject aMaxSpeed;

	/*
  a speed based ramp that determines a value from 0-1
  which is used in foot plant planning to put the
  plant at the center of the home circle (0) or on the leading
  edge of the circle (1) or somewhere between  
	*/
	static MObject aPlantSpeedBiasRamp;


	/*
	These three attributes are ramps that are used when setting
	the constraining circle radius for any given foot. 
	See HexapodAgent::update implementation.
	*/
	static MObject aAnteriorRadiusRamp;
	static MObject aLateralRadiusRamp;
	static MObject aPosteriorRadiusRamp;

	/*
	parameters related to feet in rank A B and C
	homeX and Z define the offset from center.

	radius min and max define the range of size for the 
	actual foot constraining circle

	stepIncrementRamp defines how much the stepParam 
	should be incremented based on the speed a foot is 
	traveling.

	slideProfileRamp -- UNUSED RIGHT NOW
	*/
	static MObject  aRankA;
	static MObject  aHomeAX;
	static MObject  aHomeAZ;
	static MObject	aHomeA; 
	static MObject	aRadiusMinA; 
	static MObject	aRadiusMaxA;  
	static MObject	aRadiusA; 
	static MObject	aStepIncrementRampA; 
	static MObject	aSlideProfileRampA;
	static MObject	aLiftProfileRampA;
	static MObject	aLiftProfileRangeA;


	static MObject  aRankB;
	static MObject  aHomeBX;
	static MObject  aHomeBZ;
	static MObject	aHomeB; 
	static MObject	aRadiusMinB; 
	static MObject	aRadiusMaxB;  
	static MObject	aRadiusB; 
	static MObject	aStepIncrementRampB; 
	static MObject	aSlideProfileRampB;
	static MObject	aLiftProfileRampB;
	static MObject	aLiftProfileRangeB;

	static MObject  aRankC;
	static MObject  aHomeCX;
	static MObject  aHomeCZ;
	static MObject	aHomeC; 
	static MObject	aRadiusMinC; 
	static MObject	aRadiusMaxC;  
	static MObject	aRadiusC; 
	static MObject	aStepIncrementRampC; 
	static MObject	aSlideProfileRampC;
	static MObject	aLiftProfileRampC;
	static MObject	aLiftProfileRangeC;



	static MObject	aBodyOffset;

	/* before applying offset, move the 
	body towards the average of the feet
	*/
	static MObject	aBodyFootAverageBias;
	
	static MObject	aLeftFootFeed;
	static MObject	aRightFootFeed;
	static MObject	aFeedBlend;

	static MObject	aActuatorActive;
	static MObject	aActuatorRank;
	static MObject	aActuatorInputAxis; 
	
	static MObject	aActuatorInputMin;
	static MObject	aActuatorInputMax;
	static MObject	aActuatorInputRange;
	static MObject	aActuatorInputMirror;


	static MObject	aActuatorOutputMin;
	static MObject	aActuatorOutputMax;
	static MObject	aActuatorOutputRange;

	// X,Y,Z,Step - ranges are dim for step
	static MObject	aActuatorRamp;
		// tx, ty, tz, rx, ry, rz
	static MObject	aActuatorOutputChannel; 
	static MObject	aBodyActuator;

	static MObject	aActuatorDummyRamp;
	/*
		specify what components and info to draw with 
		openGL
	*/
	static MObject	aDisplayPlants;
	static MObject	aDisplayHome;
	static MObject	aDisplayFootPosition;
	static MObject	aDisplayId;


	static MObject	aDisplaySpeed;
	static MObject	aDisplayFootLocal;
	static MObject	aDisplayAgentMatrix;


	/*
	outputs
	should be in the same order as input partjicles.
	*/
	static MObject	aOutIdIndex;
	static MObject	aOutSortedId;

	static MObject	aOutLeftA;
	static MObject	aOutLeftB;
	static MObject	aOutLeftC;
	static MObject	aOutRightA;
	static MObject	aOutRightB;
	static MObject	aOutRightC;
	static MObject	aOutPosition;
	static MObject	aOutPhi;
	static MObject	aOutScale;

private:

	MTime m_lastTimeIEvaluated;

	hexapodColony * m_colony;

	MStatus checkArrayLength(unsigned alen, unsigned len, const MString &name);
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
