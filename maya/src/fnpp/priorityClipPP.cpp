#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnTypedAttribute.h>

#include <maya/MFnDependencyNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MVectorArray.h> 
#include <maya/MDoubleArray.h> 
#include <maya/MGlobal.h> 


#include "errorMacros.h"
#include "priorityClipPP.h"
#include "jMayaIds.h"


MTypeId priorityClipPP::id(k_priorityClipPP);
MObject priorityClipPP::aInput1;
MObject priorityClipPP::aInput2;   
MObject priorityClipPP::aMax;	
MObject priorityClipPP::aOutput1;
MObject priorityClipPP::aOutput2;
MObject priorityClipPP::aOutputSum;



void * priorityClipPP::creator () {
	return new priorityClipPP;
}

/// Post constructor
void
	priorityClipPP::postConstructor()
{
	MPxNode::postConstructor();

	setExistWithoutInConnections(true);
	setExistWithoutOutConnections(true);
}

MStatus priorityClipPP::initialize () {

	MStatus st;


	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;



	aInput1 = tAttr.create("input1", "in1",MFnData::kVectorArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);
	addAttribute(aInput1);

	aInput2 = tAttr.create("input2", "in2",MFnData::kVectorArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	tAttr.setCached(false);
	addAttribute(aInput2);

	// aNormalizeInput1 = nAttr.create( "normalizeInput1", "ni",MFnNumericData::kBoolean);
	// nAttr.setStorable(true);
	// nAttr.setKeyable(true);
	// nAttr.setDefault(false);
	// addAttribute( aNormalizeInput1 ); 

	aMax = nAttr.create ("max", "mx", MFnNumericData::kDouble);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(1.0);
	addAttribute (aMax);

	aOutput1 = tAttr.create ("output1", "out1",MFnData::kVectorArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	addAttribute (aOutput1);

	aOutput2 = tAttr.create ("output2", "out2",MFnData::kVectorArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	addAttribute (aOutput2);

	aOutputSum = tAttr.create ("outputSum", "sum",MFnData::kVectorArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	addAttribute (aOutputSum);


	attributeAffects (aInput1, aOutput1);
	attributeAffects (aInput2, aOutput1);
	attributeAffects (aMax, aOutput1);

	attributeAffects (aInput1, aOutput2);
	attributeAffects (aInput2, aOutput2);
	attributeAffects (aMax, aOutput2);
	

	attributeAffects (aInput1, aOutputSum);
	attributeAffects (aInput2, aOutputSum);
	attributeAffects (aMax, aOutputSum);
	
	// attributeAffects (aDefault, aOutput);





	return MS::kSuccess;
}

priorityClipPP::priorityClipPP () {}
priorityClipPP::~priorityClipPP () {}

MStatus priorityClipPP::compute(const MPlug& plug, MDataBlock& data) {

	if (!(plug == aOutput1 || plug == aOutput2 || plug == aOutputSum)) 	return MS::kUnknownParameter;			

	MStatus st;
	MFn::Type type = MFn::kInvalid;

		// Get inputs
	MDataHandle hIn1 = data.inputValue(aInput1);
	MObject objIn1 = hIn1.data();
	MVectorArray in1 = MFnVectorArrayData(objIn1).array();

	MDataHandle hIn2 = data.inputValue(aInput2);
	MObject objIn2 = hIn2.data();
	MVectorArray in2 = MFnVectorArrayData(objIn2).array();

	double mx = data.inputValue(aMax).asDouble();

	//bool norm = data.inputValue(aNormalizeInput1).asBool();
	
	int len = in1.length();
	int len2 = in2.length();
	if (len != len2) {
		MString msg("priorityClipPP input length mismatch: ");
		msg += len;
		msg += " - ";
		msg += len2;
		msg += " - ";
		msg += MFnDependencyNode(thisMObject()).name();
		

		MGlobal::displayWarning(msg);
		return MS::kUnknownParameter;	
	}

	//cerr << "INPUT LENGTH:" << len << endl;
	MVectorArray out1(len);
	MVectorArray out2(len);
	MVectorArray outSum(len);

	for (int i = 0;i<len;i++) {
		double mag1 = in1[i].length();

		// mag1 big so only mag1
		if (mag1 >= mx) {
			out1[i] = in1[i].normal() * mx;
			out2[i] = MVector::zero;
			continue;
		}

		double mag2 = in2[i].length();
		if ((mag1 + mag2) < mx) { // max is very big so nothing changes
			out1[i] = in1[i];
			out2[i] = in2[i];
			continue;
		}
		// 2 will be clipped
		out1[i] = in1[i];
		double out2Mag = mx - mag1;
		out2[i] = in2[i].normal() * out2Mag;
	}
	for (int i = 0; i < len; ++i)
	{
		outSum[i] = out1[i] + out2[i];
	}

	MDataHandle hOut1 = data.outputValue(aOutput1);
	MFnVectorArrayData fnOut1;
	MObject objOut1 = fnOut1.create(out1);		
	hOut1.set(objOut1);
	data.setClean(aOutput1);	

	MDataHandle hOut2 = data.outputValue(aOutput2);
	MFnVectorArrayData fnOut2;
	MObject objOut2 = fnOut2.create(out2);		
	hOut2.set(objOut2);
	data.setClean(aOutput2);	

	MDataHandle hOutSum = data.outputValue(aOutputSum);
	MFnVectorArrayData fnOutSum;
	MObject objOutSum = fnOutSum.create(outSum);		
	hOutSum.set(objOutSum);
	data.setClean(aOutputSum);

	return MS::kSuccess;


}
