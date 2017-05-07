/*
 *  vectorArrayToMulti.h
 *  animal
 *
 *  Created by Julian Mann on 01/08/2006.
 *  Copyright 2006 HoolyMama. All rights reserved.
 *
 */

#ifndef vectorArrayToMulti_H
#define vectorArrayToMulti_H


#include <maya/MPxNode.h>

class vectorArrayToMulti: public MPxNode {
public:
	virtual	void postConstructor();
	vectorArrayToMulti (); 
	virtual ~vectorArrayToMulti ();
	virtual MStatus compute (const MPlug& plug, MDataBlock& data);
	static void *creator (); 
	static MStatus initialize (); 
	static MTypeId id;
private:
 
	static MObject aInput;
	static MObject aOutput;
};
#endif