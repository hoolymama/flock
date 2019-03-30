#include <maya/MFnTypedAttribute.h>


#include <maya/MFnNumericAttribute.h>

#include <maya/MFnVectorArrayData.h>
#include <maya/MVectorArray.h>

#include <maya/MGlobal.h>

#include "errorMacros.h"
#include "addVectorPP.h"
#include "jMayaIds.h"

MTypeId addVectorPP::id(k_addVectorPP);
MObject addVectorPP::aInput;
MObject addVectorPP::aScalarInput;
MObject addVectorPP::aOutput;

void *addVectorPP::creator () {
	return new addVectorPP;
}
/// Post constructor
void
addVectorPP::postConstructor() {}

MStatus addVectorPP::initialize () {

	MStatus st;
	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;


	aInput = tAttr.create ("input", "in", MFnData::kVectorArray);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setArray(true);
	addAttribute(aInput);

	aScalarInput = nAttr.create("scalarInput", "sin", MFnNumericData::k3Double);
	nAttr.setWritable(true);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	addAttribute (aScalarInput);


	aOutput = tAttr.create ("output", "out", MFnData::kVectorArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	addAttribute (aOutput);

	attributeAffects (aInput, aOutput);
	attributeAffects (aScalarInput, aOutput);

	return MS::kSuccess;
}

addVectorPP::addVectorPP () {}
addVectorPP::~addVectorPP () {}

MStatus addVectorPP::compute (const MPlug &plug, MDataBlock &data)
{

	if (!(plug == aOutput ) )	{ return MS::kUnknownParameter; }


	MStatus st;

	const double3 &scalarValue = data.inputValue(aScalarInput).asDouble3();

	MArrayDataHandle hInArray = data.inputArrayValue(aInput, &st); mser;
	unsigned canonicalLength = 0;
	bool started = false;
	MVectorArray out;
	unsigned num = hInArray.elementCount();
	//cerr << "here 1" << endl;
	unsigned len;
	for (unsigned i = 0; i < num; i++, hInArray.next()) {

		// cerr << "addVectorPP loop" << endl;
		MDataHandle hIn = hInArray.inputValue(&st);
		if (st.error()) { continue; }
		MObject dIn = hIn.data();
		MVectorArray in = MFnVectorArrayData(dIn).array();
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

	MVector sv = MVector(scalarValue[0], scalarValue[1], scalarValue[2]);

	if ((! sv.isEquivalent(MVector::zero))  &&  started ) {
		for (unsigned i = 0; i < len; i++) {
			out[i] += sv;
		}
	}

	MDataHandle hOut = data.outputValue(aOutput);
	MFnVectorArrayData fnOut;
	MObject dOut = fnOut.create(out);
	hOut.set(dOut);
	data.setClean(plug);

	return MS::kSuccess;
}
