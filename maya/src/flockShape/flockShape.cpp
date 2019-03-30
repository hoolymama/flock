#include <math.h>
// #include <vector>
#include <maya/MGlobal.h>
#include <maya/MBoundingBox.h>
#include <maya/MAnimControl.h>
#include <maya/MDagPath.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MArrayDataHandle.h>

#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnIntArrayData.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnNurbsSurfaceData.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MItDag.h>
#include <maya/MColor.h>
#include <maya/MMatrix.h>
#include <maya/MPlugArray.h>
#include <maya/MVectorArray.h>
#include <maya/MFnIkJoint.h>
#include <maya/MFileObject.h>
#include <maya/MFnSet.h>


#include <maya/MItSelectionList.h>

#include <maya/MDistance.h>
#include <maya/MAngle.h>
/*
#include <maya/.h>
#include <maya/.h>
#include <maya/.h>
*/

#include "clonerStack.h"
#include "flockShape.h"
#include "attrUtils.h"
#include "jMayaIds.h"
#include "mayaMath.h"


#if defined(OSMac_MachO_)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif



// MObject flockShape::aIdPP;
MObject flockShape::aSortedIdPP;
MObject flockShape::aIdIndexPP;

MObject flockShape::aTranslatePP;
MObject flockShape::aRotatePP;
MObject flockShape::aRotateUpPP;
MObject flockShape::aRotateType;
MObject flockShape::aRotateAimFrontAxis;
MObject flockShape::aRotateAimUpAxis;
MObject flockShape::aScalePP;
MObject flockShape::aActivePP;

MObject flockShape::aRotateOrder;

MObject flockShape::aTranslateTargetX;
MObject flockShape::aTranslateTargetY;
MObject flockShape::aTranslateTargetZ;
MObject flockShape::aTranslateTarget;
MObject flockShape::aRotateTargetX;
MObject flockShape::aRotateTargetY;
MObject flockShape::aRotateTargetZ;
MObject flockShape::aRotateTarget;
MObject flockShape::aScaleTargetX;
MObject flockShape::aScaleTargetY;
MObject flockShape::aScaleTargetZ;
MObject flockShape::aScaleTarget;

// MObject flockShape::aDetailTarget;
// MObject flockShape::aLodStrategy;
MObject flockShape::aTargetId;

MObject flockShape::aUserMap;
MObject flockShape::aUserPP;
MObject flockShape::aUserTarget;
MObject flockShape::aUserLabel;

MObject flockShape::aUserVectorMap;
MObject flockShape::aUserVectorPP;
MObject flockShape::aUserVectorTargetX;
MObject flockShape::aUserVectorTargetY;
MObject flockShape::aUserVectorTargetZ;
MObject flockShape::aUserVectorTarget;
MObject flockShape::aUserVectorLabel;


MObject flockShape::aDoFlatTargets;
MObject flockShape::aFlatTranslateTargetX;
MObject flockShape::aFlatTranslateTargetY;
MObject flockShape::aFlatTranslateTargetZ;
MObject flockShape::aFlatTranslateTarget;
MObject flockShape::aFlatRotateTargetX;
MObject flockShape::aFlatRotateTargetY;
MObject flockShape::aFlatRotateTargetZ;
MObject flockShape::aFlatRotateTarget;
MObject flockShape::aFlatScaleTargetX;
MObject flockShape::aFlatScaleTargetY;
MObject flockShape::aFlatScaleTargetZ;
MObject flockShape::aFlatScaleTarget;
MObject flockShape::aFlatUserDoubleTarget;
MObject flockShape::aFlatUserVectorTargetX;
MObject flockShape::aFlatUserVectorTargetY;
MObject flockShape::aFlatUserVectorTargetZ;
MObject flockShape::aFlatUserVectorTarget;
MObject flockShape::aFlatTargets;




// MObject flockShape::aReferenceFile;
// MObject flockShape::aLodGeometry;
// MObject flockShape::aLodGeometrySet;
// MObject flockShape::aLodGeometryStart;
// MObject flockShape::aLodGeometryLo;
// MObject flockShape::aLodGeometryHigh;
// MObject flockShape::aLodGeometryEnd;

MObject flockShape::aCullList;
MObject flockShape::aPointsBBMin;
MObject flockShape::aPointsBBMax;
MObject flockShape::aPointsBBSiz;
MObject flockShape::aPointsBB;

// MObject flockShape::aLodBBMin;
// MObject flockShape::aLodBBMax;
// MObject flockShape::aLodBBSiz;
// MObject flockShape::aLodBB;

MObject flockShape::aCurrentTime;

MObject flockShape::aInMeshes;
MObject flockShape::aMesh;
MObject flockShape::aMeshActive;

MObject flockShape::aOutMeshes;

MObject flockShape::aOutIds;

// MObject flockShape::aDrawDummy;
MTypeId flockShape::id( k_flockShape );

flockShape::flockShape() {

	m_pClonerStack = new clonerStack;
	//	cerr << "flockShape::ctor. made new clonerStack " << m_pClonerStack << endl;


}
flockShape::~flockShape() {
	if (m_pClonerStack) {
		delete m_pClonerStack;
		m_pClonerStack = 0;
	}
	//	cerr << "flockShape::dtor. deleted m_pClonerStack " << m_pClonerStack << endl;

}

void *flockShape::creator() {return new flockShape;}

