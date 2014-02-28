#include <maya/MPxNode.h>

class uvPP: public MPxNode {
	public:
virtual	void		postConstructor();
		uvPP (); virtual ~uvPP ();
		virtual MStatus compute (const MPlug& plug, MDataBlock& data);
		static void *creator (); static MStatus initialize (); static MTypeId id;
	private:
		static MObject aPosition;
		static MObject aMinUV;	   
		static MObject aMaxUV;	

		// for now use Z - maybe change in future
		static MObject aProjection;	   
		
		static MObject aOutU;
		static MObject aOutV;

};
