 /***************************************************************************
                          agentKdTree.cpp  -  description
                             -------------------
    begin                : Wed Mar 29 2006
    copyright            : (C) 2006 by Julian Mann
    email                : julian.mann@gmail.com

  this kdTree stores agent objects. It's meant to be pretty fast.



  I'm a little concerned about the constructor / destructor / makeEmpty methods
  If scenes start to crash in very odd ways - then its probably to do with stray
  or null pointers that are not correctly handled here

***************************************************************************/

#define P_TOLERANCE 0.00000001

#include <maya/MIOStream.h>
#include <math.h>

#include <maya/MVector.h>
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MIntArray.h>

#include <agentKdTree.h>

const double bigNum = 99999999999.0  ;

agentKdTree::agentKdTree() // default constructor
	:m_pPerm(0),
	m_pRoot(0),
m_maxPointsPerBucket(4)
{

	m_pPerm = new AGENT_VECTOR;
	m_pRoot = 0;
};


agentKdTree::~agentKdTree()
{
	// delete the elements in the permutations array
	if (m_pPerm) {
		AGENT_VECTOR::iterator p = m_pPerm->begin();
		while (p != m_pPerm->end()) {
			delete *p;
			*p =0;
			p++;
		}
		// then delete the array itself
		delete m_pPerm;
		m_pPerm = 0;
	}
	
	makeEmpty(m_pRoot);  // recursively delete tree 	
};

void agentKdTree::makeEmpty(agentKdNode * p) {
	// post order traversal to delete nodes
	if (p) { // if the pointer is not NULL
		if (!(p->bucket)) { // if its not a bucket node
			makeEmpty(p->loChild);	// recurse through child nodes
			makeEmpty(p->hiChild);
		}
		delete p;            // then delete this node
		p = 0;
	}
}




void agentKdTree::populate(const MVectorArray & p)
{
	unsigned int pl = p.length();
	for (unsigned int i = 0; i < pl; i++) {
		agent * pp = new agent( p[i], i);
		m_pPerm->push_back(pp);
	} 
}


const int  agentKdTree::size() const {
	return m_pPerm->size();
}


MStatus agentKdTree::build(){
	m_pRoot = build( 0, (m_pPerm->size() -1) );
	if (!m_pRoot) {
		return MS::kUnknownParameter;
	}
	return MS::kSuccess;
}


agentKdNode *  agentKdTree::build( int low,  int high	){
	
	// recursively make a new agentKdNode and 

	// reArrange the entries in perm respectively
	
	agentKdNode * p = new agentKdNode;

 	if ( unsigned((high - low) + 1) <= m_maxPointsPerBucket) {   // stop recursing
		
		p->bucket = true;
		p->loPoint = low;
		p->hiPoint = high;
		p->loChild = 0;
		p->hiChild = 0;
		
	} else {   // split the space and recursively build
		p->bucket = false;
		p->cutAxis = findMaxAxis(low, high); 
		
		int mid = ((low + high) / 2);
		wirthSelect(low, high, mid, p->cutAxis);
		p->cutVal = float(((*m_pPerm)[mid])->position(p->cutAxis));
		p->loChild = build(low, mid);
		p->hiChild = build(mid+1, high);
	}
	return p;
}


// accessor
const agentKdNode * agentKdTree::root() const {
	return m_pRoot;
};



// accessor
AGENT_VECTOR * agentKdTree::perm() {
	return m_pPerm;
};


// a selection algorithm written by someone called wirth. This is the
// only selection algo I could find with no bugs. 
void agentKdTree::wirthSelect(  int left,  int right,  int k, mayaMath::axis cutAxis )  
{
	int n = (right - left) + 1;
	if (n <= 1) return;
	register int i,j,l,m;
	agent *x;
	agent *tmp;
	
	l=left;
	m=right;
	while (l<m) {
		x = (*m_pPerm)[k];
		i=l;
		j=m;
		do {
			while (  ((*m_pPerm)[i])->position(cutAxis) <  x->position(cutAxis)  ) i++;
			while (  ((*m_pPerm)[j])->position(cutAxis) >  x->position(cutAxis)  ) j--;
			
			if (i<=j) {
				tmp = (*m_pPerm)[i];  
				(*m_pPerm)[i] =(*m_pPerm)[j] ;  
				(*m_pPerm)[j] =tmp ;
				i++; j--;
			}
		} while(i<=j);
		if (j<k) l=i;
		if (k<i) m=j;
	}
}



// The idea is just to find the axis containing the longest
// side of the bounding rectangle of the points
// From a list of N points we just take sqrtN samples
// so this keeps the calculation time down to O(N)
// which is usually ok
const mayaMath::axis agentKdTree::findMaxAxis(  int low,   int high) const {
	
	int  num = (high - low ) +1;
	int interval = int(sqrt(float(num)));
	int i;
	MVector p = ((*m_pPerm)[low])->position();
	
	double minx , miny,  minz , maxx , maxy , maxz, tmpVal;
	double sx, sy, sz;
	
	minx = p.x;
	maxx = minx;
	miny = p.y;
	maxy = miny;
	minz = p.z;
	maxz = minz;
	
	for (i= (low + interval); i<=high;i+=interval ) {
		p = ((*m_pPerm)[i])->position();
		tmpVal= p.x;
		if (tmpVal < minx) {
			minx = tmpVal;
		} else {
			if (tmpVal > maxx) {
				maxx = tmpVal;
			}
		}
		tmpVal= p.y;
		if (tmpVal < miny) {
			miny = tmpVal;
		} else {
			if (tmpVal > maxy) {
				maxy = tmpVal;
			}
		}
		tmpVal= p.z;
		if (tmpVal < minz) {
			minz = tmpVal;
		} else {
			if (tmpVal > maxz) {
				maxz = tmpVal;
			}
		}
	}
	
	sx = maxx - minx;
	sy = maxy - miny;
	sz = maxz - minz;
	
	if (sx > sy) {
		// y is not the greatest
		if (sx > sz) {
			return mayaMath::xAxis;
		} else {
			return mayaMath::zAxis;
		}
	} else {
		// x is not the greatest
		if (sy > sz) {
			return mayaMath::yAxis;
		} else {
			return mayaMath::zAxis;
		}
	}
}



