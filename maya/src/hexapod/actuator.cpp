



#include <maya/MDataBlock.h>

#include <maya/MDataHandle.h>
#include <maya/MFnDependencyNode.h>

#include "actuator.h"
#include "errorMacros.h"
#include "hexUtil.h"
/*

	hexUtil::Rank rank,
	hexUtil::ActuatorAxis axis,
	hexUtil::ActuatorChannel channel,
	bool mirror,
	bool active,
	MRampAttribute & ramp,
	double	inputMin,
	double	inputMax,
	double	outputMin,
	double	outputMax

 
*/

actuator::actuator() {}
actuator::actuator(	MDataHandle &handle, const MObject &node)
{
  MObject att, patt;
	MStatus st;
	MFnDependencyNode dn(node, &st);er;

	att = dn.attribute( "actuatorRank", &st);er;
	m_rank = (hexUtil::Rank)handle.child(att).asShort();

	att = dn.attribute( "actuatorInputAxis", &st);er;
	m_axis = (hexUtil::ActuatorAxis)handle.child(att).asShort();

	att = dn.attribute( "actuatorOutputChannel", &st);er;
	m_channel = (hexUtil::ActuatorChannel)handle.child(att).asShort();
	
	att = dn.attribute( "actuatorInputMirror", &st);er;
	m_mirror = handle.child(att).asBool();

	patt = dn.attribute("actuatorInputRange", &st); er;

	att = dn.attribute("actuatorInputMin", &st); er;
	m_inputMin = handle.child(patt).child(att).asDouble();

	att = dn.attribute("actuatorInputMax", &st); er; 
	m_inputMax = handle.child(patt).child(att).asDouble();
	
	patt = dn.attribute("actuatorOutputRange", &st); er;

	att = dn.attribute("actuatorOutputMin", &st); er; 
	m_outputMin = handle.child(patt).child(att).asDouble();

	att = dn.attribute("actuatorOutputMax", &st); er; 
	m_outputMax = handle.child(patt).child(att).asDouble();
	
	att = dn.attribute("actuatorRamp", &st);er;
	m_ramp = MRampAttribute( node , att  ); 
}


ostream& operator<<(ostream& os, const actuator& ac )  
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

actuator::~actuator(){}