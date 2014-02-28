#include <maya/MPxNode.h>

class orthogonalPP: public MPxNode {
		public:
virtual	void		postConstructor();
		orthogonalPP (); virtual ~orthogonalPP ();
		virtual MStatus compute (const MPlug& plug, MDataBlock& data);
		static void *creator (); static MStatus initialize (); static MTypeId id;
	private:
		static MObject aFront;
		static MObject aUp;	   
		static MObject aOutputLength;	   
		static MObject aDefaultRef; 

		static MObject aOutput;

};
