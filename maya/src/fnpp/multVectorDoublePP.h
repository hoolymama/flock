#include <maya/MPxNode.h>

class multVectorDoublePP: public MPxNode {
public:
	virtual	void		postConstructor();
	multVectorDoublePP (); virtual ~multVectorDoublePP ();
	virtual MStatus compute (const MPlug& plug, MDataBlock& data);
	static void *creator (); static MStatus initialize (); static MTypeId id;
private:
	static MObject aInput1;
	static MObject aInput2;	  
	static MObject aScalarInput ;
	static MObject aNormalizeVector;	   
	static MObject aOutput;

};
