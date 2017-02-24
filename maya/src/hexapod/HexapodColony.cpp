

#include <string.h>

#include <sstream>

#include <list>
#include <vector>

#include <maya/MObject.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnIntArrayData.h>
#include <maya/MGlobal.h>


#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>

#include <maya/MDoubleArray.h>
#include <maya/MIntArray.h>
#include <maya/MVectorArray.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MDataHandle.h>
#include <maya/MRampAttribute.h>
#include "mayaMath.h"


#include "rankData.h"
#include "HexapodColony.h"
#include  "actuator.h"

#if defined(OSMac_MachO_)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif


/*
HexapodColony is concerned with maintaining the correct list of agents
based on input arrays of dta from particles.

It uses a kind of zipping up algorithm to add new agents, delete dead 
agents, and update existing agents
*/
HexapodColony::HexapodColony( ) 
:m_agents()
{}


void HexapodColony::clear(){
	std::list<HexapodAgent*>::iterator iter = m_agents.begin();
	while (iter != m_agents.end()) {
		delete *iter;
		*iter =0;
		iter++;
	}
	m_agents.clear();
}

void HexapodColony::getOutputData(
	const MIntArray & idIndex,
	MVectorArray & outLA,
	MVectorArray & outLB,
	MVectorArray & outLC,
	MVectorArray & outRA,
	MVectorArray & outRB,
	MVectorArray & outRC,
	MVectorArray & outPosition,
	MVectorArray & outPhi,
	MDoubleArray & outScale
	) const
{

	unsigned len = idIndex.length();
	outLA.setLength(len);
	outLB.setLength(len);
	outLC.setLength(len);
	outRA.setLength(len);
	outRB.setLength(len);
	outRC.setLength(len);
	outPosition.setLength(len);
	outPhi.setLength(len);
	outScale.setLength(len);
	
	std::list<HexapodAgent*>::const_iterator agent = m_agents.begin();
	unsigned i = 0;
	unsigned index;

	// cerr << "idIndex.length()" << idIndex.length() << endl;
	// cerr << "m_agents.size()" << m_agents.size() << endl;


	while (agent != m_agents.end()) 
	{
		index = idIndex[i];
		MVector & la = outLA[index];
		MVector & lb = outLB[index];
		MVector & lc = outLC[index];
		MVector & ra = outRA[index];
		MVector & rb = outRB[index];
		MVector & rc = outRC[index];
		MVector & position = outPosition[index];
		MVector & phi = outPhi[index];
		double & scale = outScale[index];
		(*agent)->getOutputData(la,lb,lc,ra,rb,rc,position,phi, scale);

		// cerr << "agent: "<< (*agent)->id() << " - la: " << la << endl;
		agent++; i++;
	}
}



void HexapodColony::getDefaultOutputData(
	const  MObject& node, 
	MDataBlock& data,
	MVectorArray & outLA,
	MVectorArray & outLB,
	MVectorArray & outLC,
	MVectorArray & outRA,
	MVectorArray & outRB,
	MVectorArray & outRC,
	MVectorArray & outPosition,
	MVectorArray & outPhi,
	MDoubleArray & outScale
	) const
{

	MStatus st;
	MFnDependencyNode dn(node, &st);er;

	rankData rankA(data,  node, "A");
	rankData rankB(data,  node, "B");
	rankData rankC(data,  node, "C");

	MObject att = dn.attribute	( "bodyOffset", &st);er;
	const MVector bodyOffset = MVector(data.inputValue( att ).asFloatVector());

	outLA.append(MVector(rankA.homeX, 0, rankA.homeZ));
	outLB.append(MVector(rankB.homeX, 0, rankB.homeZ));
	outLC.append(MVector(rankC.homeX, 0, rankC.homeZ));

	outRA.append(MVector(rankA.homeX, 0, -rankA.homeZ));
	outRB.append(MVector(rankB.homeX, 0, -rankB.homeZ));
	outRC.append(MVector(rankC.homeX, 0, -rankC.homeZ));

	outPosition.append(bodyOffset);
	outPhi.append(MVector::zero);
	outScale.append(1.0);

}

 