MStatus flockShape::initialize() {
	MStatus st;

	MString method("flockShape::initialize");

	MFnNumericAttribute nAttr;
	MFnMessageAttribute mAttr;
	MFnTypedAttribute tAttr;
	MFnUnitAttribute uAttr;
	MFnCompoundAttribute cAttr;
	MFnEnumAttribute eAttr;
	MFnGenericAttribute gAttr;

	// Id Mapping attrs
	///////////////////////////////////////////////////////////////////////



	// aIdPP = tAttr.create("particleId","pid", MFnData::kDoubleArray, &st ); mser;
	// tAttr.setStorable(false);
	// tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	// st = addAttribute( aIdPP );mser;

	aSortedIdPP = tAttr.create("sortedId", "sid", MFnData::kIntArray, &st ); mser;
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	st = addAttribute( aSortedIdPP ); mser;

	aIdIndexPP = tAttr.create("idIndex", "idi", MFnData::kIntArray, &st ); mser;
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	st = addAttribute( aIdIndexPP ); mser;

	aActivePP = tAttr.create("active", "act", MFnData::kDoubleArray, &st ); mser;
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	st = addAttribute( aActivePP ); mser;
	///////////////////////////////////////////////////////////////////////



	// Transformation
	///////////////////////////////////////////////////////////////////////

	aRotateOrder = eAttr.create( "rotateOrder", "ro", mayaMath::xyz);
	eAttr.addField("xyz", mayaMath::xyz);
	eAttr.addField("yzx", mayaMath::yzx);
	eAttr.addField("zxy", mayaMath::zxy);
	eAttr.addField("xzy", mayaMath::xzy);
	eAttr.addField("yxz", mayaMath::yxz);
	eAttr.addField("zyx", mayaMath::zyx);
	eAttr.setKeyable(true);
	eAttr.setHidden(false);
	st = addAttribute( aRotateOrder ); mser;

	aTranslatePP = tAttr.create("translatePP", "tpp", MFnData::kVectorArray, &st ); mser;
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	st = addAttribute( aTranslatePP ); mser;

	aRotatePP = tAttr.create("rotatePP", "rpp", MFnData::kVectorArray, &st ); mser;
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	st = addAttribute( aRotatePP ); mser;

	aRotateUpPP = tAttr.create("rotateUpPP", "rupp", MFnData::kVectorArray, &st ); mser;
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	st = addAttribute( aRotateUpPP ); mser;


	// aScalePP = gAttr.create("scalePP", "spp");
	// gAttr.addDataAccept(MFnData::kDoubleArray);
	// gAttr.addDataAccept(MFnData::kVectorArray);
	// gAttr.setDisconnectBehavior(MFnAttribute::kReset);
	// gAttr.setStorable(false);
	// gAttr.setCached(false);
	// st = addAttribute( aScalePP );mser;

	aScalePP = tAttr.create("scalePP", "spp", MFnData::kDoubleArray, &st ); mser;
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	st = addAttribute( aScalePP ); mser;


	aRotateType = eAttr.create ("rotateType", "rtp");
	eAttr.addField( "aim",		flockShape::kRotateAim);
	eAttr.addField( "phi",	 flockShape::kRotatePhi );
	eAttr.setDefault(	flockShape::kRotateAim);
	eAttr.setHidden(false);
	addAttribute (aRotateType);

	aRotateAimFrontAxis = eAttr.create ("aimFrontAxis", "afa");
	eAttr.addField( "X",		mayaMath::xAxis);
	eAttr.addField( "Y",	 mayaMath::yAxis );
	eAttr.addField( "Z",	 mayaMath::zAxis );
	eAttr.addField( "negX",		mayaMath::xAxisNeg);
	eAttr.addField( "negY",	 mayaMath::yAxisNeg );
	eAttr.addField( "negZ",	 mayaMath::zAxisNeg );
	eAttr.setDefault(	mayaMath::xAxis);
	eAttr.setHidden(false);
	addAttribute (aRotateAimFrontAxis);

	aRotateAimUpAxis = eAttr.create ("aimUpAxis", "aua");
	eAttr.addField( "X",		mayaMath::xAxis);
	eAttr.addField( "Y",	 mayaMath::yAxis );
	eAttr.addField( "Z",	 mayaMath::zAxis );
	eAttr.addField( "negX",		mayaMath::xAxisNeg);
	eAttr.addField( "negY",	 mayaMath::yAxisNeg );
	eAttr.addField( "negZ",	 mayaMath::zAxisNeg );
	eAttr.setDefault(	mayaMath::yAxis);
	eAttr.setHidden(false);
	addAttribute (aRotateAimUpAxis);




	aTranslateTargetX = uAttr.create( "translateTargetX", "ttgx", MFnUnitAttribute::kDistance,
	                                  0.0 );
	aTranslateTargetY = uAttr.create( "translateTargetY", "ttgy", MFnUnitAttribute::kDistance,
	                                  0.0 );
	aTranslateTargetZ = uAttr.create( "translateTargetZ", "ttgz", MFnUnitAttribute::kDistance,
	                                  0.0 );
	aTranslateTarget = nAttr.create( "translateTarget", "ttg", aTranslateTargetX,
	                                 aTranslateTargetY, aTranslateTargetZ );
	nAttr.setStorable(false);
	nAttr.setReadable(true);
	st = addAttribute(aTranslateTarget);

	aRotateTargetX = uAttr.create( "rotateTargetX", "rtgx", MFnUnitAttribute::kAngle, 0.0 );
	aRotateTargetY = uAttr.create( "rotateTargetY", "rtgy", MFnUnitAttribute::kAngle, 0.0 );
	aRotateTargetZ = uAttr.create( "rotateTargetZ", "rtgz", MFnUnitAttribute::kAngle, 0.0 );
	aRotateTarget = nAttr.create( "rotateTarget", "rtg", aRotateTargetX, aRotateTargetY,
	                              aRotateTargetZ );
	nAttr.setStorable(false);
	nAttr.setReadable(true);
	st = addAttribute(aRotateTarget);

	aScaleTargetX = nAttr.create( "scaleTargetX", "stgx", MFnNumericData::kDouble );
	aScaleTargetY = nAttr.create( "scaleTargetY", "stgy", MFnNumericData::kDouble );
	aScaleTargetZ = nAttr.create( "scaleTargetZ", "stgz", MFnNumericData::kDouble);
	aScaleTarget = nAttr.create( "scaleTarget", "stg", aScaleTargetX, aScaleTargetY,
	                             aScaleTargetZ );
	nAttr.setStorable(false);
	nAttr.setReadable(true);
	st = addAttribute(aScaleTarget);


	aTargetId = nAttr.create("targetId", "tid", MFnNumericData::kLong);
	nAttr.setWritable(true);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute (aTargetId);
	///////////////////////////////////////////////////////////////////////

	////////USER DEFINED ATTS ///////////////////////////////////
	aUserPP = tAttr.create("userPP", "upp", MFnData::kDoubleArray, &st ); mser;
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	st = addAttribute( aUserPP ); mser; // not sure if this needed

	aUserTarget = nAttr.create("userTarget", "utg", MFnNumericData::kDouble);
	nAttr.setWritable(true);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);

	aUserLabel = tAttr.create("userLabel", "ulb", MFnData::kString);
	tAttr.setStorable(true);
	tAttr.setWritable(true);

	aUserMap = cAttr.create("userMap", "usm");
	cAttr.addChild(aUserPP);
	cAttr.addChild(aUserTarget);
	cAttr.addChild(aUserLabel);
	cAttr.setArray(true);
	cAttr.setIndexMatters(false);
	st = addAttribute(aUserMap);

	////////USER DEFINED VECTOR ATTS ///////////////////////////////////
	aUserVectorPP = tAttr.create("userVectorPP", "uvpp", MFnData::kVectorArray, &st ); mser;
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	st = addAttribute( aUserVectorPP ); mser; // not sure if this needed

	aUserVectorTargetX = nAttr.create( "userVectorTargetX", "vtgx", MFnNumericData::kDouble ,
	                                   1.0  );
	aUserVectorTargetY = nAttr.create( "userVectorTargetY", "vtgy", MFnNumericData::kDouble ,
	                                   1.0  );
	aUserVectorTargetZ = nAttr.create( "userVectorTargetZ", "vtgz", MFnNumericData::kDouble ,
	                                   1.0  );
	aUserVectorTarget = nAttr.create( "userVectorTarget", "uvtg", aUserVectorTargetX,
	                                  aUserVectorTargetY, aUserVectorTargetZ );
	nAttr.setWritable(true);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);

	aUserVectorLabel = tAttr.create("userVectorLabel", "uvlb", MFnData::kString);
	tAttr.setStorable(true);
	tAttr.setWritable(true);

	aUserVectorMap = cAttr.create("userVectorMap", "uvsm");
	cAttr.addChild(aUserVectorPP);
	cAttr.addChild(aUserVectorTarget);
	cAttr.addChild(aUserVectorLabel);
	cAttr.setArray(true);
	cAttr.setIndexMatters(false);
	st = addAttribute(aUserVectorMap);



	/* FLAT TARGETS */
	aDoFlatTargets = nAttr.create( "doFlatTargets", "dftg", MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(false);
	nAttr.setDefault(false);
	st = addAttribute(aDoFlatTargets);

	aFlatTranslateTargetX = uAttr.create( "flatTranslateTargetX", "fttgx",
	                                      MFnUnitAttribute::kDistance, 0.0 );
	aFlatTranslateTargetY = uAttr.create( "flatTranslateTargetY", "fttgy",
	                                      MFnUnitAttribute::kDistance, 0.0 );
	aFlatTranslateTargetZ = uAttr.create( "flatTranslateTargetZ", "fttgz",
	                                      MFnUnitAttribute::kDistance, 0.0 );
	aFlatTranslateTarget = nAttr.create(  "flatTranslateTarget",  "fttg",
	                                      aFlatTranslateTargetX, aFlatTranslateTargetY, aFlatTranslateTargetZ );
	nAttr.setStorable(false);
	nAttr.setReadable(true);
	nAttr.setWritable(false);

	aFlatRotateTargetX = uAttr.create( "flatRotateTargetX", "frtgx", MFnUnitAttribute::kAngle,
	                                   0.0 );
	aFlatRotateTargetY = uAttr.create( "flatRotateTargetY", "frtgy", MFnUnitAttribute::kAngle,
	                                   0.0 );
	aFlatRotateTargetZ = uAttr.create( "flatRotateTargetZ", "frtgz", MFnUnitAttribute::kAngle,
	                                   0.0 );
	aFlatRotateTarget = nAttr.create( "flatRotateTarget", "frtg", aFlatRotateTargetX,
	                                  aFlatRotateTargetY, aFlatRotateTargetZ );
	nAttr.setStorable(false);
	nAttr.setReadable(true);
	nAttr.setWritable(false);


	aFlatScaleTargetX = nAttr.create( "flatScaleTargetX", "fstgx", MFnNumericData::kDouble );
	aFlatScaleTargetY = nAttr.create( "flatScaleTargetY", "fstgy", MFnNumericData::kDouble );
	aFlatScaleTargetZ = nAttr.create( "flatScaleTargetZ", "fstgz", MFnNumericData::kDouble);
	aFlatScaleTarget = nAttr.create( "flatScaleTarget", "fstg", aFlatScaleTargetX,
	                                 aFlatScaleTargetY, aFlatScaleTargetZ );
	nAttr.setStorable(false);
	nAttr.setReadable(true);
	nAttr.setWritable(false);


	aFlatUserDoubleTarget = nAttr.create("flatUserDoubleTarget", "fudtg",
	                                     MFnNumericData::kDouble);
	nAttr.setStorable(false);
	nAttr.setReadable(true);
	nAttr.setWritable(false);
	nAttr.setArray(true);
	nAttr.setUsesArrayDataBuilder( true );
	nAttr.setIndexMatters(true);

	aFlatUserVectorTargetX = nAttr.create( "flatUserVectorTargetX", "fuvtgx",
	                                       MFnNumericData::kDouble , 1.0  );
	aFlatUserVectorTargetY = nAttr.create( "flatUserVectorTargetY", "fuvtgy",
	                                       MFnNumericData::kDouble , 1.0  );
	aFlatUserVectorTargetZ = nAttr.create( "flatUserVectorTargetZ", "fuvtgz",
	                                       MFnNumericData::kDouble , 1.0  );
	aFlatUserVectorTarget = nAttr.create( "flatUserVectorTarget", "fuvtg",
	                                      aFlatUserVectorTargetX, aFlatUserVectorTargetY, aFlatUserVectorTargetZ );
	nAttr.setStorable(false);
	nAttr.setReadable(true);
	nAttr.setWritable(false);

	nAttr.setArray(true);
	nAttr.setUsesArrayDataBuilder( true );
	nAttr.setIndexMatters(true);

	aFlatTargets = cAttr.create("flatTargets", "ftg");
	cAttr.addChild(aFlatTranslateTarget);
	cAttr.addChild(aFlatRotateTarget);
	cAttr.addChild(aFlatScaleTarget);
	cAttr.addChild(aFlatUserDoubleTarget);
	cAttr.addChild(aFlatUserVectorTarget);
	cAttr.setStorable(false);
	cAttr.setReadable(true);
	cAttr.setWritable(false);
	cAttr.setKeyable(true);
	cAttr.setArray(true);
	cAttr.setUsesArrayDataBuilder( true );
	cAttr.setIndexMatters(true);
	st = addAttribute(aFlatTargets);




	///////////////////////////////////////////////////////////////////////

	// TODO

	// aLodBBMin = nAttr.create( "lodBBMin", "lbn",  MFnNumericData::k3Double );
	// nAttr.setHidden(false);

	// aLodBBMax = nAttr.create( "lodBBMax", "lbx",  MFnNumericData::k3Double );
	// nAttr.setHidden(false);

	// aLodBBSiz = nAttr.create( "lodBBSiz", "lbs",  MFnNumericData::k3Double );
	// nAttr.setHidden(false);

	// aLodBB = cAttr.create("lodBB", "lbb");
	// cAttr.addChild(aLodBBMin);
	// cAttr.addChild(aLodBBMax);
	// cAttr.addChild(aLodBBSiz);
	// cAttr.setStorable(true);
	// cAttr.setHidden(false);

	// st = addAttribute(aLodBB);mser

	// aLodGeometrySet = mAttr.create("lodGeometrySet","lgse");
	// aLodGeometryStart = nAttr.create( "lodGeometryStart", "lgst",  MFnNumericData::kFloat);
	// nAttr.setKeyable(true);
	// aLodGeometryLo= nAttr.create( "lodGeometryLo", "lglo",  MFnNumericData::kFloat);
	// nAttr.setKeyable(true);
	// aLodGeometryHigh= nAttr.create( "lodGeometryHigh", "lghi",  MFnNumericData::kFloat);
	// nAttr.setKeyable(true);
	// aLodGeometryEnd= nAttr.create( "lodGeometryEnd", "lgen",  MFnNumericData::kFloat);
	// nAttr.setKeyable(true);


	// aLodGeometry = cAttr.create("lodGeometry", "lg");
	// cAttr.addChild(aLodGeometrySet);
	// cAttr.addChild(aLodGeometryStart);
	// cAttr.addChild(aLodGeometryLo);
	// cAttr.addChild(aLodGeometryHigh);
	// cAttr.addChild(aLodGeometryEnd);
	// cAttr.setArray(true);
	// cAttr.setIndexMatters(false);
	// //cAttr.setUsesArrayDataBuilder(true);
	// st = addAttribute(aLodGeometry);mser;


	aPointsBBMin = nAttr.create( "pointsBBMin", "pbn",  MFnNumericData::k3Double );
	nAttr.setHidden(false);
	//	nAttr.setKeyable(true);
	aPointsBBMax = nAttr.create( "pointsBBMax", "pbx",  MFnNumericData::k3Double );
	nAttr.setHidden(false);
	//	nAttr.setKeyable(true);
	aPointsBBSiz = nAttr.create( "pointsBBSiz", "pbs",  MFnNumericData::k3Double );
	nAttr.setHidden(false);
	//	nAttr.setKeyable(true);
	aPointsBB = cAttr.create("pointsBB", "pbb");
	cAttr.addChild(aPointsBBMin);
	cAttr.addChild(aPointsBBMax);
	cAttr.addChild(aPointsBBSiz);
	cAttr.setStorable(true);
	cAttr.setHidden(false);
	st = addAttribute(aPointsBB); mser;



	aMesh = tAttr.create("mesh", "ms", MFnData::kMesh);

	aMeshActive = nAttr.create( "meshActive", "mac", MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);

	////////////////////////////////////
	aInMeshes = cAttr.create("inMeshes", "inm");
	cAttr.addChild(aMesh);
	cAttr.addChild(aMeshActive);
	cAttr.setArray( true );
	cAttr.setDisconnectBehavior(MFnAttribute::kDelete);
	cAttr.setReadable(false);
	st = addAttribute( aInMeshes ); mser;


	aOutIds = tAttr.create("outIds", "oids", MFnData::kIntArray, &st ); mser;
	tAttr.setStorable(false);
	st = addAttribute( aOutIds ); mser;

	aCullList = tAttr.create( "cullList", "cls", MFnData::kString );
	tAttr.setStorable(true);
	addAttribute( aCullList );

	aOutMeshes = tAttr.create( "outMeshes", "oum", MFnData::kMesh );
	tAttr.setStorable(false);
	tAttr.setWritable(false);
	tAttr.setArray( true );
	tAttr.setUsesArrayDataBuilder( true );
	tAttr.setIndexMatters(true);
	tAttr.setDisconnectBehavior(MFnAttribute::kDelete);
	addAttribute( aOutMeshes );

	aCurrentTime = uAttr.create( "currentTime", "ct", MFnUnitAttribute::kTime );
	uAttr.setStorable(true);
	st =  addAttribute(aCurrentTime);  mser;




	st = attributeAffects(aCurrentTime , aOutMeshes ); mser;
	st = attributeAffects(aCurrentTime , aOutIds ); mser;
	// st =attributeAffects(aCullList ,aOutMeshes );mser;
	// st =attributeAffects(aCullList ,aOutIds );mser;
	// st =attributeAffects(aActivePP ,aOutMeshes );mser;
	// st =attributeAffects(aActivePP ,aOutIds );mser;



	// st =attributeAffects(aCurrentTime ,aFlatTargets );mser;
	// st =attributeAffects(aCullList ,aFlatTargets );mser;
	// st =attributeAffects(aActivePP ,aFlatTargets );mser;

	st = attributeAffects(aCurrentTime , aFlatTranslateTargetX ); mser;
	// st =attributeAffects(aCullList ,aFlatTranslateTargetX );mser;
	// st =attributeAffects(aActivePP ,aFlatTranslateTargetX );mser;

	st = attributeAffects(aCurrentTime , aFlatTranslateTargetY ); mser;
	// st =attributeAffects(aCullList ,aFlatTranslateTargetY );mser;
	// st =attributeAffects(aActivePP ,aFlatTranslateTargetY );mser;

	st = attributeAffects(aCurrentTime , aFlatTranslateTargetZ ); mser;
	// st =attributeAffects(aCullList ,aFlatTranslateTargetZ );mser;
	// st =attributeAffects(aActivePP ,aFlatTranslateTargetZ );mser;

	st = attributeAffects(aCurrentTime , aFlatTranslateTarget ); mser;
	// st =attributeAffects(aCullList ,aFlatTranslateTarget );mser;
	// st =attributeAffects(aActivePP ,aFlatTranslateTarget );mser;

	st = attributeAffects(aCurrentTime , aFlatRotateTargetX ); mser;
	// st =attributeAffects(aCullList ,aFlatRotateTargetX );mser;
	// st =attributeAffects(aActivePP ,aFlatRotateTargetX );mser;

	st = attributeAffects(aCurrentTime , aFlatRotateTargetY ); mser;
	// st =attributeAffects(aCullList ,aFlatRotateTargetY );mser;
	// st =attributeAffects(aActivePP ,aFlatRotateTargetY );mser;

	st = attributeAffects(aCurrentTime , aFlatRotateTargetZ ); mser;
	// st =attributeAffects(aCullList ,aFlatRotateTargetZ );mser;
	// st =attributeAffects(aActivePP ,aFlatRotateTargetZ );mser;

	st = attributeAffects(aCurrentTime , aFlatRotateTarget ); mser;
	// st =attributeAffects(aCullList ,aFlatRotateTarget );mser;
	// st =attributeAffects(aActivePP ,aFlatRotateTarget );mser;

	st = attributeAffects(aCurrentTime , aFlatScaleTargetX ); mser;
	// st =attributeAffects(aCullList ,aFlatScaleTargetX );mser;
	// st =attributeAffects(aActivePP ,aFlatScaleTargetX );mser;

	st = attributeAffects(aCurrentTime , aFlatScaleTargetY ); mser;
	// st =attributeAffects(aCullList ,aFlatScaleTargetY );mser;
	// st =attributeAffects(aActivePP ,aFlatScaleTargetY );mser;

	st = attributeAffects(aCurrentTime , aFlatScaleTargetZ ); mser;
	// st =attributeAffects(aCullList ,aFlatScaleTargetZ );mser;
	// st =attributeAffects(aActivePP ,aFlatScaleTargetZ );mser;

	st = attributeAffects(aCurrentTime , aFlatScaleTarget ); mser;
	// st =attributeAffects(aCullList ,aFlatScaleTarget );mser;
	// st =attributeAffects(aActivePP ,aFlatScaleTarget );mser;

	st = attributeAffects(aCurrentTime , aFlatUserDoubleTarget ); mser;
	// st =attributeAffects(aCullList ,aFlatUserDoubleTarget );mser;
	// st =attributeAffects(aActivePP ,aFlatUserDoubleTarget );mser;

	st = attributeAffects(aCurrentTime , aFlatUserVectorTargetX ); mser;
	// st =attributeAffects(aCullList ,aFlatUserVectorTargetX );mser;
	// st =attributeAffects(aActivePP ,aFlatUserVectorTargetX );mser;

	st = attributeAffects(aCurrentTime , aFlatUserVectorTargetY ); mser;
	// st =attributeAffects(aCullList ,aFlatUserVectorTargetY );mser;
	// st =attributeAffects(aActivePP ,aFlatUserVectorTargetY );mser;

	st = attributeAffects(aCurrentTime , aFlatUserVectorTargetZ ); mser;
	// st =attributeAffects(aCullList ,aFlatUserVectorTargetZ );mser;
	// st =attributeAffects(aActivePP ,aFlatUserVectorTargetZ );mser;

	st = attributeAffects(aCurrentTime , aFlatUserVectorTarget ); mser;
	// st =attributeAffects(aCullList ,aFlatUserVectorTarget );mser;
	// st =attributeAffects(aActivePP ,aFlatUserVectorTarget );mser;



	// st =attributeAffects(aCurrentTime,aTranslateTargetX  );mser;
	// st =attributeAffects(aCurrentTime,aTranslateTargetY  );mser;
	// st =attributeAffects(aCurrentTime,aTranslateTargetZ  );mser;
	// st =attributeAffects(aCurrentTime,aTranslateTarget  );mser;
	// st =attributeAffects(aCurrentTime,aRotateTargetX );mser;
	// st =attributeAffects(aCurrentTime,aRotateTargetY  );mser;
	// st =attributeAffects(aCurrentTime,aRotateTargetZ  );mser;
	// st =attributeAffects(aCurrentTime,aRotateTarget );mser;
	// st =attributeAffects(aCurrentTime,aScaleTargetX );mser;
	// st =attributeAffects(aCurrentTime,aScaleTargetY );mser;
	// st =attributeAffects(aCurrentTime,aScaleTargetZ );mser;
	// st =attributeAffects(aCurrentTime,aScaleTarget );mser;
	// st =attributeAffects(aCurrentTime,aUserMap );mser;


	return ( MS::kSuccess );

}

