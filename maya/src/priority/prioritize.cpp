/*
*  prioritize.cpp
*  animal
*
*  Created by Julian Mann on 30/07/2006.
*  Copyright 2006 hooly|mama. All rights reserved.
*
*/
#include <maya/MFnDependencyNode.h>
#include <maya/MGlobal.h>
#include <maya/MFnUnitAttribute.h>

#include <maya/MAnimControl.h> 

#include <maya/MArrayDataBuilder.h>
#include "prioritize.h"
#include "jMayaIds.h"

#define DRAW_OFF	0
#define DRAW_BARS	1
#define DRAW_GLYPH	2
#define DRAW_BOTH	3


MTypeId prioritize::id( k_prioritize );

MObject prioritize::aPositions;	
MObject prioritize::aGlyphScale;	

MObject prioritize::aInForce;	
MObject prioritize::aDrawColor;
MObject prioritize::aDrawColorR;
MObject prioritize::aDrawColorG;
MObject prioritize::aDrawColorB;
MObject prioritize::aActive;
// MObject prioritize::aContainerContribution;
MObject prioritize::aMagnitude;


MObject prioritize::aInData;				// compound (multi)
MObject prioritize::aMaxForce;
MObject prioritize::aEnableGraph;		// cache and do drawing


MObject prioritize::aSampleBy;	
MObject prioritize::aHeight;	
MObject prioritize::aWidth;		
MObject prioritize::aXOffset;	
MObject prioritize::aYOffset;	

MObject prioritize::aCurrentTime;	

MObject prioritize::aOutForce; 

prioritize::prioritize() {
	m_lastTimeIEvaluated = MAnimControl::currentTime();
	m_drawCache = 0;
	drawState = 0;
}

prioritize::~prioritize() {
	if (m_drawCache) delete m_drawCache;
	m_drawCache = 0;
}

MStatus prioritize::connectionBroken ( const MPlug & plug, const MPlug & otherPlug, bool asSrc ){

	MStatus st;

	MString method("prioritize::connectionBroken");
	unsigned int el;

	if (plug == aInForce) {
		MDataBlock data = forceCache();
		MArrayDataHandle hInData = data.inputArrayValue( aInData, &st ); ert;
		MArrayDataBuilder bInData = hInData.builder(&st);ert;
		el = plug.parent().logicalIndex();
		st = bInData.removeElement(el);ert;
	} 
	return MS::kSuccess;
}

