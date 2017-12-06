#include <list>
#include <maya/MPxNode.h>
#include <maya/MTime.h>
#include <maya/MDoubleArray.h>

class averageDoublePP: public MPxNode {
public:
	virtual	void		postConstructor();
	averageDoublePP (); virtual ~averageDoublePP ();
	virtual MStatus compute (const MPlug& plug, MDataBlock& data);
	static void *creator (); static MStatus initialize (); static MTypeId id;
private:
	static MObject aInput;	  
	static MObject aFrames ;   
	static MObject aOutput;
	static MObject aCurrentTime;
	static MObject aStartTime;


	MTime m_lastTimeIEvaluated;

	std::list<MDoubleArray> m_q;

};