void flockShape::drawABox(const MVector &bmin, const MVector &bmax) {
	glBegin( GL_LINES );

	////////////////// BASE
	glVertex3f(float(bmin.x), float(bmin.y), float(bmin.z));
	glVertex3f(float(bmin.x), float(bmin.y), float(bmax.z));

	glVertex3f(float(bmin.x), float(bmin.y), float(bmax.z));
	glVertex3f(float(bmax.x), float(bmin.y), float(bmax.z));

	glVertex3f(float(bmax.x), float(bmin.y), float(bmax.z));
	glVertex3f(float(bmax.x), float(bmin.y), float(bmin.z));

	glVertex3f(float(bmax.x), float(bmin.y), float(bmin.z));
	glVertex3f(float(bmin.x), float(bmin.y), float(bmin.z));

	////////////////// TOP
	glVertex3f(float(bmin.x), float(bmax.y), float(bmin.z));
	glVertex3f(float(bmin.x), float(bmax.y), float(bmax.z));

	glVertex3f(float(bmin.x), float(bmax.y), float(bmax.z));
	glVertex3f(float(bmax.x), float(bmax.y), float(bmax.z));

	glVertex3f(float(bmax.x), float(bmax.y), float(bmax.z));
	glVertex3f(float(bmax.x), float(bmax.y), float(bmin.z));

	glVertex3f(float(bmax.x), float(bmax.y), float(bmin.z));
	glVertex3f(float(bmin.x), float(bmax.y), float(bmin.z));

	/////////////////// LEGS
	glVertex3f(float(bmin.x), float(bmin.y), float(bmin.z));
	glVertex3f(float(bmin.x), float(bmax.y), float(bmin.z));

	glVertex3f(float(bmin.x), float(bmin.y), float(bmax.z));
	glVertex3f(float(bmin.x), float(bmax.y), float(bmax.z));

	glVertex3f(float(bmax.x), float(bmin.y), float(bmax.z));
	glVertex3f(float(bmax.x), float(bmax.y), float(bmax.z));

	glVertex3f(float(bmax.x), float(bmin.y), float(bmin.z));
	glVertex3f(float(bmax.x), float(bmax.y), float(bmin.z));
	glEnd();

	// make the corners big points
	glPushAttrib(GL_CURRENT_BIT);
	glPointSize(5);
	glBegin( GL_POINTS );
	glVertex3f(float(bmin.x), float(bmin.y), float(bmin.z));
	glVertex3f(float(bmin.x), float(bmin.y), float(bmax.z));
	glVertex3f(float(bmin.x), float(bmax.y), float(bmin.z));
	glVertex3f(float(bmin.x), float(bmax.y), float(bmax.z));
	glVertex3f(float(bmax.x), float(bmin.y), float(bmin.z));
	glVertex3f(float(bmax.x), float(bmin.y), float(bmax.z));
	glVertex3f(float(bmax.x), float(bmax.y), float(bmin.z));
	glVertex3f(float(bmax.x), float(bmax.y), float(bmax.z));
	glEnd();
	glPopAttrib();

}