MStatus prioritize::compute( const MPlug& plug, MDataBlock& data )
{ 


// cerr << "prioritiz::compute" << endl;


	MStatus st;
	if( plug != aOutForce)  return MS::kUnknownParameter;




	short doDraw = data.inputValue(aEnableGraph).asShort();
	if (m_drawCache){delete m_drawCache;m_drawCache = 0;}


	MTime cT =  data.inputValue( aCurrentTime).asTime();
	MTime dT = cT - m_lastTimeIEvaluated;
	m_lastTimeIEvaluated = cT;

	MVectorArray outForce;

	double dt = dT.as( MTime::kSeconds );
	if (dt <= 0.0) {
		// cerr << "priority clean" << endl;
		MDataHandle hOut =  data.outputValue( aOutForce, &st); er;
		MFnVectorArrayData fnOutput;
		MObject dOutput = fnOutput.create( outForce, &st );er;
		hOut.set( dOutput);
		data.setClean( plug );
		return( MS::kSuccess );

 
	}




	if (doDraw ) m_drawCache = new VEC_LIST;
	m_colorArray.clear();

	
	double maxContrib = data.inputValue( aMaxForce).asDouble();
	if (maxContrib < 0.0) maxContrib = 0.0;

	double magnitude = data.inputValue( aMagnitude).asDouble();
	
	MArrayDataHandle hInData = data.inputArrayValue( aInData, &st ); 

	MDoubleArray previousMag;
	MIntArray elementFull;

	bool firstIter = true;
	unsigned int firstLength = 0;	

	unsigned n = hInData.elementCount();

	MString nodeName = MFnDependencyNode(thisMObject()).name();

	

	MDataHandle hInPos = data.inputValue( aPositions, &st ); er;
	MObject dInPos= hInPos.data();
	MFnVectorArrayData fnInPos( dInPos,  &st ); er;			
	m_positions.copy(fnInPos.array( &st ));	er;		

	if (magnitude != 0.0) {
		for(unsigned i = 0;i < n; i++, hInData.next()) {

			MDataHandle hInDataCompound = hInData.inputValue(&st ); er;			if (st.error()) continue;
			bool active = hInDataCompound.child(aActive).asBool();
		//	float contrib = hInDataCompound.child(aContainerContribution).asFloat();
			MDataHandle hInForce = hInDataCompound.child( aInForce );
			MObject dInForce = hInForce.data();
			MFnVectorArrayData fnInForce( dInForce,  &st ); 				if (st.error()) continue;
			const MVectorArray & inForce = fnInForce.array( &st );				if (st.error()) continue;

			MDataHandle hDrawColor = hInDataCompound.child( aDrawColor );

			unsigned length = inForce.length();
			unsigned firstInputId=0;
			// ignore inputs that have been specified as inactive. But also ignore inputs with zero length.
			if (active && length) {
			//	cerr << "m_positions " << m_positions.length() << "length " << length << endl;

				if (firstIter) {
					firstInputId = i;
					firstLength = length;
					firstIter = false;
					outForce = MVectorArray(length, MVector::zero);
					previousMag = MDoubleArray(length, 0.0);
					elementFull = MIntArray(length, 0);
				} 

				if (length != firstLength ) {

					// MGlobal::displayWarning("Array lengths in prioritize node do not match");
						// MString msg("Array order:"+i+" doesn't match -- length:" + length);
					MString msg(nodeName + " input:");
					msg = (msg + i);
					msg = (msg + " length (");
					msg = (msg + length);
					msg = (msg + ") doesn't match input: ");
					msg = (msg + firstInputId);
					msg = (msg + " length (");
					msg = (msg + firstLength);
					msg = (msg + ")");
					MGlobal::displayWarning(msg);

					continue;
				}

				MVectorArray clippedVector;
				if (m_drawCache) clippedVector = MVectorArray(length, MVector::zero);

				for (unsigned j= 0; j < length; j++) {

					int & full = elementFull[j];					
					if (!full) {

						MVector & out = outForce[j];	
						double & prev = previousMag[j];					
						double tmpMag = inForce[j].length();


						if (tmpMag) {
							double remaining = (maxContrib - prev) ;
							if (remaining > tmpMag ) { // space left in container is enough for new force
								if (m_drawCache) clippedVector[j] = inForce[j];

								out += inForce[j];
								prev += tmpMag;
								full = 0;
							} else { // space left in container is not enough for new force
								MVector tv = (inForce[j].normal() * remaining);
								if (m_drawCache) clippedVector[j] = tv ;
								out += tv ;

								prev += remaining;
								full = 1;
							}
						}
					}
				}
				if (m_drawCache) m_drawCache->push_back(clippedVector);					
				m_colorArray.append(MColor(hDrawColor.asFloat3()));

			}
		}

		// mult the result
		if (magnitude !=1.0) {
			unsigned flen = outForce.length();
			for (unsigned j = 0;j<  flen ; j++ ) {
				outForce[j] = outForce[j]  * magnitude;
			}
		}
		
	}

	MDataHandle hOut =  data.outputValue( aOutForce, &st); er;
	MFnVectorArrayData fnOutput;
	MObject dOutput = fnOutput.create( outForce, &st );er;
	hOut.set( dOutput);
	data.setClean( plug );
	return( MS::kSuccess );
}

