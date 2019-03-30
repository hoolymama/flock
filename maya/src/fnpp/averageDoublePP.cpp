#include <list>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnTypedAttribute.h>

#include <maya/MFnUnitAttribute.h>

#include <maya/MFnNumericAttribute.h>

#include <maya/MDoubleArray.h>
#include <maya/MAnimControl.h>

#include "errorMacros.h"
#include "averageDoublePP.h"
#include "jMayaIds.h"


MTypeId averageDoublePP::id(k_averageDoublePP);
MObject averageDoublePP::aInput;
MObject averageDoublePP::aFrames ;
MObject averageDoublePP::aOutput;
MObject averageDoublePP::aCurrentTime;
MObject averageDoublePP::aStartTime;

averageDoublePP::averageDoublePP():
	m_q()
{	m_lastTimeIEvaluated = MAnimControl::currentTime(); }

averageDoublePP::~averageDoublePP() {}

void *averageDoublePP::creator () {
	return new averageDoublePP;
}

/// Post constructor
void
averageDoublePP::postConstructor()
{
	MPxNode::postConstructor();

	// setExistWithoutInConnections(true);
	// setExistWithoutOutConnections(true);
}

MStatus averageDoublePP::initialize () {

	MStatus st;


	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;
	MFnUnitAttribute	uAttr;

	aInput = tAttr.create("input", "in", MFnData::kDoubleArray);
	tAttr.setWritable(true);
	tAttr.setStorable(false);
	tAttr.setReadable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	st =  addAttribute(aInput);  mser;

	aFrames = nAttr.create("frames", "fms", MFnNumericData::kInt);
	nAttr.setWritable(true);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(3);
	st =  addAttribute(aFrames);  mser;

	aCurrentTime = uAttr.create( "currentTime", "ct", MFnUnitAttribute::kTime );
	uAttr.setStorable(true);
	st =  addAttribute(aCurrentTime);  mser;

	aStartTime = uAttr.create( "startTime", "st", MFnUnitAttribute::kTime );
	uAttr.setStorable(true);
	st =  addAttribute(aStartTime);  mser;

	aOutput = tAttr.create ("output", "out", MFnData::kDoubleArray);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);
	st =  addAttribute(aOutput);  mser;

	attributeAffects (aInput, aOutput);
	attributeAffects (aFrames, aOutput);
	attributeAffects (aCurrentTime, aOutput);
	attributeAffects (aStartTime, aOutput);




	return MS::kSuccess;
}


MStatus averageDoublePP::compute(const MPlug &plug, MDataBlock &data) {

	if (!(plug == aOutput)) 	{ return MS::kUnknownParameter; }
	MStatus st;

	MTime sT =  data.inputValue( aStartTime).asTime();
	MTime cT =  data.inputValue( aCurrentTime).asTime();
	MTime dT = cT - m_lastTimeIEvaluated;
	m_lastTimeIEvaluated = cT;
	double dt = dT.as( MTime::kSeconds );



	int frames = data.inputValue(aFrames).asInt();
	if (frames < 1) { frames = 1; }

	// Get inputs
	MDataHandle hIn = data.inputValue(aInput);
	MObject objIn = hIn.data();
	MDoubleArray in = MFnDoubleArrayData(objIn).array();

	int len = in.length();

	MDoubleArray out(len);

	if (dt < 0 || cT < sT) {
		// cerr << "time backwards or before start" << endl;
		m_q.clear();
	}
	else {
		if (m_q.size() > 0) {
			if (m_q.front().length() != len) {
				// cerr << "clearing because length difference" << endl;
				m_q.clear();
			}
		}

		MDoubleArray  vals;
		vals.copy(in);
		m_q.push_front(vals);
		if (m_q.size() > frames) {
			m_q.pop_back();
		}

		if (m_q.size() > 0) {
			const double recip =  1.0 / m_q.size();


			std::list<MDoubleArray>::const_iterator iter = m_q.begin();
			while (iter != m_q.end()) {


				for (int i = 0; i < len; ++i)
				{
					out[i] += (*iter)[i];
				}
				iter++;
			}
			for (int i = 0; i < len; ++i)
			{
				out[i] = out[i] * recip ;
			}
		}
	}


	MDataHandle hOut = data.outputValue(aOutput);
	MFnDoubleArrayData fnOut;
	MObject objOut = fnOut.create(out);
	hOut.set(objOut);
	data.setClean(plug);
	return MS::kSuccess;


}
