



#include <maya/MDataBlock.h>

#include <maya/MDataHandle.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFloatArray.h>


#include "actuator.h"
#include "errorMacros.h"
#include "hexUtil.h"



const double epsilon = 0.0000001;
actuator::actuator() {}




actuator::actuator(	MRampAttribute &rampAtt)
	: m_ramp(rampAtt)
{}

void actuator::set(	MDataHandle &handle, const MObject &node, const MPlug &rampPlug)
{
	MObject att, patt;
	MStatus st;
	MFnDependencyNode dn(node, &st); mser;

	att = dn.attribute( "actuatorRank", &st); mser;
	m_rank = (hexUtil::Rank)handle.child(att).asShort();

	att = dn.attribute( "actuatorInputAxis", &st); mser;
	m_axis = (hexUtil::ActuatorAxis)handle.child(att).asShort();

	att = dn.attribute( "actuatorOutputChannel", &st); mser;
	m_channel = (hexUtil::ActuatorChannel)handle.child(att).asShort();

	att = dn.attribute( "actuatorInputMirror", &st); mser;
	m_mirror = handle.child(att).asBool();

	patt = dn.attribute("actuatorInputRange", &st); mser;

	att = dn.attribute("actuatorInputMin", &st); mser;
	m_inputMin = handle.child(patt).child(att).asDouble();

	att = dn.attribute("actuatorInputMax", &st); mser;
	m_inputMax = handle.child(patt).child(att).asDouble();

	if (m_inputMin > m_inputMax) {
		double tmp = m_inputMin;
		m_inputMin = m_inputMax;
		m_inputMax = tmp;
	}
	if (m_inputMin >= m_inputMax - epsilon) {
		m_inputMax += epsilon;
	}


	patt = dn.attribute("actuatorOutputRange", &st); mser;

	att = dn.attribute("actuatorOutputMin", &st); mser;
	m_outputMin = handle.child(patt).child(att).asDouble();

	att = dn.attribute("actuatorOutputMax", &st); mser;
	m_outputMax = handle.child(patt).child(att).asDouble();



	// Here is where we copy the values from the actual ramp into the dummy/utility ramp
	MIntArray indices;
	MFloatArray positions;
	MFloatArray values;
	MIntArray interps;

	m_ramp.pack();
	m_ramp.getEntries(indices, positions, values, interps, &st); mser;
	MIntArray indicesToDelete;
	for (int i = 1; i < indices.length(); ++i)
	{
		indicesToDelete.append(indices[i]);
	}
	m_ramp.deleteEntries(indicesToDelete);

	unsigned nEl = rampPlug.numElements(&st); mser;

	positions.setLength(nEl);
	values.setLength(nEl);
	interps.setLength(nEl);



	for (int i = 0; i < nEl; ++i)
	{
		MPlug elPlug = rampPlug.elementByPhysicalIndex(i);

		MPlug posPlug = elPlug.child(1);
		MPlug valPlug = elPlug.child(0);
		MPlug interpPlug = elPlug.child(2);


		posPlug.getValue(positions[i]);
		valPlug.getValue(values[i]);
		interpPlug.getValue(interps[i]);

	}

	m_ramp.addEntries(positions , values , interps, &st); mser;

	indicesToDelete = MIntArray(1, 0);
	m_ramp.deleteEntries(indicesToDelete);



}



hexUtil::Rank actuator::rank() const {return m_rank;}
hexUtil::ActuatorAxis actuator::axis() const {return m_axis;}
hexUtil::ActuatorChannel actuator::channel() const {return m_channel;}

// MRampAttribute actuator::ramp {return m_ramp;}

bool actuator::mirror() const {return m_mirror;}

MVector actuator::lookup(float val, hexUtil::Side side)
{
	MStatus st;

	float inputVal, result;

	if (side == hexUtil::kRight && m_mirror)  {
		val = -val;
	}

	// cerr << "Here 1" << endl;
	if (val < m_inputMin)
	{
		inputVal = 0;
	}
	else if  (val > m_inputMax)
	{
		inputVal = 1;
	}
	else {
		inputVal = float((val - m_inputMin) / (m_inputMax - m_inputMin));
	}

	m_ramp.getValueAtPosition(  inputVal, result  , &st); mser;




	result =  float(m_outputMin +  ((m_outputMax - m_outputMin) * result));


	if (m_channel  == hexUtil::kTX ||  m_channel  == hexUtil::kRX) {
		return MVector(result, 0, 0);
	}
	if (m_channel  == hexUtil::kTY ||  m_channel  == hexUtil::kRY) {
		return MVector(0, result, 0);
	}
	// (m_channel  == hexUtil::kTZ ||  m_channel  == hexUtil::kRZ)
	return MVector(0, 0, result);
}



ostream &operator<<(ostream &os, const actuator &ac )
{
	os << "Rank:" << (short)ac.m_rank <<
	   " Axis:" <<  (short)ac.m_axis <<
	   " Channel:" << (short)ac.m_channel <<
	   " Mirror:" <<  ac.m_mirror <<
	   " InputMin: " <<  ac.m_inputMin <<
	   " InputMax: " <<  ac.m_inputMax <<
	   " OutputMin: " << ac.m_outputMin <<
	   " OutputMax: " << ac.m_outputMax;
	return os;
}

actuator::~actuator() {}