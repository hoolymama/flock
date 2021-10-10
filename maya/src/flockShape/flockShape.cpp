#include <math.h>
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

MObject flockShape::aCullList;
MObject flockShape::aPointsBBMin;
MObject flockShape::aPointsBBMax;
MObject flockShape::aPointsBBSiz;
MObject flockShape::aPointsBB;

MObject flockShape::aCurrentTime;

MObject flockShape::aInMeshes;
MObject flockShape::aMesh;
MObject flockShape::aMeshActive;

MObject flockShape::aOutMeshes;

MObject flockShape::aOutIds;

MTypeId flockShape::id( k_flockShape );

flockShape::flockShape() {

	m_pClonerStack = new clonerStack;

}
flockShape::~flockShape() {
	if (m_pClonerStack) {
		delete m_pClonerStack;
		m_pClonerStack = 0;
	}

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

 
	aSortedIdPP = tAttr.create("sortedId", "sid", MFnData::kIntArray);
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	addAttribute( aSortedIdPP );

	aIdIndexPP = tAttr.create("idIndex", "idi", MFnData::kIntArray);
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	addAttribute( aIdIndexPP );

	aActivePP = tAttr.create("active", "act", MFnData::kDoubleArray);
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	addAttribute( aActivePP );
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
	addAttribute( aRotateOrder );

	aTranslatePP = tAttr.create("translatePP", "tpp", MFnData::kVectorArray);
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	addAttribute( aTranslatePP );

	aRotatePP = tAttr.create("rotatePP", "rpp", MFnData::kVectorArray);
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	addAttribute( aRotatePP );

	aRotateUpPP = tAttr.create("rotateUpPP", "rupp", MFnData::kVectorArray);
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	addAttribute( aRotateUpPP );

	aScalePP = tAttr.create("scalePP", "spp", MFnData::kDoubleArray);
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	addAttribute( aScalePP );


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
	addAttribute(aTranslateTarget);

	aRotateTargetX = uAttr.create( "rotateTargetX", "rtgx", MFnUnitAttribute::kAngle, 0.0 );
	aRotateTargetY = uAttr.create( "rotateTargetY", "rtgy", MFnUnitAttribute::kAngle, 0.0 );
	aRotateTargetZ = uAttr.create( "rotateTargetZ", "rtgz", MFnUnitAttribute::kAngle, 0.0 );
	aRotateTarget = nAttr.create( "rotateTarget", "rtg", aRotateTargetX, aRotateTargetY,
	                              aRotateTargetZ );
	nAttr.setStorable(false);
	nAttr.setReadable(true);
	addAttribute(aRotateTarget);

	aScaleTargetX = nAttr.create( "scaleTargetX", "stgx", MFnNumericData::kDouble );
	aScaleTargetY = nAttr.create( "scaleTargetY", "stgy", MFnNumericData::kDouble );
	aScaleTargetZ = nAttr.create( "scaleTargetZ", "stgz", MFnNumericData::kDouble);
	aScaleTarget = nAttr.create( "scaleTarget", "stg", aScaleTargetX, aScaleTargetY,
	                             aScaleTargetZ );
	nAttr.setStorable(false);
	nAttr.setReadable(true);
	addAttribute(aScaleTarget);


	aTargetId = nAttr.create("targetId", "tid", MFnNumericData::kLong);
	nAttr.setWritable(true);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute (aTargetId);
	///////////////////////////////////////////////////////////////////////

	////////USER DEFINED ATTS ///////////////////////////////////
	aUserPP = tAttr.create("userPP", "upp", MFnData::kDoubleArray);
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	addAttribute( aUserPP ); // not sure if this needed

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
	addAttribute(aUserMap);

	////////USER DEFINED VECTOR ATTS ///////////////////////////////////
	aUserVectorPP = tAttr.create("userVectorPP", "uvpp", MFnData::kVectorArray);
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	addAttribute( aUserVectorPP ); // not sure if this needed

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
	addAttribute(aUserVectorMap);



	/* FLAT TARGETS */
	aDoFlatTargets = nAttr.create( "doFlatTargets", "dftg", MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(false);
	nAttr.setDefault(false);
	addAttribute(aDoFlatTargets);

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
	addAttribute(aFlatTargets);

	aPointsBBMin = nAttr.create( "pointsBBMin", "pbn",  MFnNumericData::k3Double );
	nAttr.setHidden(false);

	aPointsBBMax = nAttr.create( "pointsBBMax", "pbx",  MFnNumericData::k3Double );

	aPointsBBSiz = nAttr.create( "pointsBBSiz", "pbs",  MFnNumericData::k3Double );
	nAttr.setHidden(false);

	aPointsBB = cAttr.create("pointsBB", "pbb");
	cAttr.addChild(aPointsBBMin);
	cAttr.addChild(aPointsBBMax);
	cAttr.addChild(aPointsBBSiz);
	cAttr.setStorable(true);
	cAttr.setHidden(false);
	addAttribute(aPointsBB);

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
	addAttribute( aInMeshes );

	aOutIds = tAttr.create("outIds", "oids", MFnData::kIntArray);
	tAttr.setStorable(false);
	addAttribute( aOutIds );

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
	addAttribute(aCurrentTime);

	attributeAffects(aCurrentTime , aOutMeshes );
	attributeAffects(aCurrentTime , aOutIds );
	attributeAffects(aCurrentTime , aFlatTranslateTargetX );
	attributeAffects(aCurrentTime , aFlatTranslateTargetY );
	attributeAffects(aCurrentTime , aFlatTranslateTargetZ );
	attributeAffects(aCurrentTime , aFlatTranslateTarget );
	attributeAffects(aCurrentTime , aFlatRotateTargetX );
	attributeAffects(aCurrentTime , aFlatRotateTargetY );
	attributeAffects(aCurrentTime , aFlatRotateTargetZ );
	attributeAffects(aCurrentTime , aFlatRotateTarget );
	attributeAffects(aCurrentTime , aFlatScaleTargetX );
	attributeAffects(aCurrentTime , aFlatScaleTargetY );
	attributeAffects(aCurrentTime , aFlatScaleTargetZ );
	attributeAffects(aCurrentTime , aFlatScaleTarget );
	attributeAffects(aCurrentTime , aFlatUserDoubleTarget );
	attributeAffects(aCurrentTime , aFlatUserVectorTargetX );
	attributeAffects(aCurrentTime , aFlatUserVectorTargetY );
	attributeAffects(aCurrentTime , aFlatUserVectorTargetZ );
	attributeAffects(aCurrentTime , aFlatUserVectorTarget );



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



MStatus flockShape::compute(const MPlug &plug, MDataBlock &data) {
	//	tracer;
	MStatus st;


	if (! ( (plug == aOutMeshes)
	        ||  (plug.isElement() && plug.array() == aOutMeshes)
	        ||  (plug == aOutIds)
	      ) ) { return MS::kUnknownParameter; }

	st = computeTarget(data);
	return MS::kSuccess;



}





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



