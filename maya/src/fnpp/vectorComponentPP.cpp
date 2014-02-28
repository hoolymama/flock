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
#include "vectorComponentPP.h"
#include "jMayaIds.h"

MTypeId vectorComponentPP::id(k_vectorComponentPP);

MObject vectorComponentPP::aInput;
MObject vectorComponentPP::aComponent;
MObject vectorComponentPP::aOutput;



void * vectorComponentPP::creator () {
	return new vectorComponentPP;
}
/// Post constructor
void
vectorComponentPP::postConstructor()
{

}

MStatus vectorComponentPP::initialize () {

	MStatus st;
	MFnTypedAttribute tAttr;
	MFnEnumAttribute eAttr;
	
	aInput = tAttr.create("input", "in",MFnData::kVectorArray);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setWritable(true);
	addAttribute(aInput);	
	
	aComponent = eAttr.create ("component", "cmp");
	eAttr.addField( "x",		vectorComponentPP::kXComp);
	eAttr.addField( "y",		vectorComponentPP::kYComp);
	eAttr.addField( "z",		vectorComponentPP::kZComp);
	eAttr.addField( "mag",		vectorComponentPP::kMag);
	eAttr.addField( "squaredMag",		vectorComponentPP::kSquaredMag);
	eAttr.setDefault(	vectorComponentPP::kMag);
	eAttr.setHidden(false);	
	addAttribute (aComponent);

	
	aOutput = tAttr.create ("output", "out",MFnData::kDoubleArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	addAttribute (aOutput);


	
	attributeAffects (aInput, aOutput);
	attributeAffects (aComponent, aOutput);

	
	return MS::kSuccess;
}

vectorComponentPP::vectorComponentPP () {}
vectorComponentPP::~vectorComponentPP () {}

MStatus vectorComponentPP::compute (const MPlug& plug, MDataBlock& data)
{

	 //cerr << "in vectorComponentPP compute" << endl;

	if(!(plug == aOutput) )	return MS::kUnknownParameter;
	 //cerr << "plug == aOutput" << endl;

	MStatus st;
	
	
	MVectorArray inputs = MFnVectorArrayData(data.inputValue(aInput).data()).array();
	
	vectorComponentPP::ComponentType comp = vectorComponentPP::ComponentType(data.inputValue( aComponent).asShort());

	unsigned n = inputs.length();
	
	//cerr << "vectorComponentPP::compute inputs.length()" << inputs.length() << endl;
	MDoubleArray out(n);
	
	switch ( comp ) {
		case vectorComponentPP::kXComp:
		for (unsigned i = 0;i<n;i++) {
			MVector & in = inputs[i] ; 
			out[i] =  in.x;
		}
		break;
		case vectorComponentPP::kYComp:
		for (unsigned i = 0;i<n;i++) {
			MVector & in = inputs[i] ; 
			out[i] =  in.y;
		}
		break;
		case vectorComponentPP::kZComp:
		for (unsigned i = 0;i<n;i++) {
			MVector & in = inputs[i] ; 
			out[i] =  in.z;
		}
		break;
		
		case vectorComponentPP::kMag:
		//cerr << "vectorComponentPP::compute kMag" << endl;
		for (unsigned i = 0;i<n;i++) {
			MVector & in = inputs[i] ; 
			out[i] =  in.length();
		}
		break;
		
		case vectorComponentPP::kSquaredMag:
		for (unsigned i = 0;i<n;i++) {
			MVector & in = inputs[i] ; 
			out[i] =  ((in.x*in.x) +  (in.y*in.y) + (in.z*in.z));
		}
		break;
		default:
		for (unsigned i = 0;i<n;i++) {
			MVector & in = inputs[i] ; 
			out[i] =  in.length();
		}
	}



	MDataHandle hOut = data.outputValue(aOutput);	
	MFnDoubleArrayData fnOut;
	MObject dOut = fnOut.create(out);
	hOut.set(dOut);

	data.setClean(plug);
	// cerr << "out vectorComponentPP compute" << endl;

	return MS::kSuccess;															
}
