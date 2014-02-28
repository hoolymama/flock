#ifndef containmentPP_H
#define containmentPP_H

#include <maya/MPxNode.h>

class containmentPP: public MPxNode {
	public:
virtual	void		postConstructor();
		containmentPP (); virtual ~containmentPP ();
		virtual MStatus compute (const MPlug& plug, MDataBlock& data);
		static void *creator (); static MStatus initialize (); static MTypeId id;
	private:
		enum Side {kInside, kOutside};

		static MObject aPosition;
		static MObject aMesh;	
		static MObject aSide ;
		static MObject aOutput;	   


};
#endif