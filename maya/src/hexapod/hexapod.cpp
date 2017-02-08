

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
// #include "HexapodAgent.h"

#include "errorMacros.h"

#include "jMayaIds.h"

#include "mayaMath.h"
#include "attrUtils.h"
MObject hexapod::aParticleId;
MObject hexapod::aSortedId;
MObject hexapod::aIdIndex;

MObject hexapod::aPosition;	
MObject hexapod::aPhi;
MObject hexapod::aVelocity;	
MObject hexapod::aOmega;	
MObject hexapod::aScale;
MObject hexapod::aMesh;
MObject hexapod::aCurrentTime; 
MObject hexapod::aStartTime;

MObject hexapod::aRankA;
MObject hexapod::aHomeAX;
MObject hexapod::aHomeAZ;
MObject hexapod::aHomeA; 
MObject hexapod::aRadiusMinA; 
MObject hexapod::aRadiusMaxA;  
MObject hexapod::aRadiusA; 
MObject hexapod::aStepIncrementRampA; 
MObject hexapod::aSlideProfileRampA;

MObject hexapod::aRankB;
MObject hexapod::aHomeBX;
MObject hexapod::aHomeBZ;
MObject hexapod::aHomeB; 
MObject hexapod::aRadiusMinB; 
MObject hexapod::aRadiusMaxB;  
MObject hexapod::aRadiusB; 
MObject hexapod::aStepIncrementRampB; 
MObject hexapod::aSlideProfileRampB;


MObject hexapod::aRankC;
MObject hexapod::aHomeCX;
MObject hexapod::aHomeCZ;
MObject hexapod::aHomeC; 
MObject hexapod::aRadiusMinC; 
MObject hexapod::aRadiusMaxC;  
MObject hexapod::aRadiusC; 
MObject hexapod::aStepIncrementRampC; 
MObject hexapod::aSlideProfileRampC;

