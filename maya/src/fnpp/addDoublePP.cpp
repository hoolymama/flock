#include <maya/MFnTypedAttribute.h>


#include <maya/MFnNumericAttribute.h>

#include <maya/MFnDoubleArrayData.h>
#include <maya/MDoubleArray.h>

#include <maya/MGlobal.h>

#include "errorMacros.h"
#include "addDoublePP.h"
#include "jMayaIds.h"

MTypeId addDoublePP::id(k_addDoublePP);
MObject addDoublePP::aInput;
MObject addDoublePP::aScalarInput;
MObject addDoublePP::aOutput;

void *addDoublePP::creator () {
	return new addDoublePP;
}
/// Post constructor
void
addDoublePP::postConstructor() {}

MStatus addDoublePP::initialize () {

	MStatus st;
	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;


	aInput = tAttr.create ("input", "in", MFnData::kDoubleArray);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setArray(true);
	addAttribute(aInput);


	aScalarInput = nAttr.create("scalarInput", "sin", MFnNumericData::kDouble);
	nAttr.setWritable(true);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute (aScalarInput);


	aOutput = tAttr.create ("output", "out", MFnData::kDoubleArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	addAttribute (aOutput);

	attributeAffects (aInput, aOutput);
	attributeAffects (aScalarInput, aOutput);

	return MS::kSuccess;
}

addDoublePP::addDoublePP () {}
addDoublePP::~addDoublePP () {}

MStatus addDoublePP::compute (const MPlug &plug, MDataBlock &data)
{

	if (!(plug == aOutput ) )	{ return MS::kUnknownParameter; }


	MStatus st;

	double scalarValue = data.inputValue(aScalarInput).asDouble();

	MArrayDataHandle hInArray = data.inputArrayValue(aInput, &st); mser;
	unsigned canonicalLength = 0;
	bool started = false;
	MDoubleArray out;
	unsigned num = hInArray.elementCount();
	unsigned len;
	for (unsigned i = 0; i < num; i++, hInArray.next()) {

		MDataHandle hIn = hInArray.inputValue(&st);
		if (st.error()) { continue; }
		MObject dIn = hIn.data();
		MDoubleArray in = MFnDoubleArrayData(dIn).array();
		len = in.length();
		if (! started ) {
			canonicalLength = len;
			out.copy(in);
			started = true;
		}
		else {
			if (len == canonicalLength) {
				for (unsigned i = 0; i < len; i++) {
					out[i] += in[i];
				}
			}
		}
	}
	if (scalarValue != 0 && started) {
		for (unsigned i = 0; i < len; i++) {
			out[i] += scalarValue;
		}
	}
	MDataHandle hOut = data.outputValue(aOutput);
	MFnDoubleArrayData fnOut;
	MObject dOut = fnOut.create(out);
	hOut.set(dOut);
	data.setClean(plug);

	return MS::kSuccess;
}
