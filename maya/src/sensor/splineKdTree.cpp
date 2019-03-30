/***************************************************************************
                        splineKdTree.cpp  -  description
                           -------------------
  begin                : Mon Apr 3 2006
  copyright            : (C) 2006 by Julian Mann
  email                : julian.mann@gmail.com

this is almost the same as ptKdTree
the exception being that the data stored - i.e. the splineangles in bounding boxes,
may be in a bucket, but may overlap another bucket. Therefore each bucket node
must have attached a list of nodes that overlap. This is the overlap list
***************************************************************************/

#define P_TOLERANCE 0.000001

#include <splineKdTree.h>

splineKdTree::splineKdTree() //  constructor
	: m_perm(0),
	  m_pRoot(0),
	  m_maxPointsPerBucket(4)
{
	m_pRoot = 0;
	m_perm = new LINE_LIST;

}

splineKdTree::~splineKdTree()
{
	if (m_perm) {
		LINE_LIST::iterator p =  m_perm->begin();
		while (p != m_perm->end()) {
			delete *p;
			*p = 0;
			p++;
		}
		delete m_perm;
		m_perm = 0;
	}

	makeEmpty(m_pRoot);  // recursively delete tree
}

void splineKdTree::makeEmpty(splineDataKdNode *p) {
	if (p) { // if the pointer aint NULL
		if (!(p->bucket)) {
			makeEmpty(p->loChild);	// recurse through kids
			makeEmpty(p->hiChild);
		}
		else {
			delete p->overlapList;
			p->overlapList  = 0;
		}
		delete p;
		p = 0;					// zap the little bugger
	}
}

const splineDataKdNode *splineKdTree::root() {
	return m_pRoot;
};

//populate the tree with splineData objects
MStatus splineKdTree::populate(const MFnDynSweptGeometryData  &g, int id)  {
	MStatus st;
	MString method("splineKdTree::populate");

	unsigned int len = g.lineCount(&st); mser; // eg 20
	//cerr << "line count " << len <<  endl;

	double end = 0.0;
	double start;

	if (!len) { return MS::kFailure; }
	for (unsigned i = 0; i < len; i++) {
		start = end;
		end = ( i + 1.0 ) / double(len);
		MDynSweptLine spline =  g.sweptLine(i);
		// cerr << "start " << start << "  -- end " << end << endl;
		splineData *t = new splineData(spline, start , end , id);
		m_perm->push_back(t) ;
	}

	return MS::kSuccess;
}

int splineKdTree::size() {
	return m_perm->size();
}

void splineKdTree::build() {
	int low = 0;
	int high = (size() - 1);
	m_pRoot = build(low, high);


	LINE_LIST::iterator currentBox = m_perm->begin();
	while (currentBox != m_perm->end()) {
		setOverlapList(m_pRoot , *currentBox);
		currentBox++;
	}
}


splineDataKdNode   *splineKdTree::build(	 int low,  int high	) {
	// // cout << "in subBuild routine " << endl;
	splineDataKdNode *p = new splineDataKdNode;

	if (((high - low) + 1) <= m_maxPointsPerBucket) {
		// only bucket nodes will hold an overlapList
		p->bucket = true;
		p->loPoint = low;
		p->hiPoint = high;
		p->loChild = 0;
		p->hiChild = 0;
		p->overlapList = new LINE_LIST;
	}
	else {

		p->bucket = false;
		p->cutAxis = findMaxAxis(low, high);
		int mid = ((low + high) / 2);
		wirthSelect(low, high, mid, p->cutAxis);
		p->cutVal = ((*m_perm)[mid])->center(p->cutAxis);
		p->loChild = build(low, mid);
		p->hiChild = build(mid + 1, high);
	}
	return p;
}


void splineKdTree::wirthSelect(  int left,  int right,  int k, mayaMath::axis cutAxis )
{
	int n = (right - left) + 1;
	if (n <= 1) { return; }
	register int i, j, l, m;
	splineData *x;
	splineData *tmp;

	l = left;
	m = right;
	while (l < m) {
		x = (*m_perm)[k];
		i = l;
		j = m;
		do {
			while (  ((*m_perm)[i])->center(cutAxis) <  x->center(cutAxis)  ) { i++; }
			while (  ((*m_perm)[j])->center(cutAxis) >  x->center(cutAxis)  ) { j--; }

			if (i <= j) {
				// swap
				tmp = (*m_perm)[i];
				(*m_perm)[i] = (*m_perm)[j] ;
				(*m_perm)[j] = tmp ;
				i++; j--;
			}
		}
		while (i <= j);
		if (j < k) { l = i; }
		if (k < i) { m = j; }
	}
}

