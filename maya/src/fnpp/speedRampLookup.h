#include <maya/MPxNode.h>

class speedRampLookupPP: public MPxNode {
	
public:
	
	virtual	void		postConstructor();
	speedRampLookupPP (); 
	virtual ~speedRampLookupPP ();
	
	virtual MStatus compute (const MPlug& plug, MDataBlock& data);
	static void *creator (); 
	static MStatus initialize (); 
	static MTypeId id;
	
	
	
private:
	
	static MObject aInputMin;
	static MObject aInputMax;
	static MObject aOutputMin;
	static MObject aOutputMax;

	
	
	static MObject aInput1;
	static MObject aInput2;
	static MObject aRamp; 
	static MObject aOutput;
	
};
