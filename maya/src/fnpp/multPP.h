#include <maya/MPxNode.h>

class multPP: public MPxNode {
	public:
virtual	void		postConstructor();
		multPP (); virtual ~multPP ();
		virtual MStatus compute (const MPlug& plug, MDataBlock& data);
		static void *creator (); static MStatus initialize (); static MTypeId id;
	private:
		static MObject aInput;
		static MObject aDefault;
		static MObject aOutputD;
		static MObject aOutputV;
		static MObject aCompute;
};
