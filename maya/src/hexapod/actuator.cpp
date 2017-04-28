



#include <maya/MDataBlock.h>

#include <maya/MDataHandle.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFloatArray.h>


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


const double epsilon = 0.0000001;
actuator::actuator() {}




actuator::actuator(	MRampAttribute &rampAtt)
:m_ramp(rampAtt)
{}

void actuator::set(	MDataHandle &handle, const MObject &node, const MPlug &rampPlug)
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
	
	if (m_inputMin > m_inputMax) {
		double tmp = m_inputMin;
		m_inputMin = m_inputMax;
		m_inputMax = tmp;
	}
	if (m_inputMin >= m_inputMax-epsilon) {
		m_inputMax += epsilon;
	}


	patt = dn.attribute("actuatorOutputRange", &st); er;

	att = dn.attribute("actuatorOutputMin", &st); er; 
	m_outputMin = handle.child(patt).child(att).asDouble();

	att = dn.attribute("actuatorOutputMax", &st); er; 
	m_outputMax = handle.child(patt).child(att).asDouble();
	


	// Here is where we copy the values from the actual ramp into the dummy/utility ramp
	MIntArray indices;
	MFloatArray positions;
	MFloatArray values;
	MIntArray interps;

	m_ramp.pack();
	m_ramp.getEntries(indices, positions, values,interps, &st); er;
	MIntArray indicesToDelete;
	for (int i = 1; i < indices.length(); ++i)
	{
 		indicesToDelete.append(indices[i]);
	}
	m_ramp.deleteEntries(indicesToDelete);

	unsigned nEl = rampPlug.numElements(&st); er;
 
	positions.setLength(nEl);
	values.setLength(nEl);
	interps.setLength(nEl);

	// MObject posatt = dn.attribute("actuatorRamp_Position", &st); er; 
	// MObject valatt = dn.attribute("actuatorRamp_FloatValue", &st); er; 
	// MObject intatt = dn.attribute("actuatorRamp_Interp", &st); er; 
 


	for (int i = 0; i < nEl; ++i)
	{
		MPlug elPlug = rampPlug.elementByPhysicalIndex(i);
	 
		MPlug posPlug = elPlug.child(1);
		MPlug valPlug = elPlug.child(0);
		MPlug interpPlug = elPlug.child(2);


		// cerr << "posPlug.name()" << posPlug.name() << endl;
		// cerr << "valPlug.name()" << valPlug.name() << endl;
		// cerr << "interpPlug.name()" << interpPlug.name() << endl;
			

		posPlug.getValue(positions[i]);
		valPlug.getValue(values[i]);
		interpPlug.getValue(interps[i]);

	}
 // 		cerr << "----------: " << endl;

	// cerr << "positions: "<< positions << endl;
	// cerr << "values: "<< values << endl;
	// cerr << "interps: "<< interps << endl;
 // 		cerr << "~~~~~~~~~~~~: " << endl;

  m_ramp.addEntries(positions ,values ,interps, &st); er;

  indicesToDelete = MIntArray(1,0);
	m_ramp.deleteEntries(indicesToDelete);


	// m_ramp.getEntries(indices, positions, values,interps, &st); er;
	// cerr << "indices: "<< indices << endl;
	// cerr << "positions: "<< positions << endl;
	// cerr << "values: "<< values << endl;
	// cerr << "interps: "<< interps << endl;
 // 		cerr << "----------: " << endl;


 // 		float result;
 // 		m_ramp.getValueAtPosition(0.56f, result);

	// cerr << "result: " << result << endl;


}


// actuator::actuator(	MDataHandle &handle, const MObject &node, const MPlug &rampPlug)
// {
// 	MObject att, patt;
// 	MStatus st;
// 	MFnDependencyNode dn(node, &st);er;

// 	att = dn.attribute( "actuatorRank", &st);er;
// 	m_rank = (hexUtil::Rank)handle.child(att).asShort();