MStatus HexapodColony::update(
	double dt ,
	const  MObject& node, 
	MDataBlock& data 
	){

	MStatus st;
	MFnDependencyNode dn(node, &st);er;

	MObject att = dn.attribute	( "particleId", &st);er;
	MDoubleArray tmpParticleId = MFnDoubleArrayData(data.inputValue(att).data()).array();
	unsigned len = tmpParticleId.length(); 
	MIntArray particleId(len);
	for (int i = 0; i < len; ++i) {  
		particleId[i] = int(tmpParticleId[i]+0.1);  
	}

	MIntArray sortedId =  getIntArray("sortedId",node,data, len, &st);er;
	MIntArray idIndex =		getIntArray("idIndex",node,data, len, &st);er;
	MVectorArray pos = 		getVectorArray("position",node,data, len, &st);er;
	MVectorArray phi = 		getVectorArray("phi",node,data, len, &st);er;
	MVectorArray vel = 		getVectorArray("velocity",node,data, len, &st);er;
	MVectorArray omega =  getVectorArray("omega",node,data, len, &st);er;
	MDoubleArray scale =  getDoubleArray("scale",node,data, len, &st);er;

	rankData rankA(data,  node, "A");
	rankData rankB(data,  node, "B");
	rankData rankC(data,  node, "C");

	att = dn.attribute	( "maxSpeed", &st);er;
	double	maxSpeed	= data.inputValue(att).asDouble();
	if (maxSpeed < 0.00001) {
		MString msg("Max speed too small: ");
		msg += maxSpeed;
		msg += " Setting to default value 1.0";
		MGlobal::displayWarning(msg);
		maxSpeed = 1.0;
	}

	att = dn.attribute	( "bodyOffset", &st);er;

	const MVector bodyOffset = MVector(data.inputValue( att ).asFloatVector());

	MRampAttribute plantSpeedBiasRamp( node , dn.attribute( "plantSpeedBiasRamp" ), &st ); er; 
	MRampAttribute anteriorRadiusRamp( node,  dn.attribute( "anteriorRadiusRamp" ), &st);er;
	MRampAttribute lateralRadiusRamp(  node,  dn.attribute( "lateralRadiusRamp" ), &st);er;
	MRampAttribute posteriorRadiusRamp(node,  dn.attribute( "posteriorRadiusRamp" ), &st);er;

 	/////////////////////////////////////
	std::vector<actuator*> actuatorStack;
	MObject actuatorAtt =  dn.attribute( "bodyActuator" );
	MArrayDataHandle  hActuator = data.inputArrayValue(actuatorAtt);
	unsigned nf = hActuator.elementCount();
	for(unsigned i = 0;i < nf; i++, hActuator.next()) {
		MDataHandle hData = hActuator.inputValue();
		MObject activeAtt =  dn.attribute	( "actuatorActive", &st);er;
		bool active = hData.child(activeAtt).asBool();
		if (active) {
			actuatorStack.push_back(new actuator(hData, node));
		}
	}
  // for (int i = 0; i < actuatorStack.size(); i++){ cerr << (*actuatorStack[i]) << endl ; }
	/////////////////////////////////////


	int rPeg = 0;

	std::list<HexapodAgent*>::iterator shadow;
	std::list<HexapodAgent*>::iterator agent = m_agents.begin();

	while (! (agent == m_agents.end() && rPeg == sortedId.length())) 
	{

		if (rPeg == sortedId.length()) { 
			// finished the list of new agents. 
			// Delete the rest of existing list.

			shadow = agent; 
			agent++;
			delete *shadow;
			*shadow = 0;
			m_agents.erase(shadow);

		} else if (agent == m_agents.end() ) {
			unsigned index = idIndex[rPeg];
			// insert new agent with id idIndex[rPeg] before end
			m_agents.push_back(new HexapodAgent( 
				dt, particleId[index], pos[index], phi[index],  vel[index], omega[index],
				scale[index], rankA, rankB, rankC,
				bodyOffset
				));
			rPeg++;
		} else if (sortedId[rPeg] < (*agent)->id()) {
			unsigned index = idIndex[rPeg];
			// sortedId position less than current agent id 
			// so insert new sortedId idIndex[rPeg] 

			m_agents.insert(agent, new HexapodAgent(   			
				dt,  particleId[index], pos[index], phi[index],  vel[index], omega[index],
				scale[index], rankA, rankB, rankC, bodyOffset
				));

			rPeg++;
		} else if (sortedId[rPeg]  == (*agent)->id()) {
			unsigned index = idIndex[rPeg];
			// sortedId position equal to current agent 
			// so update with sortedId:  idIndex[rPeg] 

			(*agent)->update( 
				dt, maxSpeed, pos[index], phi[index],  vel[index], omega[index],
				scale[index], rankA, rankB, rankC,bodyOffset,
				plantSpeedBiasRamp, anteriorRadiusRamp, lateralRadiusRamp,  posteriorRadiusRamp
				);
			
				

			rPeg++;
			agent++;
		} else { 

			// sortedId position greater than current agent so removecurrent agent
			shadow = agent; 
			agent++;
			delete *shadow;
			*shadow = 0;
			m_agents.erase(shadow);
		}
	}

	for (int i = 0; i < actuatorStack.size(); i++){
		delete actuatorStack[i];
	}

	return MS::kSuccess;
}


HexapodColony::~HexapodColony(){
	clear();
}


void HexapodColony::draw(
	M3dView & view, const DisplayMask & mask	) const {

	view.beginGL(); 
	glPushAttrib(GL_CURRENT_BIT);

	std::list<HexapodAgent*>::const_iterator agent = m_agents.begin();
	double d44[4][4];
	while (agent != m_agents.end()) {
		(*agent)->draw(view,mask);
		agent++;
	}

	glPopAttrib();
	view.endGL();	
}



// id mapping - 
//////////////////////// IDS ////////////////////////
// i         	:  0   1   2   3   4   5   6   7   8 //
// particleId	: 14   6   2  12  10   5  13   8  11 // 

// idIndex tells us the index in the id array 
// sortedId  	:  2   5   6   8  10  11  12  13  14 // 
// idIndex   	:  2   5   1   7   4   8   3   6   0 //