void flockShape::draw(
  M3dView &view,
  const MDagPath &path,
  M3dView::DisplayStyle style,
  M3dView::DisplayStatus status )
{
	MStatus st;

	MObject thisNode = thisMObject();
	MPlug ptsPlug( thisNode, aPointsBB );
	//MPlug lodPlug( thisNode, aLodBB );
	MVector pbmin, pbmax;
	//MVector lbmin,lbmax;
	st = bbValue(ptsPlug, pbmin, pbmax) ;
	//st= bbValue(lodPlug, lbmin, lbmax) ;

	view.beginGL();
	//drawABox(lbmin, lbmax);
	drawABox(pbmin, pbmax);
	// drawABox(pbmin+lbmin, pbmax+lbmax);
	// drawBoxCorners(pbmin,pbmax);
	view.endGL();

}


bool flockShape::isBounded() const {
	return false;
}


MBoundingBox flockShape::boundingBox() const {
	//	MStatus st;
	MBoundingBox bb;

	return bb;
}

/**
The compute method is concerned only with generating an output mesh in the maya scene

	Strategies:

A:
Rig Loop = In this strategy we use an external rig.
	The output mesh deformations are arbitrary and will be the same as the delight render
	For every particle
	we set values on TRS + User maps + DetailTarget
	we then request geometry from inMesh and gradually build outMesh;


B: An optimization would be to do a topology check to avoid creating a new mesh every frame;


C: We register a callback that deforms and positions some input geometry without using an external maya rig.
	This will be much faster but will not be as flexible in all likelyhood;
*/


