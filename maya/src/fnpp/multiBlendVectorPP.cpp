#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnTypedAttribute.h>

#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>

#include "errorMacros.h"
#include "multiBlendVectorPP.h"
#include "jMayaIds.h"



MTypeId multiBlendVectorPP::id(k_multiBlendVectorPP);

MObject multiBlendVectorPP::aVectorArray;
MObject multiBlendVectorPP::aWeightArray;
MObject multiBlendVectorPP::aDefaultWeight;
MObject multiBlendVectorPP::aInput;

MObject multiBlendVectorPP::aNormalizeWeights;


MObject multiBlendVectorPP::aOutput;


void *multiBlendVectorPP::creator () {
	return new multiBlendVectorPP;
}

/// Post constructor
void
multiBlendVectorPP::postConstructor()
{
	MPxNode::postConstructor();

	setExistWithoutInConnections(true);
	setExistWithoutOutConnections(true);
}

MStatus multiBlendVectorPP::initialize () {

	MStatus st;

	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;
	MFnCompoundAttribute cAttr;

	aVectorArray = tAttr.create("vectorArray", "va", MFnData::kVectorArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);

	aWeightArray = tAttr.create("weightArray", "wa", MFnData::kDoubleArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);

	aDefaultWeight = nAttr.create("defaultWeight", "dwt", MFnNumericData::kDouble);
	nAttr.setWritable(true);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(1.0);

	aInput = cAttr.create("input", "in");
	cAttr.addChild(aVectorArray);
	cAttr.addChild(aWeightArray);
	cAttr.addChild(aDefaultWeight);
	cAttr.setArray(true);
	cAttr.setWritable(true);
	cAttr.setStorable(true);
	cAttr.setKeyable(true);
	addAttribute(aInput );


	aNormalizeWeights = nAttr.create( "normalizeWeights", "nml", MFnNumericData::kBoolean);
	nAttr.setHidden(false);
	nAttr.setStorable(true);
	nAttr.setReadable(true);
	nAttr.setDefault(false);
	addAttribute(aNormalizeWeights );

	aOutput = tAttr.create ("output", "out", MFnData::kVectorArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	addAttribute (aOutput);


	attributeAffects (aVectorArray, aOutput);
	attributeAffects (aWeightArray, aOutput);
	attributeAffects (aDefaultWeight, aOutput);
	attributeAffects (aInput, aOutput);

	return MS::kSuccess;
}

multiBlendVectorPP::multiBlendVectorPP () {}
multiBlendVectorPP::~multiBlendVectorPP () {}

MStatus multiBlendVectorPP::compute(const MPlug &plug, MDataBlock &data) {

	if (!(plug == aOutput)) 	{ return MS::kUnknownParameter; }

	MStatus st;
	MFn::Type type = MFn::kInvalid;
	const double epsilon = 0.000001;


	bool normalize = data.inputValue(aNormalizeWeights).asBool();

	MArrayDataHandle hInputs = data.inputArrayValue( aInput , &st);  msert;

	MVectorArray out;
	MDoubleArray totalWeight;
	bool started = false;
	unsigned len = 0;
	unsigned nInputs = hInputs.elementCount();
	for (unsigned i = 0; i < nInputs; i++, hInputs.next()) {

		// unsigned int el = hInputs.elementIndex(&st);
		if (st != MS::kSuccess) { continue; }
		MDataHandle hInput = hInputs.inputValue( &st );
		if (st != MS::kSuccess) { continue; }

		MObject dVectorArray =  hInput.child( aVectorArray).data(); ;
		MVectorArray vectorArray = MFnVectorArrayData(dVectorArray).array();
		if (!started) {
			started = true;
			len =  vectorArray.length();
			out = MVectorArray(len);
			totalWeight = MDoubleArray(len);

		}
		else {
			if (vectorArray.length() != len) {
				continue;
			}
		}

		MDoubleArray weightArray = MFnDoubleArrayData( hInput.child(
		                             aWeightArray).data() ).array();
		double defaultWeight =  hInput.child(aDefaultWeight).asDouble();

		if (weightArray.length() != len) {
			weightArray = MDoubleArray(len, defaultWeight);
		}
		else {
			for (unsigned i = 0; i < len; i++) {
				weightArray[i] = weightArray[i] * defaultWeight;
			}
		}

		for (unsigned i = 0; i < len; i++) {
			out[i] +=  vectorArray[i] * weightArray[i];
		}
		if (normalize) {
			totalWeight[i] += weightArray[i];
		}
	}

	if (normalize) {
		for (int i = 0; i < len; ++i)
		{
			if (totalWeight[i] > epsilon) {
				out[i] = out[i] / totalWeight[i];
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
