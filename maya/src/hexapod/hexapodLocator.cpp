/***************************************************************************
hexapodLocator.cpp  -  description
-------------------
    begin                : Dec 2009
    copyright            : (C) 2009 by Julian Mann
    email                : julian.mann@gmail.com

    Draw openGL lines showing coord sys
	***************************************************************************/


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
#include "jMayaIds.h"

const double deg_to_rad = 0.01745329238;
MObject hexapodLocator::aPosition;	
MObject hexapodLocator::aPhi;
MObject hexapodLocator::aScale; // home and radius values are scaled

MObject hexapodLocator::aNextPlant1; 
MObject hexapodLocator::aNextPlant2; 

MObject hexapodLocator::aLastPlant1; 
MObject hexapodLocator::aLastPlant2; 

MObject hexapodLocator::aFootPos1;
MObject hexapodLocator::aFootPos2;

MObject hexapodLocator::aHomePos1;
MObject hexapodLocator::aHomePos2;

// MObject hexapodLocator::aHomeOffset1X;
// MObject hexapodLocator::aHomeOffset1Y;
// MObject hexapodLocator::aHomeOffset1Z;
// MObject hexapodLocator::aHomeOffset1;

// MObject hexapodLocator::aHomeOffset2X;
// MObject hexapodLocator::aHomeOffset2Y;
// MObject hexapodLocator::aHomeOffset2Z;
// MObject hexapodLocator::aHomeOffset2;


MObject hexapodLocator::aRadius1Min; 
MObject hexapodLocator::aRadius1Max;  
MObject hexapodLocator::aRadius1;  

MObject hexapodLocator::aRadius2Min; 
MObject hexapodLocator::aRadius2Max; 
MObject hexapodLocator::aRadius2; 


MObject hexapodLocator::aActualRadius1;  
MObject hexapodLocator::aActualRadius2;  

MObject hexapodLocator::aRotation1;  
MObject hexapodLocator::aRotation2;  


MObject hexapodLocator::aColor1R;
MObject hexapodLocator::aColor1G;
MObject hexapodLocator::aColor1B;
MObject hexapodLocator::aColor1;

MObject hexapodLocator::aColor2R;
MObject hexapodLocator::aColor2G;
MObject hexapodLocator::aColor2B;
MObject hexapodLocator::aColor2;

MObject hexapodLocator::aDrawAxes;
MObject hexapodLocator::aDrawRotationAxes;

