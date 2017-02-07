 

#include <maya/MGlobal.h>
#include <maya/MAnimControl.h>
#include <maya/MTime.h>
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MMeshIntersector.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MRampAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MVector.h>
#include <maya/MPoint.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MMatrix.h>
#include <maya/MPlugArray.h>
#include <maya/MDataBlock.h>
#include <maya/MFnMesh.h>
#include <maya/MPxNode.h>


#include "hexapod.h"
#include "HexapodAgent.h"

#include "errorMacros.h"

#include "jMayaIds.h"

#include "mayaMath.h"
#include "attrUtils.h"


// INPUT ARRAYS
MObject hexapod::aPosition;	
MObject hexapod::aPhi;
// for planning / prediction
MObject hexapod::aVelocity;	
MObject hexapod::aOmega;	

// Moving feet positions
MObject hexapod::aLeftA;
MObject hexapod::aLeftB;
MObject hexapod::aLeftC;
MObject hexapod::aRightA;
MObject hexapod::aRightB;
MObject hexapod::aRightC;

// Home positions
MObject hexapod::aHomeLA;
MObject hexapod::aHomeLB;
MObject hexapod::aHomeLC;
MObject hexapod::aHomeRA;
MObject hexapod::aHomeRB;
MObject hexapod::aHomeRC;


// Last and next plant
MObject hexapod::aLastPlantLA;
MObject hexapod::aLastPlantLB;
MObject hexapod::aLastPlantLC;
MObject hexapod::aLastPlantRA;
MObject hexapod::aLastPlantRB;
MObject hexapod::aLastPlantRC;

MObject hexapod::aNextPlantLA; 
MObject hexapod::aNextPlantLB; 
MObject hexapod::aNextPlantLC; 
MObject hexapod::aNextPlantRA; 
MObject hexapod::aNextPlantRB; 
MObject hexapod::aNextPlantRC; 

MObject hexapod::aScale; // home and radius values are scaled
MObject hexapod::aMesh;

// 0 < StepTime < 1 means the foot is in flight.
// StepTime == 1 means the foot is landed 
MObject hexapod::aStepTimeLA;
MObject hexapod::aStepTimeLB;
MObject hexapod::aStepTimeLC;
MObject hexapod::aStepTimeRA;
MObject hexapod::aStepTimeRB;
MObject hexapod::aStepTimeRC;

// curve ramps that attenuate the step increment value based on velocity beetle's velocity
MObject hexapod::aStepIncrementRampA; 
MObject hexapod::aStepIncrementRampB; 
MObject hexapod::aStepIncrementRampC;

// MObject hexapod::aVelocityRangeLow;
// MObject hexapod::aVelocityRangeHigh;
// MObject hexapod::aVelocityRange;

// MObject hexapod::aMaxDisplacementA;
// MObject hexapod::aMaxDisplacementB;
// MObject hexapod::aMaxDisplacementC;

MObject hexapod::aRadiusMinA; 
MObject hexapod::aRadiusMaxA;  
MObject hexapod::aRadiusA; 

MObject hexapod::aRadiusMinB; 
MObject hexapod::aRadiusMaxB;  
MObject hexapod::aRadiusB; 

MObject hexapod::aRadiusMinC; 
MObject hexapod::aRadiusMaxC;  
MObject hexapod::aRadiusC; 

MObject hexapod::aSlideProfileRampA;
MObject hexapod::aSlideProfileRampB;
MObject hexapod::aSlideProfileRampC;

MObject hexapod::aCurrentTime; 

/////// OUTPUTS
MObject hexapod::aOutLeftA;
MObject hexapod::aOutLeftB;
MObject hexapod::aOutLeftC;
MObject hexapod::aOutRightA;
MObject hexapod::aOutRightB;
MObject hexapod::aOutRightC;

// Output Last and next plant
MObject hexapod::aOutLastPlantLA;
MObject hexapod::aOutLastPlantLB;
MObject hexapod::aOutLastPlantLC;
MObject hexapod::aOutLastPlantRA;
MObject hexapod::aOutLastPlantRB;
MObject hexapod::aOutLastPlantRC;

MObject hexapod::aOutNextPlantLA; 
MObject hexapod::aOutNextPlantLB; 
MObject hexapod::aOutNextPlantLC; 
MObject hexapod::aOutNextPlantRA; 
MObject hexapod::aOutNextPlantRB; 
MObject hexapod::aOutNextPlantRC; 

MObject hexapod::aOutStepTimeLA;
MObject hexapod::aOutStepTimeLB;
MObject hexapod::aOutStepTimeLC;
MObject hexapod::aOutStepTimeRA;
MObject hexapod::aOutStepTimeRB;
MObject hexapod::aOutStepTimeRC;

MObject hexapod::aOutHomeLA; 
MObject hexapod::aOutHomeLB;
MObject hexapod::aOutHomeLC;
MObject hexapod::aOutHomeRA;
MObject hexapod::aOutHomeRB;
MObject hexapod::aOutHomeRC;

