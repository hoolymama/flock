/*
 *  arrayToMulti.h
 *  animal
 *
 *  Created by Julian Mann on 01/08/2006.
 *  Copyright 2006 HoolyMama. All rights reserved.
 *
 */

#include <maya/MPxNode.h>

class arrayToMulti: public MPxNode {
public:
	virtual	void postConstructor();
	arrayToMulti (); 
	virtual ~arrayToMulti ();
	virtual MStatus compute (const MPlug& plug, MDataBlock& data);
	static void *creator (); 
	static MStatus initialize (); 
	static MTypeId id;
private:
	static MObject aDefD;
	static MObject aDefV;
	static MObject aInput;
	static MObject aOutputD;
	static MObject aOutputV;
	static MObject aSize;
};
