 
#include <maya/MVectorArray.h>
#include <maya/MString.h> 
#include <maya/MTypeId.h> 
#include <maya/MPlug.h>
#include <maya/MVector.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MColor.h>

#include <maya/MFnNumericAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnDependencyNode.h>

#include <maya/MDoubleArray.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnIntArrayData.h>
#include <maya/M3dView.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFloatVector.h>
#include "errorMacros.h"


#include "hexapodLocator.h"

#include "mayaMath.h"
#include "attrUtils.h"

#include "jMayaIds.h"

const double deg_to_rad = 0.01745329238;



MObject hexapodLocator::aPosition;	
MObject hexapodLocator::aPhi;

MObject hexapodLocator::aLeftA;
MObject hexapodLocator::aLeftB;
MObject hexapodLocator::aLeftC;
MObject hexapodLocator::aRightA;
MObject hexapodLocator::aRightB;
MObject hexapodLocator::aRightC;

MObject hexapodLocator::aHomeLA;
MObject hexapodLocator::aHomeLB;
MObject hexapodLocator::aHomeLC;
MObject hexapodLocator::aHomeRA;
MObject hexapodLocator::aHomeRB;
MObject hexapodLocator::aHomeRC;

MObject hexapodLocator::aLastPlantLA;
MObject hexapodLocator::aLastPlantLB;
MObject hexapodLocator::aLastPlantLC;
MObject hexapodLocator::aLastPlantRA;
MObject hexapodLocator::aLastPlantRB;
MObject hexapodLocator::aLastPlantRC;

MObject hexapodLocator::aNextPlantLA; 
MObject hexapodLocator::aNextPlantLB; 
MObject hexapodLocator::aNextPlantLC; 
MObject hexapodLocator::aNextPlantRA; 
MObject hexapodLocator::aNextPlantRB; 
MObject hexapodLocator::aNextPlantRC; 

MObject hexapodLocator::aScale;

MObject hexapodLocator::aRadiusMinA; 
MObject hexapodLocator::aRadiusMaxA;  
MObject hexapodLocator::aRadiusA; 

MObject hexapodLocator::aRadiusMinB; 
MObject hexapodLocator::aRadiusMaxB;  
MObject hexapodLocator::aRadiusB; 

MObject hexapodLocator::aRadiusMinC; 
MObject hexapodLocator::aRadiusMaxC;  
MObject hexapodLocator::aRadiusC; 

MObject hexapodLocator::aRadiusLA; 
MObject hexapodLocator::aRadiusLB;
MObject hexapodLocator::aRadiusLC;
MObject hexapodLocator::aRadiusRA;
MObject hexapodLocator::aRadiusRB;
MObject hexapodLocator::aRadiusRC;

MObject hexapodLocator::aColorAR;
MObject hexapodLocator::aColorAG;
MObject hexapodLocator::aColorAB;
MObject hexapodLocator::aColorA;

MObject hexapodLocator::aColorBR;
MObject hexapodLocator::aColorBG;
MObject hexapodLocator::aColorBB;
MObject hexapodLocator::aColorB;

MObject hexapodLocator::aColorCR;
MObject hexapodLocator::aColorCG;
MObject hexapodLocator::aColorCB;
MObject hexapodLocator::aColorC;

MObject hexapodLocator::aDrawAxes;
MObject hexapodLocator::aAxesScale;

MObject hexapodLocator::aDrawMinMaxRadius;
MObject hexapodLocator::aDrawRadius;

MObject hexapodLocator::aDrawHomeDots;
MObject hexapodLocator::aDrawStride;
MObject hexapodLocator::aDrawFootDots;
MObject hexapodLocator::aFootDotSize;


MTypeId hexapodLocator::id( k_hexapodLocator );

hexapodLocator::hexapodLocator() {}
hexapodLocator::~hexapodLocator() {}

const double  PI  = 3.141592653;
const double  TAU = 2.0 * PI;

MStatus hexapodLocator::compute( const MPlug& plug, MDataBlock& data )
{ 
	return MS::kUnknownParameter;
}


