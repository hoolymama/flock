



#include <maya/MDataBlock.h>
#include <maya/MPlug.h>
#include <maya/MDataHandle.h>
#include <maya/MFnDependencyNode.h>

#include "rankData.h"
#include "errorMacros.h"

rankData::rankData() {}

rankData::rankData(const MObject &node, MString rank)
{
	MStatus st;
	MFnDependencyNode dn(node, &st); mser;

	MObject att;

	MString rankN("rank");
	MString homeN("home");
	MString radiusN("radius");

	rankN += rank;
	homeN += rank;
	radiusN += rank;

	MString homeNX(homeN + "X");
	MString homeNZ(homeN + "Z");
	MString radiusNMin("radiusMin" + rank);
	MString radiusNMax("radiusMax" + rank);


	MObject aRank = dn.attribute(rankN);
	MObject aHome = dn.attribute(homeN);
	MObject aHomeX = dn.attribute(homeNX);
	MObject aHomeZ = dn.attribute(homeNZ);
	MObject aRadius  = dn.attribute(radiusN);
	MObject aRadiusMin = dn.attribute(radiusNMin);
	MObject aRadiusMax = dn.attribute(radiusNMax);



	MPlug rankPlug = dn.findPlug(aRank);
	MPlug homeXPlug = rankPlug.child(aHome).child(aHomeX);
	MPlug homeZPlug = rankPlug.child(aHome).child(aHomeZ);
	MPlug radiusMinPlug = rankPlug.child(aRadius).child(aRadiusMin);
	MPlug radiusMaxPlug = rankPlug.child(aRadius).child(aRadiusMax);


	MString childstr = MString(homeN + "X");
	MObject childatt = dn.attribute(childstr);
	st	= homeXPlug.getValue(homeX); mser;
	st	= homeZPlug.getValue(homeZ); mser;
	st	= radiusMinPlug.getValue(radiusMin); mser;
	st	= radiusMaxPlug.getValue(radiusMax); mser;


	childstr = ("stepIncrementRamp" + rank);
	att = dn.attribute(childstr);
	stepIncrementRamp = MRampAttribute( node , att  );

	childstr = ("slideProfileRamp" + rank);
	att = dn.attribute(childstr);
	slideProfileRamp = MRampAttribute( node , att  );

	childstr = ("liftProfileRamp" + rank);
	att = dn.attribute(childstr);
	liftProfileRamp = MRampAttribute( node , att  );

}

rankData::~rankData() {}