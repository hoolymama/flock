/***************************************************************************
splineData.cpp  -  description
-------------------
    begin                : Mon Apr 3 2006
    copyright            : (C) 2006 by Julian Mann
    email                : julian.mann@gmail.com

	a class to store lines in a bounding box for use in a lineKdTree.
	***************************************************************************/

#include <splineData.h>
#include <errorMacros.h>

const double TIMESTEP = 1.0f / 24.0f;
const double EPSILON = 0.0000001;
//const double NEG_EPSILON = -0.0001;

splineData::splineData(){}

splineData::splineData( MDynSweptLine &l,double uStart, double uEnd, int id):
m_v(2),
m_uCoords(2),
m_cachedParam(0.0),
m_cachedPt(MPoint::origin),
m_id(id)
{	
	m_v.set(l.vertex(0),0);
	m_v.set(l.vertex(1),1);
	
	m_uCoords.set(uStart,0);
	m_uCoords.set(uEnd,1);


//	cerr << "uCoords ctor" << m_uCoords << endl;
	
	
	computeBoundingBox();
	
	m_length = l.length();
	m_tangent = l.tangent();
}

splineData::~splineData(){}

splineData& splineData::operator=(const splineData& other) {
	if (this != &other) {
		m_v = other.m_v;
		m_box = other.m_box;
		m_uCoords = other.m_uCoords;
		m_cachedParam = other.m_cachedParam;
		m_cachedPt = other.m_cachedPt;
		m_id = other.m_id;
		m_length = other.m_length;
		m_tangent = other.m_tangent; 
	}
	return *this;
}

void splineData::computeBoundingBox(){
	m_box = MBoundingBox(m_v[0],m_v[1]);
	m_center = m_box.center();
	if (
		(m_box.width() < EPSILON) ||
		(m_box.height() < EPSILON) ||
		(m_box.depth() < EPSILON) 
		){
		m_box.expand(m_box.max() + MVector(0.0001,0.0001,0.0001));
		m_box.expand(m_box.min() - MVector(0.0001,0.0001,0.0001));
	}	
}

const MVector & splineData::center() const {return m_center;}

double  splineData::center(mayaMath::axis a) const {return  m_center[a];}

double  splineData::min(mayaMath::axis a) const {return m_box.min()[a];}

double  splineData::max(mayaMath::axis a) const {return m_box.max()[a];}


int splineData::id() const { return m_id;}


const MVector & splineData::vertex(int i) const {return m_v[i];}

const MBoundingBox & splineData::box() const {return m_box;}

const MDoubleArray & splineData::uCoords() const {return m_uCoords;}


// determine if the given spere intersects this bounding box
bool splineData::sphereIntersectsBB(const MVector &c, double r) const  {
	// params are center and radius
	return mayaMath::sphereBoxIntersection( m_box, c, r);
}

// determine if the given spere intersects the line.
// If so, store the point and bary coords of the closest point to the sphere center
// and send back the distance in dist
bool splineData::sphereIntersectsLine(const MVector &c, double r, double &dist) {
	double param;
	MVector pt  = mayaMath::closestPointOnLine(c, m_v[0], m_v[1], param ) ;
	MVector v = pt - c;
	double sqDist = (v*v);
	if ((sqDist) <= (r*r)) {  // bingo
		dist = sqrt(sqDist) ;
    	m_cachedParam = param;
		m_cachedPt = pt;
		return true;
	}
	return false;
}

const MVector & splineData::cachedPoint() const  {
	return m_cachedPt;
}
const MVector & splineData::tangent() const{
	return m_tangent;
}
const double & splineData::length() const{
	return m_length;
}
const double & splineData::cachedParam() const  {
	return m_cachedParam;
}
double splineData::calcU() const {
	return (m_uCoords[0] +    (m_cachedParam * (m_uCoords[1] - m_uCoords[0])  )   );
}





