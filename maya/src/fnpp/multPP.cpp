#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnDependencyNode.h>


#include <maya/MFnGenericAttribute.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MDoubleArray.h>
#include <maya/MVectorArray.h>
#include <maya/MGlobal.h>

#include "errorMacros.h"
#include "multPP.h"
#include "jMayaIds.h"

MTypeId multPP::id(k_multPP);
MObject multPP::aInput;
MObject multPP::aDefault;
MObject multPP::aOutputD;
MObject multPP::aOutputV;
MObject multPP::aCompute;


void *multPP::creator () {
	return new multPP;
}
/// Post constructor
void
multPP::postConstructor()
{

}

MStatus multPP::initialize () {

	MStatus st;
	MFnGenericAttribute gAttr;
	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnEnumAttribute eAttr;

	aInput = gAttr.create("input", "in");
	gAttr.addDataAccept(MFnData::kDoubleArray);
	gAttr.addDataAccept(MFnData::kVectorArray);
	gAttr.setWritable(true);
	gAttr.setArray(true);
	gAttr.setIndexMatters(false);
	gAttr.setDisconnectBehavior(MFnAttribute::kDelete);
	gAttr.setStorable(false);
	gAttr.setReadable(false);
	gAttr.setWritable(true);
	gAttr.setCached(false);

	aDefault = nAttr.create ("default", "def", MFnNumericData::k3Double);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(1.0, 1.0, 1.0);

	aOutputD = tAttr.create ("outputD", "outd", MFnData::kDoubleArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);

	aOutputV = tAttr.create ("outputV", "outv", MFnData::kVectorArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	/*
	aCompute = eAttr.create( "compute", "cpt", COMPUTE_SUCCESS  , &st ); msert;
		eAttr.addField( "success",	 COMPUTE_SUCCESS);
		eAttr.addField( "badInput",	 COMPUTE_INVALID_INPUT_DATA);
		eAttr.addField( "badOutput", COMPUTE_INVALID_OUTPUT_PLUG);
	*/
	//eAttr.setKeyable(true);
	eAttr.setHidden(false);
	//eAttr.setWritable(false);
	eAttr.setReadable(true);
	//eAttr.setConnectable(false);

	// add attributes and setup dependencies
	addAttribute(aInput);
	addAttribute (aDefault);
	addAttribute (aOutputD);
	addAttribute (aOutputV);
	addAttribute (aCompute);

	attributeAffects (aInput, aOutputD);
	attributeAffects (aDefault, aOutputD);
	attributeAffects (aInput, aOutputV);
	attributeAffects (aDefault, aOutputV);

	return MS::kSuccess;
}

multPP::multPP () {}
multPP::~multPP () {}

MStatus multPP::compute (const MPlug &plug, MDataBlock &data)
{

	// cerr << "in multPP compute" << endl;

	if (!((plug == aOutputD ) || (plug == aOutputV)))
	{ return MS::kUnknownParameter; }

	MStatus st;
	MArrayDataHandle hInArray = data.inputArrayValue(aInput, &st); mser;
	const unsigned nStartElement = hInArray.elementIndex();

	MDoubleArray outD;
	MVectorArray outV;
	MString nodeName = MFnDependencyNode(thisMObject()).name();
	// Do a prepass to decide:
	//   a) The type of data to output, and
	//   b) how many elements to process
	MFn::Type type = MFn::kInvalid;
	unsigned nLength = 0;
	do
	{
		MDataHandle hIn = hInArray.inputValue(&st);
		if (st.error()) { continue; }

		MObject objIn = hIn.data();
		if (objIn.hasFn(MFn::kVectorArrayData))
		{
			// Get length of this vector array
			const unsigned nInputLength = MFnVectorArrayData(objIn).array().length();

			if (type == MFn::kInvalid) { nLength = nInputLength; } // set length on the first pass
			//		cerr << nodeName << " length first pass = " << nLength << endl;
			if (nInputLength != nLength) { continue; } // ignore


			// If we encounter a vector array, then the only allowable output
			// is a vector array. This takes precedence over double array
			// output

			type = MFn::kVectorArrayData;
		}
		else if (objIn.hasFn(MFn::kDoubleArrayData))
		{
			// Get length of this double array
			const unsigned nInputLength = MFnDoubleArrayData(objIn).array().length();
			if (type == MFn::kInvalid)
			{
				nLength = nInputLength;

				// We may only pencil in double array output if no other input
				// has yet been encountered. Vector array output has
				// precendence
				type = MFn::kDoubleArrayData;
			}

		}

	}
	while (hInArray.next() == MS::kSuccess);




	// Did we find any input arrays?
	if (type == MFn::kInvalid) {
		// cerr << "MFn::kInvalid means there are no arrays" << endl;
		return MS::kUnknownParameter;
	}
	// If we are calculating a vectorArray plug, then we may legally derive
	// its output from doubleArray inputs (but not vice versa)
	if (plug == aOutputD && type == MFn::kVectorArrayData)
	{
		MGlobal::displayError("Invalid output plug");
		return MS::kUnknownParameter;
	}

	// Get default value
	MVector vD = data.inputValue(aDefault).asVector();
	// cerr << "got default vector" << endl;
	// Perform the appropriate series of multiplication operations
	hInArray.jumpToElement(nStartElement);
	// cerr << "here 1" << endl;
	if (plug == aOutputV)
	{
		// cerr << "aOutputV: and element = " << nStartElement  << endl;

		// Prepare output
		MDataHandle hOut = data.outputValue(aOutputV);
		//MObject objOut = hOut.data();
		MVectorArray out(nLength);
		// cerr << "out.length() = " << nLength  << endl;


		for (unsigned i = 0; i < nLength; i++)
		{ out.set(vD, i); }

		do
		{
			MDataHandle hIn = hInArray.inputValue(&st);
			if (st.error()) { continue; }

			MObject objIn = hIn.data();
			if (objIn.hasFn(MFn::kVectorArrayData))
			{
				MVectorArray in = MFnVectorArrayData(objIn).array();

				unsigned inLength = in.length();
				//		cerr << nodeName << " inLength = " << inLength << endl;
				if (nLength == inLength) {
					for (unsigned i = 0; i < nLength; i++)
					{
						MVector &o = out[i];
						const MVector &v = in[i];
						//	cerr << "VA"  << in[i] << endl;
						o.x *= v.x; o.y *= v.y; o.z *= v.z;
					}
				}
			}
			else if (objIn.hasFn(MFn::kDoubleArrayData))
			{
				MDoubleArray in = MFnDoubleArrayData(objIn).array();
				unsigned inLength = in.length();
				//	cerr << nodeName << " inLength = " << inLength << endl;

				if (nLength == inLength) {
					for (unsigned i = 0; i < nLength; i++) {
						out[i] *= in[i];
						//	cerr << "DA"  << in[i] << endl;
					}
				}
			}

		}
		while (hInArray.next() == MS::kSuccess);

		MFnVectorArrayData fnOut;
		MObject objOut = fnOut.create(out);
		hOut.set(objOut);

	}
	else if (plug == aOutputD)
	{
		MDataHandle hOut = data.outputValue(aOutputD);
		MDoubleArray out(nLength);

		for (unsigned i = 0; i < nLength; i++)
		{ out[i] = vD.x; }

		do
		{
			MDataHandle hIn = hInArray.inputValue(&st);
			if (st.error()) { continue; }

			MObject objIn = hIn.data();
			if (objIn.hasFn(MFn::kDoubleArrayData))
			{
				MDoubleArray in = MFnDoubleArrayData(objIn).array();
				unsigned inLength = in.length();
				//	cerr << nodeName << " inLength = " << inLength << endl;

				if (nLength == inLength) {
					for (unsigned i = 0; i < nLength; i++)
					{ out[i] *= in[i]; }
				}
			}

		}
		while (hInArray.next() == MS::kSuccess );

		MFnDoubleArrayData fnOut;
		MObject objOut = fnOut.create(out);
		hOut.set(objOut);
	}

	data.setClean(plug);
	// cerr << "out multPP compute" << endl;

	return MS::kSuccess;
}