mayaMath::axis splineKdTree::findMaxAxis(const  int low, const  int high) const {


	// The idea here is just to find the axis containing the longest
	// side of the bounding rectangle of the points

	// From a vector of N points we just take sqrtN samples
	// in order to keep the time down to O(N)
	// should be ok though
	double minx , miny,  minz , maxx , maxy , maxz, tmpVal;
	double sx, sy, sz;

	int  num = (high - low ) + 1;
	int interval = int(sqrt(double(num)));
	// unsigned int intervalSq = interval*interval;
	int i;
	MPoint p = ((*m_perm)[low])->center();
	minx = p.x;
	maxx = minx;
	miny = p.y;
	maxy = miny;
	minz = p.z;
	maxz = minz;

	for (i = (low + interval); i <= high; i += interval ) {
		p = ((*m_perm)[i])->center();
		tmpVal = p.x;
		if (tmpVal < minx) {
			minx = tmpVal;
		}
		else {
			if (tmpVal > maxx) {
				maxx = tmpVal;
			}
		}
		tmpVal = p.y;
		if (tmpVal < miny) {
			miny = tmpVal;
		}
		else {
			if (tmpVal > maxy) {
				maxy = tmpVal;
			}
		}
		tmpVal = p.z;
		if (tmpVal < minz) {
			minz = tmpVal;
		}
		else {
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
		}
		else {
			return mayaMath::zAxis;
		}
	}
	else {
		// x is not the greatest
		if (sy > sz) {
			return mayaMath::yAxis;
		}
		else {
			return mayaMath::zAxis;
		}
	}
}

void  splineKdTree::setOverlapList(splineDataKdNode *p,  splineData *tb  )  {
	// recursive setOverlapList method
	// put splineBox in every bucket it overlaps
	if (p->bucket) {
		p->overlapList->push_back(tb);
	}
	else {
		if (tb->min(p->cutAxis) < p->cutVal) {
			setOverlapList(p->loChild, tb);
		}
		if (tb->max(p->cutAxis) > p->cutVal) {
			setOverlapList(p->hiChild, tb);
		}
	}
}


void  splineKdTree::searchList(
  const LINE_LIST *overlapList,
  const MPoint &searchPoint,
  double &radius,
  splineData &result
) const {

	LINE_LIST::const_iterator curr;
	curr = overlapList->begin();

	while (curr != overlapList->end()) {
		if ((*curr)->sphereIntersectsBB(searchPoint, radius)  ) {
			// if the box is within our search radius, then the splineangle might also be
			double dist = 0;

			if ( (*curr)->sphereIntersectsLine(searchPoint, radius, dist)) {
				// cerr << "spline is within " << radius << " -  actual dist to spline is " << dist << endl;
				// if the spline is within the radius then it is currently the closest spline
				// because the radius has been shrinking
				// NOTE - if successful, the splineData object will have cached the bary coords and the hit point
				// so if this splineData is the final result - i.e. the closest, then the calling function
				// can just pick up the cached closest point rather than recalculate.
				// It can also ask the splineangle for th parameter

				// Now shrink the radius and set the result
				radius	=  dist;
				result =   **curr;
				// cerr << "updating result   - "  << endl;
			}
			else {
				// cerr << "spline is not within   - " << radius << endl;
			}
		}
		else {
			//  cerr << "box is not within " << radius  << endl;
		}
		curr++;
	}
}

// recursive function to find the closest splineangle
void splineKdTree::closestLine(
  const splineDataKdNode *p,
  const MPoint &searchPoint,
  double &radius,   // radius is passed as reference but is not const, this is how we shrink it during the search.
  splineData &result )  const  {

	if (p->bucket) {
		searchList( p->overlapList, searchPoint, radius, result);
	}
	else {
		double diff = searchPoint[(p->cutAxis)] -
		              p->cutVal; // distance to the cut wall for this bucket
		if (diff < 0) { // we are in the lo child so search it
			closestLine(p->loChild, searchPoint, radius, result);
			if (radius >= -diff) { // if radius overlaps the hi child then search that too
				closestLine(p->hiChild, searchPoint, radius, result);
			}
		}
		else {   // we are in the hi child so search it
			closestLine(p->hiChild, searchPoint, radius, result);
			if (radius >= diff) { // if radius overlaps the lo child then search that too
				closestLine(p->loChild, searchPoint, radius, result);
			}
		}
	}
}

LINE_LIST &splineKdTree::splineList() {
	return *m_perm;
}

