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

 

MTypeId prioritize::id( k_prioritize );

MObject prioritize::aPositions;	
MObject prioritize::aGlyphScale;	

MObject prioritize::aInForce;	
MObject prioritize::aDrawColor;
MObject prioritize::aDrawColorR;
MObject prioritize::aDrawColorG;
MObject prioritize::aDrawColorB;
MObject prioritize::aActive;
MObject prioritize::aMagnitude;


MObject prioritize::aInData;				 
MObject prioritize::aMaxForce;

MObject prioritize::aDrawBarChart;
MObject prioritize::aDrawGlyphs;
MObject prioritize::aDrawResultGlyph;

MObject prioritize::aResultDrawColor;	
MObject prioritize::aResultDrawColorR;
MObject prioritize::aResultDrawColorG;
MObject prioritize::aResultDrawColorB;

MObject prioritize::aSampleBy;	
MObject prioritize::aHeight;	
MObject prioritize::aWidth;		
MObject prioritize::aXOffset;	
MObject prioritize::aYOffset;	


MObject prioritize::aOutForce; 
MObject prioritize::aOutForces; 
prioritize::prioritize() {

}

prioritize::~prioritize() {

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
	// JPMDBG;


	MStatus st;
	if(! (( plug == aOutForce) || (plug == aOutForces)) ) return MS::kUnknownParameter;

	// JPMDBG;

	double maxForce = data.inputValue( aMaxForce).asDouble();
	if (maxForce < 0.0) maxForce = 0.0;


	MVectorArray outTotalForce;
	MDoubleArray accum;


	MArrayDataHandle ha = data.inputArrayValue( aInData, &st ); 


	bool firstIter = true;
	unsigned int len = 0;	

	unsigned numElements = ha.elementCount();


	MArrayDataHandle     hOutputForces = data.outputArrayValue( aOutForces, &st ); er;
	MArrayDataBuilder    bOutputForces = hOutputForces.builder();
	MFnVectorArrayData 	 fnOutputForce;


	const double epsilon = 1e-9;
	for(unsigned i = 0;i < numElements; i++, ha.next()) {
		// cerr << "compute input element " << i << endl;
		unsigned elementIndex = ha.elementIndex(&st); er;	if (st.error()) continue;



		MDataHandle hInDataCompound = ha.inputValue(&st ); er;	 if (st.error()) continue;

		bool active = hInDataCompound.child(aActive).asBool();

		MObject dInForce =  hInDataCompound.child( aInForce ).data();
		MFnVectorArrayData fnInForce(dInForce);


		if (firstIter) {
			len =  fnInForce.length();
			outTotalForce = MVectorArray(len);
			accum  = MDoubleArray(len);
			firstIter = false;
		}

		MVectorArray outForce = MVectorArray(len);

		if (active && (fnInForce.length() == len)){
			MVectorArray inForce = fnInForce.array();
			for (unsigned j= 0; j < len; j++) {

				if ((accum[j]+epsilon) < maxForce) {
					double remaining = (maxForce - accum[j]) ;
					double tmpMag = inForce[j].length();

					if (tmpMag > remaining) {
						outForce[j] = (inForce[j].normal() * remaining);
						accum[j] = maxForce; 
					}	else {
						outForce[j] =  inForce[j];
						accum[j] += inForce[j].length(); 
					}
					outTotalForce[j] +=  outForce[j];
				}
			}	
		}
	// cerr << "seting outputs "<< endl;
	
		MDataHandle hOutForce = bOutputForces.addElement(elementIndex);
		MObject dOutputForce = fnOutputForce.create( outForce, &st );er;
		hOutForce.set( dOutputForce);

	}

	// cerr << "seting main output "<< endl;


	MDataHandle hTotalOutputForce =  data.outputValue( aOutForce, &st); er;
	MFnVectorArrayData fnTotalOutputForce;
	MObject dTotalOutputForce = fnTotalOutputForce.create( outTotalForce, &st );er;
	hTotalOutputForce.set( dTotalOutputForce);


	hTotalOutputForce.setClean();
	hOutputForces.setAllClean();
	
	// JPMDBG;

	return( MS::kSuccess );
}

