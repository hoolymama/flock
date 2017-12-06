

#include <maya/MGlobal.h>
#include <maya/MAnimControl.h>
#include <maya/MTime.h>
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MMeshIntersector.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnDoubleArrayData.h>

#include <maya/MFnIntArrayData.h>

#include <maya/MRampAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnEnumAttribute.h>

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
#include "displayMask.h"

#include "errorMacros.h"

#include "jMayaIds.h"

#include "mayaMath.h"
#include "attrUtils.h"
#include "hexUtil.h"


MObject hexapod::aParticleId;
MObject hexapod::aSortedId;
MObject hexapod::aIdIndex;

MObject hexapod::aPosition;	
MObject hexapod::aPhi;
MObject hexapod::aVelocity;	
MObject hexapod::aOmega;	
MObject hexapod::aScale;
MObject hexapod::aCurrentTime; 
MObject hexapod::aStartTime;
MObject hexapod::aDefaultWhenDormant;


MObject hexapod::aFloorMesh;
MObject hexapod::aAnimatedFloor;
MObject hexapod::aFloorThickness;
MObject hexapod::aUseMeshFloor;


MObject hexapod::aMaxSpeed;
MObject hexapod::aPlantSpeedBiasRamp;

MObject hexapod::aAnteriorRadiusRamp;
MObject hexapod::aLateralRadiusRamp;
MObject hexapod::aPosteriorRadiusRamp;


MObject hexapod::aRankA;
MObject hexapod::aHomeAX;
MObject hexapod::aHomeAZ;
MObject hexapod::aHomeA; 
MObject hexapod::aRadiusMinA; 
MObject hexapod::aRadiusMaxA;  
MObject hexapod::aRadiusA; 
MObject hexapod::aStepIncrementRampA; 
MObject hexapod::aSlideProfileRampA;
MObject hexapod::aLiftProfileRampA;

MObject hexapod::aRankB;
MObject hexapod::aHomeBX;
MObject hexapod::aHomeBZ;
MObject hexapod::aHomeB; 
MObject hexapod::aRadiusMinB; 
MObject hexapod::aRadiusMaxB;  
MObject hexapod::aRadiusB; 
MObject hexapod::aStepIncrementRampB; 
MObject hexapod::aSlideProfileRampB;
MObject hexapod::aLiftProfileRampB;


MObject hexapod::aRankC;
MObject hexapod::aHomeCX;
MObject hexapod::aHomeCZ;
MObject hexapod::aHomeC; 
MObject hexapod::aRadiusMinC; 
MObject hexapod::aRadiusMaxC;  
MObject hexapod::aRadiusC; 
MObject hexapod::aStepIncrementRampC; 
MObject hexapod::aSlideProfileRampC;
MObject hexapod::aLiftProfileRampC;


MObject hexapod::aBodyOffset;
MObject hexapod::aBodyFootAverageBias;


MObject hexapod::aLeftFootFeed;
MObject hexapod::aRightFootFeed;
MObject hexapod::aFeedBlend;
 
MObject hexapod::aActuatorRank;
MObject hexapod::aActuatorInputMin;
MObject hexapod::aActuatorInputMax;
MObject hexapod::aActuatorInputRange;
MObject hexapod::aActuatorInputAxis;  
MObject hexapod::aActuatorRamp;
MObject hexapod::aActuatorOutputMin;
MObject hexapod::aActuatorOutputMax;
MObject hexapod::aActuatorOutputRange;
MObject hexapod::aActuatorInputMirror;
MObject hexapod::aActuatorOutputChannel;  
MObject hexapod::aActuatorActive;
MObject hexapod::aBodyActuator;


MObject hexapod::aActuatorDummyRamp;


MObject hexapod::aDisplayPlants;
MObject hexapod::aDisplayHome;
MObject hexapod::aDisplayFootPosition;
MObject hexapod::aDisplayId;
MObject hexapod::aDisplaySpeed;
MObject hexapod::aDisplayFootLocal;
MObject hexapod::aDisplayAgentMatrix;

MObject hexapod::aOutLeftA;
MObject hexapod::aOutLeftB;
MObject hexapod::aOutLeftC;
MObject hexapod::aOutRightA;
MObject hexapod::aOutRightB;
MObject hexapod::aOutRightC;
MObject hexapod::aOutPosition;
MObject hexapod::aOutPhi;