void hexapodLocator::drawCordSys(M3dView & view, const MFloatMatrix & fmat, float scale) {

	MFloatPoint p(fmat[3][0],fmat[3][1],fmat[3][2]);

	MFloatPoint x = (MFloatPoint(scale,0.0f,0.0f) * fmat);
	MFloatPoint y = (MFloatPoint(0.0f,scale,0.0f) * fmat);
	MFloatPoint z = (MFloatPoint(0.0f,0.0f,scale) * fmat);
	//cerr << "hexapodLocator::drawAxes" << endl;
// cerr << "p: " << p << endl; 
// cerr << "x: " << p << endl; 
// cerr << "y: " << p << endl; 
// cerr << "z: " << p << endl; 
// cerr << "--------------------------" << endl;
	view.setDrawColor(MColor(MColor::kRGB,1.0f, 0.0f, 0.0f));	
	glBegin( GL_LINES );

	glVertex3f( p.x , p.y , p.z );

	glVertex3f(	x.x , x.y, x.z);

	glEnd();

	view.setDrawColor(MColor(MColor::kRGB,0.0f, 1.0f, 0.0f));	
	glBegin( GL_LINES );

	glVertex3f( p.x , p.y , p.z );

	glVertex3f(	y.x , y.y, y.z);

	glEnd();	

	view.setDrawColor(MColor(MColor::kRGB,0.0f, 0.0f, 1.0f));
	glBegin( GL_LINES );

	glVertex3f( p.x , p.y , p.z );

	glVertex3f(	z.x , z.y, z.z);

	glEnd();

}
void hexapodLocator::drawFoot( 
	M3dView & view,
	const MFloatMatrix & fmat,
	const MColor & color,
	const MVector & footPos,
	const MVector & home,
	const MVector & lastPlant,
	const MVector & nextPlant,
	const float2 & radiusMinMax,
	float radius,
	const MFloatVectorArray & circle,
	bool drawMinMaxRadius,
	bool drawRadius,
	bool drawHomeDots,
	bool drawStride,
	bool drawFootDots,
	float footDotSize
	)
{
	// draw the ws stuff

	 MFloatPoint home_ws(home);
	 MFloatPoint home_local = home_ws * fmat.inverse();
 
	glPushAttrib(GL_CURRENT_BIT);

	glPointSize(3);


	glBegin( GL_POINTS );
	if (drawStride) {
		//  blackish
		view.setDrawColor(MColor(MColor::kRGB,0.1f,0.1f,0.1f));
		glVertex3f( float(lastPlant.x) , float(lastPlant.y) , float(lastPlant.z) );

		//  whitish
		view.setDrawColor(MColor(MColor::kRGB,0.9f,0.9f,0.9f));
		glVertex3f( float(nextPlant.x) , float(nextPlant.y) , float(nextPlant.z) );
	}

	if (drawHomeDots) {
		// home_ws grey
		view.setDrawColor(MColor(MColor::kRGB,0.5f,0.5f,0.5f));
		glVertex3f( home_ws.x , home_ws.y , home_ws.z );
	}
	glEnd();

	if (drawFootDots) {
		glPointSize(footDotSize);
		glBegin( GL_POINTS );
		// footPos color
		view.setDrawColor(color);
		glVertex3f( float(footPos.x) , float(footPos.y) , float(footPos.z) );

		glEnd();
	}

	// draw circles and line from last to next
	view.setDrawColor(color);
	glBegin( GL_LINES );
	if (drawStride) {
		glVertex3f( float(lastPlant.x) , float(lastPlant.y) , float(lastPlant.z) );
		glVertex3f( float(nextPlant.x) , float(nextPlant.y) , float(nextPlant.z) );
	}
	if (drawMinMaxRadius || drawRadius){
		unsigned divs = circle.length();
		MFloatPointArray c(divs);
		if (drawMinMaxRadius ){
			// inner circle
			for (int i = 0; i < divs; ++i)
			{
				c[i] = (home_local + (circle[i] * radiusMinMax[0]))  * fmat;
			}
			for (int i = 0; i < divs; ++i)
			{
				int next = (i+1)%divs;
				glVertex3f( c[i].x , c[i].y , c[i].z );
				glVertex3f( c[next].x , c[next].y , c[next].z );
			}
			// outer circle
			for (int i = 0; i < divs; ++i)
			{
				c[i] = (home_local+(circle[i] * radiusMinMax[1])) * fmat;
			}
			for (int i = 0; i < divs; ++i)
			{
				int next = (i+1)%divs;
				glVertex3f( c[i].x , c[i].y , c[i].z );
				glVertex3f( c[next].x , c[next].y , c[next].z );
			}
		}
		if (drawRadius){
			// actual circle // bluish	
			view.setDrawColor(MColor(MColor::kRGB,0.4f,0.6f,0.8f) ); 
			for (int i = 0; i < divs; ++i)
			{
				c[i] = (home_local + (circle[i] * radius)) * fmat;
			}
			for (int i = 0; i < divs; ++i)
			{
				int next = (i+1)%divs;
				glVertex3f( c[i].x , c[i].y , c[i].z );
				glVertex3f( c[next].x , c[next].y , c[next].z );
			}
		}
	}

	glEnd();
	glPopAttrib();

}

