 #include "HexapodHome.h"

const double  PI  = 3.141592653;
const double  TAU = 2.0 * PI;
const unsigned  divs = 16;

HexapodHome::HexapodHome(double x, double z, double min, double max, MColor& color)
:  m_x(x), m_z(z), m_min(min), m_max(max) , m_minCircle(divs), m_maxCircle(divs) , m_color(color)
{
	float fx = float(x);
	float fz = float(z);
	float fmin = float(min);
	float fmax = float(max);


	float gap = float(TAU) / float(divs);
	for(unsigned j = 0; j < divs; ++j) { 
		float vertx = sin(gap * j);
		float vertz = cos(gap * j);
		
		m_minCircle[j] = MFloatVector( (vertx*fmin)+fx ,0.0f,(vertz*fmin)+fz );
		m_maxCircle[j] = MFloatVector( (vertx*fmax)+fx ,0.0f,(vertz*fmax)+fz );
	}

}

HexapodHome::HexapodHome() {
	 MColor green(0.0,1.0,0.0);
	HexapodHome(0,0,0,1, green);
}

HexapodHome::~HexapodHome(){ }


void HexapodHome::drawACircle(M3dView & view, 
	const MFloatVectorArray & points, 
	const MFloatMatrix & matrix, 
	const MColor& color) const {

	MFloatPointArray c(divs);
	for (int i = 0; i < divs; ++i) { 
		c[i] = MFloatPoint(points[i])  * matrix;
	}

	view.setDrawColor(color);
	glBegin( GL_LINES );

	for (int i = 0; i < divs; ++i)
	{
		int next = (i+1)%divs;
		glVertex3f( c[i].x , c[i].y , c[i].z );
		glVertex3f( c[next].x , c[next].y , c[next].z );
	}
	glEnd();
}

void HexapodHome::draw(M3dView & view, MFloatMatrix & matrix) const {
	HexapodHome::drawACircle(view, m_minCircle, matrix, m_color);
	HexapodHome::drawACircle(view, m_maxCircle, matrix, m_color);
};


const double & HexapodHome::x()  const    {return m_x;}
const double & HexapodHome::z()  const    {return m_z;}
const double & HexapodHome::min()  const  {return m_min;}
const double & HexapodHome::max()  const  {return m_max;}
  