MStatus flockShape::computeTarget(MDataBlock &data) {
	MStatus st;
	MObject thisNode = thisMObject();
	MTime cT =  data.inputValue( aCurrentTime).asTime();

	// cerr << "cT: " << cT << endl;
	bool doFlatTargets = data.inputValue(aDoFlatTargets).asBool();


	MPlug txp(thisNode, aTranslateTargetX);
	MPlug typ(thisNode, aTranslateTargetY);
	MPlug tzp(thisNode, aTranslateTargetZ);
	MPlug rxp(thisNode, aRotateTargetX);
	MPlug ryp(thisNode, aRotateTargetY);
	MPlug rzp(thisNode, aRotateTargetZ);
	MPlug sxp(thisNode, aScaleTargetX);
	MPlug syp(thisNode, aScaleTargetY);
	MPlug szp(thisNode, aScaleTargetZ);
	MPlug userMapPlug(thisNode, aUserMap);
	MPlug userVectorMapPlug(thisNode, aUserVectorMap);

	// all the user target plugs whether connected or not
	MArrayDataHandle hUserMap = data.inputArrayValue( aUserMap, &st );
	MPlugArray userTargetPlugs;
	unsigned nUserPlugs = hUserMap.elementCount();
	for (unsigned i = 0; i < nUserPlugs; i++) {
		MPlug userMapCompoundPlug = userMapPlug.elementByPhysicalIndex(i, &st); mser;
		MPlug userTargetPlug = userMapCompoundPlug.child(aUserTarget, &st); mser;
		userTargetPlugs.append(userTargetPlug);
	}

	MArrayDataHandle hUserVectorMap = data.inputArrayValue( aUserVectorMap, &st );
	MPlugArray userVectorTargetPlugs;
	unsigned nUserVectorPlugs = hUserVectorMap.elementCount();
	for (unsigned i = 0; i < nUserVectorPlugs; i++) {
		MPlug userVectorMapCompoundPlug = userVectorMapPlug.elementByPhysicalIndex(i, &st); mser;
		MPlug userVectorTargetPlug = userVectorMapCompoundPlug.child(aUserVectorTarget, &st);
		mser;
		userVectorTargetPlugs.append(userVectorTargetPlug);
	}



	short rotateOrder = data.inputValue(aRotateOrder).asShort() + 1;
	MTransformationMatrix::RotationOrder order = MTransformationMatrix::RotationOrder(
	      rotateOrder);


	// short rotateOrder = data.inputValue(aRotateOrder).asShort();
	// MTransformationMatrix::RotationOrder ord = mayaMath::mtmRotOrder(rotateOrder);
	unsigned targetId = data.inputValue(aTargetId).asLong();

	//	tracer;


	rigSlice theSlice(thisNode, data);
	const particleStack stack =  theSlice.stack();
	unsigned stackSize = unsigned(theSlice.stack().size());

	//cerr << "stack size "<< stack.size() << endl;

	// this is the pointer to the particle whose values we want to set
	// on the output plugs ;
	particleStack::const_iterator stack_iter_ultimate_target = stack.end();
	particleStack::const_iterator stack_iter = stack.begin();



	MArrayDataHandle hInMeshes = data.inputArrayValue(aInMeshes, &st );

	m_pClonerStack->conform(thisMObject(), hInMeshes, stackSize);

	MIntArray stackIds;

	MPlug flatTargetsPlug(thisNode, aFlatTargets);


	// loop over each particle in the stack	and push meshes in the clonerStack
	while (stack_iter != stack.end()) {
		int stackId = (*stack_iter)->id();
		stackIds.append(stackId);

		// remember this particle if it is the id we want to track on the output plug


		if (targetId == (*stack_iter)->id()) {
			stack_iter_ultimate_target = stack_iter;
		}


		st =  (*stack_iter)->setPlugValues(
		        txp, typ, tzp, rxp, ryp, rzp, sxp, syp, szp,
		        userTargetPlugs, userVectorTargetPlugs, order
		      ); mser;

		m_pClonerStack->pushPoints();





		/////////////////////////////////////
		// Set the flat plugs
		// MPlug flatTargetsPlug(thisNode, aFlatTargets);

		MPlug flatElementPlug = flatTargetsPlug.elementByLogicalIndex(stackId, &st); mser;

		MPlug flatTranslatePlug = flatElementPlug.child(aFlatTranslateTarget, &st); mser;
		MPlug flatTranslatePlugX = flatTranslatePlug.child(aFlatTranslateTargetX, &st); mser;
		MPlug flatTranslatePlugY = flatTranslatePlug.child(aFlatTranslateTargetY, &st); mser;
		MPlug flatTranslatePlugZ = flatTranslatePlug.child(aFlatTranslateTargetZ, &st); mser;

		MPlug flatRotatePlug = flatElementPlug.child(aFlatRotateTarget, &st); mser;
		MPlug flatRotatePlugX = flatRotatePlug.child(aFlatRotateTargetX, &st); mser;
		MPlug flatRotatePlugY = flatRotatePlug.child(aFlatRotateTargetY, &st); mser;
		MPlug flatRotatePlugZ = flatRotatePlug.child(aFlatRotateTargetZ, &st); mser;

		MPlug flatScalePlug = flatElementPlug.child(aFlatScaleTarget, &st); mser;
		MPlug flatScalePlugX = flatScalePlug.child(aFlatScaleTargetX, &st); mser;
		MPlug flatScalePlugY = flatScalePlug.child(aFlatScaleTargetY, &st); mser;
		MPlug flatScalePlugZ = flatScalePlug.child(aFlatScaleTargetZ, &st); mser;


		MPlug flatUserDoublePlug = flatElementPlug.child(aFlatUserDoubleTarget, &st); mser;
		MPlug flatUserVectorPlug = flatElementPlug.child(aFlatUserVectorTarget, &st); mser;

		// all the user target plugs whether connected or not

		MPlugArray flatUserDoublePlugs;
		for (unsigned i = 0; i < nUserPlugs; i++) {
			MPlug flatDoublePlug = flatUserDoublePlug.elementByPhysicalIndex(i, &st); mser;
			flatUserDoublePlugs.append(flatDoublePlug);
		}

		MPlugArray flatUserVectorPlugs;
		for (unsigned i = 0; i < nUserVectorPlugs; i++) {
			MPlug flatVectorPlug = flatUserVectorPlug.elementByPhysicalIndex(i, &st); mser;
			flatUserVectorPlugs.append(flatVectorPlug);
		}

		st =  (*stack_iter)->setPlugValues(
		        flatTranslatePlugX, flatTranslatePlugY, flatTranslatePlugZ,
		        flatRotatePlugX, flatRotatePlugY, flatRotatePlugZ,
		        flatScalePlugX, flatScalePlugY, flatScalePlugZ,
		        flatUserDoublePlugs, flatUserVectorPlugs, order
		      ); mser;

		/////////////////////////////////////



		stack_iter++;
	}

	// now set the output plugs one last time so the rig ends up where we want it.
	if (stack_iter_ultimate_target != stack.end()) {
		st =  (*stack_iter_ultimate_target)->setPlugValues(
		        txp, typ, tzp, rxp, ryp, rzp, sxp, syp, szp,
		        userTargetPlugs, userVectorTargetPlugs, order
		      ); mser;
	}
	else {
		rigParticle p(nUserPlugs, nUserVectorPlugs);
		p.setPlugValues(
		  txp, typ, tzp, rxp, ryp, rzp, sxp, syp, szp,
		  userTargetPlugs, userVectorTargetPlugs, order
		); mser;
	}
	// cerr << "stackIds " << stackIds << endl;

	MArrayDataHandle  hOutput = data.outputArrayValue( aOutMeshes, &st ); mser;

	m_pClonerStack->prepareOutGeometry();
	MArrayDataBuilder bOutput = hOutput.builder();
	m_pClonerStack->doOutput(bOutput);

	hOutput.setAllClean();


	///////////////////////////////////////////////////////////////
	MDataHandle hOutIds = data.outputValue(aOutIds, &st);
	MFnIntArrayData fnOutIds;
	MObject dOutIds = fnOutIds.create( stackIds, &st ); mser;
	hOutIds.set(dOutIds);
	data.setClean(aOutIds);
	///////////////////////////////////////////////////////////////

	return MS::kSuccess;
}


// MStatus flockShape::computeFlatTargets(MDataBlock& data){
// 	MStatus st;



// 	// cerr << "flockShape::computeFlatTargets" << endl;

// 	MObject thisNode = thisMObject();
// 	MTime cT =  data.inputValue( aCurrentTime).asTime();

// 	short rotateOrder = data.inputValue(aRotateOrder).asShort()+1;
// 	MTransformationMatrix::RotationOrder order =
// 	MTransformationMatrix::RotationOrder(rotateOrder);



// 	rigSlice theSlice(thisNode, data);
// 	const particleStack stack =  theSlice.stack();
// 	unsigned stackSize = unsigned(theSlice.stack().size());

// 	particleStack::const_iterator stack_iter = stack.begin();



// 	// set up builder stuff
// 	MArrayDataHandle  haFlatTargets = data.outputArrayValue( aFlatTargets, &st ); mser;
// 	MArrayDataBuilder   bFlatTargets = haFlatTargets.builder();

// 	MIntArray stackIds;



// 	// cerr << "stack_iter: " << endl;

// 	while (stack_iter != stack.end()) {
// 		int id = (*stack_iter)->id();

// 		// cerr << "(*stack_iter)->id(): " << id << endl;

// 		MDataHandle hFlatTarget = bFlatTargets.addElement(id);

// 		const MFloatMatrix & fMat =  (*stack_iter)->transform();

// 		MMatrix mmat(fMat.matrix);
// 		cerr << "mmat" << mmat << endl;
// 		MTransformationMatrix mtmat(mmat);
// 		// cerr << "ID: " << id << "  " << fMat << endl;
//  		 // cerr << "ID: " << id << "  " << mmat << endl;


// 		double rotateValue[3];
// 		double scaleValue[3];
// 		MVector translateValue;
// 		mtmat.getRotation( rotateValue, order );
// 		translateValue = mtmat.getTranslation(MSpace::kTransform);
// 		mtmat.getScale(scaleValue, MSpace::kTransform);


// 		MDataHandle hTranslateTarget =  hFlatTarget.child(aFlatTranslateTarget);
// 		double3 &outTranslate = hTranslateTarget.asDouble3();
// 		outTranslate[0] = translateValue.x;
// 		outTranslate[1] = translateValue.y;
// 		outTranslate[2] = translateValue.z;

// 		MDataHandle hRotateTarget =  hFlatTarget.child(aFlatRotateTarget);
// 		double3 &outRotate = hRotateTarget.asDouble3();
// 		outRotate[0] = rotateValue[0];
// 		outRotate[1] = rotateValue[1];
// 		outRotate[2] = rotateValue[2];

// 		MDataHandle hScaleTarget =  hFlatTarget.child(aFlatScaleTarget);
// 		double3 &outScale = hScaleTarget.asDouble3();
// 		outScale[0] = scaleValue[0];
// 		outScale[1] = scaleValue[1];
// 		outScale[2] = scaleValue[2];


