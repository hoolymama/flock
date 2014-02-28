#include <maya/MPxNode.h>

class accumulatorPP: public MPxNode {
	public:
virtual	void		postConstructor();
		accumulatorPP (); virtual ~accumulatorPP ();
		virtual MStatus compute (const MPlug& plug, MDataBlock& data);
		static void *creator (); static MStatus initialize (); static MTypeId id;
	private:
		static MObject aInput;
		static MObject aOutput;
};
