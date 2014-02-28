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
//#include <maya/.h> 
//#include <maya/.h> 
//#include <maya/.h> 
//#include <maya/.h> 

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



class flockShape : public MPxLocatorNode{
	
public:
	
	
	
	
	flockShape();
	virtual ~flockShape();
	
	static void    *creator();
	static MStatus  initialize();
	
	// virtual void postConstructor();
	virtual MStatus  compute( const MPlug& plug, MDataBlock& block );
	
	virtual void            draw( M3dView & view, const MDagPath & path,
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
	
	// static  MObject         aLodBBMin;
	// static  MObject         aLodBBMax;
	// static  MObject         aLodBBSiz;
	// static  MObject         aLodBB;
	/////////////////////////////////////////
	
	/////////////////////////////////////////
	// static MObject  aIdPP;
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
	static MObject  aDetailTarget;
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

	// static MObject  aLodGeometry;
	// static MObject  aLodGeometrySet;
	// static MObject  aLodGeometryStart;
	// static MObject  aLodGeometryLo;
	// static MObject  aLodGeometryHigh;
	// static MObject  aLodGeometryEnd;
	
	static	MObject		aCullList;
	
	//static MObject aInMeshes;
	static MObject aInMeshes;
	static MObject aMesh;
	static MObject aMeshActive;
	
	static MObject aCurrentTime;
	
	//static MObject aInProxyMesh;
	//static MObject aInMeshActive;
	
	static MObject aOutMeshes;
	//static MObject aOutProxyMesh;
	static MObject aOutIds;

	static MTypeId  id;

	// MStatus addSlice(float sampleTime);

	MStatus emitCache(float relativeFrame=0.0f) ;
	
	MStatus emitCacheSlice(float sampleTime);
		
	MStatus removeCache(); 

	bool hasCache() const ;
	
	void cacheInfo() const;
	
	bool hasCache(float sampleTime) const;
	
	// some inlines
	// MString createProgramString(const MPlug &plug, const JFloatArray & samples=JFloatArray(0), float relativeFrame=0.0f) const ;
	enum LODStrategy {kLodInProcedural, kLodInRib  };	

	// MStatus createProgramStrings(
	// 	const MPlug &plug,
	// 	MStringArray & progStrings,
	//  	std::vector<JFloatArray> & detailRanges, 
	// 	MStringArray &lodSetNames,
	// 	const JFloatArray & samples=JFloatArray(),
	// 	float relativeFrame=0.0f
	// 	// , 	flockShape::LODStrategy strategy = flockShape::kLodInRib
	// ) const;

	// MStatus emitDetailBlock(
	// 	const MStringArray & progStrings,
	// 	const std::vector<JFloatArray> & detailRanges,
	// 	const MStringArray & lodSetNames,
	// 	const MString & argStr,
	// 	const MFloatVector & lbmin,
	// 	const MFloatVector & lbmax																
	// ) const;

	// MStatus archiveShadersAndAttribs() const;
	
	// int archiveAttribs(	const MStringArray & shapeNames) const;
	
	// int archiveShaders(	const MStringArray & shapeNames) const;
	
	MSelectionList getShapesFromPlug (const MPlug& plug) const;
	
	
private:
	
	
	void drawABox(const MVector &bbmin, const MVector &bbmax) ;
	// void drawBoxCorners(const MVector &bmin, const MVector &bmax ) ;

	
	bool updateSrcInMesh(MObject & dMesh);
	bool updateSrcInMeshProxy(MObject & dMesh);
	
	MStatus fortifyCache(); 
	
	enum RotateMethod {kRotateAim,  kRotatePhi };	
	
	// LodinRib means write a detail block in the rib
	// LodinProgram means write all geometry to one procedural call with multiple geometry flags
	
	rigSliceMap m_sliceMap;
	
	clonerStack * m_pClonerStack;

};

inline MStatus bbValue(const MPlug &bbPlug, MVector &bmin, MVector &bmax) {
	MStatus st;
	
	double minx;
	double miny;
	double minz;
	double maxx;
	double maxy;
	double maxz;
	
//	unsigned nc = bbPlug.numChildren(&st);ert;
//	if (nc <2) return MS::kFailure;
	MPlug minPlug = bbPlug.child(0,&st);er;
	MPlug maxPlug = bbPlug.child(1,&st);er;
	
//	nc = minPlug.numChildren(&st);ert;
//	if (nc <3) return MS::kFailure;
	
	MPlug minXPlug = minPlug.child(0,&st);er;
	MPlug maxXPlug = minPlug.child(1,&st);er;
	MPlug minYPlug = minPlug.child(2,&st);er;

//	nc = maxPlug.numChildren(&st);ert;
//	if (nc <3) return MS::kFailure;
	MPlug maxYPlug = maxPlug.child(0,&st);er;
	MPlug minZPlug = maxPlug.child(1,&st);er;
	MPlug maxZPlug = maxPlug.child(2,&st);er;
	
	st =minXPlug.getValue (minx );er;
	st =maxXPlug.getValue (miny );er;
	st =minYPlug.getValue (minz );er;
	st =maxYPlug.getValue (maxx );er;
	st =minZPlug.getValue (maxy );er;
	st =maxZPlug.getValue (maxz );er;
	
	
	
	bmin=MVector(minx,miny,minz);
	bmax=MVector(maxx,maxy,maxz);
	return MS::kSuccess;
	
}




/**
getTargetPlugConns.
Get all the destination connections for the given plug
Each destination plug name ishas an "=" appended to it because it will be
needed in the arg list to jMayaPP

*/
//inline MStringArray getTargetPlugConns(const MPlug &plug) {
//	MStatus st;
//
//	MStringArray result;
//	
//	if (plug.isConnected()) {
//
//		MPlugArray conns;
//		int hasConns = plug.connectedTo(conns,false,true,&st);
//
//		for (unsigned j=0;j<conns.length();j++) {
//			MPlug conn = conns[j] ;
//			MString tmpStr = conn.name();
//			MStringArray tmpSA;
//			tmpStr.split(':',tmpSA);
//			tmpStr = tmpSA[(tmpSA.length()-1)]; 
//			tmpStr += "=";
//			result.append(tmpStr);
//		}
//	} 
//
//	return result;
//}

inline std::vector<MStringArray> getTargetPlugs(const MPlug &plug) {
	MStatus st;

	unsigned n = plug.numElements();
	std::vector<MStringArray> result;	

	for (unsigned i = 0;i<n;i++){
		MPlug tmpPlug = plug.elementByPhysicalIndex(i); 
		MPlug targetPlug =  tmpPlug.child(flockShape::aUserTarget,&st);er;

		MStringArray outputs;
		MPlugArray conns;
		int hasConns = targetPlug.connectedTo(conns,false,true,&st);
		
		
		if (hasConns)  {
			for (unsigned j=0;j<conns.length();j++) {
				MPlug conn = conns[j] ;
				MString tmpStr = conn.name();
				MStringArray tmpSA;
				tmpStr.split(':',tmpSA);
				tmpStr = tmpSA[(tmpSA.length()-1)]; 
				tmpStr += "=";
				outputs.append(tmpStr);
			}
		} 
		
		
		result.push_back(outputs);
	}
	return result;
}

// inline MStatus flockShape::createProgramStrings(
// 	const MPlug &plug,
// 	MStringArray & progStrings,
//  	std::vector<JFloatArray> & detailRanges, 
// 	MStringArray &lodSetNames,
// 	const JFloatArray & samples,
// 	float relativeFrame    // , flockShape::LODStrategy strategy
// ) const {
	
// 	// we want to create the strings that form the first part of the procedural call.
// 	//If strategy is LodInRib then we generate one string for each LOD Set
// 	//If strategy is LodInProcedural then we generate one string and it contains all the LOD Sets
	
// 	// jMayaPP -f myfile.ma -g myGeomLOD_1 -m 48.75 -m 49.25 -r 49
// 	// jMayaPP -f myfile.ma -g myGeomLOD_1  -g myGeomLOD_2 -m 48.75 -m 49.25 -r 49

// 	lodSetNames.clear();
		
// 	MStatus st = MS::kSuccess;
// 	MString filename;
// 	///////////////////////lod bits first///////////////////////////
// 	unsigned n = plug.numElements();

// 	for (unsigned i = 0;i<n;i++){
// 		//cerr << "searching plug " << i  << endl;
		
// 		MPlug tmpPlug = plug.elementByPhysicalIndex(i); 
// 		MPlug setPlug =  tmpPlug.child(flockShape::aLodGeometrySet,&st);er;
// 		if (setPlug.isConnected()) {
// 			//cerr << "set plug " << i << " connected" << endl;
// 			MPlugArray conns;
// 			setPlug.connectedTo(conns,true,false,&st);
// 			if (conns.length() > 0){
// 				if (i==0){
					
// 					//
// 					MString name = 	MFnDependencyNode(conns[0].node()).name();
// 					MString refQueryCmd = ("referenceQuery -f " + name);
// 					st = MGlobal::executeCommand(refQueryCmd,filename);
// 					//cerr << " just set filename " << filename  << endl;
// 				}

// 				MPlug conn = conns[0] ;
// 				MString tmpStr;
				
// 				tmpStr = conn.name(); 
// 				MStringArray tmpSA;
// 				tmpStr.split(':',tmpSA);
// 				tmpStr = tmpSA[(tmpSA.length() -1)]; // last part of the name (i.e. namespace stripped off)
// 				tmpSA.clear(); 
// 				tmpStr.split('.',tmpSA);
// 				tmpStr = tmpSA[0];  // node name (i.e. attribute stripped off)
				
// 				// tmpStr = tmpStr + "=";
				

				
// 			  JFloatArray detailRange(4);
// 			  MPlug tmpRangePlug;
            
// 			  tmpRangePlug = tmpPlug.child(flockShape::aLodGeometryStart,&st);er;
// 			  tmpRangePlug.getValue(detailRange[0]);
// 			 // tmpStr = tmpStr + double(detailRange[0]);
// 			 // tmpStr = tmpStr + ":";
			  
// 			  tmpRangePlug =  tmpPlug.child(flockShape::aLodGeometryLo,&st);er;
// 			  tmpRangePlug.getValue(detailRange[1]);
// 			 // tmpStr = tmpStr + double(detailRange[1]);
// 			 // tmpStr = tmpStr + ":";
			
// 			  tmpRangePlug =  tmpPlug.child(flockShape::aLodGeometryHigh,&st);er;
// 			  tmpRangePlug.getValue(detailRange[2]);	
// 			 // tmpStr = tmpStr + double(detailRange[2]);
// 			 // tmpStr = tmpStr + ":";
			
// 			  tmpRangePlug =  tmpPlug.child(flockShape::aLodGeometryEnd,&st);er;
// 			  tmpRangePlug.getValue(detailRange[3]);
// 			 // tmpStr = tmpStr + double(detailRange[3]);

				
// 			 detailRanges.push_back(detailRange);	
	
				
// 			lodSetNames.append(tmpStr);
				
// 				// TODO - check validity of detail block
// 			}
// 		}
// 	}
// 	//cerr << " out of plug loop " << filename  << endl;
// 	//////samples string (-m 48.75 -m 49.25 -r 49) /////////// 
// 	MString sampleStr;
// 	std::vector<float>::const_iterator iter = samples.begin();
// 	while (iter != samples.end()) {
// 		sampleStr += " -m ";
// 		sampleStr += *iter;
// 		iter++;
// 	}
// 	sampleStr += " -r ";
// 	sampleStr += relativeFrame;
// 	/////////////////////////////////////////////////////	
	
// 	//////app and filename string (jMayaPP -f myfile.ma ) /////////// 	
// 	MString app = 	"jMayaPP -f ";
// 	app  += filename;	
// 	/////////////////////////////////////////////////////	
		
		
// 	n = lodSetNames.length(); // set n to the number of elements that actually were connected

// //   if (strategy == flockShape::kLodInProcedural) {
// //   	progStrings.setLength(1);		
// //   	MString & str = progStrings[0]; 
// //   	str = app;
// //   	for (int i=0;i<n;i++) {
// //   		str += " -g ";
// //   		str += lodNames[i];
// //   	}
// //   	str += sampleStr;
// //   } else{
		
// 	progStrings.setLength(n);		
	
// 	// we make every program string identical so that the program is only invoked once	
// 	// reconsider later - maybe do some tests
	
// 	for (unsigned i=0;i<n;i++) {
// 		MString & str = progStrings[i];
// 		str = app;
// 		//for (int j=0;j<n;j++) {
// 	   	//	str += " -g ";
// 	   	//	str += lodSetNames[j];
// 	   	//}
// 	     str += " -g ";
// 	     str +=lodSetNames[i];
// 		str += sampleStr;
// 	}			
		
// //	}

	
// 	return st;
// }

#endif
