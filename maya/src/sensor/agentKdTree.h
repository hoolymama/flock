
#ifndef _agentKdTree
#define _agentKdTree

//
// Copyright (C) 2000 hoolyMama 
// 
// File: agentKdTree.h
//
// Author: Julian Mann

                                                                                                                                                                                                            
#include <maya/MVectorArray.h>

#include <queue>

#include <agent.h>





struct agentKdNode  {
	bool bucket;					// is this node a bucket
	mayaMath::axis cutAxis;					// if not, this axis will divide it 
	float cutVal;					// at this value
	agentKdNode *loChild, *hiChild;		// and these pointers point to the children
	unsigned int loPoint, hiPoint;	// Indices into the permutations array
};


class agent;
struct knnSearchData;
struct agentDist;


//typedef std::priority_queue<knnSearchData> KNN_QUEUE;
typedef std::priority_queue<agentDist> AGENT_QUEUE;

// typedef std::vector<AGENT_VECTOR *> PD_CLUSTERS;

// this struct is used as an element in a priority queue

struct agentDist {
	double dist;
	agent * pAgent;
	bool operator< (const agentDist &other) const {return dist < other.dist;}
}  ;




class agentKdTree
{

public:

	//  constructor 
	agentKdTree();	

	// destructor
	~agentKdTree();  
	
	// return the size
	const int size() const;
	
	// set bucket capacity


	void  setMaxPointsPerBucket( int b);


	// add points to the permutations array through a mask   
	void populate(const MVectorArray &p);
	//void populate(const MVectorArray &p, const MVectorArray &v);
	
	// when all poins are added use this method to build the tree 
	MStatus build();

	// return a pointer to the root
	const agentKdNode * root() const;

	// return a pointer to the perm
	//const AGENT_VECTOR * perm() const;

	
	// This was const AGENT_VECTOR * perm() const;
	// I changed it because there is a need to average the velocities 
	// based on the velocities of nearby neighbors
	
	AGENT_VECTOR * perm();
	

	// do search only
	void  fixedRadiusSearch(
		const agentKdNode * p,
		const MVector & searchPoint,
		const double & radius, 
		AGENT_VECTOR * result
	)  const ;

void  closestNPts(
				  const agentKdNode * p,
				  const MVector & searchPoint,
				  AGENT_QUEUE *q
				  )  const;

void  closestPtsInRadius(
				  const agentKdNode * p,
				  const MVector & searchPoint,
				   AGENT_QUEUE *q
				  )  const;




void  closestNPts(
							   const agentKdNode * p,
							   const agent * searchAgent,
							   AGENT_QUEUE *q
							   )  const ;


private:

	void wirthSelect(  int left,  int right,  int k, mayaMath::axis cutAxis )  ;
	const mayaMath::axis findMaxAxis(int low, int high) const;
	void makeEmpty(agentKdNode * p) ; 

	bool pointInRadius	(  int i , const MVector & searchPoint ,const double & radius) const ;


	agentKdNode * build( int low, int high ); 

	AGENT_VECTOR * m_pPerm;						// all points
	agentKdNode * m_pRoot;					// pointer to root of tree
	unsigned int m_maxPointsPerBucket;	// at build time keep recursing until no bucket holds more than this
	// double m_dRadius;
	
};




#endif

