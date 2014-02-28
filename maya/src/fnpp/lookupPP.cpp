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
#include <maya/MFnAnimCurve.h>

#include "errorMacros.h"
#include "attrUtils.h"
#include "lookup.h"
#include "lookupPP.h"
#include "jMayaIds.h"

MTypeId lookupPP::id(k_lookupPP);
MObject lookupPP::aInput;
MObject lookupPP::aCurveResolution;
MObject lookupPP::aFnCurve;
MObject lookupPP::aOutputD;
MObject lookupPP::aOutputV;


void * lookupPP::creator () {
	return new lookupPP;
}
/// Post constructor
void
lookupPP::postConstructor()
{

}

MStatus lookupPP::initialize () {

	MStatus st;
	MFnGenericAttribute gAttr;
	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnEnumAttribute eAttr;
	
	aInput = gAttr.create("input", "in");
	gAttr.addAccept(MFnData::kDoubleArray);
	gAttr.addAccept(MFnData::kVectorArray);
	gAttr.setWritable(true);
	gAttr.setDisconnectBehavior(MFnAttribute::kReset);
	gAttr.setStorable(false);
	gAttr.setReadable(false);
	gAttr.setWritable(true);
	gAttr.setCached(false);

	aCurveResolution = nAttr.create("curveResolution","crs", MFnNumericData::kLong);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(20);
	st = addAttribute( aCurveResolution ); ert;
	
	aFnCurve = nAttr.create("functionCurve","fcv", MFnNumericData::kFloat);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	st = addAttribute( aFnCurve ); ert;
	
	aOutputD = tAttr.create ("outputD", "outd",MFnData::kDoubleArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);

	aOutputV = tAttr.create ("outputV", "outv",MFnData::kVectorArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);

	addAttribute(aInput);

	addAttribute (aFnCurve);
	addAttribute (aOutputD);
	addAttribute (aOutputV);

	attributeAffects (aInput, aOutputD);
	attributeAffects (aFnCurve, aOutputD);
	attributeAffects (aCurveResolution, aOutputD);
	
	attributeAffects (aInput, aOutputV);
	attributeAffects (aFnCurve, aOutputV);
	attributeAffects (aCurveResolution, aOutputV);
	
	return MS::kSuccess;
}

lookupPP::lookupPP () {}
lookupPP::~lookupPP () {}
MStatus lookupPP::compute (const MPlug& plug, MDataBlock& data)
{

	// cerr << "in lookupPP compute" << endl;


	if(!((plug == aOutputD ) || (plug == aOutputV)))
		return MS::kUnknownParameter;
	
	MStatus st;

	MString nodeName = MFnDependencyNode(thisMObject()).name();
	// build lookup curve
	
	
	//////////////////////////////////////////////////////////////

	unsigned cRes = data.inputValue(aCurveResolution).asLong();
	
	
	MFnAnimCurve mAniFn; 
	lookup  mLut;
	
	st = getAniCurveFn(thisMObject(),aFnCurve, mAniFn);
	if (st.error()) {
		float constVal = data.inputValue(aFnCurve).asFloat();
		mLut = lookup(constVal);
	} else {
		mLut = lookup(mAniFn, cRes); 
	}

	MDoubleArray outD;
	MVectorArray outV;

	

	
	MFn::Type type = MFn::kInvalid;
	// unsigned nLength = 0;

	MDataHandle hIn = data.inputValue(aInput);
	MObject objIn = hIn.data();
	if(objIn.hasFn(MFn::kVectorArrayData))
	{
		MVectorArray vals =  MFnVectorArrayData(objIn).array();
		unsigned nVals = vals.length();
		outV.copy(vals);
		float mag;		
		for (unsigned i = 0;i<nVals;i++) {
			mag = float(outV[i].length());
			if (mag > 0.0f) {
				outV[i] = (mLut.evaluate(mag) / mag) * outV[i] ;
			}
		}
		
		
		type = MFn::kVectorArrayData;
	} else if(objIn.hasFn(MFn::kDoubleArrayData)) {
		MDoubleArray vals =  MFnDoubleArrayData(objIn).array();
		unsigned nVals = vals.length();
		// cerr << "double input lookupVals length = " << nVals << endl;
		outD.copy(vals);
		for (unsigned i = 0;i<nVals;i++) {
			outD[i] = mLut.evaluate(float(outD[i])) ;
		}
		type = MFn::kDoubleArrayData;
	}
		
	// tests for valid connections
	if(type == MFn::kInvalid) {
		MGlobal::displayError("Invalid input type " + nodeName);
		return MS::kUnknownParameter;			
	}
	if(plug == aOutputD && type == MFn::kVectorArrayData)
	{
		MGlobal::displayError("Invalid output plug, should be a vector array, not a double array");
		return MS::kUnknownParameter;			
	} else if(plug == aOutputV && type == MFn::kDoubleArrayData)
	{
		MGlobal::displayError("Invalid output plug, should be a double array, not a vector array");
		return MS::kUnknownParameter;			
	}	
	

	if(plug == aOutputV)
	{
		// Prepare output
		MDataHandle hOut = data.outputValue(aOutputV);

		MFnVectorArrayData fnOut;
		MObject objOut = fnOut.create(outV);
		hOut.set(objOut);
		
	}
	else if(plug == aOutputD)
	{
		MDataHandle hOut = data.outputValue(aOutputD);
		// cerr << "outD.length() = " << outD.length() << endl;
		MFnDoubleArrayData fnOut;
		MObject objOut = fnOut.create(outD);
		hOut.set(objOut);
	}
	
	data.setClean(plug);
	// cerr << "out lookupPP compute" << endl;

	return MS::kSuccess;															
}
