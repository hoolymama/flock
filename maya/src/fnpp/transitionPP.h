#ifndef transitionPP_H
#define transitionPP_H

#include <maya/MPxNode.h>

class transitionPP: public MPxNode {
	public:
virtual	void		postConstructor();
		transitionPP (); virtual ~transitionPP ();
		virtual MStatus compute (const MPlug& plug, MDataBlock& data);
		static void *creator (); static MStatus initialize (); static MTypeId id;
	private:
		static MObject aInput;
		static MObject aChange;	   
		static MObject aMinMaxValue;	      
		static MObject aOutput;
		// static MObject aProfile;

};
#endif