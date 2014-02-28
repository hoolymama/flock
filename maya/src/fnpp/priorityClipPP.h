#include <maya/MPxNode.h>

class priorityClipPP: public MPxNode {
	public:
virtual	void		postConstructor();
		priorityClipPP (); virtual ~priorityClipPP ();
		virtual MStatus compute (const MPlug& plug, MDataBlock& data);
		static void *creator (); static MStatus initialize (); static MTypeId id;
	private:
		static MObject aInput1;
		static MObject aInput2;	   
		static MObject aMax;	   
		
		static MObject aOutput1;
		static MObject aOutput2;
		static MObject aOutputSum;
};
