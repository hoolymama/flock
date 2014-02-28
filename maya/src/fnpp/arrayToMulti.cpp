/*
 *  arrayToMulti.cpp
 *  animal
 *
 *  Created by Julian Mann on 01/08/2006.
 *  Copyright 2006 HoolyMama. All rights reserved.
 *
 */

#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnEnumAttribute.h>

#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>
#include "errorMacros.h"

#include "errorMacros.h"
#include "jMayaIds.h"

#include "arrayToMulti.h"

MTypeId arrayToMulti::id(k_arrayToMulti);
MObject arrayToMulti::aInput;
MObject arrayToMulti::aDefD;
MObject arrayToMulti::aDefV;

MObject arrayToMulti::aOutputD;
MObject arrayToMulti::aOutputV;
MObject arrayToMulti::aSize;


void * arrayToMulti::creator () {
	return new arrayToMulti;
}
/// Post constructor
void
arrayToMulti::postConstructor()
{
}

MStatus arrayToMulti::initialize () {

	MStatus st;
	MFnGenericAttribute gAttr;
	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnEnumAttribute eAttr;
	
	aDefD = nAttr.create ("defD", "dfd",MFnNumericData::kDouble);
	nAttr.setStorable (true);
	nAttr.setWritable (true);
	nAttr.setKeyable (true);
	nAttr.setReadable (false);
	addAttribute (aDefD);
	
	
	aDefV = nAttr.create ("defV", "dfv",MFnNumericData::k3Double);
	nAttr.setStorable (true);
	nAttr.setWritable (true);
	nAttr.setKeyable (true);
	nAttr.setReadable (false);
	addAttribute (aDefV);
	
	aInput = gAttr.create("input", "in");
	gAttr.addAccept(MFnData::kDoubleArray);
	gAttr.addAccept(MFnData::kVectorArray);
	gAttr.setWritable(true);
	gAttr.setStorable(false);
	gAttr.setReadable(false);
	gAttr.setWritable(true);
	gAttr.setCached(false);
	
	aOutputD = nAttr.create ("outputsD", "outd",MFnNumericData::kDouble);
	nAttr.setStorable (false);
	nAttr.setWritable (false);
	nAttr.setReadable (true);
	nAttr.setArray (true);
	nAttr.setDisconnectBehavior(MFnAttribute::kDelete);
	nAttr.setUsesArrayDataBuilder(true);
	
	aOutputV = nAttr.create ("outputsV", "outv",MFnNumericData::k3Double);
	nAttr.setStorable (false);
	nAttr.setWritable (false);
	nAttr.setReadable (true);
	nAttr.setArray (true);
	nAttr.setDisconnectBehavior (MFnAttribute::kDelete);
	nAttr.setUsesArrayDataBuilder(true);
	
	
	aSize = nAttr.create ("size", "siz",MFnNumericData::kLong);
	tAttr.setStorable (false);
	tAttr.setWritable (false);
	tAttr.setReadable (true);	
	
	
	// add attributes and setup dependencies
	addAttribute(aInput);
	addAttribute (aOutputD);
	addAttribute (aOutputV);
	addAttribute (aSize);
	
	attributeAffects (aDefD, aOutputD);
	attributeAffects (aDefV, aOutputV);

	
	attributeAffects (aInput, aOutputD);
	attributeAffects (aInput, aOutputV);

	attributeAffects (aInput, aSize);
	
	
	
	return MS::kSuccess;
}

arrayToMulti::arrayToMulti () {}
arrayToMulti::~arrayToMulti () {}

MStatus arrayToMulti::compute (const MPlug& plug, MDataBlock& data) 
{
	if ((plug == aOutputD ) || (plug == aOutputV )|| (plug == aSize ))
	{
	
		//cerr << "here 1"<< endl;
		MStatus st;
		
		// Get input
		MDataHandle hIn = data.inputValue( aInput, &st );
		if (st.error())	ert;
		MObject objIn  = hIn.data();
		
		bool inIsVector = objIn.hasFn(MFn::kVectorArrayData);
		bool inIsDouble = objIn.hasFn(MFn::kDoubleArrayData);
			
		// process size
		if (plug==aSize)
		{
			int size =-1;
			
			if (inIsVector)
			{
				MVectorArray in = MFnVectorArrayData(objIn).array();
				size = in.length();
			}
			else if (inIsDouble)
			{
				MDoubleArray in = MFnDoubleArrayData(objIn).array();
				size = in.length();
			}
			
			MDataHandle hSize = data.outputValue(aSize);
			hSize.set(int(size));
		} 
		else if(plug == aOutputV) // process vectors
		{
			//cerr << "here 1"<< endl;
			if(!inIsVector) ert;
			
			// get vector array
			MVectorArray in = MFnVectorArrayData(objIn).array();
			const unsigned nLength = in.length();

			unsigned multiIndex = plug.logicalIndex( &st );
			
			// set the data on the multi attribute
			MArrayDataHandle hOutArray = data.outputArrayValue( aOutputV, &st);
			MArrayDataBuilder bOutArray = hOutArray.builder( &st );
			MDataHandle hOutV = bOutArray.addElement(multiIndex, &st);
			
			MVector output;
			if (multiIndex >= nLength) 
			{
				output = data.inputValue(aDefV).asVector();
			} else {
				output = in[multiIndex];
			}
			hOutV.set(output);
			
		}
		else if(plug == aOutputD) // doubles
		{
			if(!inIsDouble) ert;
			
			// get double array
			MDoubleArray in = MFnDoubleArrayData(objIn).array();
			const unsigned nLength = in.length();
			//cerr << "len " << nLength << endl;
			unsigned multiIndex = plug.logicalIndex( &st );
			
			// set the data on the multi attribute
			MArrayDataHandle hOutArray = data.outputArrayValue( aOutputD, &st);
			MArrayDataBuilder bOutArray = hOutArray.builder( &st );
			MDataHandle hOutD = bOutArray.addElement(multiIndex, &st);
			
			double output;
			if (multiIndex >= nLength) 
			{
				output = data.inputValue(aDefD).asDouble();
			} else {
			//	cerr << "here" << endl; 
				output = in[multiIndex];
			}
			hOutD.set(output);
		}
		
		data.setClean(plug);
		return MS::kSuccess;										
	}
	
	return MS::kUnknownParameter;						
}
