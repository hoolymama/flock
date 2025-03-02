#ifndef hexapodColony_H
#define hexapodColony_H
#include <list>
#include <maya/MObject.h>
#include <maya/M3dView.h>
#include <maya/MRampAttribute.h>

#include <maya/MFnDependencyNode.h>

 
#include <maya/MItMeshPolygon.h>
#include <maya/MFnMesh.h>


#include <maya/MDataBlock.h>

#include <maya/MFnIntArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include "hexapodAgent.h"
#include "hexapodFoot.h"
#include "displayMask.h"
#include "errorMacros.h"

class hexapodColony{
public:

	hexapodColony();

	~hexapodColony();

	void clear();

	void draw( M3dView & view, const DisplayMask & mask ) const ;

	MStatus  update( 	double dt ,
		const  MObject& node,   
		MDataBlock& data 	);

	void applyActuators(	const  MObject& node,	MDataBlock& data);





	void getOutputData(
		const MIntArray & idIndex,
		MVectorArray & outLA,
		MVectorArray & outLB,
		MVectorArray & outLC,
		MVectorArray & outRA,
		MVectorArray & outRB,
		MVectorArray & outRC,
		MVectorArray & outPosition,
		MVectorArray & outPhi,
		MDoubleArray & outScale
		) const;

	void getDefaultOutputData(
		const  MObject& node, 
		MDataBlock& data,
		MVectorArray & outLA,
		MVectorArray & outLB,
		MVectorArray & outLC,
		MVectorArray & outRA,
		MVectorArray & outRB,
		MVectorArray & outRC,
		MVectorArray & outPosition,
		MVectorArray & outPhi,
		MDoubleArray & outScale
		) const;

	bool hasMesh() const;
	MStatus setMesh(const  MObject& node,  MDataBlock& data);

private:
	
	MIntArray getIntArray( const  MString & name, const MObject &node,MDataBlock& data, unsigned len, MStatus *st=0) const;
	MDoubleArray getDoubleArray( const  MString & name, const MObject &node,MDataBlock& data, unsigned len, MStatus *st=0) const;
	MVectorArray getVectorArray( const  MString & name, const MObject &node,MDataBlock& data, unsigned len, MStatus *st=0) const;


	/* use a std::list because random inserts */
	std::list<HexapodAgent*> m_agents;

 

};


inline  MIntArray hexapodColony::getIntArray(
	const  MString & name,
	const MObject &node,
	MDataBlock& data,
	unsigned len,
	MStatus * st) const
{
	MFnDependencyNode depNode(node, st); 
	if (*st != MS::kSuccess) return MIntArray();
	MObject att = depNode.attribute(name, st);
	if (*st != MS::kSuccess) return MIntArray();
	MIntArray arr =  MFnIntArrayData(data.inputValue(att).data()).array();
	if (arr.length() != len) {
		*st = MS::kUnknownParameter;
		return MIntArray();
	}
	*st = MS::kSuccess;
	return arr;
}


inline  MDoubleArray hexapodColony::getDoubleArray(
	const  MString & name,
	const MObject &node,
	MDataBlock& data,
	unsigned len,
	MStatus * st) const
{
	MFnDependencyNode depNode(node, st); 
	if (*st != MS::kSuccess) return MDoubleArray(len, 0.0);
	MObject att = depNode.attribute(name, st);
	if (*st != MS::kSuccess) return MDoubleArray(len, 0.0);
	MDoubleArray arr =  MFnDoubleArrayData(data.inputValue(att).data()).array();
	if (arr.length() != len) {
		*st = MS::kUnknownParameter;
		return MDoubleArray(len, 0.0);
	}
	*st = MS::kSuccess;
	return arr;
}


inline  MVectorArray hexapodColony::getVectorArray(
	const  MString & name,
	const MObject &node,
	MDataBlock& data,
	unsigned len,
	MStatus * st) const
{
	MFnDependencyNode depNode(node, st); 
	if (*st != MS::kSuccess) return MVectorArray(len, MVector::zero);
	MObject att = depNode.attribute(name, st);
	if (*st != MS::kSuccess) return MVectorArray(len, MVector::zero);
	MVectorArray arr =  MFnVectorArrayData(data.inputValue(att).data()).array();
 
	if (arr.length() != len) {
		*st = MS::kUnknownParameter;
		return MVectorArray(len, MVector::zero);
	}
	*st = MS::kSuccess;
	return arr;
}





#endif

