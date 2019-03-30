#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MVectorArray.h>
#include "errorMacros.h"
#include "orthogonalPP.h"
#include "jMayaIds.h"


#define OUTLEN_NORMALIZE	0
#define OUTLEN_ORIGINAL	1
#define OUTLEN_PROJECTED	2

MTypeId orthogonalPP::id(k_orthogonalPP);
MObject orthogonalPP::aFront;
MObject orthogonalPP::aUp;
MObject orthogonalPP::aOutputLength;
MObject orthogonalPP::aOutput;
MObject orthogonalPP::aDefaultRef;


void *orthogonalPP::creator () {
	return new orthogonalPP;
}

/// Post constructor
void
orthogonalPP::postConstructor()
{
	MPxNode::postConstructor();

	setExistWithoutInConnections(true);
	setExistWithoutOutConnections(true);
}

MStatus orthogonalPP::initialize () {

	MStatus st;


	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;

	MFnEnumAttribute eAttr;

	aFront = tAttr.create("vector", "vec", MFnData::kVectorArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);
	addAttribute(aFront);

	aUp = tAttr.create("reference", "ref", MFnData::kVectorArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);
	addAttribute(aUp);

	aDefaultRef = nAttr.create ("defaultReference", "defr", MFnNumericData::k3Double);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(0.0, 1.0, 0.0);
	addAttribute(aDefaultRef);

	aOutputLength = eAttr.create( "OUTLEN_NORMALIZE", "olen", OUTLEN_NORMALIZE);
	eAttr.addField("normalize", OUTLEN_NORMALIZE);
	eAttr.addField("original", OUTLEN_ORIGINAL);
	eAttr.addField("projected", OUTLEN_PROJECTED);
	eAttr.setKeyable(true);
	eAttr.setHidden(false);
	st = addAttribute( aOutputLength ); mser;

	aOutput = tAttr.create ("output", "out", MFnData::kVectorArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	addAttribute (aOutput);

	// add attributes and setup dependencies




	attributeAffects (aDefaultRef, aOutput);
	attributeAffects (aOutputLength, aOutput);
	attributeAffects (aFront, aOutput);
	attributeAffects (aUp, aOutput);




	return MS::kSuccess;
}

orthogonalPP::orthogonalPP () {}
orthogonalPP::~orthogonalPP () {}

MStatus orthogonalPP::compute(const MPlug &plug, MDataBlock &data) {

	if (!(plug == aOutput)) 	{ return MS::kUnknownParameter; }

	MStatus st;
	MFn::Type type = MFn::kInvalid;

	// Get inputs
	MDataHandle hIn1 = data.inputValue(aFront);
	MObject objIn1 = hIn1.data();
	MVectorArray vec = MFnVectorArrayData(objIn1).array();

	MDataHandle hIn2 = data.inputValue(aUp);
	MObject objIn2 = hIn2.data();
	MVectorArray ref = MFnVectorArrayData(objIn2).array();

	short outLen = data.inputValue(aOutputLength).asShort();

	MVector vD = data.inputValue(aDefaultRef).asVector();


	int len = vec.length();
	if (len != ref.length()) {
		ref = MVectorArray(len, vD);
	}

	MVectorArray out(len);
	MVector cross;
	if (outLen == OUTLEN_NORMALIZE) {
		for (int i = 0; i < len; i++) {
			if (vec[i].isParallel(ref[i])) {out[i] = MVector::zero; continue;}
			cross = vec[i] ^ ref[i];
			out[i] = (cross ^ ref[i]).normal();
		}
	}
	else if (outLen == OUTLEN_ORIGINAL) {
		for (int i = 0; i < len; i++) {
			if (vec[i].isParallel(ref[i])) {out[i] = MVector::zero; continue;}
			cross = vec[i] ^ ref[i];
			out[i] = (cross ^ ref[i]).normal() * vec[i].length();
		}
	}
	else {
		// projected
		for (int i = 0; i < len; i++) {
			if (vec[i].isParallel(ref[i])) {out[i] = MVector::zero; continue;}
			cross = vec[i] ^ ref[i];
			MVector tmp = (cross ^ ref[i]).normal();
			out[i] = tmp * double(tmp * vec[i]); // length of dot prod
		}
	}


	MDataHandle hOut = data.outputValue(aOutput);
	MFnVectorArrayData fnOut;
	MObject objOut = fnOut.create(out);
	hOut.set(objOut);
	data.setClean(plug);
	return MS::kSuccess;


}
