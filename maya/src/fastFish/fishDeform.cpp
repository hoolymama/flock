/*
*  fishDeform.cpp
	* 
	*
	*  Created by Julian Mann on 12/02/2007.
	*  Copyright 2007 hoolyMama. All rights reserved.
	*
*/

#include "fishDeform.h"



#include <maya/MIOStream.h>
#include <math.h>

#include <maya/MFnMeshData.h>

#include <maya/MDoubleArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMesh.h>  
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnIntArrayData.h>
#include <maya/MPlugArray.h>
#include <maya/MFnPluginData.h>
#include <maya/MFloatPointArray.h>
#include <maya/MPointArray.h>
#include <maya/MString.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnMesh.h>
#include <maya/MFloatMatrix.h>
#include <maya/MGlobal.h>

#include <JFloatMatrix.h>
#include <JFloatVector.h>

#include <fishSkeleton.h>

#include <fishDeform.h>    

#include <errorMacros.h>
#include "jMayaIds.h"



MObject fishDeform::aPositions;
MObject fishDeform::aDirections;
MObject fishDeform::aUps;
MObject fishDeform::aScales;

MObject fishDeform::aPivot;
MObject fishDeform::aPivotOffsetMult;

MObject fishDeform::aActive;
MObject fishDeform::aCullList;
MObject fishDeform::aInMesh;
MObject fishDeform::aOutMesh;


MObject fishDeform::aOffset;
MObject fishDeform::aBend;
MObject fishDeform::aAmplitude;

//MObject fishDeform::aOffsetMult;
//MObject fishDeform::aBendMult;
//MObject fishDeform::aAmplitudeMult;
//MObject fishDeform::aScaleMult;

MObject fishDeform::aWavelength;
MObject fishDeform::aDropoff;
MObject fishDeform::aStart;


MObject fishDeform::aNumBones;
MObject fishDeform::aBoneStart;
MObject fishDeform::aBoneEnd;
MObject fishDeform::aSmoothBind;

MObject fishDeform::aOutPoints ;
MObject fishDeform::aOutDirections;


const float epsilon = 	float(1e-6);

MTypeId fishDeform::id( k_fishDeform );

fishDeform::fishDeform()
	: m_points(),
	m_counts(),
	m_connects(),
	m_Us(),
	m_Vs(),
	m_uvCounts(),
	m_uvIds(),
	m_schoolSize(0),
	m_outGeom( MObject::kNullObj )
{

}

fishDeform::~fishDeform() {

}

MStatus fishDeform::initialize()
{

	MStatus st;
	MString method("fishDeform::initialize");

	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;
	MFnEnumAttribute eAttr;


	aPositions = tAttr.create("position", "pos", MFnData::kVectorArray, &st);er;
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setCached(false);
	st = addAttribute( aPositions ); er;

	aDirections = tAttr.create("direction", "dir", MFnData::kVectorArray, &st);er;
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setCached(false);
	st = addAttribute( aDirections ); er;

	aUps = tAttr.create("upVectors", "ups", MFnData::kVectorArray, &st);er;
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setCached(false);
	st = addAttribute( aUps ); er;


	aScales = tAttr.create("scale", "scl", MFnData::kDoubleArray, &st);er;
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setCached(false);
	st = addAttribute( aScales ); er;

	aActive = tAttr.create("active", "atv", MFnData::kDoubleArray, &st);er;
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setCached(false);
	st = addAttribute( aActive ); er;

	aPivot = tAttr.create("pivot", "pvt", MFnData::kDoubleArray, &st);er;
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setCached(false);
	st = addAttribute( aPivot ); er;


	aPivotOffsetMult= nAttr.create("pivotOffsetMult","pom", MFnNumericData::kFloat, 1.0f, &st ); er;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aPivotOffsetMult); er;
	


	aCullList = tAttr.create( "cullList", "cls", MFnData::kString );
	tAttr.setStorable(true);
	addAttribute( aCullList );


	aInMesh= tAttr.create("inMesh","im", MFnData::kMesh);
	tAttr.setReadable(false);
	st = addAttribute( aInMesh );er;



	aBend = tAttr.create("bend", "ben", MFnData::kDoubleArray, &st);er;
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setCached(false);
	st = addAttribute( aBend ); er;



	aOffset = tAttr.create("offset", "off", MFnData::kDoubleArray, &st);er;
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setCached(false);
	st = addAttribute( aOffset ); er;


	aAmplitude = tAttr.create("amplitude", "amp", MFnData::kDoubleArray, &st);er;
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setCached(false);
	st = addAttribute( aAmplitude ); er;

	aWavelength	= nAttr.create("wavelength","wl", MFnNumericData::kFloat, 1.0f, &st ); er;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aWavelength); er;

	aStart	= nAttr.create("start","sta", MFnNumericData::kFloat, 1.0f, &st ); er;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aStart); er;

	aDropoff	= nAttr.create("dropoff","drp", MFnNumericData::kFloat, 1.0f, &st ); er;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aDropoff); er;


