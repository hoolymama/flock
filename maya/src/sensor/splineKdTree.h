#ifndef _splineKdTree
#define _splineKdTree

#include <maya/MIOStream.h>
#include <math.h>

#include <maya/MString.h>
#include <maya/MVector.h>
#include <maya/MIntArray.h>
#include <maya/MFnDynSweptGeometryData.h>
#include <maya/MDynSweptTriangle.h>
#include <maya/MItMeshPolygon.h>

#include <splineData.h>
#include <errorMacros.h>


class     splineData ;

typedef vector<splineData*> LINE_LIST;


struct splineDataKdNode  {
	bool bucket;							/// is this node a bucket
	mayaMath::axis cutAxis;							/// if not, this axis will divide it 
	double cutVal;							/// at this value
	splineDataKdNode *loChild, *hiChild;		/// and these pointers point to the children
	unsigned int loPoint, hiPoint;			/// Indices into the permutations array
	LINE_LIST * overlapList;	/// list of splineDatas whose boumding boxes intersect this bucket

	
};


class splineKdTree 
{
	public:

		splineKdTree();			

		~splineKdTree();  
		const splineDataKdNode * root();
		splineKdTree& operator=(const splineKdTree & otherData );

		void 		build(); 							/// build root from m_perm

		splineDataKdNode * 	build( int low,  int high ); 	/// build children recursively
		int size() ;
		//MStatus 	getSize(int &siz);  			/// size of m_perm and _prismVector - returns failure if they are different

		void  	setMaxPointsPerBucket( int b) {if (b < 1) {m_maxPointsPerBucket = 1;} else {m_maxPointsPerBucket = b;}}

		MStatus 	init();

		MStatus 	populate(const MFnDynSweptGeometryData &g, int id) ;

		void wirthSelect(  int left,  int right,  int k, mayaMath::axis cutAxis )  ;

		void makeEmpty() ; 
		
		void makeEmpty(splineDataKdNode * p) ; 

		void  setOverlapList(splineDataKdNode * p,  splineData * sph  );
		
		LINE_LIST & splineList();

  	void closestLine(const splineDataKdNode * p,const MPoint &searchPoint,  double & radius,  splineData &  result )  const ;
	
	private:	
		mayaMath::axis 		findMaxAxis(const  int low,const  int  high) const;
	void  searchList(const LINE_LIST * overlapList,const MPoint &searchPoint, double & radius, splineData & result) const   ;
		LINE_LIST * m_perm;				/// pointer to (permutations) - a list of pointers to elements of _splineDataVector
		splineDataKdNode * m_pRoot;							/// pointer to root of tree
		 int m_maxPointsPerBucket;				/// at build time keep recursing until no bucket holds more than this
		
} ;

#endif


