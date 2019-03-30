#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFloatPointArray.h>


#include <maya/MMeshIntersector.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMesh.h>
#include <maya/MRampAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>

#include "errorMacros.h"
#include "meshProximityPP.h"
#include "jMayaIds.h"


MTypeId meshProximityPP::id(k_meshProximityPP);


MObject meshProximityPP::aPosition;
MObject meshProximityPP::aMesh;
MObject meshProximityPP::aFalloffDistance;
MObject meshProximityPP::aFalloffFunction;
MObject meshProximityPP::aMagnitude;
MObject meshProximityPP::aSide ;
MObject meshProximityPP::aOutput;

void *meshProximityPP::creator () {
	return new meshProximityPP;
}

/// Post constructor
void
meshProximityPP::postConstructor()
{
	MPxNode::postConstructor();

	setExistWithoutInConnections(true);
	setExistWithoutOutConnections(true);
}

MStatus meshProximityPP::initialize () {

	MStatus st;


	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;
	MFnEnumAttribute eAttr;

	aPosition = tAttr.create("positions", "pos", MFnData::kVectorArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);
	addAttribute(aPosition);

	aMesh = tAttr.create( "mesh", "msh", MFnData::kMesh, &st ); mser
	tAttr.setReadable(false);
	st = addAttribute(aMesh);	mser;

	aSide = eAttr.create( "side", "sd", meshProximityPP::kInside);
	eAttr.addField("inside", meshProximityPP::kInside);
	eAttr.addField("outside", meshProximityPP::kOutside);
	eAttr.addField("both", meshProximityPP::kBoth);
	eAttr.setKeyable(true);
	eAttr.setHidden(false);
	st = addAttribute( aSide ); mser;


	aFalloffDistance = nAttr.create( "falloffDistance", "fds", MFnNumericData::kFloat);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	addAttribute(aFalloffDistance);

	aMagnitude = nAttr.create( "magnitude", "mag", MFnNumericData::kFloat);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	addAttribute(aMagnitude);

	aFalloffFunction = MRampAttribute::createCurveRamp("falloffFunction", "ffu");
	st = addAttribute( aFalloffFunction ); mser;

	aOutput = tAttr.create ("output", "out", MFnData::kVectorArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	addAttribute (aOutput);

	attributeAffects (aPosition, aOutput);
	attributeAffects (aMesh, aOutput);
	attributeAffects (aSide, aOutput);
	attributeAffects (aFalloffDistance, aOutput);
	attributeAffects (aMagnitude, aOutput);
	attributeAffects (aFalloffFunction, aOutput);

	return MS::kSuccess;
}

meshProximityPP::meshProximityPP () {}
meshProximityPP::~meshProximityPP () {}

MStatus meshProximityPP::compute(const MPlug &plug, MDataBlock &data) {

	if (!(plug == aOutput)) 	{ return MS::kUnknownParameter; }

	MStatus st;
	MFn::Type type = MFn::kInvalid;

	MDataHandle hIn = data.inputValue(aPosition);
	MObject dIn = hIn.data();
	MVectorArray in = MFnVectorArrayData(dIn).array();

	meshProximityPP::Side side = meshProximityPP::Side(data.inputValue(aSide).asShort());

	int len = in.length();
	MVectorArray out(len);
	float falloffDistance = data.inputValue(aFalloffDistance).asFloat();
	float magnitude = data.inputValue(aMagnitude).asFloat();

	if ((len > 0) && (falloffDistance > 0.0) && (magnitude != 0.0)) {
		MObject dMesh =  data.inputValue(aMesh).asMeshTransformed();
		MFnMesh meshFn(dMesh, &st);
		if (! st.error()) {

			MObject thisObj = thisMObject();
			MRampAttribute rampAttr( thisObj, aFalloffFunction, &st ); mser;
			MMeshIntersector intersector;
			MPointOnMesh pointInfo;
			MFloatPoint hitPoint;
			MFloatVector normal;
			st = intersector.create( dMesh ); mser;
			for (int i = 0; i < len; i++) {

				MPoint pt = MPoint(in[i]);
				st = intersector.getClosestPoint( pt, pointInfo , falloffDistance);
				if (! st.error()) {
					hitPoint =  pointInfo.getPoint() ;

					MVector diff = pt - MPoint(hitPoint);

					if (! diff.isEquivalent(MVector::zero)) {
						double dist = diff.length();

						normal = pointInfo.getNormal();
						if (side != meshProximityPP::kBoth) {
							float dot = MFloatVector(diff) * normal;
							if ((side == meshProximityPP::kOutside) && (dot < 0.0)) { continue; }
							if ((side == meshProximityPP::kInside) && (dot >= 0.0)) { continue; }
						}
						MVector diffN = diff.normal();
						double distN = dist / falloffDistance;
						float val;
						rampAttr.getValueAtPosition( distN, val, &st ); mser;
						out[i] = diffN * double(val * magnitude);
					}
				}
			}
		}
	}

	MDataHandle hOut = data.outputValue(aOutput);
	MFnVectorArrayData fnOut;
	MObject objOut = fnOut.create(out);
	hOut.set(objOut);
	data.setClean(plug);
	return MS::kSuccess;
}