//   aOffsetMult	= nAttr.create("offsetMult","oml", MFnNumericData::kFloat, 1.0f, &st ); er;
//   nAttr.setHidden(false);
//   nAttr.setKeyable(true);
//   nAttr.setStorable(true);
//   nAttr.setWritable(true);
//   st = addAttribute(aOffsetMult); er;
//
//   aBendMult	= nAttr.create("bendMult","bml", MFnNumericData::kFloat, 1.0f, &st ); er;
//   nAttr.setHidden(false);
//   nAttr.setKeyable(true);
//   nAttr.setStorable(true);
//   nAttr.setWritable(true);
//   st = addAttribute(aBendMult); er;
//
//   aAmplitudeMult	= nAttr.create("amplitudeMult","aml", MFnNumericData::kFloat, 1.0f, &st ); er;
//   nAttr.setHidden(false);
//   nAttr.setKeyable(true);
//   nAttr.setStorable(true);
//   nAttr.setWritable(true);
//   st = addAttribute(aAmplitudeMult); er;
//
//   aScaleMult	= nAttr.create("scaleMult","sml", MFnNumericData::kFloat, 1.0f, &st ); er;
//   nAttr.setHidden(false);
//   nAttr.setKeyable(true);
//   nAttr.setStorable(true);
//   nAttr.setWritable(true);
//   st = addAttribute(aScaleMult); er;



	aNumBones	= nAttr.create("numTailBones","ntb", MFnNumericData::kShort, 8, &st ); er;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aNumBones); er;

	aBoneStart	= nAttr.create("headLength","hln", MFnNumericData::kFloat, 15.0f, &st ); er;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aBoneStart); er;


	aBoneEnd	= nAttr.create("tailLength","tln", MFnNumericData::kFloat, 20.0f, &st ); er;
	nAttr.setHidden(false);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	st = addAttribute(aBoneEnd); er;

	aSmoothBind = nAttr.create( "smoothBind", "smb",MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(false);
	addAttribute( aSmoothBind );


	aOutMesh = tAttr.create( "outMesh", "om", MFnData::kMesh );
	tAttr.setStorable(false);
	tAttr.setWritable(false);
	addAttribute( aOutMesh );


	aOutPoints = tAttr.create("outPoints", "opts", MFnData::kVectorArray, &st);er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	st = addAttribute(aOutPoints );er;	

	aOutDirections = tAttr.create("outDirections", "odrs", MFnData::kVectorArray, &st);er;
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	st = addAttribute(aOutDirections );er;





	st =attributeAffects(aNumBones ,aOutMesh );er;
	st =attributeAffects(aBoneStart ,aOutMesh );er;
	st =attributeAffects(aBoneEnd ,aOutMesh );er;
	st =attributeAffects(aSmoothBind ,aOutMesh );er;
	st =attributeAffects(aPivot ,aOutMesh );er;
	st =attributeAffects(aPivotOffsetMult ,aOutMesh );er;



	st =attributeAffects(aBend ,aOutMesh );er;
	st =attributeAffects(aOffset ,aOutMesh );er;
	st =attributeAffects(aAmplitude ,aOutMesh );er;
	st =attributeAffects(aWavelength ,aOutMesh );er;
	st =attributeAffects(aDropoff ,aOutMesh );er;
	st =attributeAffects(aStart ,aOutMesh );er;

 //  st =attributeAffects(aOffsetMult	  ,aOutMesh );er; 
 //  st =attributeAffects(aBendMult	      ,aOutMesh );er;
 //  st =attributeAffects(aAmplitudeMult   ,aOutMesh );er;
 //  st =attributeAffects(aScaleMult	      ,aOutMesh );er;


	st =attributeAffects(aPositions ,aOutMesh );er;
	st =attributeAffects(aDirections ,aOutMesh );er;
	st =attributeAffects(aUps ,aOutMesh );er;
	st =attributeAffects(aScales ,aOutMesh );er;
	st =attributeAffects(aActive ,aOutMesh );er;
	st =attributeAffects(aInMesh ,aOutMesh );er;
	st =attributeAffects(aCullList ,aOutMesh );er;




	return( MS::kSuccess );

}