MObject hexapod::aOutScale;
MObject hexapod::aOutIdIndex;
MObject hexapod::aOutSortedId;

MTypeId hexapod::id( k_hexapod );




hexapod::hexapod(){
	m_colony = new HexapodColony();
	m_lastTimeIEvaluated = MAnimControl::currentTime();
}

hexapod::~hexapod(){
	delete m_colony;
}

void *hexapod::creator()
{	
	return new hexapod;
}

bool hexapod::isBounded() const
{
	return false;
}

MBoundingBox hexapod::boundingBox() const
{
	return MBoundingBox();
}

void hexapod::draw(
	M3dView & view, 
	const MDagPath & path, 
	M3dView::DisplayStyle style,  
	M3dView::DisplayStatus status ) 
{

	MObject thisNode = thisMObject();

	DisplayMask mask;

	MPlug plug( thisNode, aDisplayPlants);
	plug.getValue(mask.displayPlants);

	plug.setAttribute( aDisplayHome);
	plug.getValue(mask.displayHome);

	plug.setAttribute( aDisplayFootPosition);
	plug.getValue(mask.displayFootPosition);

	plug.setAttribute( aDisplayId);
	plug.getValue(mask.displayId);

	plug.setAttribute( aDisplaySpeed);
	plug.getValue(mask.displaySpeed);

	plug.setAttribute( aDisplayFootLocal);
	plug.getValue(mask.displayFootLocal);

	plug.setAttribute( aDisplayAgentMatrix);
	plug.getValue(mask.displayAgentMatrix);



	m_colony->draw(view, mask);
 
}



MStatus hexapod::compute(const MPlug& plug, MDataBlock& data)