// 		const MFloatArray & floatValues =  (*stack_iter)->values()  ;
// 		MDataHandle hFlatUserDoubleArray =  hFlatTarget.child(aFlatUserDoubleTarget);
// 		MArrayDataHandle	haFlatUserDoubleTarget(hFlatUserDoubleArray, &st);mser;
// 		MArrayDataBuilder  bFlatUserDoubleTarget = haFlatUserDoubleTarget.builder();
// 		for(unsigned i = 0;i < floatValues.length(); i++) {
// 			MDataHandle hFlatUserDoubleTarget =  bFlatUserDoubleTarget.addElement(i);
// 			double &flatUserDoubleTarget = hFlatUserDoubleTarget.asDouble();
// 			flatUserDoubleTarget = double(floatValues[i]);
// 		}



// 		const MFloatVectorArray & vectorValues = (*stack_iter)->vectorValues();
// 		MDataHandle hFlatUserVectorArray =  hFlatTarget.child(aFlatUserVectorTarget);
// 		MArrayDataHandle	haFlatUserVectorTarget(hFlatUserVectorArray, &st);mser;
// 		MArrayDataBuilder  bFlatUserVectorTarget = haFlatUserVectorTarget.builder();
// 		for(unsigned i = 0;i < vectorValues.length(); i++) {
// 			MDataHandle hFlatUserVectorTarget =  bFlatUserVectorTarget.addElement(i);
// 			double3 &flatUserVectorTarget = hFlatUserVectorTarget.asDouble3();
// 			flatUserVectorTarget[0] = vectorValues[i].x;
// 			flatUserVectorTarget[1] = vectorValues[i].y;
// 			flatUserVectorTarget[2] = vectorValues[i].z;
// 		}

// 		stackIds.append(id);
// 		stack_iter++;
// 	}
//   // cerr << "Done stack_iterartion " << endl;

// 	haFlatTargets.setAllClean();
//   // cerr << "haFlatTargets.setAllClean()" << endl;


// 	// ///////////////////////////////////////////////////////////////
// 	// MDataHandle hOutIds = data.outputValue(aOutIds, &st);
// 	// MFnIntArrayData fnOutIds;
// 	// MObject dOutIds = fnOutIds.create( stackIds, &st ); mser;
// 	// hOutIds.set(dOutIds);
// 	// data.setClean(aOutIds);
// 	// ///////////////////////////////////////////////////////////////

// 	// cerr << "data.setClean(aOutIds)" << endl;

// 	return MS::kSuccess;
// }

MStatus flockShape::compute(const MPlug &plug, MDataBlock &data) {
	//	tracer;
	MStatus st;


	if (! ( (plug == aOutMeshes)
	        ||  (plug.isElement() && plug.array() == aOutMeshes)
	        ||  (plug == aOutIds)
	      ) ) { return MS::kUnknownParameter; }

	st = computeTarget(data);
	return MS::kSuccess;


	// bool isFlatTargetPlug = (
	// 	(plug == aFlatTranslateTargetX)
	// 	||  (plug == aFlatTranslateTargetY)
	// 	||  (plug == aFlatTranslateTargetZ)
	// 	||  (plug == aFlatTranslateTarget)
	// 	||  (plug == aFlatRotateTargetX)
	// 	||  (plug == aFlatRotateTargetY)
	// 	||  (plug == aFlatRotateTargetZ)
	// 	||  (plug == aFlatRotateTarget)
	// 	||  (plug == aFlatScaleTargetX)
	// 	||  (plug == aFlatScaleTargetY)
	// 	||  (plug == aFlatScaleTargetZ)
	// 	||  (plug == aFlatScaleTarget)
	// 	||  (plug == aFlatUserDoubleTarget)
	// 	||  (plug == aFlatUserVectorTargetX)
	// 	||  (plug == aFlatUserVectorTargetY)
	// 	||  (plug == aFlatUserVectorTargetZ)
	// 	||  (plug == aFlatUserVectorTarget)
	// 	||  (plug == aFlatTargets)
	// 	);

	// 	bool isValidPlug = (



	// 	)


	// bool isMeshesPug = (((plug.isElement()) && (plug.array() == aOutMeshes))  || (plug == aOutMeshes) );

	// cerr << "------------------------------------------------- " << endl;
	// cerr << "PLUG: " << plug.name() << endl;
	// 	cerr << "plug.isElement(): " << plug.isElement() << endl;
	// cerr << "(plug.array() == aOutMeshes): " << (plug.array() == aOutMeshes) << endl;
	// cerr << "isMeshesPug: " << isMeshesPug<< endl;

	// bool doFlatTargets = data.inputValue(aDoFlatTargets).asBool();




	// if (doFlatTargets && isFlatTargetPlug) {
	// 	cerr << "computeFlatTargets(data)"  << endl;
	// 	st = computeFlatTargets(data);
	//   data.setClean(plug);
	// 	return MS::kSuccess;
	// }

	// if ( (!doFlatTargets) && isMeshesPug) {
	// 	cerr << "computeTarget(data)" << endl;
	// 	st = computeTarget(data);
	// 	data.setClean(plug);
	// 	return MS::kSuccess;
	// }





	// return MS::kUnknownParameter;
}





// MStatus flockShape::addSlice(float sampleTime){

// // return
// 	rigSliceMap::const_iterator slice_iter = m_sliceMap.find(sampleTime);
// 	if (slice_iter != m_sliceMap.end()) {
// 		return( MS::kUnknownParameter );
// 	}

// 	MObject thisNode = thisMObject();
// 	MDataBlock  data = forceCache();
// 	rigSlice * theSlice = new rigSlice(thisNode, data);

// 	// every time-slice in the map must be the same length for motoion blur to work.
// 	// however, some particles may die or be born within the motion block.
// 	// this can leave gaps in this slice or the previous slices respectfully
// 	// therefore we need to plug gaps in this slice with respect to the most recent slice,
// 	// and then in all previous slices with respect to this slice.
// 	// We do this by copying and inserting missing particles into the destination list from the source list

// 	// TODO - when filling gaps, rather than copy to a new particle, just point to the original
// 	// (flag as soft, so we know not to delete it)

// 	if (! m_sliceMap.empty()) {

// 	// first, plug gaps where particles died in this slice.
// 		rigSliceMap::reverse_iterator rit  = m_sliceMap.rbegin();
// 		rigSlice * prevSlice = rit->second;
// 		theSlice->plugGaps(*prevSlice);

// 	// now plug gaps in other slices if particles were born in this slice
// 		rigSliceMap::iterator it  = m_sliceMap.begin();
// 		for (;it !=m_sliceMap.end();it++) {
// 			rigSlice * oldSlice = it->second;
// 			oldSlice->plugGaps(*theSlice);
// 		}
// 	}

// 	m_sliceMap[sampleTime] = theSlice;
// 	return MS::kSuccess;
// }

MStatus flockShape::fortifyCache() {
	MStatus st = MS::kSuccess;
	if ( m_sliceMap.size() > 1) {

		rigSliceMap::iterator leftIt  = m_sliceMap.begin();
		rigSliceMap::const_iterator rightIt  = m_sliceMap.begin();
		rightIt++;

		for (; rightIt != m_sliceMap.end(); rightIt++, leftIt++) {
			rigSlice *leftSlice = leftIt->second;
			const rigSlice *rightSlice = rightIt->second;
			st = leftSlice->fortify(*rightSlice);
		}
	}
	return st;
}

// RtVoid deleteProgData(RtString  * data) {
// 	delete data[0];
// 	delete data[1];
// 	delete data;
// }

// MStatus flockShape::emitDetailBlock(
// 	const MStringArray & progStrings,
// 	const std::vector<JFloatArray> & detailRanges,
// 	const MStringArray & lodSetNames,
// 	const MString & argStr,
// 	const MFloatVector & lbminf,
// 	const MFloatVector & lbmaxf
// ) const {

// 	MStatus st;

// 	// cerr << "emitDetailBlock -- progStrings" <<  progStrings << endl;
// 	// cerr << "emitDetailBlock -- lodSetNames" <<  lodSetNames << endl;
// 	// cerr << "emitDetailBlock -- argStr" <<  argStr << endl;


// 	MString detailString="Detail [ ";
// 	for (unsigned i=0;i<3;i++){
// 		detailString+=lbminf[i];
// 		detailString+=" ";
// 		detailString+=lbmaxf[i];
// 		detailString+=" ";
// 	}
// 	detailString+="]\n";


// 	/////detail//////////////////////
// 	unsigned n = progStrings.length();
// // 	if (lodSetNames.length() != n)  cerr << "lodSetNames.length()  " << lodSetNames.length()  << endl;
// 	bool do_detail = (n > 1) ;
// 	RtBound bound = { lbminf.x, lbmaxf.x, lbminf.y, lbmaxf.y, lbminf.z, lbmaxf.z } ;
// 	RiAttributeBegin();
// 	if (do_detail) RiDetail(bound); // bug in RiDetail

// 	// cerr << "detailRanges.length() "<<  detailRanges.size() < endl;
// 	// cerr << "HERE -- AAA" <<  endl;
// 	for (unsigned i=0;i<n;i++) {
// 		//const MString thisArg(lodSetNames[i]+" "+argStr);

// 		if (do_detail)  RiDetailRange(detailRanges[i][0],detailRanges[i][1],detailRanges[i][2],detailRanges[i][3] );
// 		///// build the progData//////////////////////////

