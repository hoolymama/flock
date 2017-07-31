/*
 *  prioritize.h
 *  animal
 *
 *  Created by Julian Mann on 30/07/2006.
 *  Copyright 2006 hooly|mama. All rights reserved.
 *
 */

#ifndef _prioritize_H
#define _prioritize_H

#if defined(OSMac_MachO_)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
// #include <GLKit/GLKMatrix4.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <maya/MIOStream.h>

#include <vector>


#include <math.h>
#include <maya/MString.h> 
#include <maya/MTypeId.h> 
#include <maya/MPlug.h>
#include <maya/MVector.h>
#include <maya/MPoint.h>
#include <maya/MVectorArray.h>
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MDataHandle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MColor.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MColorArray.h>  
#include <maya/MTime.h>  


#include <maya/MPxLocatorNode.h> 
#include <maya/MDataBlock.h>

#include <maya/M3dView.h>

#include <maya/MDoubleArray.h>
#include <maya/MColorArray.h>

#include "errorMacros.h"


typedef std::vector<MVectorArray> VEC_LIST; // list of arrays for draw data


class prioritize : public MPxLocatorNode
{
public:
	prioritize();
	virtual ~prioritize(); 
	virtual void postConstructor();
	
    virtual MStatus   		compute( const MPlug& plug, MDataBlock& data );
	virtual MStatus			connectionBroken ( const MPlug & plug, const MPlug & otherPlug, bool asSrc );
	
	virtual void            draw( M3dView & view, const MDagPath & path, 
								  M3dView::DisplayStyle style,
								  M3dView::DisplayStatus status );
	
	virtual bool            isBounded() const;
	virtual MBoundingBox    boundingBox() const; 
	
	static  void *          creator();
	static  MStatus         initialize();
	
	
	
	static	MTypeId		id;
	
private:
	static  MObject        	aGlyphScale;	
	static  MObject         aPositions;	
	
	static  MObject         aInForce;	
	static	MObject         aDrawColor;	// graph draw color
	static	MObject         aDrawColorR;	// graph draw color
	static	MObject         aDrawColorG;	// graph draw color
	static	MObject         aDrawColorB;	// graph draw color
	static	MObject         aActive;
	// static	MObject         aContainerContribution;
	static	MObject         aMagnitude;
	
	static	MObject         aInData;		// compound (multi)
	
	static MObject			aMaxForce;
	static MObject          aEnableGraph;		// cache and do drawing
	
	static	MObject         aSampleBy;		
	static	MObject         aHeight;		
	static	MObject         aWidth;		
	static	MObject         aXOffset;		
	static	MObject         aYOffset;		

	static	MObject         aOutForces; 
	static	MObject         aOutForce; 
 
 
	
};

#endif




