MObject hexapodLocator::aAxesScale;
MObject hexapodLocator::aDrawMinMaxRadius;
MObject hexapodLocator::aDrawActualRadius;
MObject hexapodLocator::aDrawHomeDots;
MObject hexapodLocator::aDrawhexapod;
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
	const MFloatVector & color,
	const MFloatVector & lastPlant,
	const MFloatVector & nextPlant,
	const MFloatVector & footPos,
	const MFloatVector & homeOffset,
	const MVector & rotation,
	float innerRadius,
	float outerRadius,
	float actualRadius,
	const MFloatVectorArray & circle,
	bool drawRotationAxes,
	float axesScale,
	bool drawMinMaxRadius,
	bool drawActualRadius,
	bool drawHomeDots,
	bool drawhexapod,
	bool drawFootDots,
	float footDotSize
)
{
	// draw the ws stuff

	MFloatPoint home_ws = MFloatPoint(homeOffset) * fmat;

	glPushAttrib(GL_CURRENT_BIT);
		
	glPointSize(3);
		
	glBegin( GL_POINTS );

	if (drawhexapod) {
		// lastPlant blackish
		view.setDrawColor(MColor(MColor::kRGB,0.1f,0.1f,0.1f));
		glVertex3f( lastPlant.x , lastPlant.y , lastPlant.z );
	
		// nextPlant whitish
		view.setDrawColor(MColor(MColor::kRGB,0.9f,0.9f,0.9f));
		glVertex3f( nextPlant.x , nextPlant.y , nextPlant.z );
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
		view.setDrawColor(MColor(MColor::kRGB,color.x,color.y,color.z));
		glVertex3f( footPos.x , footPos.y , footPos.z );
			
		glEnd();
	}

	// draw circles and line from last to next
	view.setDrawColor(MColor(MColor::kRGB,color.x,color.y,color.z));
	glBegin( GL_LINES );
	if (drawhexapod) {
		glVertex3f( lastPlant.x , lastPlant.y , lastPlant.z );
		glVertex3f( nextPlant.x , nextPlant.y , nextPlant.z );
	}
	if (drawMinMaxRadius || drawActualRadius){
		unsigned divs = circle.length();
		MFloatVectorArray c(divs);
		if (drawMinMaxRadius ){
			// inner circle
			for (int i = 0; i < divs; ++i)
			{
				c[i] = MFloatVector(MFloatPoint((circle[i] * innerRadius) + homeOffset) * fmat);
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
				c[i] = MFloatVector(MFloatPoint((circle[i] * outerRadius) + homeOffset) * fmat);
			}
			for (int i = 0; i < divs; ++i)
			{
				int next = (i+1)%divs;
				glVertex3f( c[i].x , c[i].y , c[i].z );
				glVertex3f( c[next].x , c[next].y , c[next].z );
			}
		}
		if (drawActualRadius){
			// actual circle // bluish	
			view.setDrawColor(MColor(MColor::kRGB,0.4f,0.6f,0.8f) ); 
			for (int i = 0; i < divs; ++i)
			{
				c[i] = MFloatVector(MFloatPoint((circle[i] * actualRadius) + homeOffset) * fmat);
			}
			for (int i = 0; i < divs; ++i)
			{
				int next = (i+1)%divs;
				glVertex3f( c[i].x , c[i].y , c[i].z );
				glVertex3f( c[next].x , c[next].y , c[next].z );
			}
		}
		if (drawRotationAxes) {
			MTransformationMatrix mtmat;

			MTransformationMatrix::RotationOrder ord = MTransformationMatrix::kXYZ ;
	
			double3 rotValue;
			rotValue[0] = rotation.x * deg_to_rad;
			rotValue[1] = rotation.y * deg_to_rad;
			rotValue[2] = rotation.z * deg_to_rad;
			//cerr << "rotValue: " << MVector(rotValue[0],rotValue[1],rotValue[2]) << endl;
			mtmat.setRotation( rotValue, ord );
			mtmat.setTranslation( MVector(footPos), MSpace::kWorld);

			MMatrix footMat = mtmat.asMatrix();
			MFloatMatrix fmat(footMat.matrix);

			drawCordSys(view, fmat,  axesScale);
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
	// DBG;
	MObject d;
	MFnVectorArrayData fnV;
	MFnDoubleArrayData fnD;
	// DBG;

	MPlug plug( thisNode, aPosition);
	st = plug.getValue(d);
	fnV.setObject(d);
	MVectorArray pos  = fnV.array();
	unsigned pl = pos.length();
	// DBG;

	plug.setAttribute(aPhi);
	st = plug.getValue(d);
	fnV.setObject(d);
	if (fnV.length() != pl) return;
	MVectorArray phi = fnV.array();
	// DBG;

	st = plug.setAttribute(aLastPlant1);er;
	st = plug.getValue(d);
	st = fnV.setObject(d); 
	if (fnV.length() != pl) return;
	MVectorArray lastPlant1 = fnV.array();
	// DBG;

	plug.setAttribute(aLastPlant2);
	st = plug.getValue(d);
	fnV.setObject(d);
	if (fnV.length() != pl) return;
	MVectorArray lastPlant2 = fnV.array();
	// DBG;

	plug.setAttribute(aNextPlant1);
	st = plug.getValue(d);
	fnV.setObject(d);
	if (fnV.length() != pl) return;
	MVectorArray nextPlant1 = fnV.array();
	// DBG;

	plug.setAttribute(aNextPlant2);
	st = plug.getValue(d);
	fnV.setObject(d);
	if (fnV.length() != pl) return;
	MVectorArray nextPlant2 = fnV.array();
	// DBG;

	plug.setAttribute(aFootPos1);
	st = plug.getValue(d);
	fnV.setObject(d);
	if (fnV.length() != pl) return;
	MVectorArray footPos1 = fnV.array();
	// DBG;

	plug.setAttribute(aFootPos2);
	st = plug.getValue(d);
	fnV.setObject(d);
	if (fnV.length() != pl) return;
	MVectorArray footPos2 = fnV.array();
	// DBG;

	plug.setAttribute(aHomePos1);
	st = plug.getValue(d);
	fnV.setObject(d);
	if (fnV.length() != pl) return;
	MVectorArray homePos1 = fnV.array();
	// DBG;

	plug.setAttribute(aHomePos2);
	st = plug.getValue(d);
	fnV.setObject(d);
	if (fnV.length() != pl) return;
	MVectorArray homePos2 = fnV.array();
	// DBG;

	plug.setAttribute(aRotation1);
	st = plug.getValue(d);
	fnV.setObject(d);
	if (fnV.length() != pl) return;
	MVectorArray rotation1 = fnV.array();
	// DBG;

	plug.setAttribute(aRotation2);
	st = plug.getValue(d);
	fnV.setObject(d);
	if (fnV.length() != pl) return;
	MVectorArray rotation2 = fnV.array();
	// DBG;

	plug.setAttribute(aActualRadius1);
	st = plug.getValue(d);
	fnD.setObject(d);
	if (fnD.length() != pl) return;
	MDoubleArray actualRadius1 = fnD.array();
	// DBG;

	plug.setAttribute(aActualRadius2);
	st = plug.getValue(d);
	fnD.setObject(d);
	if (fnD.length() != pl) return;
	MDoubleArray actualRadius2 = fnD.array();
	// DBG;

	plug.setAttribute(aScale);
	st = plug.getValue(d);
	fnD.setObject(d);
	if (fnD.length() != pl) return;
	MDoubleArray scale = fnD.array();
	// DBG;

	// float3 homeOffset1;
	// plug.setAttribute( aHomeOffset1X );			
	// plug.getValue( homeOffset1[0]);
	// plug.setAttribute( aHomeOffset1Y );			
	// plug.getValue( homeOffset1[1]);
	// plug.setAttribute( aHomeOffset1Z );			
	// plug.getValue( homeOffset1[2]);
	// // // DBG;

	// float3 homeOffset2;
	// plug.setAttribute( aHomeOffset2X );			
	// plug.getValue( homeOffset2[0]);
	// plug.setAttribute( aHomeOffset2Y );			
	// plug.getValue( homeOffset2[1]);
	// plug.setAttribute( aHomeOffset2Z );			
	// plug.getValue( homeOffset2[2]);
	// // DBG;

	float2 radius1MinMax;
	plug.setAttribute( aRadius1Min );			
	plug.getValue( radius1MinMax[0]);
	plug.setAttribute( aRadius1Max );			
	plug.getValue( radius1MinMax[1]);
	// DBG;

	float2 radius2MinMax;
	plug.setAttribute( aRadius2Min );			
	plug.getValue( radius2MinMax[0]);
	plug.setAttribute( aRadius2Max );			
	plug.getValue( radius2MinMax[1]);
	// DBG;

	float3 color1;
	plug.setAttribute( aColor1R );			
	plug.getValue( color1[0]);
	plug.setAttribute( aColor1G );			
	plug.getValue( color1[1]);
	plug.setAttribute( aColor1B );			
	plug.getValue( color1[2]);
	// DBG;

	float3 color2;
	plug.setAttribute( aColor2R );			
	plug.getValue( color2[0]);
	plug.setAttribute( aColor2G );			
	plug.getValue( color2[1]);
	plug.setAttribute( aColor2B );			
	plug.getValue( color2[2]);

	// DBG;
	bool drawAxes;
	plug.setAttribute( aDrawAxes);
	plug.getValue(drawAxes);
	bool drawRotationAxes;
	plug.setAttribute( aDrawRotationAxes);
	plug.getValue(drawRotationAxes);
	float axesScale;
	plug.setAttribute( aAxesScale);
	plug.getValue(axesScale);
	bool drawMinMaxRadius;
	plug.setAttribute( aDrawMinMaxRadius);
	plug.getValue(drawMinMaxRadius);
	bool drawActualRadius;
	plug.setAttribute( aDrawActualRadius);
	plug.getValue(drawActualRadius);
	bool drawHomeDots;
	plug.setAttribute( aDrawHomeDots);
	plug.getValue(drawHomeDots);
	bool drawhexapod;
	plug.setAttribute( aDrawhexapod);
	plug.getValue(drawhexapod);
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
	for(unsigned j = 0; j < divs; ++j) { circle[j] = MFloatVector(sin(gap * j),0.0f,cos(gap * j));}
	//DBG;
	// cerr << "PL" << pl << endl;
	view.beginGL(); 
	for (int i = 0; i < pl; ++i)
	{
		// cerr << "pos[i] " << pos[i] << endl;
		// cerr << "phi[i] " << phi[i] << endl;
		// cerr << "scale[i] " << scale[i] << endl;

		MMatrix mat = mayaMath::matFromPhi(
			pos[i], 
			phi[i], 
			MVector(scale[i],scale[i],scale[i])
		);
		MFloatMatrix fmat(mat.matrix);

		// need these home positions in local space
		MFloatVector homeOffset1 = MFloatVector(MFloatPoint(homePos1[i]) * fmat.inverse());
		MFloatVector homeOffset2 = MFloatVector(MFloatPoint(homePos2[i]) * fmat.inverse());
		if (drawAxes){
			drawCordSys(view, fmat, axesScale);
		}


		drawFoot( 
			view,
			fmat,
			MFloatVector(color1),
			MFloatVector(lastPlant1[i]), MFloatVector(nextPlant1[i]), MFloatVector(footPos1[i]),
			homeOffset1, 
			rotation1[i],
			radius1MinMax[0], radius1MinMax[1], 
			float(actualRadius1[i]),
			circle,
			drawRotationAxes,
			axesScale,
			drawMinMaxRadius,
			drawActualRadius,
			drawHomeDots,
			drawhexapod,
			drawFootDots,
			footDotSize
		);

		drawFoot( 
			view,
			fmat,
			MFloatVector(color2),
			MFloatVector(lastPlant2[i]), MFloatVector(nextPlant2[i]), MFloatVector(footPos2[i]),
			homeOffset2, 
			rotation2[i],
			radius2MinMax[0], radius2MinMax[1], 
			float(actualRadius2[i]),
			circle,
			drawRotationAxes,
			axesScale,
			drawMinMaxRadius,
			drawActualRadius,
			drawHomeDots,
			drawhexapod,
			drawFootDots,
			footDotSize
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

	aScale = tAttr.create("scales", "scl", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aScale ); er;

	aLastPlant1 = tAttr.create("lastPlant1", "lp1", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aLastPlant1 ); er;

	aLastPlant2 = tAttr.create("lastPlant2", "lp2", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aLastPlant2 ); er;

	aNextPlant1 = tAttr.create("nextPlant1", "np1", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aNextPlant1 ); er;

	aNextPlant2 = tAttr.create("nextPlant2", "np2", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aNextPlant2 ); er;

	aFootPos1 = tAttr.create("footPos1", "fp1", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aFootPos1 ); er;

	aFootPos2 = tAttr.create("footPos2", "fp2", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aFootPos2 ); er;

	aActualRadius1 = tAttr.create("actualRadius1", "ar1", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aActualRadius1 ); er;

	aActualRadius2 = tAttr.create("actualRadius2", "ar2", MFnData::kDoubleArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aActualRadius2 ); er;

	aHomePos1 = tAttr.create("homePos1", "hp1", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aHomePos1 ); er;

	aHomePos2 = tAttr.create("homePos2", "hp2", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aHomePos2 ); er;

	aRotation1 = tAttr.create("rotation1", "ro1", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aRotation1 ); er;

	aRotation2 = tAttr.create("rotation2", "ro2", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	st = addAttribute( aRotation2 ); er;



	// aHomeOffset1X = nAttr.create ("homeOffset1X", "ho1x", MFnNumericData::kFloat);
	// aHomeOffset1Y = nAttr.create ("homeOffset1Y", "ho1y", MFnNumericData::kFloat);
	// aHomeOffset1Z = nAttr.create ("homeOffset1Z", "ho1z", MFnNumericData::kFloat);
	// aHomeOffset1 = nAttr.create( "homeOffset1", "ho1", aHomeOffset1X, aHomeOffset1Y, aHomeOffset1Z);
	// nAttr.setStorable (true);
	// nAttr.setWritable(true);
	// nAttr.setKeyable(true);
	// nAttr.setDefault(-1.0, 0.0, 0.0);
	// addAttribute(aHomeOffset1);
	

	// aHomeOffset2X = nAttr.create ("homeOffset2X", "ho2x", MFnNumericData::kFloat);
	// aHomeOffset2Y = nAttr.create ("homeOffset2Y", "ho2y", MFnNumericData::kFloat);
	// aHomeOffset2Z = nAttr.create ("homeOffset2Z", "ho2z", MFnNumericData::kFloat);
	// aHomeOffset2 = nAttr.create( "homeOffset2", "ho2", aHomeOffset2X, aHomeOffset2Y, aHomeOffset2Z);
	// nAttr.setStorable (true);
	// nAttr.setWritable(true);
	// nAttr.setKeyable(true);
	// nAttr.setDefault(1.0, 0.0, 0.0);
	// addAttribute(aHomeOffset2);



	aRadius1Min = nAttr.create( "radius1Min", "r1mn", MFnNumericData::kFloat);
	aRadius1Max = nAttr.create( "radius1Max", "r1mx", MFnNumericData::kFloat);
	aRadius1 = nAttr.create( "radius1", "r1", aRadius1Min, aRadius1Max);
	nAttr.setStorable(false);
	nAttr.setReadable(true);
	st = addAttribute(aRadius1);

	aRadius2Min = nAttr.create( "radius2Min", "r2mn", MFnNumericData::kFloat);
	aRadius2Max = nAttr.create( "radius2Max", "r2mx", MFnNumericData::kFloat);
	aRadius2 = nAttr.create( "radius2", "r2", aRadius2Min, aRadius2Max);
	nAttr.setStorable(false);
	nAttr.setReadable(true);
	st = addAttribute(aRadius2);


	aColor1R = nAttr.create ("color1R", "c1r", MFnNumericData::kFloat);
	aColor1G = nAttr.create ("color1G", "c1g", MFnNumericData::kFloat);
	aColor1B = nAttr.create ("color1B", "c1b", MFnNumericData::kFloat);
	aColor1 = nAttr.create( "color1", "c1", aColor1R, aColor1G, aColor1B);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(-1.0, 0.0, 0.0);
	nAttr.setUsedAsColor(true);
	addAttribute(aColor1);
	

	aColor2R = nAttr.create ("color2R", "c2r", MFnNumericData::kFloat);
	aColor2G = nAttr.create ("color2G", "c2g", MFnNumericData::kFloat);
	aColor2B = nAttr.create ("color2B", "c2b", MFnNumericData::kFloat);
	aColor2 = nAttr.create( "color2", "c2", aColor2R, aColor2G, aColor2B);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(1.0, 0.0, 0.0);
	nAttr.setUsedAsColor(true);
	addAttribute(aColor2);


	aDrawAxes = nAttr.create("drawAxes", "dax", MFnNumericData::kBoolean);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	addAttribute(aDrawAxes);

	aDrawRotationAxes = nAttr.create("drawRotationAxes", "drax", MFnNumericData::kBoolean);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	addAttribute(aDrawRotationAxes);

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

	aDrawActualRadius = nAttr.create("drawActualRadius", "dar" , MFnNumericData::kBoolean);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	addAttribute(aDrawActualRadius);

	aDrawHomeDots = nAttr.create("drawHomeDots", "dhd" , MFnNumericData::kBoolean);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	addAttribute(aDrawHomeDots);

	aDrawhexapod = nAttr.create("drawhexapod", "dst" , MFnNumericData::kBoolean);
	nAttr.setStorable (true);
	nAttr.setWritable(true);
	nAttr.setKeyable(true);
	addAttribute(aDrawhexapod);

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



