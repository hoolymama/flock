



#include <maya/MDataBlock.h>

#include <maya/MDataHandle.h>
#include <maya/MFnDependencyNode.h>

#include "actuator.h"
#include "errorMacros.h"

actuator::actuator() {}

actuator::actuator(MDataBlock &data, 	const MObject &node, MString rank)
{
	MStatus st;
	MFnDependencyNode dn(node, &st);er;

	MString rankN("rank");
	MString homeN("home");
	MString radiusN("radius");
 
	rankN+=rank;
	homeN+=rank;
	radiusN+=rank;
 
	MObject att = dn.attribute(rankN); 
	MDataHandle hRank = data.inputValue(att);
 
	att = dn.attribute(homeN, &st);er;
 
	MString childstr = (homeN+"X");
	MObject childatt = dn.attribute(childstr);
	homeX	= hRank.child(att).child(childatt).asDouble();

	childstr = (homeN+"Z");
	childatt = dn.attribute(childstr);
	homeZ	= hRank.child(att).child(childatt).asDouble();

	att = dn.attribute(radiusN, &st);er;
 
	childstr = ("radiusMin"+rank);
	childatt = dn.attribute(childstr);
	radiusMin	= hRank.child(att).child(childatt).asDouble();
 
	childstr = ("radiusMax"+rank);
	childatt = dn.attribute(childstr);

	radiusMax	= hRank.child(att).child(childatt).asDouble();
 
	childstr = ("stepIncrementRamp"+rank);
	att = dn.attribute(childstr);
	stepIncrementRamp = MRampAttribute( node , att  ); 

	childstr = ("slideProfileRamp"+rank);
	att = dn.attribute(childstr);
	slideProfileRamp = MRampAttribute( node , att  ); 

	childstr = ("liftProfileRamp"+rank);
	att = dn.attribute(childstr);
	liftProfileRamp = MRampAttribute( node , att  ); 
 
}

actuator::~actuator(){}