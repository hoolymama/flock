
#ifndef _hexapod
#define _hexapod


#include <maya/MVectorArray.h> 
#include <maya/MDoubleArray.h> 
#include <maya/MEulerRotation.h>
#include <maya/MTime.h> 
#include "mayaMath.h"
#include <maya/MTypeId.h> 
 

#define ROT_TO_DEG  57.295779524


class hexapod: public MPxNode
{

public:
	hexapod() ;

	virtual ~hexapod();

	static void		*creator();
	static MStatus	initialize();

	// will compute output force.
	virtual MStatus	compute( const MPlug& plug, MDataBlock& data );

	static MTypeId	id;
	// INPUT ARRAYS
	static MObject	aPosition;	
	static MObject	aPhi;
	static MObject	aVelocity;	
	static MObject	aOmega;	

	// Moving feet positions
	static MObject	aLeftA;
	static MObject	aLeftB;
	static MObject	aLeftC;
	static MObject	aRightA;
	static MObject	aRightB;
	static MObject	aRightC;

// Home positions
 
 	static MObject  aHomeLA;
 	static MObject  aHomeLB;
 	static MObject  aHomeLC;
 	static MObject  aHomeRA;
 	static MObject  aHomeRB;
 	static MObject  aHomeRC;

	
	// Last and next plant
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

	static MObject	aScale; // home and radius values are scaled
	static MObject	aMesh;

	// 0 < StepTime < 1 means the foot is in flight.
	// StepTime == 1 means the foot is landed 
	static MObject	aStepTimeLA;
	static MObject	aStepTimeLB;
	static MObject	aStepTimeLC;
	static MObject	aStepTimeRA;
	static MObject	aStepTimeRB;
	static MObject	aStepTimeRC;

 	// curve ramps that attenuate the step increment value based on velocity beetle's velocity
	static MObject	aStepIncrementRampA; 
	static MObject	aStepIncrementRampB; 
	static MObject	aStepIncrementRampC;


	// static MObject aVelocityRangeLow;
	// static MObject aVelocityRangeHigh;
	// static MObject aVelocityRange;

	// static MObject aMaxDisplacementA;
	// static MObject aMaxDisplacementB;
	// static MObject aMaxDisplacementC;


	static MObject	aRadiusMinA; 
	static MObject	aRadiusMaxA;  
	static MObject	aRadiusA; 

	static MObject	aRadiusMinB; 
	static MObject	aRadiusMaxB;  
	static MObject	aRadiusB; 

	static MObject	aRadiusMinC; 
	static MObject	aRadiusMaxC;  
	static MObject	aRadiusC; 


	static MObject	aSlideProfileA;
 	static MObject	aSlideProfileB;
 	static MObject	aSlideProfileC;
 
	static MObject	aCurrentTime; 



	/////// OUTPUTS
	static MObject	aOutLeftA;
	static MObject	aOutLeftB;
	static MObject	aOutLeftC;
	static MObject	aOutRightA;
	static MObject	aOutRightB;
	static MObject	aOutRightC;


	// Output Last and next plant
	static MObject	aOutLastPlantLA;
	static MObject	aOutLastPlantLB;
	static MObject	aOutLastPlantLC;
	static MObject	aOutLastPlantRA;
	static MObject	aOutLastPlantRB;
	static MObject	aOutLastPlantRC;

	static MObject	aOutNextPlantLA; 
	static MObject	aOutNextPlantLB; 
	static MObject	aOutNextPlantLC; 
	static MObject	aOutNextPlantRA; 
	static MObject	aOutNextPlantRB; 
	static MObject	aOutNextPlantRC; 
 
	static MObject	aOutStepTimeLA;
	static MObject	aOutStepTimeLB;
	static MObject	aOutStepTimeLC;
	static MObject	aOutStepTimeRA;
	static MObject	aOutStepTimeRB;
	static MObject	aOutStepTimeRC;
  
 	static MObject  aOutHomeLA;
 	static MObject  aOutHomeLB;
 	static MObject  aOutHomeLC;
 	static MObject  aOutHomeRA;
 	static MObject  aOutHomeRB;
 	static MObject  aOutHomeRC;

  static MObject  aOutRadiusLA; 
 	static MObject  aOutRadiusLB;
 	static MObject  aOutRadiusLC;
 	static MObject  aOutRadiusRA;
 	static MObject  aOutRadiusRB;
 	static MObject  aOutRadiusRC;
 
	// these atts simply echoes of the input atts - 
	// they are for the locator
	static MObject	aOutPosition;  
	static MObject	aOutPhi;  
	static MObject	aOutScale;  

 

private:
	
	MTime m_lastTimeIEvaluated;
	


};


#endif