void hexapodLocator::draw( M3dView & view, const MDagPath & path, 
	M3dView::DisplayStyle style,
	M3dView::DisplayStatus status )
{ 

	MStatus st;
	MObject thisNode = thisMObject();
 
	MObject d;
	MFnVectorArrayData fnV;
	MFnDoubleArrayData fnD;
 
	MPlug plug( thisNode, aPosition);
	st = plug.getValue(d);
	fnV.setObject(d);
	MVectorArray pos  = fnV.array();
	unsigned pl = pos.length();
	// DBG;

	MVectorArray phi = getVectorArray(thisNode, aPhi, pl, &st);erv;
	MVectorArray leftA = getVectorArray(thisNode, aLeftA, pl, &st);erv;
	MVectorArray leftB = getVectorArray(thisNode, aLeftB, pl, &st);erv;
	MVectorArray leftC = getVectorArray(thisNode, aLeftC, pl, &st);erv;
	MVectorArray rightA = getVectorArray(thisNode, aRightA, pl, &st);erv;
	MVectorArray rightB = getVectorArray(thisNode, aRightB, pl, &st);erv;
	MVectorArray rightC = getVectorArray(thisNode, aRightC, pl, &st);erv;
	MVectorArray homeLA = getVectorArray(thisNode, aHomeLA, pl, &st);erv;
	MVectorArray homeLB = getVectorArray(thisNode, aHomeLB, pl, &st);erv;
	MVectorArray homeLC = getVectorArray(thisNode, aHomeLC, pl, &st);erv;
	MVectorArray homeRA = getVectorArray(thisNode, aHomeRA, pl, &st);erv;
	MVectorArray homeRB = getVectorArray(thisNode, aHomeRB, pl, &st);erv;
	MVectorArray homeRC = getVectorArray(thisNode, aHomeRC, pl, &st);erv;
	MVectorArray lastPlantLA = getVectorArray(thisNode, aLastPlantLA, pl, &st);erv;
	MVectorArray lastPlantLB = getVectorArray(thisNode, aLastPlantLB, pl, &st);erv;
	MVectorArray lastPlantLC = getVectorArray(thisNode, aLastPlantLC, pl, &st);erv;
	MVectorArray lastPlantRA = getVectorArray(thisNode, aLastPlantRA, pl, &st);erv;
	MVectorArray lastPlantRB = getVectorArray(thisNode, aLastPlantRB, pl, &st);erv;
	MVectorArray lastPlantRC = getVectorArray(thisNode, aLastPlantRC, pl, &st);erv;
	MVectorArray nextPlantLA = getVectorArray(thisNode, aNextPlantLA, pl, &st);erv; 
	MVectorArray nextPlantLB = getVectorArray(thisNode, aNextPlantLB, pl, &st);erv; 
	MVectorArray nextPlantLC = getVectorArray(thisNode, aNextPlantLC, pl, &st);erv; 
	MVectorArray nextPlantRA = getVectorArray(thisNode, aNextPlantRA, pl, &st);erv; 
	MVectorArray nextPlantRB = getVectorArray(thisNode, aNextPlantRB, pl, &st);erv; 
	MVectorArray nextPlantRC = getVectorArray(thisNode, aNextPlantRC, pl, &st);erv; 
 

	MDoubleArray radiusLA = getDoubleArray(thisNode, aRadiusLA, pl, &st);erv; 
	MDoubleArray radiusLB = getDoubleArray(thisNode, aRadiusLB, pl, &st);erv; 
  MDoubleArray radiusLC = getDoubleArray(thisNode, aRadiusLC, pl, &st);erv; 
  MDoubleArray radiusRA = getDoubleArray(thisNode, aRadiusRA, pl, &st);erv; 
	MDoubleArray radiusRB = getDoubleArray(thisNode, aRadiusRB, pl, &st);erv; 
  MDoubleArray radiusRC = getDoubleArray(thisNode, aRadiusRC, pl, &st);erv; 
 
 	MDoubleArray scale = getDoubleArray(thisNode, aScale, pl, &st);erv; 
 

	float2 radiusMinMaxA;
	plug.setAttribute( aRadiusMinA );			
	plug.getValue( radiusMinMaxA[0]);
	plug.setAttribute( aRadiusMaxA );			
	plug.getValue( radiusMinMaxA[1]);
	// DBG;

	float2 radiusMinMaxB;
	plug.setAttribute( aRadiusMinB );			
	plug.getValue( radiusMinMaxB[0]);
	plug.setAttribute( aRadiusMaxB );			
	plug.getValue( radiusMinMaxB[1]);
	// DBG;

	float2 radiusMinMaxC;
	plug.setAttribute( aRadiusMinC );			
	plug.getValue( radiusMinMaxC[0]);
	plug.setAttribute( aRadiusMaxC );			
	plug.getValue( radiusMinMaxC[1]);
	// DBG;

 
	float3 cA;
	plug.setAttribute( aColorAR );			
	plug.getValue( cA[0]);
	plug.setAttribute( aColorAG );			
	plug.getValue( cA[1]);
	plug.setAttribute( aColorAB );			
	plug.getValue( cA[2]);
	MColor colorA(cA);	// DBG;

	float3 cB;
	plug.setAttribute( aColorBR );			
	plug.getValue( cB[0]);
	plug.setAttribute( aColorBG );			
	plug.getValue( cB[1]);
	plug.setAttribute( aColorBB );			
	plug.getValue( cB[2]);
	MColor colorB(cB);	// DBG;

	float3 cC;
	plug.setAttribute( aColorCR );			
	plug.getValue( cC[0]);
	plug.setAttribute( aColorCG );			
	plug.getValue( cC[1]);
	plug.setAttribute( aColorCB );			
	plug.getValue( cC[2]);
	MColor colorC(cC);
	// DBG;


	// DBG;
	bool drawAxes;
	plug.setAttribute( aDrawAxes);
	plug.getValue(drawAxes);
	float axesScale;
	plug.setAttribute( aAxesScale);
	plug.getValue(axesScale);
	bool drawMinMaxRadius;
	plug.setAttribute( aDrawMinMaxRadius);
	plug.getValue(drawMinMaxRadius);
	bool drawRadius;
	plug.setAttribute( aDrawRadius);
	plug.getValue(drawRadius);
	bool drawHomeDots;
	plug.setAttribute( aDrawHomeDots);
	plug.getValue(drawHomeDots);
	bool drawStride;
	plug.setAttribute( aDrawStride);
	plug.getValue(drawStride);
	bool drawFootDots;
	plug.setAttribute( aDrawFootDots);
	plug.getValue(drawFootDots);
	float footDotSize;
	plug.setAttribute( aFootDotSize);
	plug.getValue(footDotSize);
	

	// make a circle of radius 1 in xz
	unsigned divs = 16;	
	MFloatVectorArray circle(divs);
	float gap = float(TAU) / float(divs);
	for(unsigned j = 0; j < divs; ++j) { 
		circle[j] = MFloatVector(sin(gap * j),0.0f,cos(gap * j));
	}
 

  view.beginGL(); 
	for (int i = 0; i < pl; ++i)
	{
 
		MMatrix mat = mayaMath::matFromPhi(
			pos[i], 
			phi[i], 
			MVector(scale[i],scale[i],scale[i])
			);
		MFloatMatrix fmat(mat.matrix);

		drawFoot( 
			view, fmat, colorA, leftA[i], homeLA[i], 
			lastPlantLA[i], nextPlantLA[i], radiusMinMaxA, radiusLA[i], circle,
			drawMinMaxRadius, drawRadius, drawHomeDots, drawStride, drawFootDots, footDotSize
			);
 

		drawFoot( 
			view, fmat, colorB, leftB[i], homeLB[i], 
			lastPlantLB[i], nextPlantLB[i], radiusMinMaxB, radiusLB[i], circle,
			drawMinMaxRadius, drawRadius, drawHomeDots, drawStride, drawFootDots, footDotSize
			);


		drawFoot( 
			view, fmat, colorC, leftC[i], homeLC[i], 
			lastPlantLC[i], nextPlantLC[i], radiusMinMaxC, radiusLC[i], circle,
			drawMinMaxRadius, drawRadius, drawHomeDots, drawStride, drawFootDots, footDotSize
			);

		drawFoot( 
			view, fmat, colorA, rightA[i], homeRA[i], 
			lastPlantRA[i], nextPlantRA[i], radiusMinMaxA, radiusRA[i], circle,
			drawMinMaxRadius, drawRadius, drawHomeDots, drawStride, drawFootDots, footDotSize
			);
 

		drawFoot( 
			view, fmat, colorB, rightB[i], homeRB[i], 
			lastPlantRB[i], nextPlantRB[i], radiusMinMaxB, radiusRB[i], circle,
			drawMinMaxRadius, drawRadius, drawHomeDots, drawStride, drawFootDots, footDotSize
			);


		drawFoot( 
			view, fmat, colorC, rightC[i], homeRC[i], 
			lastPlantRC[i], nextPlantRC[i], radiusMinMaxC, radiusRC[i], circle,
			drawMinMaxRadius, drawRadius, drawHomeDots, drawStride, drawFootDots, footDotSize
			);




	}
	view.endGL();	
	//DBG;

}

