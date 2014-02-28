#include <maya/MPxNode.h>

class vectorComponentPP: public MPxNode {
	public:
virtual	void		postConstructor();
		vectorComponentPP (); virtual ~vectorComponentPP ();
		virtual MStatus compute (const MPlug& plug, MDataBlock& data);
		static void *creator (); static MStatus initialize (); static MTypeId id;
		
		
		enum ComponentType {kXComp, kYComp, kZComp, kMag,kSquaredMag};
	private:
		static MObject aInput;
		static MObject aComponent;
		static MObject aOutput;

};
