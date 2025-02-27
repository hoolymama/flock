

#include <string.h>

#include <sstream>

#include <vector>
#include <algorithm>
#include <maya/MObject.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnIntArrayData.h>


#include <maya/MDoubleArray.h>
#include <maya/MIntArray.h>
#include <maya/MVectorArray.h>


#include <maya/MArrayDataHandle.h>


#include "mayaMath.h"
#include "flockShape.h"

#include "rigSlice.h"



rigSlice::rigSlice(MObject &node, MDataBlock &data)
	: m_nValues(0), m_nVectorValues(0)
{
	MStatus st;

	// id mapping - get yer head around that
	//////////////////////// IDS ////////////////////////
	// i         :  0   1   2   3   4   5   6   7   8 //
	// id        : 14   6   2  12  10   5  13   8  11 //
	// sortedId  :  2   5   6   8  10  11  12  13  14 //
	// idIndex   :  2   5   1   7   4   8   3   6   0 //


	// const MDoubleArray ids = MFnDoubleArrayData(data.inputValue(flockShape::aIdPP).data()).array();
	//unsigned len = ids.length();
	const MIntArray sortedIds = MFnIntArrayData(data.inputValue(
	                              flockShape::aSortedIdPP).data()).array();
	unsigned len = sortedIds.length();
	const MIntArray idIndex = MFnIntArrayData(data.inputValue(
	                            flockShape::aIdIndexPP).data()).array();
	// TODO - if problems with sortedIds or idIndex, we do our own sort and generate them

	//  cerr << "///////////////////////" << endl;
	//  cerr << "i" << "\t" << "ids" <<  "\t" << "sortIds" <<  "\t" << "idIndex" << endl;
	//  for (unsigned i=0;i<ids.length();i++) {
	//  	cerr << i << "\t" << ids[i] <<  "\t" << sortedIds[i] <<  "\t" << idIndex[i] << endl;
	//  }


	MString cullListString =  data.inputValue( flockShape::aCullList ).asString();
	const MDoubleArray  activepp  = MFnDoubleArrayData(data.inputValue(
	                                  flockShape::aActivePP).data()).array();
	//unsigned targetId = data.inputValue(flockShape::aTargetId).asLong();
	/////////////////////////////////////////////////////////////////


	//////////////////////// TRANSFORMATION ////////////////////////
	const MVectorArray tpp = MFnVectorArrayData(data.inputValue(
	                           flockShape::aTranslatePP).data()).array();
	const MVectorArray rpp = MFnVectorArrayData(data.inputValue(
	                           flockShape::aRotatePP).data()).array();
	const MVectorArray rupp = MFnVectorArrayData(data.inputValue(
	                            flockShape::aRotateUpPP).data()).array();
	const MDoubleArray spp = MFnDoubleArrayData(data.inputValue(
	                           flockShape::aScalePP).data()).array();
	//MObject o = data.inputValue(flockShape::aScalePP).data();

	// MVectorArray spp_v;
	// MDoubleArray spp_d;
	// if (o.hasFn(MFn::kVectorArrayData)){
	// 	spp_v =  MFnVectorArrayData(o).array();
	// } else if (o.hasFn(MFn::kDoubleArrayData)){
	// 	spp_d = MFnDoubleArrayData(o).array();
	// }

	short rotType = data.inputValue(flockShape::aRotateType).asShort();
	mayaMath::axis aimFrontAxis = mayaMath::axis(data.inputValue(
	                                flockShape::aRotateAimFrontAxis).asShort());
	mayaMath::axis  aimUpAxis = mayaMath::axis(data.inputValue(
	                              flockShape::aRotateAimUpAxis).asShort());
	/////////////////////////////////////////////////////////////////


	MPlug userPpPlug;
	MPlug compoundPlug;

	//////////////////////// USER MAPS ////////////////////////
	MPlug userMapPlug(node, flockShape::aUserMap );

	std::vector<MDoubleArray > userArrays;
	MArrayDataHandle hInUserMap = data.inputArrayValue( flockShape::aUserMap, &st ); mser;
	unsigned num = hInUserMap.elementCount();
	MIntArray isDoubleArray;
	for (unsigned i = 0; i < num; i++, hInUserMap.next()) {
		MDataHandle hCompound = hInUserMap.inputValue(&st ); mser;
		MDataHandle hUserPP = hCompound.child( flockShape::aUserPP );

		// if connected, get the data, otherwise make a 1 element array from the value of the target - plug
		compoundPlug = userMapPlug.elementByPhysicalIndex(i, &st); mser;
		userPpPlug = compoundPlug.child(flockShape::aUserPP);

		MDoubleArray da;
		if (userPpPlug.isConnected()) {
			MObject userObject = hUserPP.data();
			if (MFnDoubleArrayData(userObject).array().length() == len) {
				da =  MFnDoubleArrayData(userObject).array();
				isDoubleArray.append(1);
			}
		}

		if (  da.length() != len ) {
			double v = hCompound.child( flockShape::aUserTarget ).asDouble();
			da.setLength(1);
			da.set(v , 0);
			isDoubleArray.append(0);
		}
		userArrays.push_back(da);
		m_nValues++;
	}
	//////////////////////// USER MAPS ////////////////////////



	//////////////////////// USER VECTOR MAPS ////////////////////////
	// MPlug userPpPlug;
	// MPlug compoundPlug;
	MPlug userVectorMapPlug(node, flockShape::aUserVectorMap );

	std::vector<MVectorArray > userVectorArrays;
	MArrayDataHandle hInUserVectorMap = data.inputArrayValue( flockShape::aUserVectorMap,
	                                    &st ); mser;
	num = hInUserVectorMap.elementCount();
	MIntArray isVectorArray;
	for (unsigned i = 0; i < num; i++, hInUserVectorMap.next()) {
		MDataHandle hCompound = hInUserVectorMap.inputValue(&st ); mser;
		MDataHandle hUserVectorPP = hCompound.child( flockShape::aUserVectorPP );

		// if connected, get the data, otherwise make a 1 element array from the value of the target - plug
		compoundPlug = userVectorMapPlug.elementByPhysicalIndex(i, &st); mser;
		userPpPlug = compoundPlug.child(flockShape::aUserVectorPP);

		MVectorArray va;
		if (userPpPlug.isConnected()) {
			MObject userVectorObject = hUserVectorPP.data();
			if (MFnVectorArrayData(userVectorObject).array().length() == len) {
				va =  MFnVectorArrayData(userVectorObject).array();
				isVectorArray.append(1);
			}
		}

		if (  va.length() != len  ) {
			MVector v = hCompound.child( flockShape::aUserVectorTarget ).asVector();
			va.setLength(1);
			va.set(v , 0);
			isVectorArray.append(0);
		}
		userVectorArrays.push_back(va);
		m_nVectorValues++;
	}
	//////////////////////// USER VECTOR MAPS ////////////////////////



	bool ids_valid = (len == idIndex.length());
	bool do_t = (len == tpp.length()) ;
	bool do_r = (len == rpp.length());
	bool do_u = (len == rupp.length());
	bool do_s  = (len == spp.length());




	// cull list and activePP strategy:
	// any particles in the cull list, or with an activePP value less than 0.5 will be culled
	// this means there will never be a rig particle made
	////////////////////////////////////////////////////////////////////////////
	MIntArray iaActivepp(len);
	if (activepp.length() != len) {
		for (unsigned i = 0; i < len; i++) {
			iaActivepp.set(1, i);
		}
	}
	else {
		for (unsigned i = 0; i < len; i++) {
			iaActivepp.set((activepp[i] > 0.5), i);
		}
	}



	// id mapping - get yer ghead around that
	//////////////////////// IDS ////////////////////////
	// i         :  0   1   2   3   4   5   6   7   8 //
	// id        : 14   6   2  12  10   5  13   8  11 //
	// sortedId  :  2   5   6   8  10  11  12  13  14 //
	// idIndex   :  2   5   1   7   4   8   3   6   0 //
	MStringArray sa;
	MStringArray commaSA;
	MStringArray tmpSA;

	cullListString.split(' ', sa);
	//	cerr << "cullListString: " << sa << endl;
	unsigned nCull   = sa.length();
	if (nCull > 0) {
		std::vector<int> cullIds;
		for (unsigned j   = 0; j < nCull; j++) {
			cullIds.push_back(sa[j].asInt());
		}

		// sort (cullIds.begin(), cullIds.end());
		//	cullIds.sort();
		std::sort(cullIds.begin(), cullIds.end());
		unsigned j = 0;
		//	cerr << "cullIds are now sorted" << endl;

		std::vector<int>::const_iterator cii;
		//    for(cii=cullIds.begin(); cii!=cullIds.end(); cii++)
		//    {
		// 	cerr << *cii <<", " ;
		//    }
		//	cerr << endl;

		//		cerr << cullIds << endl;
		for (unsigned i = 0; (i < len && j < nCull); i++) {
			//			cerr << "i,j:"<< i<<":"<<j;
			//			cerr << " cullIds[j]" << cullIds[j] << " - "<< sortedIds[i] << endl;
			if (cullIds[j] == sortedIds[i]) {
				//	cerr << i << "\t" << "\t" << sortedIds[i] <<  "\t" << idIndex[i] << endl;
				//				cerr << "MATCH: set iaActivepp["<<  idIndex[i] << "] 0" << endl;
				unsigned theIndex = idIndex[i];
				//				cerr << "theIndex " << theIndex << endl;
				iaActivepp.set(0, theIndex );
				j++;
				//	continue;
			}
			else if  (cullIds[j] < sortedIds[i]) {
				//				cerr << "No MATCH: <"<< endl;
				j++;
			}
			else {
				//				cerr << "No MATCH: >"<< endl;
			}
			// else (cullIds[j] > sortedIds[i] , so continue
		}
	}
	////////////////////////////////////////////////////////////////////////////

	//	cerr << iaActivepp << endl;

	MVector t, r, u, s;

	std::vector< MDoubleArray >::const_iterator da_iter = userArrays.begin();
	std::vector< MVectorArray >::const_iterator va_iter = userVectorArrays.begin();




	if (ids_valid) {
		for (unsigned i = 0; i < len; i++) {
			unsigned particleId = sortedIds[i];  // going in sorted particleId order

			unsigned index = idIndex[i];
			if (! iaActivepp[index] ) { continue; }

			int arrayCount = 0;

			// create the user float values
			MFloatArray fa;

			for ( da_iter = userArrays.begin(); da_iter != userArrays.end();
			      da_iter++, arrayCount++) {
				//cerr "da iter refers to pointer " << *da_iter << endl;
				//const MDoubleArray & array = *da_iter;

				// the double array may be a 1 element array (effectively a scalar) if the PP plug was not connected.
				if (isDoubleArray[arrayCount]) {
					fa.append( float(  (*da_iter)[index]   ) );
				}
				else {
					fa.append( float(  (*da_iter)[0] ) );
				}
			}

			// create the user vector values
			MFloatVectorArray fva;
			arrayCount = 0;
			for ( va_iter = userVectorArrays.begin(); va_iter != userVectorArrays.end();
			      va_iter++, arrayCount++) {
				//cerr "da iter refers to pointer " << *da_iter << endl;
				//const MDoubleArray & array = *da_iter;

				// the double array may be a 1 element array (effectively a scalar) if the PP plug was not connected.
				if (isVectorArray[arrayCount]) {
					fva.append( MFloatVector(  (*va_iter)[index]   ) );
				}
				else {
					fva.append( MFloatVector(  (*va_iter)[0] ) );
				}
			}



			if (do_t) {
				t = tpp[index];
			}
			else {
				t = MVector::zero;
			}

			if (do_s) {
				s =  MVector(spp[index], spp[index], spp[index]);
			}
			else {
				s = MVector(1.0, 1.0, 1.0);
			}



			rigParticle *particle = 0;

			if (rotType == 0) { // aim
				if (do_r) {r = rpp[index];}
				else {r = MVector::xAxis;}
				if (do_u) {u = rupp[index];}
				else {u = MVector::yAxis;}
				particle = new rigParticle(particleId, t, r, u, s, fa, fva, aimFrontAxis, aimUpAxis);
			}
			else	{
				if (do_r) {r = rpp[index];}
				else {r = MVector::zero;}
				particle = new rigParticle(particleId, t, r, s, fa, fva);
			}
			//cerr << "storing pointer to particle " << particle << " on m_stack" << endl;
			m_stack.push_back(particle);

		}
	}

}