void prioritize::draw( M3dView & view, const MDagPath & path, 
	M3dView::DisplayStyle style,
	M3dView::DisplayStatus status )
{ 
	// JPMDBG;
	MStatus st;

	MObject thisNode = thisMObject();

	MPlug barsPlug( thisNode, aDrawBarChart );
	bool drawBars;
	st = barsPlug.getValue (drawBars ); er;
	
	MPlug glyphsPlug( thisNode, aDrawGlyphs );
	bool drawGlyphs;
	st = glyphsPlug.getValue (drawGlyphs ); er;
	
	MPlug resultPlug( thisNode, aDrawResultGlyph );
	bool drawResultGlyphs;
	st = resultPlug.getValue (drawResultGlyphs ); er;

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


	if (xOffset < 0) xOffset=0;
	if (yOffset < 0) yOffset=0;
	if (width  < 0.01 ) width=0.01;
	if (height  < 0.01 ) height=0.01;
	if (xOffset > 0.99) xOffset=0.99;
	if (yOffset > 0.99) yOffset=0.99;
	if ((xOffset+width) > 1) width = 1-xOffset;
	if ((yOffset+height) > 1) height = 1-yOffset;

	int pw = view.portWidth();
	int ph = view.portHeight();
	int x0, x1, y0, y1;
	int yStart; 
	int yEnd; 
	x0 = int(xOffset * pw);
	y0 = int(yOffset * ph);
	x1 = int((xOffset + width) * pw);
	y1 = int((yOffset + height) * ph);


	MString msg = MFnDependencyNode(thisNode).name();
	msg += " cannot draw unless you connect an array of positions";


	MPlug positionPlug( thisNode, aPositions );
	MObject dPositions;
	st = positionPlug.getValue(dPositions);
	if (st.error()) {
		MGlobal::displayWarning(msg);
		// cerr << msg << endl;
		return;
	}
	MFnVectorArrayData fnV(dPositions);
	MVectorArray position = fnV.array(&st); 
	if (st.error()) {
		MGlobal::displayWarning(msg);
		return;
	}
	unsigned len = position.length();


	MObject thisObj = thisMObject();
	MPlug inDataPlug(thisObj, aInData);
	MPlug outForcePlug(thisObj, aOutForces);

	unsigned numIn =  inDataPlug.numElements();

	view.beginGL(); 
	glPushAttrib(GL_CURRENT_BIT);
	glClear(GL_CURRENT_BIT);

		// draw the graph

	
	if (drawBars) {
		// JPMDBG;
		if ((pw * width) < (len / sampleBy)) {
			sampleBy = short((len / (pw * width)) + 1);
		}
		int numSamples =  short(len / sampleBy );

		MIntArray xPositions(numSamples);
		MIntArray yPositions(numSamples);

		double range = width * pw;
		double spacing = 1.0 / numSamples;
		for (unsigned i = 0; i< numSamples; i++){
			xPositions[i] = int((xOffset*pw) + (  range * i * spacing ));
		}
		yPositions = MIntArray(numSamples, int(yOffset * ph));

		double graphHeight =  ph * height ;
		double normalizer = 	graphHeight	/ maxForce;
		int tmp ;



		// 	// setup projection
		// 	////////////////////////////
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

		// draw the outline
		////////////////////////////
		glBegin( GL_LINE_LOOP );
		glVertex2i(x0, y0);
		glVertex2i(x0, y1);
		glVertex2i(x1, y1);
		glVertex2i(x1, y0); 
		glEnd();

		MPoint posTopRight, farClp; 
		view.viewToWorld( x1, y1,  posTopRight, farClp ) ;
		view.drawText(MFnDependencyNode(thisNode).name(), posTopRight,  M3dView::kRight);


		glBegin(GL_LINES);

		for (int i = 0; i < numIn; ++i)
		{
			MPlug inPlug = inDataPlug.elementByPhysicalIndex(i);
			unsigned logicalIndex = inPlug.logicalIndex();

			MPlug colorPlug  = inPlug.child(aDrawColor) ;
			float colorRed,colorGreen,colorBlue;
			colorPlug.child(0).getValue(colorRed);
			colorPlug.child(1).getValue(colorGreen);
			colorPlug.child(2).getValue(colorBlue);

			MPlug outPlug = outForcePlug.elementByLogicalIndex(logicalIndex);
			MObject o;
			st = outPlug.getValue(o);er;
			MFnVectorArrayData fnV(o, &st);er;
			MVectorArray dir = fnV.array();

			view.setDrawColor( MColor( MColor::kRGB, colorRed, colorGreen, colorBlue ) );

			for (unsigned i=0, j=0; ((i<len) && (j < numSamples)) ;i+=sampleBy,j++) {
				int screenMag =  int(dir[i].length() * normalizer) ;
				glVertex2i(xPositions[j],yPositions[j]);	
				tmp = yPositions[j] + screenMag;
				glVertex2i(xPositions[j],tmp);	
				yPositions[j] = tmp;
			}
		}
		glEnd();
		glMatrixMode( GL_PROJECTION );
		glPopMatrix();
		glMatrixMode( GL_MODELVIEW );
		glPopMatrix();
		// JPMDBG;
	}




	if (drawGlyphs) {
		// JPMDBG;
		glBegin(GL_LINES);

		for (int i = 0; i < numIn; ++i)
		{
			MPlug inPlug = inDataPlug.elementByPhysicalIndex(i);
			unsigned logicalIndex = inPlug.logicalIndex();

			MPlug colorPlug  = inPlug.child(aDrawColor) ;
			float colorRed,colorGreen,colorBlue;
			colorPlug.child(0).getValue(colorRed);
			colorPlug.child(1).getValue(colorGreen);
			colorPlug.child(2).getValue(colorBlue);

			MPlug outPlug = outForcePlug.elementByLogicalIndex(logicalIndex);
			MObject o;
			st = outPlug.getValue(o);er;
			MFnVectorArrayData fnV(o, &st);er;
			MVectorArray dir = fnV.array();

			view.setDrawColor( MColor( MColor::kRGB, colorRed, colorGreen, colorBlue ) );



			for (unsigned i=0 ;  i<len  ;i++) {

				glVertex3f(float(position[i].x) ,float(position[i].y) ,float(position[i].z) );
				glVertex3f(
					(float((dir[i].x * glyphScale ) + position[i].x)),
					(float((dir[i].y * glyphScale ) + position[i].y)),
					(float((dir[i].z * glyphScale ) + position[i].z))
					);


			}
		}

		glEnd();	

		// JPMDBG;
	}



	if (drawResultGlyphs) {
		// JPMDBG;

		MPlug totalForcePlug( thisNode, aOutForce );

		MObject dOutForce;
		st = totalForcePlug.getValue(dOutForce);

		MFnVectorArrayData fnOutForce(dOutForce);
		MVectorArray force = fnOutForce.array(&st); er;
		unsigned len = force.length();


		glBegin(GL_LINES);



		MPlug colorPlug(thisNode, aResultDrawColor) ;
		float colorRed,colorGreen,colorBlue;
		colorPlug.child(0).getValue(colorRed);
		colorPlug.child(1).getValue(colorGreen);
		colorPlug.child(2).getValue(colorBlue);

		view.setDrawColor( MColor( MColor::kRGB, colorRed, colorGreen, colorBlue ) );

		for (unsigned i=0 ;  i<len  ;i++) {

			glVertex3f(float(position[i].x) ,float(position[i].y) ,float(position[i].z) );
			glVertex3f(
				(float((force[i].x * glyphScale ) + position[i].x)),
				(float((force[i].y * glyphScale ) + position[i].y)),
				(float((force[i].z * glyphScale ) + position[i].z))
				);
		}

		glEnd();	

		// JPMDBG;
		// cerr << endl;
	}




	glPopAttrib();	

	view.endGL();
	// JPMDBG;
}

