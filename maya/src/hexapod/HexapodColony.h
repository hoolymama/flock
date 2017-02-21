#ifndef HexapodColony_H
#define HexapodColony_H
#include <list>
#include <maya/MObject.h>
#include <maya/M3dView.h>
#include <maya/MRampAttribute.h>

#include <maya/MFnDependencyNode.h>

#include <maya/MDataBlock.h>

#include <maya/MFnIntArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include "HexapodAgent.h"
#include "HexapodFoot.h"
#include "displayMask.h"
#include "errorMacros.h"

class HexapodColony{
public:

	HexapodColony();

	~HexapodColony();

	void clear();

	void draw( M3dView & view, const DisplayMask & mask ) const ;

	MStatus  update( 	double dt ,
		const  MObject& node,   
		MDataBlock& data 	);

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
private:
	
	MIntArray getIntArray( const  MString & name, const MObject &node,MDataBlock& data, unsigned len, MStatus *st=0) const;
	MDoubleArray getDoubleArray( const  MString & name, const MObject &node,MDataBlock& data, unsigned len, MStatus *st=0) const;
	MVectorArray getVectorArray( const  MString & name, const MObject &node,MDataBlock& data, unsigned len, MStatus *st=0) const;

	/* use a std::list because random inserts */
	std::list<HexapodAgent*> m_agents;


};

inline  MIntArray HexapodColony::getIntArray(
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


inline  MDoubleArray HexapodColony::getDoubleArray(
	const  MString & name,
	const MObject &node,
	MDataBlock& data,
	unsigned len,
	MStatus * st) const
{
	MFnDependencyNode depNode(node, st); 
	if (*st != MS::kSuccess) return MDoubleArray();
	MObject att = depNode.attribute(name, st);
	if (*st != MS::kSuccess) return MDoubleArray();
	MDoubleArray arr =  MFnDoubleArrayData(data.inputValue(att).data()).array();
	if (arr.length() != len) {
		*st = MS::kUnknownParameter;
		return MDoubleArray();
	}
	*st = MS::kSuccess;
	return arr;
}


inline  MVectorArray HexapodColony::getVectorArray(
	const  MString & name,
	const MObject &node,
	MDataBlock& data,
	unsigned len,
	MStatus * st) const
{
	MFnDependencyNode depNode(node, st); 
	if (*st != MS::kSuccess) return MVectorArray();
	MObject att = depNode.attribute(name, st);
	if (*st != MS::kSuccess) return MVectorArray();
	MVectorArray arr =  MFnVectorArrayData(data.inputValue(att).data()).array();
	if (arr.length() != len) {
		*st = MS::kUnknownParameter;
		return MVectorArray();
	}
	*st = MS::kSuccess;
	return arr;
}





#endif

