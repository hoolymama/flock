#include <maya/MPxNode.h>

class crossPP: public MPxNode {
	public:
virtual	void		postConstructor();
		crossPP (); virtual ~crossPP ();
		virtual MStatus compute (const MPlug& plug, MDataBlock& data);
		static void *creator (); static MStatus initialize (); static MTypeId id;
	private:
		static MObject aInput1;
		static MObject aInput2;	   
		static MObject aNormalizeInputs;
		static MObject aOutput;

};
