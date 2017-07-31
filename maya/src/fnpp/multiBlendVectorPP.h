#include <maya/MPxNode.h>

class multiBlendVectorPP: public MPxNode {
		public:
virtual	void		postConstructor();
		multiBlendVectorPP (); virtual ~multiBlendVectorPP ();
		virtual MStatus compute (const MPlug& plug, MDataBlock& data);
		static void *creator (); static MStatus initialize (); static MTypeId id;
	private:
		static MObject aVectorArray;	   
		static MObject aWeightArray;	
    static MObject aDefaultWeight;	   
		static MObject aInput;

		static MObject aNormalizeWeights;
		static MObject aOutput;

};