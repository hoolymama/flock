#include <maya/MPxNode.h>

class conditionVPP: public MPxNode {
public:
	virtual	void		postConstructor();
	conditionVPP (); virtual ~conditionVPP ();
	virtual MStatus compute (const MPlug& plug, MDataBlock& data);
	static void *creator (); static MStatus initialize (); static MTypeId id;
private:

	static MObject aFirstTerm;
	static MObject aSecondTerm;	
	static MObject aSecondTermDefault;	

	static MObject aOperation;

	static MObject aVectorIfTrue ;
	static MObject aVectorIfFalse;	   
	static MObject aOutput;

};
