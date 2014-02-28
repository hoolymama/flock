#include <maya/MPxNode.h>

class binormalPP: public MPxNode {
	public:
virtual	void		postConstructor();
		binormalPP (); virtual ~binormalPP ();
		virtual MStatus compute (const MPlug& plug, MDataBlock& data);
		static void *creator (); static MStatus initialize (); static MTypeId id;
	private:
		static MObject aInput1;
		static MObject aInput2;	   
		static MObject aOutput;

};