void* fishDeform::creator(){return new fishDeform();}


void fishDeform::boneDeformation(	
unsigned numActive,  
 const MVectorArray &poss, 
	const MVectorArray &dirs, 
	const MVectorArray &ups, 
	const MDoubleArray &scales, 
	const MDoubleArray &actives, 
	const MDoubleArray &pivot, 
	const MDoubleArray &bends, 
	const MDoubleArray &offsets, 
	const MDoubleArray &amps, 
	float headLength, 
	float tailLength, 
	short numTailBones,
	float wavelength, 
	float dropoff, 
	float start, 
	//float offMlt, 
	//float ampMlt, 
	//float bendMlt, 
	float pivotMlt, 
	bool smooth,
	MFloatPointArray & verts,
	MVectorArray &outPts,
	MVectorArray &outDirs

)  {
	MStatus st;
	MString method("fishDeform::boneDeformation");
	const float bendMlt  = -0.055;
	const float ampMlt = 0.540;
	const float offMlt  = 2.225;
	
   // bendMlt =  bendMagic;
   // ampMlt =  ampMagic;
   // offMlt = offsetMagic;
   // 
   // bendMlt = 	bendMlt * bendMagic;
   // ampMlt = 	ampMlt * ampMagic;
   // offMlt = offMlt * offsetMagic;
	
	unsigned nPoints = m_points.length();
	unsigned nPositions = poss.length();
	bool someMissing =(numActive !=nPositions);
	// if (someMissing) if (actives[i] < 0.5) continue;


	fishSkeleton  * pFishSkeleton =  new fishSkeleton( numTailBones, tailLength, headLength);


	JFloatVectorArray oneFishVerts(nPoints);
	for (unsigned i =0;i<nPoints;i++) {
		oneFishVerts[i] = JFloatVector(m_points[i].x,m_points[i].y,m_points[i].z);
	}
	pFishSkeleton->bindSkin(oneFishVerts, smooth);

	unsigned nVerts = (nPoints*numActive);
	verts.setLength(nVerts);
	unsigned index = 0;

	JFloatMatrix transformMat;
	/// JFloatVector theVert, aVert;
	JFloatVector front, up, pos;

	for (unsigned i=0;i<nPositions;i++) {
		if (someMissing) if (actives[i] < 0.5) continue;

		float amp = float(amps[i] * ampMlt);
		float off = float(offsets[i] * offMlt);
		float bend = float(bends[i] * bendMlt );		
		float scale = float(scales[i]);

		front.x = float(dirs[i].x);		front.y = float( dirs[i].y);		front.z =  float(dirs[i].z);
		up.x =  float(ups[i].x);		up.y =  float(ups[i].y);		up.z =  float(ups[i].z);

		pos.x =  float(poss[i].x);		pos.y =  float(poss[i].y);		pos.z =  float(poss[i].z);




		transformMat.setToIdentity();
		transformMat.orientZ(front, up);
		transformMat.scale(scale);
		transformMat+=pos;


		// pose the bones
		pFishSkeleton->pose(amp,off,bend,wavelength, dropoff,start);
		// move to world
		
		JFloatVector pOff(0.0,0.0,pivot[i] * pivotMlt);
		pFishSkeleton->translateBy(pOff);
		pFishSkeleton->transformBy(transformMat);


		JFloatVectorArray newVerts(oneFishVerts);

		pFishSkeleton->transform(newVerts);

		// now we have a skeleton in the right place
		for (unsigned j = 0;j<nPoints;j++) {
			// points are relative to the bones they are to be transformed by
			const JFloatVector & theVert = newVerts[j];
			verts.set( MFloatPoint(theVert.x,theVert.y,theVert.z) ,index);
			index++;
		}
		/*
		for (unsigned k = 0;k< pFishSkeleton->getBoneCount();k++ ) {
			JFloatVector aVert(JFloatVector::zero);
			pFishSkeleton->transformVertex(k, aVert);
			outPts.append(  MVector( double(aVert.x),  double(aVert.y),double(aVert.z))  );
		}
		*/
	}

	delete pFishSkeleton;

	return;	

}
/**/
			
