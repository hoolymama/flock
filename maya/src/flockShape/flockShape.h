#ifndef  __flockShape_H__
#define  __flockShape_H__

#include <vector>
#include <maya/MFileIO.h>
#include <maya/MIOStream.h>
#include <maya/MPxLocatorNode.h>
#include <maya/MDataBlock.h>
#include <maya/MFnDagNode.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>

#include <maya/MVector.h>

#include <maya/MFloatArray.h>
#include <maya/MStatus.h>
#include <maya/MGlobal.h>

#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
 

#include <maya/MFnPluginData.h>
// #include "ri.h"

#include "errorMacros.h"
#include "rigSlice.h"
#include "clonerStack.h"

#include "jTypes.h"

#define LEAD_COLOR        18  // green
#define ACTIVE_COLOR      15  // white
#define ACTIVE_AFFECTED_COLOR  8  // purple
#define DORMANT_COLOR      4  // blue
#define HILITE_COLOR      17  // pale blue
#define DORMANT_VERTEX_COLOR  8  // purple
#define ACTIVE_VERTEX_COLOR    16  // yellow

#define POINT_SIZE        2.0



class flockShape : public MPxLocatorNode {

public:




	flockShape();
	virtual ~flockShape();

	static void    *creator();
	static MStatus  initialize();

	// virtual void postConstructor();
	virtual MStatus  compute( const MPlug &plug, MDataBlock &block );

	virtual void            draw( M3dView &view, const MDagPath &path,
	                              M3dView::DisplayStyle style,
	                              M3dView::DisplayStatus status );



	// Bounding box methods
	/////////////////////////////////////////
	virtual bool            isBounded() const;
	virtual MBoundingBox    boundingBox() const;

	static  MObject         aPointsBBMin;
	static  MObject         aPointsBBMax;
	static  MObject         aPointsBBSiz;
	static  MObject         aPointsBB;

	static MObject  aSortedIdPP;
	static MObject  aIdIndexPP;
	/////////////////////////////////////////

	/////////////////////////////////////////
	static MObject  aTranslatePP;
	static MObject  aRotatePP;
	static MObject  aRotateUpPP;
	static MObject  aRotateType;
	static MObject  aRotateAimFrontAxis;
	static MObject  aRotateAimUpAxis;
	static MObject  aScalePP;
	static MObject  aActivePP;
	/////////////////////////////////////////

	static MObject  aRotateOrder;
	static MObject  aTranslateTargetX;
	static MObject  aTranslateTargetY;
	static MObject  aTranslateTargetZ;
	static MObject  aTranslateTarget;
	static MObject  aRotateTargetX;
	static MObject  aRotateTargetY;
	static MObject  aRotateTargetZ;
	static MObject  aRotateTarget;
	static MObject  aScaleTargetX;
	static MObject  aScaleTargetY;
	static MObject  aScaleTargetZ;
	static MObject  aScaleTarget;


	// If aDetailTarget is connected to an attribute on the rig,
	// the detail will be set for this attribute in the render
	// The rig TD may do whatever he wants with this value
	// static MObject  aDetailTarget;
	//	static MObject  aLodStrategy;
	static MObject  aTargetId;


	static MObject  aUserMap;
	static MObject  aUserPP;
	static MObject  aUserTarget;
	static MObject  aUserLabel;



	static MObject  aUserVectorMap;
	static MObject  aUserVectorPP;
	static MObject  aUserVectorTargetX;
	static MObject  aUserVectorTargetY;
	static MObject  aUserVectorTargetZ;
	static MObject  aUserVectorTarget;
	static MObject  aUserVectorLabel;


