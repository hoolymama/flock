#ifndef splineData_H
#define splineData_H

#include <maya/MIOStream.h>
#include <math.h>

#include <maya/MDynSweptLine.h>
#include <maya/MBoundingBox.h>
#include <maya/MPointArray.h>
#include <maya/MVectorArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFloatPointArray.h>

#include <maya/MVector.h>
#include <maya/MDoubleArray.h>
#include <vector>

#include <errorMacros.h>
#include <mayaMath.h>

// enum axis {xAxis, yAxis ,zAxis };

// typedef mayaMath::axis axis;
using namespace std;


class splineData{
public:
	splineData();
	splineData( MDynSweptLine &l,double uStart, double uEnd, int id);
	
	~splineData();
	
	splineData& operator=(const splineData& other) ;
	
	const MVector & center() const;
	double center(mayaMath::axis a) const ;
	
	double min(mayaMath::axis a) const ;
	double max(mayaMath::axis a) const ;
	
	//double area() const ;
	int id() const;

	bool sphereIntersectsBB(const MVector &c,   double r) const  ;
	bool sphereIntersectsLine(const MVector &c,  double r, double &dist) ;
	
	const MBoundingBox & box() const ;
	const MVector & vertex(int i) const ;
   	const MDoubleArray & uCoords() const ;
	const MVector & cachedPoint() const;
	const MVector & tangent() const;
	const double & cachedParam() const;
	const double & length() const;
	double calcU() const ;
private:
		
 	void computeBoundingBox();
   	
	MVectorArray m_v;
	MVector m_end;
	MBoundingBox m_box;
	MVector m_center;
	MDoubleArray m_uCoords;
	double m_cachedParam;
	MVector m_cachedPt;
	MVector m_tangent;
	int m_id;
	double m_length;
	
};

#endif