void prioritize::draw( M3dView & view, const MDagPath & path, 
	M3dView::DisplayStyle style,
	M3dView::DisplayStatus status )
{ 

	MStatus st;



	MObject thisNode = thisMObject();




	MPlug enPlug( thisNode, aEnableGraph );
	short doDrawing; 
	st = enPlug.getValue (doDrawing  );

	if (doDrawing == DRAW_OFF) {
		return;
	}
	if ( ! m_drawCache) return;

	// get values from plugs
	/////////////////////////////////////////
// MVectorArray positions;
// if (doDrawing > 1) {
// 	MPlug posPlug( thisNode, aPositions );
// 	MObject dPositions;
// 	st = posPlug.getValue (dPositions ); er;
// 	if (dPositions.hasFn(MFn::kVectorArrayData)){
// 		MFnVectorArrayData fnPositions(dPositions);
// 		positions = fnPositions.array();
// 	}
// }

	MPlug gPlug( thisNode, aGlyphScale );
	double glyphScale; 
	st = gPlug.getValue (glyphScale ); er;

	MPlug mPlug( thisNode, aMaxForce );
	double maxForce; 
	st = mPlug.getValue (maxForce ); er;

	MPlug sPlug( thisNode, aSampleBy );
	short sampleBy; 
	st = sPlug.getValue (sampleBy ); er;

	MPlug hPlug( thisNode, aHeight );
	double height; 
	st = hPlug.getValue (height ); er;

	MPlug wPlug( thisNode, aWidth );
	double width; 
	st = wPlug.getValue (width ); er;

	MPlug xPlug( thisNode, aXOffset );
	double xOffset; 
	st = xPlug.getValue (xOffset ); er;

	MPlug yPlug( thisNode, aYOffset );
	double yOffset; 
	st = yPlug.getValue (yOffset ); er;
	/////////////////////////////////////////



	if (sampleBy < 1)  sampleBy = 1;
	// unsigned int i, j;

	VEC_LIST::iterator iter = m_drawCache->begin();
	unsigned int count = 0;


	bool firstGoRound = true;

	int pw = view.portWidth();
	int ph = view.portHeight();

	int x0, x1, y0, y1;

	//int yStart; 
	//int yEnd; 

	x0 = int(xOffset * pw);
	y0 = int(yOffset * ph);
	x1 = int((xOffset + width) * pw);
	y1 = int((yOffset + height) * ph);


	if (!((x1>x0)&&(y1> y0))) return;

	view.beginGL(); 


	glPushAttrib(GL_CURRENT_BIT);
	glClear(GL_CURRENT_BIT);


	if ((doDrawing == DRAW_BARS)|| (doDrawing == DRAW_BOTH)) {


		// setup projection
		////////////////////////////
		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
		glLoadIdentity();
 
		gluOrtho2D(
			0.0, (GLdouble) pw,
			0.0, (GLdouble) ph
			);
		glMatrixMode( GL_MODELVIEW );
		glShadeModel(GL_SMOOTH);
		////////////////////////////


		// draw the outline
		////////////////////////////
		glBegin( GL_LINE_LOOP );
		glVertex2i(x0, y0);
		glVertex2i(x0, y1);
		glVertex2i(x1, y1);
		glVertex2i(x1, y0); 
		glEnd();
		////////////////////////////


		// draw maxVal
		MPoint posTopRight, farClp; 
		view.viewToWorld( x1, y1,  posTopRight, farClp ) ;
		view.drawText(MFnDependencyNode(thisNode).name(), posTopRight,  M3dView::kRight);




		MIntArray xPositions;
		MIntArray yPositions;

		unsigned int numSamples = 1;

		// draw bar chart
		////////////////////////////
		glBegin(GL_LINES);
		while (iter != m_drawCache->end()) {

			view.setDrawColor(m_colorArray[count]);
			MVectorArray & curVectors = *iter;
			unsigned int len = curVectors.length();
			if (len) {


				if (firstGoRound ){
					if ((pw * width) < (len / sampleBy)) 
						sampleBy = short((len / (pw * width)) + 1);

					numSamples =  short(len / sampleBy );

					xPositions.setLength(numSamples);
					double range = width * pw;
					double spacing = 1.0 / numSamples;
					for (unsigned i = 0; i< numSamples; i++){
						xPositions[i] = int((xOffset*pw) + (  range * i * spacing ));
					}
					yPositions = MIntArray(numSamples, int(yOffset * ph));
					firstGoRound = false;
				}

				double graphHeight =  ph * height ;

				double normalizer = 	graphHeight	/ maxForce;
				int tmp ;

				for (unsigned i=0,   j=0; ((i<len) && (j < numSamples)) ;i+=sampleBy,j++) {
					int screenMag =  int(curVectors[i].length() * normalizer) ;

					glVertex2i(xPositions[j],yPositions[j]);	
					tmp = yPositions[j] + screenMag;
					glVertex2i(xPositions[j],tmp);	

					yPositions[j] = tmp;

				}
			}			
			count++;
			iter++;
		}

		glEnd();
		////////////////////////////


		glMatrixMode( GL_PROJECTION );
		glPopMatrix();
		glMatrixMode( GL_MODELVIEW );
		glPopMatrix();
	}	
	// draw posGlyphs
	////////////////////////////
	if ((doDrawing == DRAW_GLYPH) || (doDrawing == DRAW_BOTH)) {

		unsigned int pl = m_positions.length();
		if (pl) {
			iter = m_drawCache->begin(); 
			count = 0;
			glBegin(GL_LINES);
			while (iter != m_drawCache->end()) {
				MVectorArray & curVectors = *iter;
				unsigned int len = curVectors.length();
				if (len == pl) {
					view.setDrawColor(m_colorArray[count]);
					for (unsigned i=0; i<len ;i++) {


						glVertex3f(float(m_positions[i].x) ,float(m_positions[i].y) ,float(m_positions[i].z) );
						glVertex3f(
							(float((curVectors[i].x * glyphScale ) + m_positions[i].x)),
							(float((curVectors[i].y * glyphScale ) + m_positions[i].y)),
							(float((curVectors[i].z * glyphScale ) + m_positions[i].z))
							);
					}
				} //else {
				//		cerr << "len=" << len << "  --  pl=" << pl << endl;

				//	}
				count++;
				iter++;
			}
			glEnd();	
		}
	}

	////////////////////////////		
	/*
	if (!drawState) {

#ifndef _WIN32
glXSwapBuffers( view.display(), view.window() );
#else
SwapBuffers( view.deviceContext() );
#endif // _WIN32
drawState = true;
}
*/

	glPopAttrib();	

	view.endGL();
	//cerr  << "out draw" << endl;
}

