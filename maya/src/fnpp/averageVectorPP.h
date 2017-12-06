#include <list>
#include <maya/MPxNode.h>
#include <maya/MTime.h>
#include <maya/MVectorArray.h> 

class averageVectorPP: public MPxNode {
public:
	virtual	void		postConstructor();
	averageVectorPP (); virtual ~averageVectorPP ();
	virtual MStatus compute (const MPlug& plug, MDataBlock& data);
	static void *creator (); static MStatus initialize (); static MTypeId id;
private:
	static MObject aInput;	  
	static MObject aFrames ;   
	static MObject aOutput;
	static MObject aCurrentTime;
	static MObject aStartTime;


	MTime m_lastTimeIEvaluated;

	std::list<MVectorArray> m_q;

};