	/* Multi target stuff */
	static MObject  aDoFlatTargets;
	static MObject  aFlatTranslateTargetX;
	static MObject  aFlatTranslateTargetY;
	static MObject  aFlatTranslateTargetZ;
	static MObject  aFlatTranslateTarget;
	static MObject  aFlatRotateTargetX;
	static MObject  aFlatRotateTargetY;
	static MObject  aFlatRotateTargetZ;
	static MObject  aFlatRotateTarget;
	static MObject  aFlatScaleTargetX;
	static MObject  aFlatScaleTargetY;
	static MObject  aFlatScaleTargetZ;
	static MObject  aFlatScaleTarget;
	static MObject  aFlatUserDoubleTarget;
	static MObject  aFlatUserVectorTargetX;
	static MObject  aFlatUserVectorTargetY;
	static MObject  aFlatUserVectorTargetZ;
	static MObject  aFlatUserVectorTarget;
	static MObject  aFlatTargets;


 

	static	MObject		aCullList;


	static MObject aInMeshes;
	static MObject aMesh;
	static MObject aMeshActive;

	static MObject aCurrentTime;

	static MObject aOutMeshes;

	static MObject aOutIds;

	static MTypeId  id;

	MStatus emitCache(float relativeFrame = 0.0f) ;

	MStatus emitCacheSlice(float sampleTime);

	MStatus removeCache();

	bool hasCache() const ;

	void cacheInfo() const;

	bool hasCache(float sampleTime) const;

	// some inlines
	enum LODStrategy {kLodInProcedural, kLodInRib  };

	MSelectionList getShapesFromPlug (const MPlug &plug) const;


private:

	MStatus computeTarget(MDataBlock &data);

	MStatus computeFlatTargets(MDataBlock &data);

	void drawABox(const MVector &bbmin, const MVector &bbmax) ;


	bool updateSrcInMesh(MObject &dMesh);
	bool updateSrcInMeshProxy(MObject &dMesh);

	MStatus fortifyCache();

	enum RotateMethod {kRotateAim,  kRotatePhi };

	rigSliceMap m_sliceMap;

	clonerStack *m_pClonerStack;

};

inline MStatus bbValue(const MPlug &bbPlug, MVector &bmin, MVector &bmax) {
	MStatus st;

	double minx;
	double miny;
	double minz;
	double maxx;
	double maxy;
	double maxz;

	MPlug minPlug = bbPlug.child(0, &st); mser;
	MPlug maxPlug = bbPlug.child(1, &st); mser;


	MPlug minXPlug = minPlug.child(0, &st); mser;
	MPlug maxXPlug = minPlug.child(1, &st); mser;
	MPlug minYPlug = minPlug.child(2, &st); mser;

	MPlug maxYPlug = maxPlug.child(0, &st); mser;
	MPlug minZPlug = maxPlug.child(1, &st); mser;
	MPlug maxZPlug = maxPlug.child(2, &st); mser;

	st = minXPlug.getValue (minx ); mser;
	st = maxXPlug.getValue (miny ); mser;
	st = minYPlug.getValue (minz ); mser;
	st = maxYPlug.getValue (maxx ); mser;
	st = minZPlug.getValue (maxy ); mser;
	st = maxZPlug.getValue (maxz ); mser;



	bmin = MVector(minx, miny, minz);
	bmax = MVector(maxx, maxy, maxz);
	return MS::kSuccess;

}




inline std::vector<MStringArray> getTargetPlugs(const MPlug &plug) {
	MStatus st;

	unsigned n = plug.numElements();
	std::vector<MStringArray> result;

	for (unsigned i = 0; i < n; i++) {
		MPlug tmpPlug = plug.elementByPhysicalIndex(i);
		MPlug targetPlug =  tmpPlug.child(flockShape::aUserTarget, &st); mser;

		MStringArray outputs;
		MPlugArray conns;
		int hasConns = targetPlug.connectedTo(conns, false, true, &st);


		if (hasConns)  {
			for (unsigned j = 0; j < conns.length(); j++) {
				MPlug conn = conns[j] ;
				MString tmpStr = conn.name();
				MStringArray tmpSA;
				tmpStr.split(':', tmpSA);
				tmpStr = tmpSA[(tmpSA.length() - 1)];
				tmpStr += "=";
				outputs.append(tmpStr);
			}
		}


		result.push_back(outputs);
	}
	return result;
}

#endif