// 		char *  argStrChar = new char[argStr.length()+1];
// 		strcpy( argStrChar, argStr.asChar());
// 		char * progStrChar = new char[progStrings[i].length()+1];
// 		strcpy( progStrChar, progStrings[i].asChar());
// 		RtString * progData = new RtString[2];
// 		progData[0] = progStrChar;
// 		progData[1] = argStrChar;
// 		//////////////////////////////////

// 		/// EMIT TWC OBSOLETE
// 		/// RiProcedural(progData,bound,RiProcRunProgram,(RtProcFreeFunc)deleteProgData);

// 	}


// 	RiAttributeEnd(); // detail block
// //////////////////////////////////
// 	return MS::kSuccess;
// }

// MStatus flockShape::emitCache(float relativeFrame)  {
// // need to write in to the rib the archives of attribs and shaders that are attached to the objects in the LOD sets used in this flock

// // need the user map attributes, and the target connections
// // need the lod BB
// // need the reference scene
// // need the LOD sets and their ranges
// 	if (!hasCache()) return MS::kUnknownParameter;

// 	MStatus st;

// 	MObject thisNode = thisMObject();



// ////////////////user defined target plugs////////////////////
// 	MPlug userMapPlug( thisNode, aUserMap );
// 	std::vector<MStringArray> targetPlugsVector = getTargetPlugs(userMapPlug);
// ////////////////////////////////////

// ////////////////detail target plug////////////////////
// //	MPlug detailPlug( thisNode, aDetailTarget );
// //	MStringArray detailTargetConns = getTargetPlugConns(detailPlug);
// ////////////////////////////////////

// // before we emit the cache we traverse the lists in the map
// // making cross links to corresponding elements. This will make it easier to
// // loop through, and will also give us the chance to check the validity of the structure.
// // TODO - The values arrays should be the same size as the targetPlugsVector - so maybe pass those as well.
// 	st = fortifyCache();msert;
// //cerr << "cache is valid and fortified" << endl;
// ////////////////////////////////////


// ////////////////get Bound////////////////////
// 	MPlug lodBBPlug( thisNode, aLodBB );
// 	MVector lbmin,lbmax;
// 	st= bbValue(lodBBPlug, lbmin, lbmax) ;
// 	MFloatVector lbminf(lbmin);
// 	MFloatVector lbmaxf(lbmax);
// ////////////////////////////////////


// ////////////////Motion Block////////////////////
// 	bool doBur =false;
// 	unsigned nSamples = unsigned(m_sliceMap.size());
// 	if (nSamples >1) doBur=true;
// 	JFloatArray relativeBlurSamples;
// 	JFloatArray blurSamples;

// 	rigSliceMap::const_iterator sliceMapIter = m_sliceMap.begin();
// 	while (sliceMapIter != m_sliceMap.end()) {
// 		relativeBlurSamples.push_back(sliceMapIter->first - relativeFrame);
// 		blurSamples.push_back(sliceMapIter->first);
// 		sliceMapIter++;
// 	}
// 	RtFloat * rtSamples = new RtFloat[nSamples];
// 	std::copy( relativeBlurSamples.begin(), relativeBlurSamples.end(), rtSamples);
// ////////////////////////////////////


// 	MPlug geomPlug( thisNode, aLodGeometry );
// ////////////////Archive connected shaders and attribs nodes ////////////////////
// 	st = archiveShadersAndAttribs();mser;
// ////////////////////////////////////





// ////////////////RunProgram Str////////////////////

// 	//enum LODStrategy {kLodInProcedural, kLodInRib  };
// //	MPlug strategyPlug( thisNode, aLodStrategy );
// //	short strat;
// //	st = strategyPlug.getValue(strat);
// //	flockShape::LODStrategy  strategy = (flockShape::LODStrategy)strat;

// 	//cerr << "make prog strings" << endl;


// 	MStringArray progStrings;
// 	MStringArray lodSetNames;
// 	std::vector<JFloatArray> detailRanges;

// 	st = createProgramStrings(geomPlug,progStrings, detailRanges, lodSetNames, blurSamples, relativeFrame) ;

// //	cerr << "progStrings" <<  progStrings << endl;

// //	cerr << "lodSetNames" <<  lodSetNames << endl;


// //	st = createProgramStrings(geomPlug,progStrings, detailRanges, blurSamples, relativeFrame,strategy) ;






// ////////////////////////////////////





// //////////////////////// loop over all particles ///////////////////
// 	const particleStack & firstStack =  m_sliceMap.begin()->second->stack();
// 	particleStack::const_iterator  peg = firstStack.begin();
// 	MString thisNodeName = MFnDependencyNode(thisNode).name();

// 	while (peg != firstStack.end()) {



// 		MString ident =thisNodeName; ident += "["; ident += (*peg)->id(); ident +="]";

// 		const char * namePtr = ident.asChar();
// 		RiAttributeBegin();
// 		RiAttribute( "identifier", "string name", &namePtr, RI_NULL );
// 		RiTransformBegin();
// 		if (doBur) RiMotionBeginV(nSamples, rtSamples);

// 		const rigParticle * currSample = *peg;

// 		std::vector<MStringArray> tpvCopy = targetPlugsVector;
// 		unsigned nPlugs = unsigned(tpvCopy.size());


// 		while (currSample != 0) {

// 		// write the transform
// 		////////////////////////////
// 			RtMatrix ribMatrix;
// 		//cerr << "currSample->transform() " << currSample->transform() << endl;
// 			currSample->transform().get(ribMatrix);
// 			RiConcatTransform( ribMatrix );
// 		////////////////////////////


// 		// append values to all output plug specifications. Note, there may be more than one destination plug per value
// 		// Therefore: foreach value, we loop over plugs
// 		////////////////////////////
// 			const MFloatArray & vals = currSample->values();
// 			if (nPlugs == currSample->values().length()) {
// 				for (unsigned i=0;i<nPlugs;i++) {
// 					for (unsigned j = 0;j< tpvCopy[i].length();j++) {
// 						(tpvCopy[i])[j]	+= currSample->values()[i];
// 						if (currSample->future())  {
// 							(tpvCopy[i])[j] += ":";
// 						}
// 					}
// 				}
// 			}
// 		////////////////////////////

// 			currSample = currSample->future();
// 		}
// 		if (doBur) RiMotionEnd();



// 	// build the procedural data strings
// 	////////////////////////////
// 		MString argStr;
// 		MString spacer = "";
// 		for (unsigned i=0;i<nPlugs;i++) {
// 			for (unsigned j = 0;j< tpvCopy[i].length();j++) {
// 				argStr += spacer;spacer=" ";
// 				argStr += (tpvCopy[i])[j];
// 			}
// 		}


// 		/// EMIT TWC OBSOLETE
// 		/// emitDetailBlock(progStrings,detailRanges,lodSetNames,argStr,lbminf,lbmaxf);

// 		RiTransformEnd();
// 		RiAttributeEnd();
// 	//////////////////////////////////


// 		peg++;
// 	}


// 	return MS::kSuccess;

// }

// // TODO - DRY up 2 versions of emit
// MStatus flockShape::emitCacheSlice(float sampleTime) {

// 	if (!hasCache(sampleTime)) return MS::kUnknownParameter;

// 	MStatus st;

// 	MObject thisNode = thisMObject();

// ////////////////user defined target plugs////////////////////
// 	MPlug userMapPlug( thisNode, aUserMap );
// 	std::vector<MStringArray> targetPlugsVector = getTargetPlugs(userMapPlug);
// ////////////////////////////////////

// ////////////////get Bound////////////////////
// 	MPlug lodBBPlug( thisNode, aLodBB );
// 	MVector lbmin,lbmax;
// 	st= bbValue(lodBBPlug, lbmin, lbmax) ;
// 	MFloatVector lbminf(lbmin);
// 	MFloatVector lbmaxf(lbmax);
// ////////////////////////////////////

// ////////////////RunProgram Str////////////////////
// 	MPlug geomPlug( thisNode, aLodGeometry );
// 	MStringArray progStrings;
// 	MStringArray lodSetNames;
// 	std::vector<JFloatArray> detailRanges;
// 	st = createProgramStrings(geomPlug,progStrings, detailRanges,lodSetNames) ;
// ////////////////////////////////////

// //////////////////////// loop over all particles ///////////////////
// 	const particleStack & stack =  m_sliceMap.find(sampleTime)->second->stack();

// 	particleStack::const_iterator  peg = stack.begin();
// 	MString thisNodeName = MFnDependencyNode(thisNode).name();

// 	while (peg != stack.end()) {

// 		MString ident =thisNodeName; ident += "["; ident += (*peg)->id(); ident +="]";

// 		const char * namePtr = ident.asChar();
// 		RiAttributeBegin();
// 		RiAttribute( "identifier", "string name", &namePtr, RI_NULL );
// 		RiTransformBegin();

// 		const rigParticle * currSample = *peg;


// 		std::vector<MStringArray> tpvCopy = targetPlugsVector;
// 		unsigned nPlugs = unsigned(tpvCopy.size());

// 	// write the transform
// 	////////////////////////////
// 		RtMatrix ribMatrix;
// 		currSample->transform().get(ribMatrix);
// 		RiConcatTransform( ribMatrix );
// 	////////////////////////////


