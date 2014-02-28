#include <maya/MPxNode.h>

class blendDoublePP: public MPxNode {
		public:
virtual	void		postConstructor();
		blendDoublePP (); virtual ~blendDoublePP ();
		virtual MStatus compute (const MPlug& plug, MDataBlock& data);
		static void *creator (); static MStatus initialize (); static MTypeId id;
	private:
		static MObject aInput1;
		static MObject aInput2;	   
		static MObject aWeight;	   
		static MObject aOutput;

};
