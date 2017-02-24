#ifndef actuator_H
#define actuator_H

#include <maya/MString.h>
#include <maya/MObject.h>

#include <maya/MRampAttribute.h>
#include <maya/MDataBlock.h>
#include "hexUtil.h"
#include <iostream>  
class actuator {
public:

	actuator();
 	actuator(MDataHandle &handle, const MObject &node);
 
	~actuator();
	
	friend ostream& operator<<(ostream& os, const actuator& actuator);  

private:

	hexUtil::Rank m_rank;
	hexUtil::ActuatorAxis m_axis;
	hexUtil::ActuatorChannel m_channel;

	bool m_mirror;
 
	MRampAttribute m_ramp;
	double	m_inputMin;
	double	m_inputMax;
	double	m_outputMin;
	double	m_outputMax;

} ;


#endif

