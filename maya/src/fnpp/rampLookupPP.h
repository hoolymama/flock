#include <maya/MPxNode.h>

class rampLookupPP: public MPxNode {
	
public:
	
	virtual	void		postConstructor();
	rampLookupPP (); 
	virtual ~rampLookupPP ();
	
	virtual MStatus compute (const MPlug& plug, MDataBlock& data);
	static void *creator (); 
	static MStatus initialize (); 
	static MTypeId id;
	
	
	
private:
	
	static MObject aInputMin;
	static MObject aInputMax;
	static MObject aOutputMin;
	static MObject aOutputMax;

	
	
	static MObject aInput;
	static MObject aRamp; 
	static MObject aOutput;
	
};