// 	// append values to all output plug specifications
// 	////////////////////////////
// 		const MFloatArray & vals = currSample->values();
// 		if (nPlugs == currSample->values().length()) {
// 			for (unsigned i=0;i<nPlugs;i++) {
// 				for (unsigned j = 0;j< tpvCopy[i].length();j++) {
// 					(tpvCopy[i])[j]	+= currSample->values()[i];
// 				}
// 			}
// 		}
// 	////////////////////////////

// 	// build the procedural data strings
// 	////////////////////////////
// 		MString argStr;
// 		MString spacer = "";
// 		for (unsigned i=0;i<nPlugs;i++) {
// 			for (unsigned j = 0;j< tpvCopy[i].length();j++) {
// 				argStr += spacer;spacer=" ";
// 				argStr += (tpvCopy[i])[j];
// 			}
// 		}

// 		/// EMIT TWC OBSOLETE
// 		/// emitDetailBlock(progStrings,detailRanges,lodSetNames,argStr,lbminf,lbmaxf);

// 		RiTransformEnd();
// 		RiAttributeEnd();

// 		peg++;
// 	}


// 	return MS::kSuccess;

// }

MStatus flockShape::removeCache() {
	rigSliceMap::iterator iter = m_sliceMap.begin();
	while (iter != m_sliceMap.end()) {
		if (iter->second) {
			delete iter->second;
		}
		iter++;
	}
	m_sliceMap.clear();
	return MS::kSuccess;
}

bool flockShape::hasCache() const {
	return (! m_sliceMap.empty());
}

bool flockShape::hasCache(float sampleTime) const {
	rigSliceMap::const_iterator slice_iter = m_sliceMap.find(sampleTime);
	return (slice_iter != m_sliceMap.end()) ;
}

void flockShape::cacheInfo() const {
	cerr << "-----------------------------------" << endl;
	rigSliceMap::const_iterator iter = m_sliceMap.begin();
	while (iter != m_sliceMap.end()) {
		cerr << "sample time: " << iter->first << endl;
		if (iter->second) {
			iter->second->info();
		}
		else {
			cerr << "problem with particle slice" << endl;
		}
		iter++;
	}
	cerr << "-----------------------------------" << endl;
}

MSelectionList flockShape::getShapesFromPlug (const MPlug &plug) const {

	// get every shape contained within sets or under nodes connected to the input plug
	MStatus st;
	MSelectionList shapesList;

	if (! plug.isConnected()) 	{ return shapesList; }

	MPlugArray conns;
	plug.connectedTo(conns, true, false, &st); mser;
	if ( conns.length() == 0)	{ return shapesList; }

	MObject theNode = conns[0].node();
	MSelectionList tmpSel;

	if (theNode.hasFn(MFn::kSet)) {
		MFnSet(theNode).getMembers (tmpSel, false);
	}
	else if (theNode.hasFn(MFn::kDagNode)) {
		tmpSel.add(theNode);
	}
	else {
		return shapesList;
	}


	// now find all descendent shapes
	MItDag dagIter;
	MDagPath rootPath;
	MItSelectionList selIter(tmpSel);

	for ( ; !selIter.isDone(); selIter.next()) {
		selIter.getDagPath(rootPath) ;
		dagIter.reset(rootPath);
		for ( ; !dagIter.isDone(); dagIter.next()) {
			MObject o = dagIter.currentItem();
			if (
			  (o.hasFn(MFn::kMesh)) ||
			  (o.hasFn(MFn::kNurbsCurve )) ||
			  (o.hasFn(MFn::kNurbsSurface )) ||
			  (o.hasFn(MFn::kStroke )) ||
			  (o.hasFn(MFn::kSubdiv))
			) {
				MDagPath path;
				dagIter.getPath(path);

				if (! MFnDagNode(path).isIntermediateObject() ) {
					shapesList.add(path);
				}
			}
		}
	}
	return shapesList;
}

// int flockShape::archiveAttribs(	const MStringArray & shapeNames) const {
// 	MStatus st;
// 	unsigned ns = shapeNames.length();

// //MStringArray allGeoAttribs;
// 	MSelectionList allGeoAttribs; // use a selectionList because it removes duplicates implicitly
// 	MStringArray geoAttribs;

// 	for (unsigned i=0;i<ns;i++) {
// 		geoAttribs.clear();

// 		MString cmd = "delightNodeQuery -shape \"";
// 		cmd += shapeNames[i];
// 		cmd += "\" -nodeType \"delightGeoAttribs\" -all";

// 		st = MGlobal::executeCommand(cmd,geoAttribs);mser;

// 		unsigned ng = geoAttribs.length();

// 		for (unsigned j=0;j<ng;j++) {
// 			allGeoAttribs.add(geoAttribs[j]);
// 		}

// 	}
// // now we have all the geoAttribs nodes in a selection list
// 	allGeoAttribs.getSelectionStrings(geoAttribs);
// 	unsigned ng = geoAttribs.length();
// //cerr << "here: " << __LINE__ << endl;
// 	for (unsigned j=0;j<ng;j++) {
// 	// archive name must not have the namespace, so strip it out

// 		MString archiveName;
// 		MStringArray tmpSA;
// 		geoAttribs[j].split(':',tmpSA);
// 		archiveName = tmpSA[(tmpSA.length() -1)];


// 		RiArchiveBegin( const_cast <char *>(archiveName.asChar()) , RI_NULL);

// 		MString outputCmd = "DGA_output( \"";
// 		outputCmd += geoAttribs[j];
// 		outputCmd += "\",\"\")"; // second argument (renderpass for some legacy sohofx hook) is empty

// 		st = MGlobal::executeCommand(outputCmd);mser;

// 		RiArchiveEnd();

// 	}
// 	return  ng;
// }

// int flockShape::archiveShaders(	const MStringArray & shapeNames) const {
// 	MStatus st;
// //cerr << "here: " << __LINE__ << endl;
// 	unsigned ns = shapeNames.length();
// //MStringArray allGeoAttribs;
// 	unsigned ts=0;
// //cerr << "here: " << __LINE__ << endl;
// 	MStringArray shaderTypes;
// 	shaderTypes.append("surface");
// 	shaderTypes.append("displacement");
// 	shaderTypes.append("atmosphere");
// 	shaderTypes.append("interior");
// // cerr << "here: " << __LINE__ << endl;

// 	for (unsigned t=0;t<4;t++) {

// 		MSelectionList allShaders; // use a selectionList because it removes duplicates implicitly
// 		MStringArray allShaderStrings; // use a selectionList because it removes duplicates implicitly

// 		for (unsigned i=0;i<ns;i++) {
// 			MString shader;


// 			MString cmd = "delightNodeQuery -shape \"";
// 			cmd += shapeNames[i];
// 			cmd += "\" -nodeType \"";
// 			cmd += shaderTypes[t];
// 			cmd += "\" -highest";

// 			st = MGlobal::executeCommand(cmd,shader);mser;

// 			if (shader != "") st = allShaders.add(shader);mser;
// 		}

// 		allShaders.getSelectionStrings(allShaderStrings);
// 		unsigned nss = allShaderStrings.length();
// 		ts += nss;

// 		for (unsigned j=0;j<nss;j++) {

// 			MString shader = allShaderStrings[j];
// 		// archive name must not have the namespace, so strip it out
// 			MString archiveName;
// 			MStringArray tmpSA;
// 			shader.split(':',tmpSA);
// 			archiveName = tmpSA[(tmpSA.length() -1)];
// 			archiveName = (archiveName + " " +shaderTypes[t]);

// 			RiArchiveBegin( const_cast <char *>(archiveName.asChar()), RI_NULL);
// 			MString outputCmd = "DSN_output( \"";
// 			outputCmd += shader;
// 			outputCmd += "\",\"";
// 			outputCmd += shaderTypes[t];
// 			outputCmd += "\",\"\")";

// 			st = MGlobal::executeCommand(outputCmd);mser;
// 			RiArchiveEnd();

// 		}
// 	}


// // now we have all the shader nodes in a selection list

// 	return  ts;
// }

// MStatus flockShape::archiveShadersAndAttribs() const {

// // cerr << "in archiveShadersAndAttribs" << endl;
// // we need to dig down to every shape contained within or under the sets or nodes connected to the lod geometry plug
// 	MStatus st = MS::kSuccess;
// 	MPlug geomPlug( thisMObject(), aLodGeometry );
// 	MSelectionList allShapesList;
// 	unsigned n = geomPlug.numElements();
// //MStringArray lodNames;
// 	for (unsigned i = 0;i<n;i++){
// 		MPlug tmpPlug = geomPlug.elementByPhysicalIndex(i);
// 		MPlug setPlug =  tmpPlug.child(aLodGeometrySet,&st);mser;
// 		allShapesList.merge(getShapesFromPlug(setPlug));
// 	}
// 	MStringArray shapeNames;
// 	allShapesList.getSelectionStrings(shapeNames);
// 	unsigned ns = shapeNames.length();

// 	archiveAttribs(shapeNames);
// 	archiveShaders(shapeNames);

// 	return st;
// }