bool prioritize::isBounded() const
{ 
	return false;
}
MBoundingBox prioritize::boundingBox() const
{   

	return MBoundingBox( MPoint(), MPoint() );
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

	aInForce = tAttr.create("inSignal", "isg", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);

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
	aPositions = tAttr.create("positionPP", "ppp", MFnData::kVectorArray , &st ); er;
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kReset);
	st = addAttribute(aPositions);

	aMaxForce = nAttr.create( "maxSignal", "mxs",MFnNumericData::kDouble);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(1.0);
	nAttr.setMin(0.00);
	st =addAttribute(aMaxForce);


	aDrawBarChart = nAttr.create( "drawBarChart", "dbc",MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);
	st = addAttribute( aDrawBarChart ); er;


	aDrawGlyphs = nAttr.create( "drawGlyphs", "dgl",MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);
	st = addAttribute( aDrawGlyphs ); er;

	aDrawResultGlyph = nAttr.create( "drawResultGlyph", "drgl",MFnNumericData::kBoolean);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(true);
	st = addAttribute( aDrawResultGlyph ); er;


	aResultDrawColorR = nAttr.create( "resultDrawColorR", "rdcr",MFnNumericData::kFloat);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setConnectable(true);
	nAttr.setDefault(0.0f);

	aResultDrawColorG = nAttr.create( "resultDrawColorG", "rdcg",MFnNumericData::kFloat);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setConnectable(true);
	nAttr.setDefault(0.0f);

	aResultDrawColorB = nAttr.create( "resultDrawColorB", "rdcb",MFnNumericData::kFloat);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setConnectable(true);
	nAttr.setDefault(0.0f);

	aResultDrawColor = nAttr.create( "resultDrawColor", "rdrc",aResultDrawColorR, aResultDrawColorG, aResultDrawColorB);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setUsedAsColor(true);
	st =addAttribute(aResultDrawColor);

	aSampleBy = nAttr.create( "sampleBy", "smp",MFnNumericData::kShort);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(1);
	st =addAttribute(aSampleBy);

	aHeight = nAttr.create( "graphHeight", "gph",MFnNumericData::kDouble);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(0.2);
	 nAttr.setMin(0.01);
	 nAttr.setMax(1.00);
	 	 nAttr.setSoftMin(0.01);
	 nAttr.setSoftMax(1.00);
	st =addAttribute(aHeight);

	aWidth = nAttr.create( "graphWidth", "gpw",MFnNumericData::kDouble);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(0.2);
		 nAttr.setMin(0.01);
	 nAttr.setMax(1.00);
	 	 nAttr.setSoftMin(0.01);
	 nAttr.setSoftMax(1.00);
	st =addAttribute(aWidth);

	aXOffset = nAttr.create( "xOffset", "xof",MFnNumericData::kDouble);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(0.02);
		 nAttr.setMin(0.00);
	 nAttr.setMax(0.99);
	 	 nAttr.setSoftMin(0.00);
	 nAttr.setSoftMax(0.99);
	st =addAttribute(aXOffset);

	aYOffset = nAttr.create( "yOffset", "yof",MFnNumericData::kDouble);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(0.02);
		 nAttr.setMin(0.00);
	 nAttr.setMax(0.99);
	 	 nAttr.setSoftMin(0.00);
	 nAttr.setSoftMax(0.99);
	st =addAttribute(aYOffset);

	aGlyphScale = nAttr.create( "glyphScale", "gsc",MFnNumericData::kDouble);
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(1.0);
	st =addAttribute(aGlyphScale);


	aOutForces = tAttr.create("outForces","ofcs", MFnData::kVectorArray);
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	tAttr.setArray(true);
	tAttr.setUsesArrayDataBuilder(true);
	st = addAttribute( aOutForces ); er;

	aOutForce = tAttr.create("outSignal","osg", MFnData::kVectorArray);
	tAttr.setStorable(false);
	tAttr.setReadable(true);
	st = addAttribute( aOutForce ); er;

	st = attributeAffects(aInForce, aOutForce );er;
	st = attributeAffects(aActive, aOutForce );er;
	st = attributeAffects(aInData, aOutForce );er;
	st = attributeAffects(aMaxForce, aOutForce );er;

	st = attributeAffects(aInForce, aOutForces );er;
	st = attributeAffects(aActive, aOutForces );er;
	st = attributeAffects(aInData, aOutForces );er;
	st = attributeAffects(aMaxForce, aOutForces );er;

	return MS::kSuccess;
}

void prioritize::postConstructor()
{
	MFnDependencyNode nodeFn(thisMObject());
	nodeFn.setName("prioritizeShape#"); 
	// 	setExistWithoutInConnections(true);
	//	setExistWithoutOutConnections(true);
}
