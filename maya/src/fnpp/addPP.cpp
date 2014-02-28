#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MDoubleArray.h>
#include <maya/MVectorArray.h>
#include <maya/MGlobal.h>

#include "errorMacros.h"
#include "addPP.h"
#include "jMayaIds.h"

MTypeId addPP::id(k_addPP);
MObject addPP::aInput;
MObject addPP::aDefault;
MObject addPP::aOutputD;
MObject addPP::aOutputV;
MObject addPP::aCompute;


void * addPP::creator () {
	return new addPP;
}
/// Post constructor
void
	addPP::postConstructor()
{

}

MStatus addPP::initialize () {

	MStatus st;
	MFnGenericAttribute gAttr;
	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnEnumAttribute eAttr;

	aInput = gAttr.create("input", "in");
	gAttr.addAccept(MFnData::kDoubleArray);
	gAttr.addAccept(MFnData::kVectorArray);
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
	nAttr.setDefault(0.0,0.0,0.0);

	aOutputD = tAttr.create ("outputD", "outd",MFnData::kDoubleArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);

	aOutputV = tAttr.create ("outputV", "outv",MFnData::kVectorArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	/*
	aCompute = eAttr.create( "compute", "cpt", COMPUTE_SUCCESS  , &st ); ert;
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

addPP::addPP () {}
addPP::~addPP () {}

MStatus addPP::compute (const MPlug& plug, MDataBlock& data)
{

	// cerr << "in addPP compute" << endl;
	if(!((plug == aOutputD ) || (plug == aOutputV)))	return MS::kUnknownParameter;

	MVector vD = data.inputValue(aDefault).asVector();

	MStatus st;
	MArrayDataHandle hInArray = data.inputArrayValue(aInput, &st);er;
	const unsigned nStartElement = hInArray.elementIndex();	
	MFn::Type type = MFn::kInvalid;
	unsigned nLength = 0;	
	bool gotData = false;

	if(plug == aOutputD )  {
		double defVal = vD.x;
		MDoubleArray out;
		MDataHandle hOut = data.outputValue(aOutputD);
		do
		{ 
			MDataHandle hIn = hInArray.inputValue(&st);
			if(st.error()) continue;

			MObject objIn = hIn.data();
			if(objIn.hasFn(MFn::kDoubleArrayData))
			{	
				MDoubleArray in = MFnDoubleArrayData(objIn).array();

				unsigned inLength = in.length();
				if (! gotData ) { // start accumulating
					nLength = inLength;
					gotData = true;
					out.copy(in);
					// add in the default val
					for(unsigned i = 0; i < nLength;i++) out[i] += defVal;
				} else {
					if (nLength == inLength) {
						for(unsigned i = 0; i < nLength;i++) out[i] += in[i];
					}
				}

			} 
		} while(hInArray.next() == MS::kSuccess);
		
		MFnDoubleArrayData fnOut;
		MObject objOut = fnOut.create(out);
		hOut.set(objOut);

	} else if (plug == aOutputV )  {
		MVectorArray out;
		MDataHandle hOut = data.outputValue(aOutputV);
		do
		{ 
			MDataHandle hIn = hInArray.inputValue(&st);
			if(st.error()) continue;

			MObject objIn = hIn.data();
			if(objIn.hasFn(MFn::kVectorArrayData))
			{	
				MVectorArray in = MFnVectorArrayData(objIn).array();

				unsigned inLength = in.length();
				if (! gotData ) { // start accumulating
					nLength = inLength;
					gotData = true;
					out.copy(in);
					for(unsigned i = 0; i < nLength;i++) out[i] += vD;
				} else {
					if (nLength == inLength) {
						for(unsigned i = 0; i < nLength;i++) out[i] += in[i];
					}
				}
			} 
		} while(hInArray.next() == MS::kSuccess);
		MFnVectorArrayData fnOut;
		MObject objOut = fnOut.create(out);
		hOut.set(objOut);	
	}

	data.setClean(plug);
	// cerr << "out addPP compute" << endl;

	return MS::kSuccess;															
}
