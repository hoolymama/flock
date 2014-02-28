/***************************************************************************
splinePool.h  -  description
-------------------
    begin                : Fri Mar 31 2006
    copyright            : (C) 2006 by Julian Mann
    email                : julian.mann@gmail.com
	***************************************************************************/
#ifndef _splinePool
#define _splinePool

#include <maya/MPxNode.h>
#include <maya/MTypeId.h>
#include <splinePoolData.h>
/// hello
class splinePool : public MPxNode
{
public:
	
	splinePool();
	///
	virtual				~splinePool();
	///
	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );
	
	static  void*		creator();
	///
	static  MStatus		initialize();
	///
	static  MObject		aInput;
	///
	static  MObject		aOutput;
	
	static MTypeId	id;
	///
	
private:
		///
		splinePoolData * m_pd;
	
};

#endif