bool hexapodLocator::isBounded() const
{ 
	return false;
}

void* hexapodLocator::creator()
{
	return new hexapodLocator();
}

MStatus hexapodLocator::initialize()
{ 
	MStatus	st;
	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	//MFnCompoundAttribute cAttr;
	//MFnEnumAttribute eAttr;

	aPosition = tAttr.create("position", "pos", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aPosition ); er;
	
	aPhi = tAttr.create("phi", "phi", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aPhi ); er;

	aScale = tAttr.create("scale", "scl", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aScale ); er;


	aLeftA = tAttr.create("leftA", "lta", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aLeftA); er;
	aLeftB = tAttr.create("leftB", "ltb", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aLeftB); er;
	aLeftC = tAttr.create("leftC", "ltc", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aLeftC); er;
	aRightA = tAttr.create("rightA", "rta", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aRightA); er;
	aRightB = tAttr.create("rightB", "rtb", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aRightB); er;
	aRightC = tAttr.create("rightC", "rtc", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aRightC); er;
	aHomeLA = tAttr.create("homeLA", "hla", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aHomeLA); er;
	aHomeLB = tAttr.create("homeLB", "hlb", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aHomeLB); er;
	aHomeLC = tAttr.create("homeLC", "hlc", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aHomeLC); er;
	aHomeRA = tAttr.create("homeRA", "hra", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aHomeRA); er;
	aHomeRB = tAttr.create("homeRB", "hrb", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aHomeRB); er;
	aHomeRC = tAttr.create("homeRC", "hrc", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aHomeRC); er;
	aLastPlantLA = tAttr.create("lastPlantLA", "lpla", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aLastPlantLA); er;
	aLastPlantLB = tAttr.create("lastPlantLB", "lplb", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aLastPlantLB); er;
	aLastPlantLC = tAttr.create("lastPlantLC", "lplc", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aLastPlantLC); er;
	aLastPlantRA = tAttr.create("lastPlantRA", "lpra", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aLastPlantRA); er;
	aLastPlantRB = tAttr.create("lastPlantRB", "lprb", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aLastPlantRB); er;
	aLastPlantRC = tAttr.create("lastPlantRC", "lprc", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aLastPlantRC); er;
	aNextPlantLA = tAttr.create("nextPlantLA", "npla", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aNextPlantLA); er; 
	aNextPlantLB = tAttr.create("nextPlantLB", "nplb", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aNextPlantLB); er; 
	aNextPlantLC = tAttr.create("nextPlantLC", "nplc", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aNextPlantLC); er; 
	aNextPlantRA = tAttr.create("nextPlantRA", "npra", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aNextPlantRA); er; 
	aNextPlantRB = tAttr.create("nextPlantRB", "nprb", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aNextPlantRB); er; 
	aNextPlantRC = tAttr.create("nextPlantRC", "nprc", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aNextPlantRC); er; 


	aRadiusLA= tAttr.create("radiusLA", "rdla", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aRadiusLA ); er;
	aRadiusLB= tAttr.create("radiusLB", "rdlb", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aRadiusLB ); er;
	aRadiusLC= tAttr.create("radiusLC", "rdlc", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aRadiusLC ); er;
	aRadiusRA= tAttr.create("radiusRA", "rdra", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aRadiusRA ); er;
	aRadiusRB= tAttr.create("radiusRB", "rdrb", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aRadiusRB ); er;
	aRadiusRC= tAttr.create("radiusRC", "rdrc", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aRadiusRC ); er;

	aRadiusMinA = nAttr.create( "radiusMinA", "rmna", MFnNumericData::kDouble);
	aRadiusMaxA = nAttr.create( "radiusMaxA", "rmxa", MFnNumericData::kDouble);
	aRadiusA = nAttr.create( "radiusA", "ra", aRadiusMinA, aRadiusMaxA);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	st = addAttribute(aRadiusA);

	aRadiusMinB = nAttr.create( "radiusMinB", "rmnb", MFnNumericData::kDouble);
	aRadiusMaxB = nAttr.create( "radiusMaxB", "rmxb", MFnNumericData::kDouble);
	aRadiusB = nAttr.create( "radiusB", "rb", aRadiusMinB, aRadiusMaxB);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	st = addAttribute(aRadiusB);

	aRadiusMinC = nAttr.create( "radiusMinC", "rmnc", MFnNumericData::kDouble);
	aRadiusMaxC = nAttr.create( "radiusMaxC", "rmxc", MFnNumericData::kDouble);
	aRadiusC = nAttr.create( "radiusC", "rc", aRadiusMinC, aRadiusMaxC);
	nAttr.setStorable(true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	st = addAttribute(aRadiusC);


	aColorAR = nAttr.create ("colorAR", "car", MFnNumericData::kFloat);
	aColorAG = nAttr.create ("colorAG", "cag", MFnNumericData::kFloat);
	aColorAB = nAttr.create ("colorAB", "cab", MFnNumericData::kFloat);
	aColorA = nAttr.create( "colorA", "ca", aColorAR, aColorAG, aColorAB);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(-1.0, 0.0, 0.0);
	nAttr.setUsedAsColor(true);
	addAttribute(aColorA);

	aColorBR = nAttr.create ("colorBR", "cbr", MFnNumericData::kFloat);
	aColorBG = nAttr.create ("colorBG", "cbg", MFnNumericData::kFloat);
	aColorBB = nAttr.create ("colorBB", "cbb", MFnNumericData::kFloat);
	aColorB = nAttr.create( "colorB", "cb", aColorBR, aColorBG, aColorBB);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(-1.0, 0.0, 0.0);
	nAttr.setUsedAsColor(true);
	addAttribute(aColorB);

	aColorCR = nAttr.create ("colorCR", "ccr", MFnNumericData::kFloat);
	aColorCG = nAttr.create ("colorCG", "ccg", MFnNumericData::kFloat);
	aColorCB = nAttr.create ("colorCB", "ccb", MFnNumericData::kFloat);
	aColorC = nAttr.create( "colorC", "cc", aColorCR, aColorCG, aColorCB);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(-1.0, 0.0, 0.0);
	nAttr.setUsedAsColor(true);
	addAttribute(aColorC);

	aDrawAxes = nAttr.create("drawAxes", "dax", MFnNumericData::kBoolean);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	addAttribute(aDrawAxes);

	aAxesScale = nAttr.create("axesScale", "axs", MFnNumericData::kFloat);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	addAttribute(aAxesScale);

	aDrawMinMaxRadius = nAttr.create("drawMinMaxRadius", "dmmr" , MFnNumericData::kBoolean);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	addAttribute(aDrawMinMaxRadius);

	aDrawRadius = nAttr.create("drawRadius", "dr" , MFnNumericData::kBoolean);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	addAttribute(aDrawRadius);

	aDrawHomeDots = nAttr.create("drawHomeDots", "dhd" , MFnNumericData::kBoolean);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	addAttribute(aDrawHomeDots);

	aDrawStride = nAttr.create("drawStride", "dst" , MFnNumericData::kBoolean);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	addAttribute(aDrawStride);

	aDrawFootDots = nAttr.create("drawFootDots", "dfd" , MFnNumericData::kBoolean);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	addAttribute(aDrawFootDots);

	aFootDotSize = nAttr.create("footDotSize", "fds" , MFnNumericData::kFloat);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	addAttribute(aFootDotSize);




	return MS::kSuccess;
}



