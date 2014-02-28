#include <maya/MPxNode.h>

class signPP: public MPxNode {
	public:
virtual	void		postConstructor();
		signPP (); virtual ~signPP ();
		virtual MStatus compute (const MPlug& plug, MDataBlock& data);
		static void *creator (); static MStatus initialize (); static MTypeId id;
		
	private:
		static MObject aInput;
		static MObject aOutputIfPositive;
		static MObject aOutputIfNegative;
		static MObject aOutput;

};