void fishDeform::transformOnly(		
	unsigned numActive,
	const MVectorArray &pos, 
	const MVectorArray &dir, 
	const MVectorArray &up, 
	const MDoubleArray &scales, 
	const MDoubleArray &actives, 
	const MDoubleArray &pivot, 
	float pivotMlt, 
	MFloatPointArray & verts
	) 
{
	MStatus st;
	MString method("fishDeform::appendTransformedSource");
	// same as above - but no deformations


	unsigned nPoints = m_points.length();
	unsigned nPositions = pos.length();
	bool someMissing =(numActive !=nPositions);
	// if (someMissing) if (actives[i] < 0.5) continue;


	verts.setLength(nPoints*numActive);
	MFloatMatrix fmat;
	unsigned index = 0;

	for (unsigned i=0;i<nPositions;i++) {
		if (someMissing) if (actives[i] < 0.5) continue;

		MFloatVector zAxis = MFloatVector(dir[i].normal());
		if (zAxis.isEquivalent(MFloatVector::zero)) {
			zAxis = (MFloatVector::zAxis);
		}


		MFloatVector yAxis = MFloatVector(up[i]);
		// if they are paralell then we have to resolve the situation 
		// so - if we are on the real yAxis then use the real zAxis else use the real yAxis for up
		if  ( zAxis.isParallel(yAxis) ) { // gotta do something, 
			if ( yAxis.isParallel(MFloatVector::yAxis) ) {
				zAxis = (MFloatVector::zAxis) ;
			} else {
				yAxis = (MFloatVector::zAxis);	
			}
		}

		MFloatVector xAxis = (yAxis^zAxis).normal(); // cross prod
		yAxis = zAxis^xAxis; // re-align y to make orthonormal axes

		// now plug them into the matrix
		fmat[0][0] = float(xAxis.x * scales[i]);fmat[0][1] = float(xAxis.y);			fmat[0][2] = float(xAxis.z);			fmat[0][3] = 0.0f;
		fmat[1][0] = float(yAxis.x);			fmat[1][1] = float(yAxis.y * scales[i]);fmat[1][2] = yAxis.z;			fmat[1][3] = 0.0f;
		fmat[2][0] = float(zAxis.x);			fmat[2][1] = float(zAxis.y);			fmat[2][2] = float(zAxis.z * scales[i]);		fmat[2][3] =  0.0f;
		fmat[3][0] = float(pos[i].x);			fmat[3][1] = float(pos[i].y);			fmat[3][2] = float(pos[i].z);			fmat[3][3] = 1.0f;		
		MFloatVector pOff(0.0,0.0,(pivot[i] * pivotMlt));

		for (unsigned j = 0;j<nPoints;j++) {
			
			verts.set( (m_points[j] + pOff) *fmat  ,index);
			index++;
		}
	}
}



