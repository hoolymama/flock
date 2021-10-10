#ifndef actuator_H
#define actuator_H

#include <maya/MString.h>
#include <maya/MObject.h>

#include <maya/MRampAttribute.h>
#include <maya/MDataBlock.h>

#include <maya/MPlug.h>
#include <maya/MVector.h>

#include "hexUtil.h"
#include <iostream>  

class actuator {
public:

	actuator();
	actuator(	MRampAttribute &rampAtt);
	void set(	MDataHandle &handle, const MObject &node, const MPlug &rampPlug);
	~actuator();
	
	friend ostream& operator<<(ostream& os, const actuator& actuator);  

	hexUtil::Rank rank() const;
	hexUtil::ActuatorAxis axis() const;
	hexUtil::ActuatorChannel channel() const;
	
	bool mirror() const ;

	MVector lookup(float val, hexUtil::Side side);

private:

	hexUtil::Rank m_rank;
	hexUtil::ActuatorAxis m_axis;

	bool m_mirror;
	MRampAttribute m_ramp;
	double	m_inputMin;
	double	m_inputMax;


	hexUtil::ActuatorChannel m_channel;
	double	m_outputMin;
	double	m_outputMax;



} ;


#endif

