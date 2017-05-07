/*
 *  phiArrayToMulti.h
 *  animal
 *
 *  Created by Julian Mann on 01/08/2006.
 *  Copyright 2006 HoolyMama. All rights reserved.
 *
 */

#ifndef phiArrayToMulti_H
#define phiArrayToMulti_H


#include <maya/MPxNode.h>

class phiArrayToMulti: public MPxNode {
public:
	virtual	void postConstructor();
	phiArrayToMulti (); 
	virtual ~phiArrayToMulti ();
	virtual MStatus compute (const MPlug& plug, MDataBlock& data);
	static void *creator (); 
	static MStatus initialize (); 
	static MTypeId id;
private:
 
 	static MObject aRotateOrder;
	static MObject aInput;
	static MObject aOutput;
	static MObject aOutputX;
	static MObject aOutputY;
	static MObject aOutputZ;

};
#endif