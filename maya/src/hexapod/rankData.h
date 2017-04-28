#ifndef rankData_H
#define rankData_H

#include <maya/MString.h>
#include <maya/MObject.h>

#include <maya/MRampAttribute.h>
#include <maya/MDataBlock.h>

class rankData {
public:

	rankData();
	rankData(const MObject &node, MString rank);
	
	~rankData();


	double	homeX;
	double	homeZ;

	double	radiusMin;
	double	radiusMax;

	MRampAttribute stepIncrementRamp;
	MRampAttribute slideProfileRamp;
	MRampAttribute liftProfileRamp;

} ;


#endif