MStatus fishDeform::compute( const MPlug& plug, MDataBlock& data )
{

	MStatus st;
	MString method("fishDeform::compute");
	if (! (
		(plug == aOutMesh) ||
		(plug == aOutDirections) ||
		(plug == aOutPoints) 
		)) return MS::kUnknownParameter;

	MVectorArray positions = MFnVectorArrayData(data.inputValue(aPositions).data()).array();
	MVectorArray dirs = MFnVectorArrayData(data.inputValue(aDirections).data()).array();
	MVectorArray ups = MFnVectorArrayData(data.inputValue(aUps).data()).array();
	MDoubleArray scales = MFnDoubleArrayData(data.inputValue(aScales).data()).array();
	MDoubleArray active = MFnDoubleArrayData(data.inputValue(aActive).data()).array();
	MDoubleArray pivot = MFnDoubleArrayData(data.inputValue(aPivot).data()).array();
	MDoubleArray bends = MFnDoubleArrayData(data.inputValue(aBend).data()).array();
	MDoubleArray offsets = MFnDoubleArrayData(data.inputValue(aOffset).data()).array();
	MDoubleArray amps = MFnDoubleArrayData(data.inputValue(aAmplitude).data()).array();

	float wavelength = data.inputValue(aWavelength).asFloat();
	float dropoff = data.inputValue(aDropoff).asFloat();
	float start = data.inputValue(aStart).asFloat();
	float pivotMult = data.inputValue(aPivotOffsetMult).asFloat();

  //  float offsetMult = data.inputValue(aOffsetMult).asFloat();
  //  float bendMult = data.inputValue(aBendMult).asFloat();
  //  float amplitudeMult = data.inputValue(aAmplitudeMult).asFloat();
  //  float scaleMult = data.inputValue(aScaleMult).asFloat();

  //  MDoubleArray scales(scalesTmp);
  //
  //  if (scaleMult != 1.0) {
  //  	unsigned scalesLen = scales.length();
  //  	for (unsigned i = 0;i< scalesLen;i++) scales[i] = scales[i] * scaleMult;
  //  }


	short nBones = data.inputValue(aNumBones).asShort();
	float boneStart = data.inputValue(aBoneStart).asFloat();
	float boneEnd = data.inputValue(aBoneEnd).asFloat();
	bool smoothBind = data.inputValue(aSmoothBind).asBool();


	MString cullListString =  data.inputValue( aCullList ).asString();

	MVectorArray outPts;
	MVectorArray outDirs;

	unsigned pl = positions.length();

	// make active array valid if its not already
	if (pl != active.length()) {
		active=MDoubleArray(pl,1.0);
	}

	// get 
	//MIntArray postCacheCullList;
	MStringArray sa;
	cullListString.split(' ', sa);
	unsigned sal= sa.length();

	unsigned cullFish;
	for (unsigned i=0;i<sal;i++){
		cullFish = sa[i].asInt();
		if (cullFish >= pl ) {
			// MGlobal::displayWarning( "ignoring culled fish: " + cullFish+" out of range 0 - "+ pl);
		} else {
			active.set(0.0, cullFish );
		}
	}

	// count active fish
	unsigned numActive = 0;
	for (unsigned i =0;i<pl;i++) {
		if (active[i] >= 0.5) {
			numActive++;
		}
	}

	//////////////////////////////////////////////







	//cerr << "numActive " << numActive << endl;

	//cerr << "pl " << pl << endl;

	if ( 
		(pl == dirs.length()) && 
		(pl == ups.length())   &&
		(pl == scales.length())
		) 
	{

		// if the number of particles changed, or the input mesh changed, then the output
		// mesh will need to be rebuilt with a data creator. Otherwise we just set new vertex  positions.
		///////////////////////////////////////////////////////////////////
		bool schoolSizeChanged = (numActive != m_schoolSize);
		m_schoolSize = numActive;
		MDataHandle hMesh = data.inputValue(aInMesh, &st );er;	
		MObject dMesh = hMesh.asMeshTransformed();
		MFnMesh meshFn(dMesh);
		bool meshTopologyChanged = updateSrcObject(dMesh);
		bool needsRebuild = (meshTopologyChanged || schoolSizeChanged);
		///////////////////////////////////////////////////////////////////

		MFloatPointArray vertices;
		MIntArray faceCounts;
		MIntArray connectivity;

		if (pivot.length() != pl) pivot = MDoubleArray(pl);

		if ((pl == bends.length()) && (pl == offsets.length())&& (pl == amps.length())) {
			// cerr << "transforming and deforming" << endl;
			// do transform and deform

			boneDeformation(
				numActive,positions,dirs,ups,scales,active,pivot,
				bends,offsets,amps, 
				boneStart, boneEnd, nBones,
				wavelength,dropoff,start,
				/*offsetMult , amplitudeMult,bendMult ,*/
				pivotMult,
				smoothBind, vertices, outPts, outDirs
				);

		} else {

			transformOnly(numActive,positions,dirs,ups,scales,active, pivot,pivotMult, vertices);

		}
		// moved from here - hope its ok
		// st = m_outMeshFn.setObject(m_outGeom);er;
		
		if (!needsRebuild){
			
			st = m_outMeshFn.setObject(m_outGeom);er;
			
			
			//cerr << "not rebuilding" << endl;
			m_outMeshFn.setPoints(vertices);	
		} else {
			//cerr << "rebuilding" << endl;
			unsigned nCounts = (m_counts.length() );
			unsigned nPoints = (m_points.length() );
			unsigned index = 0;
			faceCounts.setLength(nCounts * numActive);

			for (unsigned i = 0;i<numActive;i++) {
				for (unsigned j = 0;j<nCounts;j++) {
					faceCounts.set(m_counts[j],index);
					index++;
				}
			}

			unsigned nConnects = (m_connects.length() );
			index = 0;
			connectivity.setLength(nConnects * numActive);
			for (unsigned i = 0;i<numActive;i++) {
				unsigned startIndex = i*nPoints;
				for (unsigned j = 0;j<nConnects;j++) {

					connectivity.set( ( startIndex + m_connects[j] ) ,index);
					index++;
				}
			}

			m_outGeom = m_outMeshCreator.create(&st);er;
			m_outMeshFn.create( vertices.length(), faceCounts.length(), vertices, faceCounts, connectivity, m_outGeom, &st );  er;
			//m_outMeshFn.create( vertices.length(), faceCounts.length(), vertices, faceCounts, connectivity, m_Us, m_Vs, m_outGeom, &st );  er;	



			// UV counts and Ids

			unsigned nUVCounts = m_uvCounts.length();
			if (nUVCounts == nCounts) {

				MIntArray uvCounts;
				MIntArray uvIds;


				for (unsigned i = 0;i<numActive;i++) {// number of fish
					for (unsigned j = 0;j<nUVCounts;j++) { // number of polygons in one fish
						uvCounts.append(m_uvCounts[j]);
					}
				}

				unsigned nUVIds = (m_uvIds.length() );
				for (unsigned i = 0;i<numActive;i++) {
					for (unsigned j = 0;j<nUVIds;j++) {
						uvIds.append(m_uvIds[j]);
					}
				}
				st = m_outMeshFn.setUVs(m_Us, m_Vs);	er;	
				st = m_outMeshFn.assignUVs ( uvCounts, uvIds );		er;		

			} else {
				cerr << "nUVCounts counts don't match nCounts: " <<  nUVCounts << " " <<  nCounts << endl;
			}
		}
	}

	// output
	/////////////////////////////////////////////////////////////// 
	MDataHandle hMesh = data.outputValue(aOutMesh, &st);
	hMesh.set(m_outGeom);
	data.setClean(aOutMesh);	
	/////////////////////////////////////////////////////////////// 

	MDataHandle hOutPoints = data.outputValue(aOutPoints);
	MFnVectorArrayData fnOutPoints;
	MObject dOutPoints = fnOutPoints.create(outPts);
	hOutPoints.set(dOutPoints);
	st = data.setClean( aOutPoints );er;

	MDataHandle hOutDirections = data.outputValue(aOutDirections);
	MFnVectorArrayData fnOutDirections;
	MObject dOutDirections = fnOutDirections.create(outDirs);
	hOutDirections.set(dOutDirections);
	st = data.setClean( aOutDirections );er;



	return MS::kSuccess;
}



