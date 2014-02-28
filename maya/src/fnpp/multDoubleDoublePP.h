#include <maya/MPxNode.h>

class multDoubleDoublePP: public MPxNode {
	public:
virtual	void		postConstructor();
		multDoubleDoublePP (); virtual ~multDoubleDoublePP ();
		virtual MStatus compute (const MPlug& plug, MDataBlock& data);
		static void *creator (); static MStatus initialize (); static MTypeId id;
	private:
		static MObject aInput1;
		static MObject aInput2;	   
		static MObject aDefault;	   
		
		static MObject aOutput;

};
