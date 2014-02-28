/*
 *  steerRemap.h
 *  jtools
 *
 *  Created by Julian Mann on 29/12/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */

#include <maya/MPxNode.h>
#include <errorMacros.h>


class steerRemap: public MPxNode {
public:
	//bvirtual	void		postConstructor();
	steerRemap (); 
	virtual ~steerRemap ();
	
	virtual MStatus compute (const MPlug& plug, MDataBlock& data);
	static void *creator (); 
	static MStatus initialize (); 
	static MTypeId id;
private:
		
	enum RemapMethod {kRemap, kMult};
		
	static MObject aRotMethod;		
	static MObject aMagMethod;	
		
	static MObject aReference;		
	static MObject aGoal;		
	
	static MObject aRotCurve;
	static MObject aMagCurve;	
	static MObject aOutput;		

};













