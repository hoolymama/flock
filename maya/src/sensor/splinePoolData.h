/***************************************************************************
                          splinePoolData.h  -  description
                             -------------------
    begin                : Fri Mar 31 2006
    copyright            : (C) 2006 by Julian Mann
    email                : julian.mann@gmail.com
 ***************************************************************************/

#ifndef _splinePoolData
#define _splinePoolData

//
// Copyright (C) 2006 hoolyMama
//
// Author: Julian Mann

#include <maya/MIOStream.h>
#include <string>
#include <vector>
#include <math.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MVector.h>
#include <maya/MObject.h>
#include <maya/MPxData.h>
#include <maya/MTypeId.h>

#include <splineKdTree.h>



class splinePoolData : public MPxData
{
public:

	splinePoolData();

	virtual	~splinePoolData();

	static	void*		creator();

	virtual	void		copy(const MPxData&);

	splinePoolData& operator=(const splinePoolData &);

	// type
	virtual	MTypeId		typeId() const {return id;}
	static	MTypeId		id;

	
	virtual	MString		name() const {return typeName;}
	static	MString		typeName;

	void	create(MDataHandle &h);
	void    create(MArrayDataHandle &ah);
	void	clear();

	splineKdTree*	tree() const;
/*
   MStatus closestPointOnLine(
       const MVector &searchPoint,
       MVector &resultPoint,
       double &resultU,
       double &resultParam
   ) const ;
   MStatus closestPointOnLine(
       const MVector &searchPoint,
       MVector &resultPoint
   ) const ;
	MStatus closestPointsOnLine(
	    const MVectorArray &searchPoints,
	    MVectorArray &resultPoints,
	    MDoubleArray &resultUs,
	    //MIntArray &resultIds,
	    MDoubleArray &resultParams
	) const ;

    MStatus closestPointsOnLine(
       const MVectorArray &searchPoints,
       MVectorArray &resultPoints
   ) const ;
	
	MStatus closestTangentsOnLine(
		const MVectorArray &searchPoints,
		MVectorArray &resultTangents
	  ) const;
	
	MStatus closestTangentsOnLine(
	const MVectorArray &searchPoints,
	const MVectorArray &forces,
	MVectorArray &resultTangents
	) const;
	
	MStatus blendClosestTangentsOnLine(
	 const MVectorArray &searchPoints,
	 double distance,
	 double attenuation,
	 MVectorArray &resultTangents
	 ) const;
	
	MStatus blendClosestTangentsOnLine(
	 const MVectorArray &searchPoints,
	 const MVectorArray &forces,
	 double distance,
	 double attenuation,
	 MVectorArray &resultTangents
	 ) const ;
 */
   int size();

private:

	splineKdTree 	*	m_pTree; 			// the ptKdTree

};

#endif


