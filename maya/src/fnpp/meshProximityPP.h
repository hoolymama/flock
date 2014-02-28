#ifndef meshProximityPP_H
#define meshProximityPP_H

#include <maya/MPxNode.h>

class meshProximityPP: public MPxNode {
	public:
virtual	void		postConstructor();
		meshProximityPP (); virtual ~meshProximityPP ();
		virtual MStatus compute (const MPlug& plug, MDataBlock& data);
		static void *creator (); static MStatus initialize (); static MTypeId id;
	private:
		enum Side {kInside, kOutside, kBoth};

		static MObject aPosition;
		static MObject aMesh;	
		static MObject aFalloffDistance;
		static MObject aFalloffFunction;
		static MObject aMagnitude;
		static MObject aSide ;
		static MObject aOutput;	   


};
#endif