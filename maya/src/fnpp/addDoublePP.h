
#ifndef addDoublePP_H
#define addDoublePP_H



#include <maya/MPxNode.h>

class addDoublePP: public MPxNode {
	public:
virtual	void		postConstructor();
		addDoublePP (); virtual ~addDoublePP ();
		virtual MStatus compute (const MPlug& plug, MDataBlock& data);
		static void *creator (); static MStatus initialize (); static MTypeId id;
	private:
		static MObject aInput;
    static MObject aScalarInput;
		static MObject aOutput;
};

 
#endif