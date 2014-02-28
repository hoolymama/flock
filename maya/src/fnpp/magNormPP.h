#include <maya/MPxNode.h>

class magNormPP: public MPxNode {
	public:
virtual	void		postConstructor();
		magNormPP (); virtual ~magNormPP ();
		virtual MStatus compute (const MPlug& plug, MDataBlock& data);
		static void *creator (); static MStatus initialize (); static MTypeId id;
	private:
		static MObject aInput;
		static MObject aOutputMag;
		static MObject aOutputNormal;

};