MObject hexapod::aOutLeftA;
MObject hexapod::aOutLeftB;
MObject hexapod::aOutLeftC;
MObject hexapod::aOutRightA;
MObject hexapod::aOutRightB;
MObject hexapod::aOutRightC;

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

	aMesh = tAttr.create("mesh", "msh", MFnData::kMesh, &st );er
	tAttr.setReadable(false);
	st = addAttribute(aMesh);	er;

	aCurrentTime = uAttr.create( "currentTime", "ct", MFnUnitAttribute::kTime );
	uAttr.setStorable(true);
	st =  addAttribute(aCurrentTime);  er;
	
	aStartTime = uAttr.create( "startTime", "st", MFnUnitAttribute::kTime );
	uAttr.setStorable(true);
	st =  addAttribute(aStartTime);  er;
	


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

	aRankA = cAttr.create("rankA","rka");
	cAttr.addChild(aHomeA);
	cAttr.addChild(aRadiusA);
	cAttr.addChild(aStepIncrementRampA);
	cAttr.addChild(aSlideProfileRampA);
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

	aRankB = cAttr.create("rankB","rkb");
	cAttr.addChild(aHomeB);
	cAttr.addChild(aRadiusB);
	cAttr.addChild(aStepIncrementRampB);
	cAttr.addChild(aSlideProfileRampB);
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

	aRankC = cAttr.create("rankC","rkc");
	cAttr.addChild(aHomeC);
	cAttr.addChild(aRadiusC);
	cAttr.addChild(aStepIncrementRampC);
	cAttr.addChild(aSlideProfileRampC);
	st = addAttribute(aRankC);er;






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

	st = attributeAffects( aCurrentTime, aOutLeftA );er;
	st = attributeAffects( aCurrentTime, aOutLeftB );er;
	st = attributeAffects( aCurrentTime, aOutLeftC );er;
	st = attributeAffects( aCurrentTime, aOutRightA );er;
	st = attributeAffects( aCurrentTime, aOutRightB );er;
	st = attributeAffects( aCurrentTime, aOutRightC );er;

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
		(plug == aOutRightC) 
		)) 
	{
		return( MS::kUnknownParameter);
	}


	MObject thisNode = thisMObject();

	MTime cT = timeValue( data, aCurrentTime );
	MTime sT = timeValue( data, aStartTime );
	MTime dT = cT - m_lastTimeIEvaluated;
	MTime oT = cT - sT;  // offset from start frame
	m_lastTimeIEvaluated = cT;

	double dt = dT.as( MTime::kSeconds );

	/////////////////////// INPUT ARRAYS //////////////////////////
	MDoubleArray tmpParticleId = MFnDoubleArrayData(data.inputValue(aParticleId).data()).array();
	unsigned len = tmpParticleId.length(); 
	cerr << "len " << len << endl;
	MIntArray particleId(len);
	for (int i = 0; i < len; ++i) {  particleId[i] = int(tmpParticleId[i]+0.1);  }

	MIntArray sortedId = MFnIntArrayData(data.inputValue(aSortedId).data()).array();
	MIntArray idIndex = MFnIntArrayData(data.inputValue(aIdIndex).data()).array();
	MVectorArray pos = MFnVectorArrayData(data.inputValue(aPosition).data()).array();
	MVectorArray phi = MFnVectorArrayData(data.inputValue(aPhi).data()).array();
	MVectorArray vel = MFnVectorArrayData(data.inputValue(aVelocity).data()).array();
	MVectorArray omega = MFnVectorArrayData(data.inputValue(aOmega).data()).array();
	MDoubleArray scale = MFnDoubleArrayData(data.inputValue(aScale).data()).array();

	if (checkArrayLength(sortedId, len, "hexapod::sortedId") != MS::kSuccess) return  MS::kUnknownParameter;
	if (checkArrayLength(idIndex, len, "hexapod::idIndex") != MS::kSuccess) return  MS::kUnknownParameter;
	if (checkArrayLength(pos, len, "hexapod::pos") != MS::kSuccess) return  MS::kUnknownParameter;
	if (checkArrayLength(phi, len, "hexapod::phi") != MS::kSuccess) return  MS::kUnknownParameter;
	if (checkArrayLength(vel, len, "hexapod::vel") != MS::kSuccess) return  MS::kUnknownParameter;
	if (checkArrayLength(omega, len, "hexapod::omega") != MS::kSuccess) return  MS::kUnknownParameter;	
	if (checkArrayLength(scale, len, "hexapod::scale") != MS::kSuccess) return  MS::kUnknownParameter;



	//////////////////////////// MESH ////////////////////////////
	MObject dMesh =  data.inputValue(aMesh).asMeshTransformed();
	MFnMesh meshFn(dMesh, &st) ;
	if (st.error()) {MGlobal::displayError("hexapod Needs a mesh") ; return MS::kUnknownParameter;}
	MMeshIsectAccelParams ap = meshFn.autoUniformGridParams();
	MItMeshPolygon polyIter(dMesh, &st);
	if (st.error()) {MGlobal::displayError("hexapod Needs a mesh") ; return MS::kUnknownParameter;}
	////////////////////////////////////////////////////////////////////////////////////


	// MDataHandle hRankA = block.inputValue(aRankA);
	// double	radiusMinA	= hRankA.child(aRadiusMinA).asDouble();
	// double	radiusMaxA	= hRankA.child(aRadiusMaxA).asDouble();
	// double	radiusMinA	= hRankA.child(aRadiusMinA).asDouble();
	// double	radiusMaxA	= hRankA.child(aRadiusMaxA).asDouble();


	// /////////////////////// RAMP ATTRIBUTES //////////////////////////
	// MRampAttribute stepIncrementRampA( thisMObject() , aStepIncrementRampA, &st ); er; 
	// MRampAttribute stepIncrementRampB( thisMObject() , aStepIncrementRampB, &st ); er; 
	// MRampAttribute stepIncrementRampC( thisMObject() , aStepIncrementRampC, &st ); er;

	// MRampAttribute slideProfileRampA( thisMObject() , aSlideProfileRampA, &st ); er; 
	// MRampAttribute slideProfileRampB( thisMObject() , aSlideProfileRampB, &st ); er; 
	// MRampAttribute slideProfileRampC( thisMObject() , aSlideProfileRampC, &st ); er;
	// ///////////////////////////////////////////////////////////////////////////

	// 	/////////////////////// VECTOR ATTRIBUTES //////////////////////////
	// MVector homeLA = data.inputValue(aHomeLA).asVector();
	// MVector homeLB = data.inputValue(aHomeLB).asVector();
	// MVector homeLC = data.inputValue(aHomeLC).asVector();
	// MVector homeRA = data.inputValue(aHomeRA).asVector();
	// MVector homeRB = data.inputValue(aHomeRB).asVector();
	// MVector homeRC = data.inputValue(aHomeRC).asVector();
	// ///////////////////////////////////////////////////////////////////////////


	// /////////////////////// DOUBLE ATTRIBUTES //////////////////////////
	// double	radiusMinA	= data.inputValue(aRadiusMinA).asDouble();
	// double	radiusMaxA	= data.inputValue(aRadiusMaxA).asDouble();
	// double	radiusMinB	= data.inputValue(aRadiusMinB).asDouble();
	// double	radiusMaxB	= data.inputValue(aRadiusMaxB).asDouble();
	// double	radiusMinC	= data.inputValue(aRadiusMinC).asDouble();
	// double	radiusMaxC	= data.inputValue(aRadiusMaxC).asDouble();


	/////////////////////// VALIDATE ARRAYS //////////////////////////







	if (dt > 0.0 && oT > MTime(0.0)) {

		m_colony->update(
			particleId, sortedId, idIndex,
			pos, phi, vel, omega, scale);


		// for (int i = 0; i < len; ++i)
		// {

	// HexapodAgent agent(pos[i], phi[i], scale[i]);
	// 	// agent->set()
	//  agent.createFootLA(
	//  	leftA[i], homeLA, lastPlantLA[i], nextPlantLA[i],
	//   stepTimeLA[i], radiusMinA, radiusMaxA
	//   );


		// }
	} else {
		m_colony->clear();
	}


	/////// PREP OUTPUTS
	MVectorArray outLeftA(len);
	MVectorArray outLeftB(len);
	MVectorArray outLeftC(len);
	MVectorArray outRightA(len);
	MVectorArray outRightB(len);
	MVectorArray outRightC(len);


	///// OUTPUTS
	st = outputData(hexapod::aOutLeftA, data, outLeftA);
	st = outputData(hexapod::aOutLeftB, data, outLeftB);
	st = outputData(hexapod::aOutLeftC, data, outLeftC);
	st = outputData(hexapod::aOutRightA, data, outRightA);
	st = outputData(hexapod::aOutRightB, data, outRightB);
	st = outputData(hexapod::aOutRightC, data, outRightC);


	return( MS::kSuccess );
}

