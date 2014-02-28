#include <maya/MPxNode.h>

class sinePP: public MPxNode {
	public:
virtual	void		postConstructor();
		sinePP (); virtual ~sinePP ();
		virtual MStatus compute (const MPlug& plug, MDataBlock& data);
		static void *creator (); static MStatus initialize (); static MTypeId id;
	private:
		static MObject aInput;
		static MObject aAmplitude;	     
		static MObject aOffsetY;	     
		static MObject aOutput;

};