MObject hexapod::aOutRadiusLA; 
MObject hexapod::aOutRadiusLB;
MObject hexapod::aOutRadiusLC;
MObject hexapod::aOutRadiusRA;
MObject hexapod::aOutRadiusRB;
MObject hexapod::aOutRadiusRC;

// these atts simply echoes of the input atts - 
// they are for the locator
MObject hexapod::aOutPosition;  
MObject hexapod::aOutPhi;  
MObject hexapod::aOutScale;  



MTypeId hexapod::id( k_hexapod );

hexapod::hexapod(){

	m_lastTimeIEvaluated = MAnimControl::currentTime();
}

hexapod::~hexapod(){}

void *hexapod::creator()
{	
	return new hexapod;
}


MStatus hexapod::initialize()
{
	MStatus st;
	MString method("hexapod::initialize");

	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MRampAttribute rAttr;
	MFnUnitAttribute	uAttr;



	aPosition = tAttr.create("position", "pos", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aPosition ); er;

	aPhi = tAttr.create("phi", "phi", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aPhi ); er;

	aVelocity = tAttr.create("velocity", "vel", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aVelocity ); er;

	aOmega = tAttr.create("omega", "omg", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aOmega ); er;

	aLeftA = tAttr.create("leftA", "lta", MFnData::kVectorArray, &st); er;
		tAttr.setStorable(false);
	st = addAttribute( aLeftA ); er;

	aLeftB = tAttr.create("leftB", "ltb", MFnData::kVectorArray, &st); er;
		tAttr.setStorable(false);
	st = addAttribute( aLeftB ); er;

	aLeftC = tAttr.create("leftC", "ltc", MFnData::kVectorArray, &st); er;
		tAttr.setStorable(false);
	st = addAttribute( aLeftC ); er;

	aRightA = tAttr.create("rightA", "rta", MFnData::kVectorArray, &st); er;
		tAttr.setStorable(false);
	st = addAttribute( aRightA ); er;

	aRightB = tAttr.create("rightB", "rtb", MFnData::kVectorArray, &st); er;
		tAttr.setStorable(false);
	st = addAttribute( aRightB ); er;

	aRightC = tAttr.create("rightC", "rtc", MFnData::kVectorArray, &st); er;
		tAttr.setStorable(false);
	st = addAttribute( aRightC ); er;

	aHomeLA = nAttr.createPoint( "homeLA", "hla");;
	nAttr.setStorable(true); nAttr.setWritable(true); nAttr.setKeyable(true); 
	st = addAttribute(aHomeLA);er;
	
	aHomeLB = nAttr.createPoint( "homeLB", "hlb");;
	nAttr.setStorable(true); nAttr.setWritable(true); nAttr.setKeyable(true); 
	st = addAttribute(aHomeLB);er;
	
	aHomeLC = nAttr.createPoint( "homeLC", "hlc");;
	nAttr.setStorable(true); nAttr.setWritable(true); nAttr.setKeyable(true); 
	st = addAttribute(aHomeLC);er;
	
	aHomeRA = nAttr.createPoint( "homeRA", "hra");;
	nAttr.setStorable(true); nAttr.setWritable(true); nAttr.setKeyable(true); 
	st = addAttribute(aHomeRA);er;
	
	aHomeRB = nAttr.createPoint( "homeRB", "hrb");;
	nAttr.setStorable(true); nAttr.setWritable(true); nAttr.setKeyable(true); 
	st = addAttribute(aHomeRB);er;
	
	aHomeRC = nAttr.createPoint( "homeRC", "hrc");;
	nAttr.setStorable(true); nAttr.setWritable(true); nAttr.setKeyable(true); 
	st = addAttribute(aHomeRC);er;

	aLastPlantLA = tAttr.create("lastPlantLA", "lpla", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aLastPlantLA); er;
	aLastPlantLB = tAttr.create("lastPlantLB", "lplb", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aLastPlantLB); er;
	aLastPlantLC = tAttr.create("lastPlantLC", "lplc", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aLastPlantLC); er;
	aLastPlantRA = tAttr.create("lastPlantRA", "lpra", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aLastPlantRA); er;
	aLastPlantRB = tAttr.create("lastPlantRB", "lprb", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aLastPlantRB); er;
	aLastPlantRC = tAttr.create("lastPlantRC", "lprc", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aLastPlantRC); er;
	aNextPlantLA = tAttr.create("nextPlantLA", "npla", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aNextPlantLA); er; 
	aNextPlantLB = tAttr.create("nextPlantLB", "nplb", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aNextPlantLB); er; 
	aNextPlantLC = tAttr.create("nextPlantLC", "nplc", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aNextPlantLC); er; 
	aNextPlantRA = tAttr.create("nextPlantRA", "npra", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aNextPlantRA); er; 
	aNextPlantRB = tAttr.create("nextPlantRB", "nprb", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aNextPlantRB); er; 
	aNextPlantRC = tAttr.create("nextPlantRC", "nprc", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aNextPlantRC); er; 

	aScale = tAttr.create("scale", "scl", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aScale ); er;

	aMesh = tAttr.create("mesh", "msh", MFnData::kMesh, &st );er
	tAttr.setReadable(false);
	st = addAttribute(aMesh);	er;

	aStepTimeLA = tAttr.create("stepTimeLA", "stla", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aStepTimeLA ); er;

	aStepTimeLB = tAttr.create("stepTimeLB", "stlb", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aStepTimeLB ); er;

	aStepTimeLC = tAttr.create("stepTimeLC", "stlc", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aStepTimeLC ); er;

	aStepTimeRA = tAttr.create("stepTimeRA", "stra", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aStepTimeRA ); er;

	aStepTimeRB = tAttr.create("stepTimeRB", "strb", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aStepTimeRB ); er;

	aStepTimeRC = tAttr.create("stepTimeRC", "strc", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aStepTimeRC ); er;


	aStepIncrementRampA = rAttr.createCurveRamp("stepIncrementRampA","sta",&st);er;
	st = addAttribute( aStepIncrementRampA );er;
	aStepIncrementRampB = rAttr.createCurveRamp("stepIncrementRampB","stb",&st);er;
	st = addAttribute( aStepIncrementRampB );er;
	aStepIncrementRampC = rAttr.createCurveRamp("stepIncrementRampC","stc",&st);er;
	st = addAttribute( aStepIncrementRampC );er;



	// aVelocityRangeLow = nAttr.create( "velocityRangeLow", "vlrl", MFnNumericData::kDouble); ;
	// aVelocityRangeHigh = nAttr.create( "velocityRangeHigh", "vlrh", MFnNumericData::kDouble);;
	// aVelocityRange = nAttr.create( "velocityRange", "vlr", aVelocityRangeLow, aVelocityRangeHigh);
	// nAttr.setStorable(true);
	// nAttr.setWritable(true);
	// nAttr.setKeyable(true);
	// st = addAttribute(aVelocityRange);


	// aMaxDisplacementA = nAttr.create( "maxDisplacementA", "mxda", MFnNumericData::kDouble);
	// nAttr.setKeyable(true);
	// nAttr.setStorable(true);
	// nAttr.setDefault(1.0f);
	// nAttr.setMin(0.0f);
	// st = addAttribute(aMaxDisplacementA);er;


	// aMaxDisplacementB = nAttr.create( "maxDisplacementB", "mxdb", MFnNumericData::kDouble);
	// nAttr.setKeyable(true);
	// nAttr.setStorable(true);
	// nAttr.setDefault(1.0f);
	// nAttr.setMin(0.0f);
	// st = addAttribute(aMaxDisplacementB);er;


	// aMaxDisplacementC = nAttr.create( "maxDisplacementC", "mxdc", MFnNumericData::kDouble);
	// nAttr.setKeyable(true);
	// nAttr.setStorable(true);
	// nAttr.setDefault(1.0f);
	// nAttr.setMin(0.0f);
	// st = addAttribute(aMaxDisplacementC);er;


	aRadiusMinA = nAttr.create( "radiusMinA", "rmna", MFnNumericData::kDouble);
	aRadiusMaxA = nAttr.create( "radiusMaxA", "rmxa", MFnNumericData::kDouble);
	aRadiusA = nAttr.create( "radiusA", "ra", aRadiusMinA, aRadiusMaxA);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	st = addAttribute(aRadiusA);

	aRadiusMinB = nAttr.create( "radiusMinB", "rmnb", MFnNumericData::kDouble);
	aRadiusMaxB = nAttr.create( "radiusMaxB", "rmxb", MFnNumericData::kDouble);
	aRadiusB = nAttr.create( "radiusB", "rb", aRadiusMinB, aRadiusMaxB);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	st = addAttribute(aRadiusB);

	aRadiusMinC = nAttr.create( "radiusMinC", "rmnc", MFnNumericData::kDouble);
	aRadiusMaxC = nAttr.create( "radiusMaxC", "rmxc", MFnNumericData::kDouble);
	aRadiusC = nAttr.create( "radiusC", "rc", aRadiusMinC, aRadiusMaxC);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	st = addAttribute(aRadiusC);

	aSlideProfileRampA = rAttr.createCurveRamp("slideProfileRampA","slpa",&st);er;
	st = addAttribute(aSlideProfileRampA );er;

	aSlideProfileRampB = rAttr.createCurveRamp("slideProfileRampB","slpb",&st);er;
	st = addAttribute(aSlideProfileRampB );er;

	aSlideProfileRampC = rAttr.createCurveRamp("slideProfileRampC","slpc",&st);er;
	st = addAttribute(aSlideProfileRampC );er;

	aCurrentTime = uAttr.create( "currentTime", "ct", MFnUnitAttribute::kTime );
	uAttr.setStorable(true);
	st =  addAttribute(aCurrentTime);  er;

	/////// OUTPUTS
	aOutLeftA = tAttr.create("outLeftA", "ola", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutLeftA ); er;

	aOutLeftB = tAttr.create("outLeftB", "olb", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutLeftB ); er;

	aOutLeftC = tAttr.create("outLeftC", "olc", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutLeftC ); er;

	aOutRightA = tAttr.create("outRightA", "ora", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutRightA ); er;

	aOutRightB = tAttr.create("outRightB", "orb", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutRightB ); er;

	aOutRightC = tAttr.create("outRightC", "orc", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutRightC ); er;

 	// Output Last and next plant
	aOutLastPlantLA = tAttr.create("outLastPlantLA", "olpla", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutLastPlantLA ); er;

	aOutLastPlantLB = tAttr.create("outLastPlantLB", "olplb", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutLastPlantLB ); er;

	aOutLastPlantLC = tAttr.create("outLastPlantLC", "olplc", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutLastPlantLC ); er;

	aOutLastPlantRA = tAttr.create("outLastPlantRA", "olpra", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutLastPlantRA ); er;

	aOutLastPlantRB = tAttr.create("outLastPlantRB", "olprb", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutLastPlantRB ); er;

	aOutLastPlantRC = tAttr.create("outLastPlantRC", "olprc", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutLastPlantRC ); er;

	aOutNextPlantLA = tAttr.create("outNextPlantLA", "onpla", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutNextPlantLA ); er;

	aOutNextPlantLB = tAttr.create("outNextPlantLB", "onplb", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutNextPlantLB ); er;

	aOutNextPlantLC = tAttr.create("outNextPlantLC", "onplc", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutNextPlantLC ); er;

	aOutNextPlantRA = tAttr.create("outNextPlantRA", "onpra", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutNextPlantRA ); er;

	aOutNextPlantRB = tAttr.create("outNextPlantRB", "onprb", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutNextPlantRB ); er;

	aOutNextPlantRC = tAttr.create("outNextPlantRC", "onprc", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutNextPlantRC ); er;


	aOutStepTimeLA = tAttr.create("outStepTimeLA", "ostla", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutStepTimeLA ); er;

	aOutStepTimeLB = tAttr.create("outStepTimeLB", "ostlb", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutStepTimeLB ); er;

	aOutStepTimeLC = tAttr.create("outStepTimeLC", "ostlc", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutStepTimeLC ); er;

	aOutStepTimeRA = tAttr.create("outStepTimeRA", "ostra", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutStepTimeRA ); er;

	aOutStepTimeRB = tAttr.create("outStepTimeRB", "ostrb", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutStepTimeRB ); er;

	aOutStepTimeRC = tAttr.create("outStepTimeRC", "ostrc", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutStepTimeRC ); er;


	aOutHomeLA = tAttr.create("outHomeLA", "ohla", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutHomeLA ); er;


  // VISUALIZATION
	aOutHomeLB = tAttr.create("outHomeLB", "ohlb", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutHomeLB ); er;

	aOutHomeLC = tAttr.create("outHomeLC", "ohlc", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutHomeLC ); er;

	aOutHomeRA = tAttr.create("outHomeRA", "ohra", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutHomeRA ); er;

	aOutHomeRB = tAttr.create("outHomeRB", "ohrb", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutHomeRB ); er;

	aOutHomeRC = tAttr.create("outHomeRC", "ohrc", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutHomeRC ); er;

	aOutRadiusLA = tAttr.create("outRadiusLA", "arla", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutRadiusLA ); er;

	aOutRadiusLB = tAttr.create("outRadiusLB", "arlb", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutRadiusLB ); er;

	aOutRadiusLC = tAttr.create("outRadiusLC", "arlc", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutRadiusLC ); er;

	aOutRadiusRA = tAttr.create("outRadiusRA", "arra", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutRadiusRA ); er;

	aOutRadiusRB = tAttr.create("outRadiusRB", "arrb", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutRadiusRB ); er;

	aOutRadiusRC = tAttr.create("outRadiusRC", "arrc", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutRadiusRC ); er;

	aOutPosition = tAttr.create("outPosition", "opos", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutPosition ); er;

	aOutPhi = tAttr.create("outPhi", "ophi", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutPhi ); er;

	aOutScale = tAttr.create("outScale", "osc", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutScale ); er;




	st = attributeAffects( aCurrentTime, aOutLeftA );er;
	st = attributeAffects( aCurrentTime, aOutLeftB );er;
	st = attributeAffects( aCurrentTime, aOutLeftC );er;
	st = attributeAffects( aCurrentTime, aOutRightA );er;
	st = attributeAffects( aCurrentTime, aOutRightB );er;
	st = attributeAffects( aCurrentTime, aOutRightC );er;
	st = attributeAffects( aCurrentTime, aOutLastPlantLA );er;
	st = attributeAffects( aCurrentTime, aOutLastPlantLB );er;
	st = attributeAffects( aCurrentTime, aOutLastPlantLC );er;
	st = attributeAffects( aCurrentTime, aOutLastPlantRA );er;
	st = attributeAffects( aCurrentTime, aOutLastPlantRB );er;
	st = attributeAffects( aCurrentTime, aOutLastPlantRC );er;
	st = attributeAffects( aCurrentTime, aOutNextPlantLA );er;
	st = attributeAffects( aCurrentTime, aOutNextPlantLB );er;
	st = attributeAffects( aCurrentTime, aOutNextPlantLC );er;
	st = attributeAffects( aCurrentTime, aOutNextPlantRA );er;
	st = attributeAffects( aCurrentTime, aOutNextPlantRB );er;
	st = attributeAffects( aCurrentTime, aOutNextPlantRC );er;
	st = attributeAffects( aCurrentTime, aOutStepTimeLA );er;
	st = attributeAffects( aCurrentTime, aOutStepTimeLB );er;
	st = attributeAffects( aCurrentTime, aOutStepTimeLC );er;
	st = attributeAffects( aCurrentTime, aOutStepTimeRA );er;
	st = attributeAffects( aCurrentTime, aOutStepTimeRB );er;
	st = attributeAffects( aCurrentTime, aOutStepTimeRC );er;
	st = attributeAffects( aCurrentTime, aOutHomeLA );er;
	st = attributeAffects( aCurrentTime, aOutHomeLB );er;
	st = attributeAffects( aCurrentTime, aOutHomeLC );er;
	st = attributeAffects( aCurrentTime, aOutHomeRA );er;
	st = attributeAffects( aCurrentTime, aOutHomeRB );er;
	st = attributeAffects( aCurrentTime, aOutHomeRC );er;
	st = attributeAffects( aCurrentTime, aOutRadiusLA );er;
	st = attributeAffects( aCurrentTime, aOutRadiusLB );er;
	st = attributeAffects( aCurrentTime, aOutRadiusLC );er;
	st = attributeAffects( aCurrentTime, aOutRadiusRA );er;
	st = attributeAffects( aCurrentTime, aOutRadiusRB );er;
	st = attributeAffects( aCurrentTime, aOutRadiusRC );er;
	st = attributeAffects( aCurrentTime, aOutPosition );er;
	st = attributeAffects( aCurrentTime, aOutPhi );er;
	st = attributeAffects( aCurrentTime, aOutScale );er;


	return( MS::kSuccess );
}




