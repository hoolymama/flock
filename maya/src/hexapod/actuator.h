#ifndef actuator_H
#define actuator_H

#include <maya/MString.h>
#include <maya/MObject.h>

#include <maya/MRampAttribute.h>
#include <maya/MDataBlock.h>

class actuator {
public:

enum Rank { kAnterior, kMedial, kPosterior };
enum ActuatorAxis { kStepParam, kX, kY, kZ};
enum ActuatorChannel { kTX, kTY, kTZ, kRX, kRY, kRZ };



	actuator();
	actuator(MDataBlock &data, 	const MObject &node, MString rank);
	
	~actuator();

	Rank rank;
	ActuatorAxis axis;
	ActuatorChannel channel;
	
	double	homeX;
	double	homeZ;

	double	radiusMin;
	double	radiusMax;

	MRampAttribute stepIncrementRamp;
	MRampAttribute slideProfileRamp;
	MRampAttribute liftProfileRamp;

} ;


#endif

