
#ifndef addVectorPP_H
#define addVectorPP_H



#include <maya/MPxNode.h>

class addVectorPP: public MPxNode {
	public:
virtual	void		postConstructor();
		addVectorPP (); virtual ~addVectorPP ();
		virtual MStatus compute (const MPlug& plug, MDataBlock& data);
		static void *creator (); static MStatus initialize (); static MTypeId id;
	private:
		static MObject aInput;
    static MObject aScalarInput;
		static MObject aOutput;
};

 
#endif