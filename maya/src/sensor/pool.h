/***************************************************************************
                          pool.h  -  description
                             -------------------
    begin                : Mon Apr 3 2006
    copyright            : (C) 2006 by Julian Mann
    email                : julian.mann@gmail.com
***************************************************************************/

#ifndef _pool
#define _pool

#include <maya/MPxNode.h>
#include <maya/MTypeId.h>
#include <poolData.h>
/// hello
class pool : public MPxNode
{
public:

						pool();
///
	virtual				~pool();
///
	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );

	static  void*		creator();
///
	static  MStatus		initialize();
	
	static  MObject		aPosition;

	static  MObject		aOutput;

	static MTypeId	id;
///

private:
///
	poolData * m_pd;

};

#endif