bool fishDeform::updateSrcObject(MObject & dMesh){


	// we maintain our own mesh data 
	// so we can compare the input mesh every frame to see if it changed
	MStatus st;
	MString method("fishDeform::updateSrcObject");

	MFnMesh meshFn(dMesh);
	bool topologyChanged = false;

	// update points array
	MFloatPointArray pnts;
	MIntArray pCounts;
	MIntArray pConnects;

	// compare points against old points
	st = meshFn.getPoints(pnts); er;
	if (pnts.length() != m_points.length()) topologyChanged=true;

	// now copy in the new points
	m_points.copy(pnts);

	// update counts and connects
	//////////////////////////////////////
	unsigned numPolys=meshFn.numPolygons();
	for (unsigned i=0;i<numPolys;i++) {
		MIntArray vts;
		meshFn.getPolygonVertices(i, vts);
		int siz = meshFn.polygonVertexCount(i);
		pCounts.append(siz);
		for ( int j=0;j<siz;j++) {
			pConnects.append(vts[j]);
		}
	}
	//////////////////////////////////////

	// compare new counts and connects to existing arrays
	//////////////////////////////////////
	if (pCounts.length() == m_counts.length()) {
		for (unsigned i=0;i<pCounts.length();i++) {
			if (pCounts[i] != m_counts[i]) {
				topologyChanged = true;
			}
		}
	} else {
		topologyChanged = true;
	}

	if (pConnects.length() == m_connects.length()) {
		for (unsigned i=0;i<pConnects.length();i++) {
			if (pConnects[i] != m_connects[i]) {
				topologyChanged = true;
			}
		}
	} else {
		topologyChanged = true;
	}
	//////////////////////////////////////

	// copy new counts and connects
	//////////////////////////////////////
	if (topologyChanged) {
		m_counts.copy(pCounts);
		m_connects.copy(pConnects);

		// get UVs
		m_Us.clear();
		m_Vs.clear();
		m_uvCounts.clear();
		m_uvIds.clear();
		if (meshFn.numUVs() >0){
			st = meshFn.getUVs( m_Us,  m_Vs) ;er;
			st = meshFn.getAssignedUVs( 	 m_uvCounts,  m_uvIds) ;er;
		}

	}
	//////////////////////////////////////


	//	MFloatArray uVals;
	//MFloatArray uVals;
	return topologyChanged;
}