// 	att = dn.attribute( "actuatorInputAxis", &st);er;
// 	m_axis = (hexUtil::ActuatorAxis)handle.child(att).asShort();

// 	att = dn.attribute( "actuatorOutputChannel", &st);er;
// 	m_channel = (hexUtil::ActuatorChannel)handle.child(att).asShort();
	
// 	att = dn.attribute( "actuatorInputMirror", &st);er;
// 	m_mirror = handle.child(att).asBool();

// 	patt = dn.attribute("actuatorInputRange", &st); er;

// 	att = dn.attribute("actuatorInputMin", &st); er;
// 	m_inputMin = handle.child(patt).child(att).asDouble();

// 	att = dn.attribute("actuatorInputMax", &st); er; 
// 	m_inputMax = handle.child(patt).child(att).asDouble();
	
// 	if (m_inputMin > m_inputMax) {
// 		double tmp = m_inputMin;
// 		m_inputMin = m_inputMax;
// 		m_inputMax = tmp;
// 	}
// 	if (m_inputMin >= m_inputMax-epsilon) {
// 		m_inputMax += epsilon;
// 	}


// 	patt = dn.attribute("actuatorOutputRange", &st); er;

// 	att = dn.attribute("actuatorOutputMin", &st); er; 
// 	m_outputMin = handle.child(patt).child(att).asDouble();

// 	att = dn.attribute("actuatorOutputMax", &st); er; 
// 	m_outputMax = handle.child(patt).child(att).asDouble();
	


// 	unsigned nEl = rampPlug.numElements(&st); er;
// 	m_positions.setLength(nEl);
// 	m_values.setLength(nEl);
// 	m_interps.setLength(nEl);


// 	for (int i = 0; i < nEl; ++i)
// 	{
// 		MPlug elPlug = rampPlug.elementByPhysicalIndex(i);
// 		unsigned numChildren = elPlug.numChildren(&st); er;

// 		MPlug posPlug = elPlug.child(0);
// 		MPlug valPlug = elPlug.child(1);
// 		MPlug interpPlug = elPlug.child(2);

// 		posPlug.getValue(positions[i]);
// 		valPlug.getValue(values[i]);
// 		interpPlug.getValue(interps[i]);
		
// 	}


// 	// cerr << "positions: " << positions << endl;
// 	// cerr << "values: " << values << endl;
// 	// cerr << "interps: " << interps << endl;
	
// 	// m_ramp = MRampAttribute();

// 	// m_ramp.addEntries(positions ,values ,interps, &st); er;

// 	// MObject plugAtt = plug.attribute(&st); er;
//  // 	MObject plugNode = plug.node(&st); er;
// 	// MString name = plug.name(&st);er;
// 	// cerr << "plug name: " << name << endl;

// 	// att = dn.attribute("actuatorRamp", &st);er;
// 	// MPlug rampPlug = plug.child(att);
// 	// MString rname = rampPlug.name(&st);er;
// 	// cerr << "plug rname: " << rname << endl;



// 	// m_ramp = MRampAttribute( rampPlug , &st);er; 

// }

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
		val=-val;
	}

	// cerr << "Here 1" << endl;
	if (val < m_inputMin) 
	{
		inputVal = 0;
	} else if  (val > m_inputMax)
	{
		inputVal = 1;
	} else {
		inputVal = float((val - m_inputMin) / (m_inputMax - m_inputMin));
	}

	m_ramp.getValueAtPosition(  inputVal, result  , &st);er; 
 



	result =  float(m_outputMin +  ((m_outputMax - m_outputMin) * result));
 
 
	if (m_channel  == hexUtil::kTX ||  m_channel  == hexUtil::kRX) {
		return MVector(result,0,0);
	}
	if (m_channel  == hexUtil::kTY ||  m_channel  == hexUtil::kRY) {
		return MVector(0,result,0);
	} 
	// (m_channel  == hexUtil::kTZ ||  m_channel  == hexUtil::kRZ) 
	return MVector(0,0,result); 
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