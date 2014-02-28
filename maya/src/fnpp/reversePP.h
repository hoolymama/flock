#include <maya/MPxNode.h>

class reversePP: public MPxNode {
	public:
virtual	void		postConstructor();
		reversePP (); virtual ~reversePP ();
		virtual MStatus compute (const MPlug& plug, MDataBlock& data);
		static void *creator (); static MStatus initialize (); static MTypeId id;
	private:
		static MObject aInput;
		static MObject aOutput;

};