MStatus hexapod::compute(const MPlug& plug, MDataBlock& data)

{
	MStatus st;
	MString method("hexapod::compute");

 
	if(!(
		(plug == aOutLeftA) ||
		(plug == aOutLeftB) ||
		(plug == aOutLeftC) ||
		(plug == aOutRightA) ||
		(plug == aOutRightB) ||
		(plug == aOutRightC) ||
		(plug == aOutLastPlantLA) ||
		(plug == aOutLastPlantLB) ||
		(plug == aOutLastPlantLC) ||
		(plug == aOutLastPlantRA) ||
		(plug == aOutLastPlantRB) ||
		(plug == aOutLastPlantRC) ||
		(plug == aOutNextPlantLA) ||
		(plug == aOutNextPlantLB) ||
		(plug == aOutNextPlantLC) ||
		(plug == aOutNextPlantRA) ||
		(plug == aOutNextPlantRB) ||
		(plug == aOutNextPlantRC) ||
		(plug == aOutStepTimeLA) ||
		(plug == aOutStepTimeLB) ||
		(plug == aOutStepTimeLC) ||
		(plug == aOutStepTimeRA) ||
		(plug == aOutStepTimeRB) ||
		(plug == aOutStepTimeRC) ||
		(plug == aOutHomeLA) ||
		(plug == aOutHomeLB) ||
		(plug == aOutHomeLC) ||
		(plug == aOutHomeRA) ||
		(plug == aOutHomeRB) ||
		(plug == aOutHomeRC) ||
		(plug == aOutRadiusLA) ||
		(plug == aOutRadiusLB) ||
		(plug == aOutRadiusLC) ||
		(plug == aOutRadiusRA) ||
		(plug == aOutRadiusRB) ||
		(plug == aOutRadiusRC) ||
		(plug == aOutPosition) ||
		(plug == aOutPhi) ||
		(plug == aOutScale)
		)) {
			return( MS::kUnknownParameter);
		}

 
		MObject thisNode = thisMObject();

		MTime cT =  data.inputValue( aCurrentTime).asTime();
		MTime dT = cT - m_lastTimeIEvaluated;
		m_lastTimeIEvaluated = cT;
		double dt = dT.as( MTime::kSeconds );

		/////////////////////// INPUT VECTOR ARRAYS //////////////////////////
		MVectorArray pos = MFnVectorArrayData(data.inputValue(aPosition).data()).array();
		MVectorArray phi = MFnVectorArrayData(data.inputValue(aPhi).data()).array();
		MVectorArray vel = MFnVectorArrayData(data.inputValue(aVelocity).data()).array();
		MVectorArray omega = MFnVectorArrayData(data.inputValue(aOmega).data()).array();

		MVectorArray	leftA= MFnVectorArrayData(data.inputValue(aLeftA).data()).array();
		MVectorArray	leftB= MFnVectorArrayData(data.inputValue(aLeftB).data()).array();
		MVectorArray	leftC= MFnVectorArrayData(data.inputValue(aLeftC).data()).array();
		MVectorArray	rightA= MFnVectorArrayData(data.inputValue(aRightA).data()).array();
		MVectorArray	rightB= MFnVectorArrayData(data.inputValue(aRightB).data()).array();
		MVectorArray	rightC= MFnVectorArrayData(data.inputValue(aRightC).data()).array();

		MVectorArray	lastPlantLA= MFnVectorArrayData(data.inputValue(aLastPlantLA).data()).array();
		MVectorArray	lastPlantLB= MFnVectorArrayData(data.inputValue(aLastPlantLB).data()).array();
		MVectorArray	lastPlantLC= MFnVectorArrayData(data.inputValue(aLastPlantLC).data()).array();
		MVectorArray	lastPlantRA= MFnVectorArrayData(data.inputValue(aLastPlantRA).data()).array();
		MVectorArray	lastPlantRB= MFnVectorArrayData(data.inputValue(aLastPlantRB).data()).array();
		MVectorArray	lastPlantRC= MFnVectorArrayData(data.inputValue(aLastPlantRC).data()).array();

		MVectorArray	nextPlantLA= MFnVectorArrayData(data.inputValue(aNextPlantLA).data()).array(); 
		MVectorArray	nextPlantLB= MFnVectorArrayData(data.inputValue(aNextPlantLB).data()).array(); 
		MVectorArray	nextPlantLC= MFnVectorArrayData(data.inputValue(aNextPlantLC).data()).array(); 
		MVectorArray	nextPlantRA= MFnVectorArrayData(data.inputValue(aNextPlantRA).data()).array(); 
		MVectorArray	nextPlantRB= MFnVectorArrayData(data.inputValue(aNextPlantRB).data()).array(); 
		MVectorArray	nextPlantRC= MFnVectorArrayData(data.inputValue(aNextPlantRC).data()).array(); 
		/////////////////////////////////////////////////////////////////////

		/////////////////////// INPUT DOUBLE ARRAYS //////////////////////////
		MDoubleArray scale = MFnDoubleArrayData(data.inputValue(aScale).data()).array();
		MDoubleArray stepTimeLA = MFnDoubleArrayData(data.inputValue(aStepTimeLA).data()).array();
		MDoubleArray stepTimeLB = MFnDoubleArrayData(data.inputValue(aStepTimeLB).data()).array();
		MDoubleArray stepTimeLC = MFnDoubleArrayData(data.inputValue(aStepTimeLC).data()).array();
		MDoubleArray stepTimeRA = MFnDoubleArrayData(data.inputValue(aStepTimeRA).data()).array();
		MDoubleArray stepTimeRB = MFnDoubleArrayData(data.inputValue(aStepTimeRB).data()).array();
		MDoubleArray stepTimeRC = MFnDoubleArrayData(data.inputValue(aStepTimeRC).data()).array();
		/////////////////////////////////////////////////////////////////////

		/////////////////////// RAMP ATTRIBUTES //////////////////////////
		MRampAttribute stepIncrementRampA( thisMObject() , aStepIncrementRampA, &st ); er; 
		MRampAttribute stepIncrementRampB( thisMObject() , aStepIncrementRampB, &st ); er; 
		MRampAttribute stepIncrementRampC( thisMObject() , aStepIncrementRampC, &st ); er;

		MRampAttribute slideProfileRampA( thisMObject() , aSlideProfileRampA, &st ); er; 
		MRampAttribute slideProfileRampB( thisMObject() , aSlideProfileRampB, &st ); er; 
		MRampAttribute slideProfileRampC( thisMObject() , aSlideProfileRampC, &st ); er;
		///////////////////////////////////////////////////////////////////////////

 		/////////////////////// VECTOR ATTRIBUTES //////////////////////////
		MVector homeLA = data.inputValue(aHomeLA).asVector();
		MVector homeLB = data.inputValue(aHomeLB).asVector();
		MVector homeLC = data.inputValue(aHomeLC).asVector();
		MVector homeRA = data.inputValue(aHomeRA).asVector();
		MVector homeRB = data.inputValue(aHomeRB).asVector();
		MVector homeRC = data.inputValue(aHomeRC).asVector();
		///////////////////////////////////////////////////////////////////////////


		/////////////////////// DOUBLE ATTRIBUTES //////////////////////////
		double	radiusMinA	= data.inputValue(aRadiusMinA).asDouble();
		double	radiusMaxA	= data.inputValue(aRadiusMaxA).asDouble();
		double	radiusMinB	= data.inputValue(aRadiusMinB).asDouble();
		double	radiusMaxB	= data.inputValue(aRadiusMaxB).asDouble();
		double	radiusMinC	= data.inputValue(aRadiusMinC).asDouble();
		double	radiusMaxC	= data.inputValue(aRadiusMaxC).asDouble();




		// double  velocityRangeLow 		= data.inputValue(aVelocityRangeLow).asDouble();
		// double  velocityRangeHigh 		= data.inputValue(aVelocityRangeHigh).asDouble();

		// double  maxDisplacementA 		= data.inputValue(aMaxDisplacementA).asDouble();
		// double  maxDisplacementB 		= data.inputValue(aMaxDisplacementB).asDouble();
		// double  maxDisplacementC 		= data.inputValue(aMaxDisplacementC).asDouble();
		/////////////////////////////////////////////////////////////////////

		/////////////////////// VALIDATE ARRAYS //////////////////////////
		unsigned pl = pos.length();
 		cerr << "pl " << pl << endl;
 


		if (checkArrayLength(phi, pl, "hexapod::phi") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(vel, pl, "hexapod::vel") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(omega, pl, "hexapod::omega") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(leftA, pl, "hexapod::leftA") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(leftB, pl, "hexapod::leftB") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(leftC, pl, "hexapod::leftC") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(rightA, pl, "hexapod::rightA") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(rightB, pl, "hexapod::rightB") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(rightC, pl, "hexapod::rightC") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(lastPlantLA, pl, "hexapod::lastPlantLA") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(lastPlantLB, pl, "hexapod::lastPlantLB") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(lastPlantLC, pl, "hexapod::lastPlantLC") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(lastPlantRA, pl, "hexapod::lastPlantRA") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(lastPlantRB, pl, "hexapod::lastPlantRB") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(lastPlantRC, pl, "hexapod::lastPlantRC") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(nextPlantLA, pl, "hexapod::nextPlantLA") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(nextPlantLB, pl, "hexapod::nextPlantLB") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(nextPlantLC, pl, "hexapod::nextPlantLC") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(nextPlantRA, pl, "hexapod::nextPlantRA") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(nextPlantRB, pl, "hexapod::nextPlantRB") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(nextPlantRC, pl, "hexapod::nextPlantRC") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(scale, pl, "hexapod::scale") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(stepTimeLA, pl, "hexapod::stepTimeLA") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(stepTimeLB, pl, "hexapod::stepTimeLB") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(stepTimeLC, pl, "hexapod::stepTimeLC") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(stepTimeRA, pl, "hexapod::stepTimeRA") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(stepTimeRB, pl, "hexapod::stepTimeRB") != MS::kSuccess) return  MS::kUnknownParameter;
		if (checkArrayLength(stepTimeRC, pl, "hexapod::stepTimeRC") != MS::kSuccess) return  MS::kUnknownParameter;


		/////// PREP OUTPUTS
		MVectorArray outLeftA(pl);
		MVectorArray outLeftB(pl);
		MVectorArray outLeftC(pl);
		MVectorArray outRightA(pl);
		MVectorArray outRightB(pl);
		MVectorArray outRightC(pl);

		MVectorArray outLastPlantLA(pl);
		MVectorArray outLastPlantLB(pl);
		MVectorArray outLastPlantLC(pl);
		MVectorArray outLastPlantRA(pl);
		MVectorArray outLastPlantRB(pl);
		MVectorArray outLastPlantRC(pl);

		MVectorArray outNextPlantLA(pl); 
		MVectorArray outNextPlantLB(pl); 
		MVectorArray outNextPlantLC(pl); 
		MVectorArray outNextPlantRA(pl); 
		MVectorArray outNextPlantRB(pl); 
		MVectorArray outNextPlantRC(pl); 

		MDoubleArray outStepTimeLA(pl);
		MDoubleArray outStepTimeLB(pl);
		MDoubleArray outStepTimeLC(pl);
		MDoubleArray outStepTimeRA(pl);
		MDoubleArray outStepTimeRB(pl);
		MDoubleArray outStepTimeRC(pl);

		MVectorArray outHomeLA(pl);
		MVectorArray outHomeLB(pl);
		MVectorArray outHomeLC(pl);
		MVectorArray outHomeRA(pl);
		MVectorArray outHomeRB(pl);
		MVectorArray outHomeRC(pl);

		MDoubleArray outRadiusLA(pl); 
		MDoubleArray outRadiusLB(pl);
		MDoubleArray outRadiusLC(pl);
		MDoubleArray outRadiusRA(pl);
		MDoubleArray outRadiusRB(pl);
		MDoubleArray outRadiusRC(pl);

		MVectorArray outPosition(pos);
		MVectorArray outPhi(phi);
		MDoubleArray outScale(scale);

		if (dt > 0.0) {

			//////////////////////////// MESH ////////////////////////////
			MObject dMesh =  data.inputValue(aMesh).asMeshTransformed();
			MFnMesh meshFn(dMesh, &st) ;
			if (st.error()) {MGlobal::displayError("hexapod Needs a mesh") ; return MS::kUnknownParameter;}
			MMeshIsectAccelParams ap = meshFn.autoUniformGridParams();
			MItMeshPolygon polyIter(dMesh, &st);
			if (st.error()) {MGlobal::displayError("hexapod Needs a mesh") ; return MS::kUnknownParameter;}
			////////////////////////////////////////////////////////////////////////////////////

			for (int i = 0; i < pl; ++i)
			{

				HexapodAgent agent(pos[i], phi[i], scale[i]);
  			// agent->set()
				 agent.createFootLA(
				 	leftA[i], homeLA, lastPlantLA[i], nextPlantLA[i],
				  stepTimeLA[i], radiusMinA, radiusMaxA, 
				  slideProfileRampA, stepIncrementRampA
				  );
		 

			}
		}


				// st = stepForAgent(
				// 	doFoot2Aim,
				// 	foot2AimOffset,
				// 	foot1AimBlend,
				// 	meshFn,
				// 	&ap,
				// 	polyIter,
				// 	meshDist,
				// 	meshOff,
				// 	dt,
				// 	pos[i], 
				// 	phi[i],
				// 	vel[i],
				// 	omega[i],
				// 	homeOffset1,
				// 	homeOffset2,
				// 	defaultOffset1[i],
				// 	defaultOffset2[i],
				// 	defaultPhi1[i],
				// 	defaultPhi2[i],
				// 	lastPlant1[i],
				// 	lastPlant2[i],
				// 	nextPlant1[i],
				// 	nextPlant2[i],

				// 	meshCoherenceBits[i],
				// 	advancePlant1,
				// 	advancePlant2,
				// 	advanceHome1,
				// 	advanceHome2,
				// 	radius1Min,
				// 	radius1Max,
				// 	radius2Min,
				// 	radius2Max,
				// 	stepTime1[i],
				// 	stepTime2[i],
				// 	rampAttr1,
				// 	rampAttr2,
				// 	incRampAttr1,
				// 	incRampAttr2,
				// 	velocityRangeLow,
				// 	velocityRangeHigh,
				// 	maxDisplacement1,
				// 	maxDisplacement2,
				// 	scale[i],
				// 	blendToDefault1[i],
				// 	blendToDefault2[i],
				// 	newFootPos1[i],
				// 	newFootPos2[i],
				// 	outRotation1[i],
				// 	outRotation2[i],
				// 	newLastPlant1[i],
				// 	newLastPlant2[i],
				// 	newNextPlant1[i],
				// 	newNextPlant2[i],

				// 	newMeshCoherenceBits[i],
				// 	outHomePos1[i],
				// 	outHomePos2[i],
				// 	newStepTime1[i],
				// 	newStepTime2[i],
				// 	outRadius1[i],
				// 	outRadius2[i]
				// 	);
		///// OUTPUTS
		st = outputData(hexapod::aOutLeftA, data, outLeftA);
		st = outputData(hexapod::aOutLeftB, data, outLeftB);
		st = outputData(hexapod::aOutLeftC, data, outLeftC);
		st = outputData(hexapod::aOutRightA, data, outRightA);
		st = outputData(hexapod::aOutRightB, data, outRightB);
		st = outputData(hexapod::aOutRightC, data, outRightC);
		st = outputData(hexapod::aOutLastPlantLA, data, outLastPlantLA);
		st = outputData(hexapod::aOutLastPlantLB, data, outLastPlantLB);
		st = outputData(hexapod::aOutLastPlantLC, data, outLastPlantLC);
		st = outputData(hexapod::aOutLastPlantRA, data, outLastPlantRA);
		st = outputData(hexapod::aOutLastPlantRB, data, outLastPlantRB);
		st = outputData(hexapod::aOutLastPlantRC, data, outLastPlantRC);
		st = outputData(hexapod::aOutNextPlantLA, data, outNextPlantLA); 
		st = outputData(hexapod::aOutNextPlantLB, data, outNextPlantLB); 
		st = outputData(hexapod::aOutNextPlantLC, data, outNextPlantLC); 
		st = outputData(hexapod::aOutNextPlantRA, data, outNextPlantRA); 
		st = outputData(hexapod::aOutNextPlantRB, data, outNextPlantRB); 
		st = outputData(hexapod::aOutNextPlantRC, data, outNextPlantRC); 
		st = outputData(hexapod::aOutStepTimeLA, data, outStepTimeLA);
		st = outputData(hexapod::aOutStepTimeLB, data, outStepTimeLB);
		st = outputData(hexapod::aOutStepTimeLC, data, outStepTimeLC);
		st = outputData(hexapod::aOutStepTimeRA, data, outStepTimeRA);
		st = outputData(hexapod::aOutStepTimeRB, data, outStepTimeRB);
		st = outputData(hexapod::aOutStepTimeRC, data, outStepTimeRC);
		st = outputData(hexapod::aOutHomeLA, data, outHomeLA);
		st = outputData(hexapod::aOutHomeLB, data, outHomeLB);
		st = outputData(hexapod::aOutHomeLC, data, outHomeLC);
		st = outputData(hexapod::aOutHomeRA, data, outHomeRA);
		st = outputData(hexapod::aOutHomeRB, data, outHomeRB);
		st = outputData(hexapod::aOutHomeRC, data, outHomeRC);
		st = outputData(hexapod::aOutRadiusLA, data, outRadiusLA); 
		st = outputData(hexapod::aOutRadiusLB, data, outRadiusLB);
		st = outputData(hexapod::aOutRadiusLC, data, outRadiusLC);
		st = outputData(hexapod::aOutRadiusRA, data, outRadiusRA);
		st = outputData(hexapod::aOutRadiusRB, data, outRadiusRB);
		st = outputData(hexapod::aOutRadiusRC, data, outRadiusRC);
		st = outputData(hexapod::aOutPosition, data, outPosition);  
		st = outputData(hexapod::aOutPhi, data, outPhi);  
		st = outputData(hexapod::aOutScale, data, outScale);  

		return( MS::kSuccess );
	}

