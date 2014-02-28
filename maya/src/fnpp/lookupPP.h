#include <maya/MPxNode.h>

class lookupPP: public MPxNode {
	
	public:
	
		virtual	void		postConstructor();
		lookupPP (); virtual ~lookupPP ();
		virtual MStatus compute (const MPlug& plug, MDataBlock& data);
		static void *creator (); static MStatus initialize (); static MTypeId id;
	


	private:
			

		static MObject aCurveResolution;	
		static MObject aInput;
		static MObject aFnCurve; 
		static MObject aOutputD;
		static MObject aOutputV;

};
