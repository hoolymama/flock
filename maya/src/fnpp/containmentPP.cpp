#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFloatPointArray.h>


#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMesh.h>

#include <maya/MFnNumericAttribute.h>
#include <maya/MVectorArray.h> 
#include <maya/MDoubleArray.h> 

#include "errorMacros.h"
#include "containmentPP.h"
#include "jMayaIds.h"


MTypeId containmentPP::id(k_containmentPP);

MObject containmentPP::aPosition;
MObject containmentPP::aMesh;	
MObject containmentPP::aSide ;
MObject containmentPP::aOutput;	  

void * containmentPP::creator () {
	return new containmentPP;
}

/// Post constructor
void
	containmentPP::postConstructor()
{
	MPxNode::postConstructor();

	setExistWithoutInConnections(true);
	setExistWithoutOutConnections(true);
}

MStatus containmentPP::initialize () {

	MStatus st;


	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;
	MFnEnumAttribute eAttr;

	aPosition = tAttr.create("positions", "pos",MFnData::kVectorArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);
	addAttribute(aPosition);

	aMesh = tAttr.create( "mesh", "msh", MFnData::kMesh, &st );er
	tAttr.setReadable(false);
	st = addAttribute(aMesh);	er;
	
	aSide = eAttr.create( "side", "sd", containmentPP::kInside);
	eAttr.addField("inside", containmentPP::kInside);
	eAttr.addField("outside", containmentPP::kOutside);
	eAttr.setKeyable(true);
	eAttr.setHidden(false);
	st = addAttribute( aSide );er;

	aOutput = tAttr.create ("output", "out",MFnData::kDoubleArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	addAttribute (aOutput);

	attributeAffects (aPosition, aOutput);
	attributeAffects (aMesh, aOutput);
	attributeAffects (aSide, aOutput);

	return MS::kSuccess;
}

containmentPP::containmentPP () {}
containmentPP::~containmentPP () {}

MStatus containmentPP::compute(const MPlug& plug, MDataBlock& data) {
	//DBG;
	if (!(plug == aOutput)) 	return MS::kUnknownParameter;			

	MStatus st;
	MFn::Type type = MFn::kInvalid;
	//DBG;

		// Get inputs
	MDataHandle hIn = data.inputValue(aPosition);
	MObject dIn = hIn.data();
	MVectorArray in = MFnVectorArrayData(dIn).array();

	//DBG;

	containmentPP::Side side = containmentPP::Side(data.inputValue(aSide).asShort());
	//DBG;

	int len = in.length();
	MDoubleArray out(len, 0.0);
	//DBG;
	if (len > 0) {
 		MObject dMesh =  data.inputValue(aMesh).asMeshTransformed();
    	MFnMesh meshFn(dMesh, &st);
    	if (st.error()) return MS::kUnknownParameter;
	////DBG;

		MMeshIsectAccelParams ap = meshFn.autoUniformGridParams();

		//DBG;

		MFloatPointArray hitPoints;
		bool hit;
		// bool isInside = false;
		MFloatVector rayDirection(MFloatVector::yAxis);
	////DBG;
		for (int i = 0;i<len;i++) {
			MFloatPoint rayOrigin(in[i].x,in[i].y,in[i].z);

			hit = meshFn.allIntersections(
				rayOrigin,rayDirection,
				0,0,false,MSpace::kWorld,99999999.0f,false,
				&ap,false,hitPoints,0,0,0,0,0);
			if (hit) {
				if (hitPoints.length() % 2 == 1 ) {
					out[i] = 1.0;
				}
			}
		}
		if (side ==containmentPP::kOutside) {
			for (int i = 0; i < len; ++i)
			{
				out[i] = 1.0 - out[i];
			}
		}
	}
 	//DBG;




	MDataHandle hOut = data.outputValue(aOutput);
	MFnDoubleArrayData fnOut;
	MObject objOut = fnOut.create(out);		
	hOut.set(objOut);
	data.setClean(plug);				
	return MS::kSuccess;


}