bool prioritize::isBounded() const
{ 
	return true;
}
MBoundingBox prioritize::boundingBox() const
{   
	//cerr << " in boundingBox" << endl;
	MStatus st;

	MObject thisNode = thisMObject();

	MPlug enPlug( thisNode, aEnableGraph );


	MPlug hPlug( thisNode, aHeight );
	double height; 
	st = hPlug.getValue (height ); er;

	MPlug wPlug( thisNode, aWidth );
	double width; 
	st = wPlug.getValue (width ); er;

	MPlug xPlug( thisNode, aXOffset );
	double xOffset; 
	st = xPlug.getValue (xOffset ); er;

	MPlug yPlug( thisNode, aYOffset );
	double yOffset; 
	st = yPlug.getValue (yOffset ); er;

	M3dView view = M3dView::active3dView();
	int pw = view.portWidth();
	int ph = view.portHeight();

	MPoint nearPt;
	MPoint farPt;
	short scrCorner1X	=	short( xOffset * pw);
	short scrCorner1Y	=	short( yOffset * ph);
	short scrCorner2X	=	short((xOffset + width) * pw) ;
	short scrCorner2Y	=	short((yOffset + height) * ph);

	view.viewToWorld(scrCorner1X, scrCorner1Y, nearPt, farPt);
	MPoint corner1((nearPt + farPt )*0.5);
	view.viewToWorld(scrCorner2X, scrCorner2Y , nearPt, farPt);
	MPoint corner2(( nearPt + farPt)*0.5);

	//cerr << " out boundingBox" << endl;

	return MBoundingBox( corner1, corner2 );
}

void* prioritize::creator()
{
	return new prioritize();
}

