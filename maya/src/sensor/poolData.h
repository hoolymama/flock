/***************************************************************************
poolData.h  -  description
-------------------
    begin                : Fri Mar 31 2006
    copyright            : (C) 2006 by Julian Mann
    email                : julian.mann@gmail.com
	***************************************************************************/

#ifndef _poolData
#define _poolData

//
// Copyright (C) 2006 hoolyMama
//
// Author: Julian Mann

#include <maya/MIOStream.h>

#include <vector>
#include <math.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MVector.h>
#include <maya/MObject.h>
#include <maya/MPxData.h>
#include <maya/MTypeId.h>
#include <maya/MString.h>
#include <agentKdTree.h>

class poolData : public MPxData
{
public:
	// constructor
	poolData();
	
	// destructor
	virtual	~poolData();
	
	// creator
	static	void*		creator();
	
	// copy constructor
	virtual	void		copy(const MPxData&);
	
	// operator =
	poolData& operator=(const poolData &);
	
	// type
	virtual	MTypeId		typeId() const {return id;}
	static	MTypeId		id;
	
	// name
	virtual	MString		name() const {return typeName;}
	static	MString		typeName;
	
	
	// create some new member data
	void create(MDataHandle &hp);
	
	// clear member data
	void	clear();
	
	// accessor
	 agentKdTree*	tree() const;
	
	// accessor
					
	int size();
	void closestNPoints(const agent * searchAgent, double radius, int num, AGENT_VECTOR * neighbors, MDoubleArray &dists )  const	;
	void closestNPoints(const MVector &searchPoint, double radius, int num, AGENT_VECTOR * neighbors, MDoubleArray &dists )  const ;

	//void closestNPoints(const MVector &searchPoint, double radius, int num, AGENT_VECTOR * neighbors, MDoubleArray &dists )  const;
private:
		
		agentKdTree 	*	m_pTree; 			// the agentKdTree
	
};

#endif


