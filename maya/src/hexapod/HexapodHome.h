#ifndef HexapodHome_H
#define HexapodHome_H

#include <maya/MFloatVectorArray.h>
#include <maya/MFloatPointArray.h>


#include <maya/MColor.h>
#include <maya/M3dView.h>
#include <maya/MFloatMatrix.h>

class HexapodHome{
public:


	HexapodHome();

	HexapodHome(double x, double z, double min, double max, MColor& color);

	~HexapodHome();

	void draw(M3dView & view, MFloatMatrix & matrix) const;


void drawACircle(
	M3dView & view, 
	const MFloatVectorArray & points, 
	const MFloatMatrix & matrix, 
	const MColor& color) const;

	const double & x()  const ;
	const double & z()  const ;
	const double & min()  const ;
	const double & max()  const ;


private:
	double m_x;
	double m_z;
	double m_min;
	double m_max;
	MFloatVectorArray m_minCircle;
	MFloatVectorArray m_maxCircle;
	MColor m_color;
};



#endif