MStatus prioritize::initialize()
{ 

	MStatus st;


	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnCompoundAttribute cAttr;
	MFnEnumAttribute eAttr;
	MFnUnitAttribute uAttr;

	aInForce = tAttr.create("inSignal", "isg", MFnData::kVectorArray , &st ); ert;
	tAttr.setStorable(false);
	//st = addAttribute( aInForce ); e;

	aActive = nAttr.create( "active", "act",MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);

	aDrawColorR = nAttr.create( "drawColorR", "dcr",MFnNumericData::kFloat);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setConnectable(true);
	nAttr.setDefault(0.0f);

	aDrawColorG = nAttr.create( "drawColorG", "dcg",MFnNumericData::kFloat);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setConnectable(true);
	nAttr.setDefault(0.0f);

	aDrawColorB = nAttr.create( "drawColorB", "dcb",MFnNumericData::kFloat);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setConnectable(true);
	nAttr.setDefault(0.0f);

	aDrawColor = nAttr.create( "drawColor", "drc", aDrawColorR , aDrawColorG, aDrawColorB);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setUsedAsColor(true);

  //  aContainerContribution = nAttr.create( "containerContribution", "ccn",MFnNumericData::kFloat);
  //  nAttr.setStorable(true);
  //  nAttr.setKeyable(true);
  //  nAttr.setDefault(1.0f);

	aInData = cAttr.create("inputData", "ind");
	cAttr.addChild(aInForce);
	cAttr.addChild(aActive);
	cAttr.addChild(aDrawColor);
	cAttr.setStorable(true);
	cAttr.setIndexMatters(false);
	cAttr.setReadable(false);
	cAttr.setArray(true);
	cAttr.setDisconnectBehavior(MFnAttribute::kDelete);
	cAttr.setUsesArrayDataBuilder(true);
	st = addAttribute(aInData);

	//st =addAttribute(aDrawColor);
	aPositions = tAttr.create("positionPP", "ppp", MFnData::kVectorArray , &st ); ert;
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	st = addAttribute(aPositions);

	aMaxForce = nAttr.create( "maxSignal", "mxs",MFnNumericData::kDouble);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(1.0);
	st =addAttribute(aMaxForce);

	aMagnitude = nAttr.create( "magnitude", "mag",MFnNumericData::kDouble);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(1.0);
	st =addAttribute(aMagnitude);
	
	aEnableGraph = eAttr.create("drawStyle","dst");
	eAttr.addField( "off"	,  DRAW_OFF     );
	eAttr.addField( "bars"	,  DRAW_BARS 	);
	eAttr.addField( "glyps"	,  DRAW_GLYPH   );
	eAttr.addField( "both"	,  DRAW_BOTH 	);
	eAttr.setHidden( false );
	eAttr.setKeyable( true );
	st = addAttribute( aEnableGraph ); ert;

	aSampleBy = nAttr.create( "sampleBy", "smp",MFnNumericData::kShort);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(1);
	st =addAttribute(aSampleBy);

	aHeight = nAttr.create( "graphHeight", "gph",MFnNumericData::kDouble);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(0.2);
	st =addAttribute(aHeight);

	aWidth = nAttr.create( "graphWidth", "gpw",MFnNumericData::kDouble);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(0.2);
	st =addAttribute(aWidth);

	aXOffset = nAttr.create( "xOffset", "xof",MFnNumericData::kDouble);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(0.02);
	st =addAttribute(aXOffset);

	aYOffset = nAttr.create( "yOffset", "yof",MFnNumericData::kDouble);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(0.02);
	st =addAttribute(aYOffset);

	aGlyphScale = nAttr.create( "glyphScale", "gsc",MFnNumericData::kDouble);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(1.0);
	st =addAttribute(aGlyphScale);

	aOutForce = tAttr.create("outSignal","osg", MFnData::kVectorArray);
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	st = addAttribute( aOutForce ); ert;

	aCurrentTime = uAttr.create( "currentTime", "ct", MFnUnitAttribute::kTime );
	uAttr.setStorable(true);
	st =  addAttribute(aCurrentTime);  er;


	st = attributeAffects( aCurrentTime, aOutForce );ert;

	return MS::kSuccess;
}

void prioritize::postConstructor()
{
	MFnDependencyNode nodeFn(thisMObject());
	nodeFn.setName("prioritizeShape#"); 
	// 	setExistWithoutInConnections(true);
	//	setExistWithoutOutConnections(true);
}
