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
#include "accumulatorPP.h"
#include "jMayaIds.h"

MTypeId accumulatorPP::id(k_accumulatorPP);
MObject accumulatorPP::aInput;
MObject accumulatorPP::aOutput;

void * accumulatorPP::creator () {
	return new accumulatorPP;
}
/// Post constructor
void
	accumulatorPP::postConstructor()
{

}

MStatus accumulatorPP::initialize () {

	MStatus st;
	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;

	aInput = tAttr.create ("input", "in",MFnData::kVectorArray);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setArray(true);
	addAttribute(aInput);

	aOutput = tAttr.create ("output", "out",MFnData::kVectorArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	addAttribute (aOutput);	

	attributeAffects (aInput, aOutput);
	return MS::kSuccess;
}

accumulatorPP::accumulatorPP () {}
accumulatorPP::~accumulatorPP () {}

MStatus accumulatorPP::compute (const MPlug& plug, MDataBlock& data)
{

	if(!(plug == aOutput ) )	return MS::kUnknownParameter;


	MStatus st;
	MArrayDataHandle hInArray = data.inputArrayValue(aInput, &st);er;
	const unsigned nStartElement = hInArray.elementIndex();	
	unsigned canonicalLength = 0;	
	MVectorArray out;
	do{ 
		MDataHandle hIn = hInArray.inputValue(&st);
		if(st.error()) continue;
		MObject dIn = hIn.data();
		MVectorArray in = MFnVectorArrayData(dIn).array();
		unsigned len = in.length();
		if (canonicalLength == 0) { 
			canonicalLength = len;
			out.copy(in);
		} else {
			if (len == canonicalLength) {
				for(unsigned i = 0; i < len;i++) out[i] += in[i];
			}
		}

	} while(hInArray.next() == MS::kSuccess);

	MDataHandle hOut = data.outputValue(aOutput);
	MFnVectorArrayData fnOut;
	MObject dOut = fnOut.create(out);
	hOut.set(dOut);	
	data.setClean(plug);

	return MS::kSuccess;															
}
