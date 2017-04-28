#ifndef Ground_H
#define Ground_H
#include <maya/MFnMesh.h>
#include <maya/MObject.h>


/*
Ground, manages the ground mesh.
*/
class Ground {
public:
	Ground();
	~Ground();

	MStatus setMesh(const  MObject& node,  MDataBlock& data) ;

	MPoint  project(const MPoint &src, const MMatrix & mat)  ;

bool  valid() const ;
bool  isAnimated() const ;


private:
	bool m_isAnimated;
	bool m_useMesh;
	MObject m_mesh;
	MFnMesh m_meshFn;
	MMeshIsectAccelParams m_intersector;
 

} ;


#endif


