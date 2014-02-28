/*
 *  extractDoubleElements.h
 *  jtools
 *
 *  Created by Julian Mann on 11/13/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <maya/MPxNode.h>

class extractElements : public MPxNode
	{
	public:
		virtual	void		postConstructor();
		extractElements();
		virtual              ~extractElements();
		virtual MStatus      compute( const MPlug& plug, MDataBlock& data );
		static  void*        creator();
		static  MStatus      initialize();
		
	public:
		// input attributes
		static   MObject           aInput;
		static   MObject           aIndexPP;	  
		// static   MObject           aDefault;
		
		// output attributes
		static   MObject           aOutputD;
		static   MObject           aOutputV;
		
		static   MTypeId           id;
		
	};

