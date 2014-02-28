#include <maya/MPxNode.h>

class coordSysComponentPP: public MPxNode {
		public:
virtual	void		postConstructor();
		coordSysComponentPP (); virtual ~coordSysComponentPP ();
		virtual MStatus compute (const MPlug& plug, MDataBlock& data);
		static void *creator (); static MStatus initialize (); static MTypeId id;
	private:
		static MObject aFront;
		static MObject aUp;	   
		static MObject aInput;	   
		static MObject aFrontComponent;	   
		static MObject aUpComponent;	   
		static MObject aSideComponent;	   
		static MObject aOutput;

};