rigSlice::~rigSlice() {
	while (!m_stack.empty()) {
		if (m_stack.front()) {
			delete m_stack.front();
		}
		m_stack.pop_front();
	}
}
const particleStack &rigSlice::stack() const {return m_stack;}



// plugging gaps works like this
// every time-slice in the map must be the same length for motion blur to work.
// however, some particles may die or be born within the motion block.
// this can leave gaps in this slice or the previous slices respectfully
// therefore we need to plug gaps in this slice with respect to the most recent slice,
// and then in all previous slices with respect to this slice.
// We do this by copying and inserting missing particles into the destination list from the source list

int rigSlice::plugGaps(const rigSlice &src) {
	int numFilled = 0;

	particleStack::iterator iter =  m_stack.begin();
	const particleStack src_stack =  src.stack();
	particleStack::const_iterator src_iter =  src_stack.begin();


	while (src_iter != src_stack.end()) {
		if (iter == m_stack.end() ) {
			rigParticle *np = new rigParticle(**src_iter);
			m_stack.insert(iter, np);
			src_iter++;
			numFilled++;
		}
		else if ((*src_iter)->id() > (*iter)->id() ) {
			iter++;
		}
		else if ((*src_iter)->id() == (*iter)->id()  ) {
			iter++;		src_iter++;
		}
		else if ((*src_iter)->id() <  (*iter)->id()  ) {
			rigParticle *np = new rigParticle(**src_iter);
			m_stack.insert(iter, np);
			src_iter++;
			numFilled++;
		}
	}
	// cerr << "number of gaps filled  "<< numFilled << endl;
	return numFilled;
}

// fortify

MStatus rigSlice::fortify(const rigSlice &target) {
	MStatus st;
	particleStack::iterator iter =  m_stack.begin();
	particleStack::const_iterator target_iter = target.stack().begin();

	while (    (iter != m_stack.end())   &&  (target_iter != target.stack().end())   ) {
		st = (*iter)->setFuture(*target_iter); msert;
		iter++;
		target_iter++;
	}

	// check they are both done, otherwise it means they were different lengths
	if ((iter == m_stack.end())  && (target_iter == target.stack().end())) {
		return MS::kSuccess;
	}
	return MS::kFailure;
}

void rigSlice::info() const {

	particleStack::const_iterator iter =  m_stack.begin();
	std::stringstream strm;
	while (iter != m_stack.end()) {
		cerr << "id:[" << (*iter)->id() << "] -- Matrix: " << (*iter)->transform() <<
		     " -- Values: " <<  (*iter)->values() << endl;
		iter++;
	}
}

const unsigned &rigSlice::userValuesCount() const {
	return m_nValues;
}

const unsigned &rigSlice::userVectorValuesCount() const {
	return m_nVectorValues;
}