// is the point within the radius
bool  agentKdTree::pointInRadius(  int i , const MVector & searchPoint ,const double & radius) const {
	MVector  diff = (((*m_pPerm)[i])->position() - searchPoint);
	double dl =  diff.length() ;
	if ((dl < radius) ) { 
		if ( dl > P_TOLERANCE) return 1;
	} 
	return 0;
}



// do a fixed radius search and return the result points in a AGENT_VECTOR
void  agentKdTree::fixedRadiusSearch(
								const agentKdNode * p,
								const MVector & searchPoint,
								const double & radius, 
								AGENT_VECTOR * result
								)  const  {
	if (!p) return;
	if (p->bucket) {
		for (unsigned int i = p->loPoint; i <= p->hiPoint;i++ ) {
			if (pointInRadius(i , searchPoint , radius)) {
				result->push_back((*m_pPerm)[i]);
			}
		}
	} else {
		double diff = searchPoint[(p->cutAxis)] - p->cutVal; // distance to the cut wall for this bucket
		if (diff<0) { // we are in the lo child so search it
			fixedRadiusSearch(p->loChild,searchPoint,radius,result);
			if (radius >= -diff) { // if radius overlaps the hi child then search that too
				fixedRadiusSearch(p->hiChild,searchPoint,radius,result);
			}
		} else { // we are in the hi child so search it 
			fixedRadiusSearch(p->hiChild,searchPoint,radius,result);
			if (radius >= diff) { // if radius overlaps the lo child then search that too
				fixedRadiusSearch(p->loChild,searchPoint,radius,result);
			}
		}
	}
}


void  agentKdTree::closestNPts(
							   const agentKdNode * p,
							   const agent * searchAgent,
							   AGENT_QUEUE *q
							   )  const  {
	// NOTE - The priority queue should already have the right number of elements in it
	// i.e. the number of nearest neighbors we are looking for
	if (!p) return;
	if (p->bucket) {
		double dl;
		for (unsigned int i = p->loPoint; i <= p->hiPoint;i++ ) {
			if (!((*m_pPerm)[i] == searchAgent)) { // this point is not me
				dl = (((*m_pPerm)[i])->position() - searchAgent->position()).length(); // speed up by implementing sq distTo()
				if ( (dl < q->top().dist)) {
					q->pop();
					agentDist ksd;
					ksd.dist = dl;
					ksd.pAgent =  ((*m_pPerm)[i]);
					q->push(ksd)  ;
				}
			}
		}
	} else {
		double diff = searchAgent->position(p->cutAxis) - p->cutVal; // distance from search point  to the cut wall for this bucket
		if (diff<0) { // we are in the lo child so search it
			closestNPts(p->loChild,searchAgent,q);
			if (q->top().dist >= -diff) { // if radius overlaps the hi child then search that too
				closestNPts(p->hiChild,searchAgent,q);
			}
		} else { // we are in the hi child so search it
			closestNPts(p->hiChild,searchAgent,q);
			if (q->top().dist >= diff) { // if radius overlaps the lo child then search that too
				closestNPts(p->loChild,searchAgent,q);
			}
		}
	}
 }






void  agentKdTree::closestNPts(
							const agentKdNode * p,
							const MVector & searchPoint,
							AGENT_QUEUE *q
							)  const  {
	// NOTE - The priority queue should already have the right number of elements in it
	// i.e. the number of nearest neighbors we are looking for
	if (!p) return;
	if (p->bucket) {
		double dl;
		for (unsigned int i = p->loPoint; i <= p->hiPoint;i++ ) {
			dl = (((*m_pPerm)[i])->position() - searchPoint).length();
			// the tolerance check ensures that we are not looking at ourself
			if ((P_TOLERANCE < dl) && (dl < q->top().dist)) {
				q->pop();
				agentDist ksd;
				
				ksd.dist = dl;
				ksd.pAgent =  ((*m_pPerm)[i]);
				q->push(ksd)  ;
			}
		}
	} else {
		double diff = searchPoint[(p->cutAxis)] - p->cutVal; // distance to the cut wall for this bucket
		if (diff<0) { // we are in the lo child so search it
			closestNPts(p->loChild,searchPoint,q);
			if (q->top().dist >= -diff) { // if radius overlaps the hi child then search that too
				closestNPts(p->hiChild,searchPoint,q);
			}
		} else { // we are in the hi child so search it
			closestNPts(p->hiChild,searchPoint,q);
			if (q->top().dist >= diff) { // if radius overlaps the lo child then search that too
				closestNPts(p->loChild,searchPoint,q);
			}
		}
	}
}