{
	MStatus st;

 
	if(!(
		(plug == aOutLeftA) ||
		(plug == aOutLeftB) ||
		(plug == aOutLeftC) ||
		(plug == aOutRightA) ||
		(plug == aOutRightB) ||
		(plug == aOutRightC) ||
		(plug == aOutPosition) ||
		(plug == aOutScale) ||
		(plug == aOutIdIndex) ||
		(plug == aOutSortedId) ||		
		(plug == aOutPhi) 
		)) 
	{
		return( MS::kUnknownParameter);
	}

 

	MObject thisNode = thisMObject();

 
	MTime cT = timeValue( data, aCurrentTime );
	MTime sT = timeValue( data, aStartTime );
	MTime dT = cT - m_lastTimeIEvaluated;
	MTime oT = cT - sT;  
	m_lastTimeIEvaluated = cT;

	double dt = dT.as( MTime::kSeconds );

	bool showDefault = data.inputValue(aDefaultWhenDormant).asBool();

 
	/////// PREP OUTPUTS
	MVectorArray outLeftA;
	MVectorArray outLeftB;
	MVectorArray outLeftC;
	MVectorArray outRightA;
	MVectorArray outRightB;
	MVectorArray outRightC;
	MVectorArray outPosition;
	MVectorArray outPhi;
	MDoubleArray outScale;

	MIntArray idIndex = MFnIntArrayData(data.inputValue(aIdIndex).data()).array();
	MIntArray sortedId = MFnIntArrayData(data.inputValue(aSortedId).data()).array();
 

	// if (data.inputValue(aAnimatedFloor).asBool() ||  
	// 	(! m_colony->hasMesh()) ||  oT == MTime(0.0)) 
	// {
 // 		m_colony->setMesh(thisNode, data);
	// }


	if (data.inputValue(aAnimatedFloor).asBool() || 
		(! m_colony->hasMesh()) ||  oT == MTime(0.0)) 
	{
		m_colony->setMesh(thisNode, data);
	}

 
 	/*
 	Time going backwards or we are before the start frame
 	*/
	if (dt < 0 ||  oT < MTime(0.0)) {
 
		m_colony->clear();
		data.setClean(hexapod::aOutIdIndex);
		data.setClean(hexapod::aOutSortedId);
		data.setClean(hexapod::aOutLeftA);
		data.setClean(hexapod::aOutLeftB);
		data.setClean(hexapod::aOutLeftC);
		data.setClean(hexapod::aOutRightA);
		data.setClean(hexapod::aOutRightB);
		data.setClean(hexapod::aOutRightC);
		data.setClean(hexapod::aOutPosition);
		data.setClean(hexapod::aOutPhi);
		data.setClean(hexapod::aOutScale);

		return( MS::kSuccess );
	}


	/*
	We want to show the default pose when no particles exist
	*/
	if( (sortedId.length() == 0) && showDefault) {
	 
		m_colony->clear();
		m_colony->getDefaultOutputData(
			thisNode, data,
			outLeftA,outLeftB,outLeftC,outRightA,
			outRightB,outRightC,outPosition,outPhi, outScale
			);
		st = outputData(hexapod::aOutIdIndex, data, MIntArray(1,0) );er;
		st = outputData(hexapod::aOutSortedId, data,MIntArray(1,0) );er;
		st = outputData(hexapod::aOutLeftA, data, outLeftA);er;
		st = outputData(hexapod::aOutLeftB, data, outLeftB);er;
		st = outputData(hexapod::aOutLeftC, data, outLeftC);er;
		st = outputData(hexapod::aOutRightA, data, outRightA);er;
		st = outputData(hexapod::aOutRightB, data, outRightB);er;
		st = outputData(hexapod::aOutRightC, data, outRightC);er;
		st = outputData(hexapod::aOutPosition, data, outPosition);er;
		st = outputData(hexapod::aOutPhi, data, outPhi);er;
		st = outputData(hexapod::aOutScale, data, outScale);er;
 
		return( MS::kSuccess );
	}

	/*
	If time is moving forward, do a sim step
	*/
	if (dt >  0.0 ) {
			 
		// cerr << "Moving forwards - updating....." << endl;
		m_colony->update(dt, thisNode, data);
		m_colony->applyActuators(thisNode, data);
 
	}

 /*
	output whatever is stored
 */
			 
	m_colony->getOutputData(idIndex, outLeftA,outLeftB,outLeftC,outRightA,
		outRightB,outRightC,outPosition,outPhi, outScale
		);

	st = outputData(hexapod::aOutIdIndex, data, idIndex);
	st = outputData(hexapod::aOutSortedId, data, sortedId);
	st = outputData(hexapod::aOutLeftA, data, outLeftA);
	st = outputData(hexapod::aOutLeftB, data, outLeftB);
	st = outputData(hexapod::aOutLeftC, data, outLeftC);
	st = outputData(hexapod::aOutRightA, data, outRightA);
	st = outputData(hexapod::aOutRightB, data, outRightB);
	st = outputData(hexapod::aOutRightC, data, outRightC);
	st = outputData(hexapod::aOutPosition, data, outPosition);
	st = outputData(hexapod::aOutPhi, data, outPhi);
	st = outputData(hexapod::aOutScale, data, outScale);
	 
	return( MS::kSuccess );
}


