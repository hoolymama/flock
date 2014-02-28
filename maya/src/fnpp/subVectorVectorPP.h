#include <maya/MPxNode.h>

class subVectorVectorPP: public MPxNode {
	public:
virtual	void		postConstructor();
		subVectorVectorPP (); virtual ~subVectorVectorPP ();
		virtual MStatus compute (const MPlug& plug, MDataBlock& data);
		static void *creator (); static MStatus initialize (); static MTypeId id;
	private:
		static MObject aInput1;
		static MObject aInput2;	   
		static MObject aMultiplier;	   
		// static MObject aDefault;	   
		
		static MObject aOutput;

};