MStatus hexapod::initialize()
{
	MStatus st;
	MString method("hexapod::initialize");

	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MRampAttribute rAttr;
	MFnUnitAttribute	uAttr;
	MFnCompoundAttribute cAttr;
	MFnEnumAttribute eAttr;

	aParticleId = tAttr.create("particleId","pid", MFnData::kDoubleArray, &st ); er;
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	st = addAttribute( aParticleId);er;

	aSortedId = tAttr.create("sortedId","sid", MFnData::kIntArray, &st ); er;
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	st = addAttribute( aSortedId);er;

	aIdIndex = tAttr.create("idIndex","idi", MFnData::kIntArray, &st ); er;
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	st = addAttribute( aIdIndex );er;

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

	aScale = tAttr.create("scale", "scl", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aScale ); er;

	aFloorMesh = tAttr.create("floorMesh", "fmsh", MFnData::kMesh, &st );er
	tAttr.setReadable(false);
	st = addAttribute(aFloorMesh);	er;

	aAnimatedFloor = nAttr.create( "animatedFloor", "anfl",MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);
	addAttribute( aAnimatedFloor ); 

	aFloorThickness = nAttr.create( "floorThickness", "fthk", MFnNumericData::kDouble);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	st = addAttribute( aFloorThickness );er;


	aUseMeshFloor = nAttr.create( "snapFeetToMesh", "sftm",MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);
	addAttribute( aUseMeshFloor ); 


	aCurrentTime = uAttr.create( "currentTime", "ct", MFnUnitAttribute::kTime );
	uAttr.setStorable(true);
	st =  addAttribute(aCurrentTime);  er;

	aStartTime = uAttr.create( "startTime", "st", MFnUnitAttribute::kTime );
	uAttr.setStorable(true);
	st =  addAttribute(aStartTime);  er;
	

	aDefaultWhenDormant = nAttr.create( "defaultWhenDormant", "dwd",MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);
	addAttribute( aDefaultWhenDormant ); 



	aMaxSpeed = nAttr.create("maxSpeed", "msp", MFnNumericData::kDouble);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(100.0);
	st = addAttribute(aMaxSpeed); er;

	aPlantSpeedBiasRamp = rAttr.createCurveRamp("plantSpeedBiasRamp","psbr",&st);er;
	st = addAttribute( aPlantSpeedBiasRamp );er;

	aAnteriorRadiusRamp= rAttr.createCurveRamp("anteriorRadiusRamp","arr",&st);er;
	st = addAttribute( aAnteriorRadiusRamp );er;
	aLateralRadiusRamp= rAttr.createCurveRamp("lateralRadiusRamp","lrr",&st);er;
	st = addAttribute( aLateralRadiusRamp );er;
	aPosteriorRadiusRamp= rAttr.createCurveRamp("posteriorRadiusRamp","prr",&st);er;
	st = addAttribute( aPosteriorRadiusRamp );er;

	aActuatorDummyRamp= rAttr.createCurveRamp("actuatorDummyRamp","acdr",&st);er;
	st = addAttribute( aActuatorDummyRamp );er;




	aHomeAX = nAttr.create( "homeAX", "hax", MFnNumericData::kDouble);
	aHomeAZ = nAttr.create( "homeAZ", "haz", MFnNumericData::kDouble);
	aHomeA  = nAttr.create( "homeA", "ha", aHomeAX, aHomeAZ);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);

	aRadiusMinA = nAttr.create( "radiusMinA", "rmna", MFnNumericData::kDouble);
	aRadiusMaxA = nAttr.create( "radiusMaxA", "rmxa", MFnNumericData::kDouble);
	aRadiusA = nAttr.create( "radiusA", "ra", aRadiusMinA, aRadiusMaxA);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);

	aStepIncrementRampA = rAttr.createCurveRamp("stepIncrementRampA","sta",&st);er;
	st = addAttribute( aStepIncrementRampA );er;

	aSlideProfileRampA = rAttr.createCurveRamp("slideProfileRampA","slpa",&st);er;
	st = addAttribute(aSlideProfileRampA );er;

	aLiftProfileRampA = rAttr.createCurveRamp("liftProfileRampA","lfpa",&st);er;
	st = addAttribute(aLiftProfileRampA );er;


	aRankA = cAttr.create("rankA","rka");
	cAttr.addChild(aHomeA);
	cAttr.addChild(aRadiusA);
	cAttr.addChild(aStepIncrementRampA);
	cAttr.addChild(aSlideProfileRampA);
	cAttr.addChild(aLiftProfileRampA);
	st = addAttribute(aRankA);er;



	aHomeBX = nAttr.create( "homeBX", "hbx", MFnNumericData::kDouble);
	aHomeBZ = nAttr.create( "homeBZ", "hbz", MFnNumericData::kDouble);
	aHomeB  = nAttr.create( "homeB", "hb", aHomeBX, aHomeBZ);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);

	aRadiusMinB = nAttr.create( "radiusMinB", "rmnb", MFnNumericData::kDouble);
	aRadiusMaxB = nAttr.create( "radiusMaxB", "rmxb", MFnNumericData::kDouble);
	aRadiusB = nAttr.create( "radiusB", "rb", aRadiusMinB, aRadiusMaxB);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);

	aStepIncrementRampB = rAttr.createCurveRamp("stepIncrementRampB","stb",&st);er;
	st = addAttribute( aStepIncrementRampB );er;

	aSlideProfileRampB = rAttr.createCurveRamp("slideProfileRampB","slpb",&st);er;
	st = addAttribute(aSlideProfileRampB );er;

	aLiftProfileRampB = rAttr.createCurveRamp("liftProfileRampB","lfpb",&st);er;
	st = addAttribute(aLiftProfileRampB );er;

	aRankB = cAttr.create("rankB","rkb");
	cAttr.addChild(aHomeB);
	cAttr.addChild(aRadiusB);
	cAttr.addChild(aStepIncrementRampB);
	cAttr.addChild(aSlideProfileRampB);
	cAttr.addChild(aLiftProfileRampB);
	st = addAttribute(aRankB);er;


	aHomeCX = nAttr.create( "homeCX", "hcx", MFnNumericData::kDouble);
	aHomeCZ = nAttr.create( "homeCZ", "hcz", MFnNumericData::kDouble);
	aHomeC  = nAttr.create( "homeC",  "hc", aHomeCX, aHomeCZ);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);

	aRadiusMinC = nAttr.create( "radiusMinC", "rmnc", MFnNumericData::kDouble);
	aRadiusMaxC = nAttr.create( "radiusMaxC", "rmxc", MFnNumericData::kDouble);
	aRadiusC = nAttr.create( "radiusC", "rc", aRadiusMinC, aRadiusMaxC);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);

	aStepIncrementRampC = rAttr.createCurveRamp("stepIncrementRampC","stc",&st);er;
	st = addAttribute( aStepIncrementRampC );er;

	aSlideProfileRampC = rAttr.createCurveRamp("slideProfileRampC","slpc",&st);er;
	st = addAttribute(aSlideProfileRampC );er;
	
	aLiftProfileRampC = rAttr.createCurveRamp("liftProfileRampC","lfpc",&st);er;
	st = addAttribute(aLiftProfileRampC );er;

	aRankC = cAttr.create("rankC","rkc");
	cAttr.addChild(aHomeC);
	cAttr.addChild(aRadiusC);
	cAttr.addChild(aStepIncrementRampC);
	cAttr.addChild(aSlideProfileRampC);
	cAttr.addChild(aLiftProfileRampC);
	st = addAttribute(aRankC);er;

	aBodyOffset= nAttr.createPoint( "bodyOffset", "bof" );
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setReadable(true);
	nAttr.setHidden(false);
	nAttr.setWritable(true);
	st = addAttribute( aBodyOffset );er;

	aBodyFootAverageBias = nAttr.create( "bodyFootAverageBias", "bfab", MFnNumericData::kDouble);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	st = addAttribute( aBodyFootAverageBias );er;

	aLeftFootFeed = tAttr.create("leftFootFeed","lff", MFnData::kVectorArray, &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aLeftFootFeed);er;

	aRightFootFeed = tAttr.create("rightFootFeed","rff", MFnData::kVectorArray, &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aRightFootFeed);er;

	aFeedBlend = tAttr.create("feedBlend","fdbl", MFnData::kDoubleArray, &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aFeedBlend);er;
 

	aActuatorRank= eAttr.create("actuatorRank","acrk");
	eAttr.addField( "A"	,  hexUtil::kAnterior     );
	eAttr.addField( "B"	,  hexUtil::kMedial     );
	eAttr.addField( "C"	,  hexUtil::kPosterior     );
	eAttr.setDefault( hexUtil::kAnterior );
	eAttr.setHidden( false );
	eAttr.setKeyable( true );

	aActuatorActive= nAttr.create( "actuatorActive", "acac",MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);

	aActuatorInputMin = nAttr.create( "actuatorInputMin", "aimn", MFnNumericData::kDouble);
	aActuatorInputMax = nAttr.create( "actuatorInputMax", "aimx", MFnNumericData::kDouble, 1.0);
	aActuatorInputRange  = nAttr.create( "actuatorInputRange",  "air", aActuatorInputMin, aActuatorInputMax);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);

	aActuatorInputMirror= nAttr.create( "actuatorInputMirror", "acim",MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);

	aActuatorOutputMin = nAttr.create( "actuatorOutputMin", "aomn", MFnNumericData::kDouble);
	aActuatorOutputMax = nAttr.create( "actuatorOutputMax", "aomx", MFnNumericData::kDouble, 1.0);
	aActuatorOutputRange  = nAttr.create( "actuatorOutputRange",  "aor", aActuatorOutputMin, aActuatorOutputMax);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);



	aActuatorInputAxis= eAttr.create("actuatorInputAxis","aia");
	eAttr.addField( "stepParam"	,  hexUtil::kStepParam     );
	eAttr.addField( "X"	,  hexUtil::kX     );
	eAttr.addField( "Y"	,  hexUtil::kY     );
	eAttr.addField( "Z"	,  hexUtil::kZ     );
	eAttr.setDefault( hexUtil::kStepParam );
	eAttr.setHidden( false );
	eAttr.setKeyable( true );

	aActuatorRamp = rAttr.createCurveRamp("actuatorRamp","acrm",&st);er;
	st = addAttribute(aActuatorRamp );er;

	// aActuatorOutputScale = nAttr.create("actuatorOutputScale", "aosc", MFnNumericData::kFloat);
	// nAttr.setStorable (true);
	// nAttr.setWritable(true);
	// nAttr.setKeyable(true);
	// nAttr.setDefault(1.0);

	aActuatorOutputChannel= eAttr.create("actuatorOutputChannel","aoch");
	eAttr.addField( "tx"	,  hexUtil::kTX);
	eAttr.addField( "ty"	,  hexUtil::kTY);
	eAttr.addField( "tz"	,  hexUtil::kTZ);
	eAttr.addField( "rx"	,  hexUtil::kRX);
	eAttr.addField( "ry"	,  hexUtil::kRY);
	eAttr.addField( "rz"	,  hexUtil::kRZ);	
	eAttr.setDefault( hexUtil::kTY );
	eAttr.setHidden( false );
	eAttr.setKeyable( true );

	aBodyActuator = cAttr.create("bodyActuator","bact");
	cAttr.addChild(aActuatorRank);
	cAttr.addChild(aActuatorInputRange);
	cAttr.addChild(aActuatorInputAxis);
	cAttr.addChild(aActuatorInputMirror);
	cAttr.addChild(aActuatorRamp);
	cAttr.addChild(aActuatorOutputRange);
	cAttr.addChild(aActuatorOutputChannel);
	cAttr.addChild(aActuatorActive);
	cAttr.setArray(true);
	st = addAttribute(aBodyActuator);er;




	aDisplayPlants = nAttr.create( "displayPlants", "dpl",MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);
	addAttribute( aDisplayPlants ); 
	aDisplayHome = nAttr.create( "displayHome", "dhm",MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);
	addAttribute( aDisplayHome ); 
	aDisplayFootPosition = nAttr.create( "displayFootPosition", "dfp",MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);
	addAttribute( aDisplayFootPosition ); 
	aDisplayId = nAttr.create( "displayId", "did",MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);
	addAttribute( aDisplayId ); 

	aDisplaySpeed = nAttr.create( "displaySpeed", "dsp",MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);
	addAttribute( aDisplaySpeed ); 

	aDisplayFootLocal = nAttr.create( "displayFootLocal", "dfl",MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);
	addAttribute( aDisplayFootLocal ); 

	aDisplayAgentMatrix = nAttr.create( "displayAgentMatrix", "damx",MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);
	addAttribute( aDisplayAgentMatrix ); 

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

	aOutScale = tAttr.create("outScale", "oscl", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutScale ); er;

	aOutIdIndex = tAttr.create("outIdIndex", "oidx", MFnData::kIntArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutIdIndex ); er;

	aOutSortedId = tAttr.create("outSortedId", "osid", MFnData::kIntArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setWritable(false);
	st = addAttribute( aOutSortedId ); er;


	st = attributeAffects( aCurrentTime, aOutLeftA );er;
	st = attributeAffects( aCurrentTime, aOutLeftB );er;
	st = attributeAffects( aCurrentTime, aOutLeftC );er;
	st = attributeAffects( aCurrentTime, aOutRightA );er;
	st = attributeAffects( aCurrentTime, aOutRightB );er;
	st = attributeAffects( aCurrentTime, aOutRightC );er;
	st = attributeAffects( aCurrentTime, aOutPosition );er;
	st = attributeAffects( aCurrentTime, aOutPhi );er;
	st = attributeAffects( aCurrentTime, aOutScale );er;
	st = attributeAffects( aCurrentTime, aOutIdIndex );er;
	st = attributeAffects( aCurrentTime, aOutSortedId );er;

	return( MS::kSuccess );
}


