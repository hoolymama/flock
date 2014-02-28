

#include <string.h>

#include <sstream>

#include <vector>
#include <maya/MObject.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnIntArrayData.h>

#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>

#include <maya/MDoubleArray.h>
#include <maya/MIntArray.h>
#include <maya/MVectorArray.h>

#include <maya/MArrayDataHandle.h>


#include "mayaMath.h"
#include "flockShape.h"
#include "clonerStack.h"


clonerStack::clonerStack()
	:m_schoolSize(0),
	m_schoolSizeChanged(true)
{}


clonerStack::~clonerStack(){
	meshClonerMap::iterator it  = m_clonerMap.begin();
	for (;it !=m_clonerMap.end();it++) {
		if (it->second) {
			delete it->second;
			it->second = 0;
		}
		m_clonerMap.erase(it);
	}
	
}

// make sure the stack matches the array of plugs

// we want to persist the information corresponding to each input plug so that
// we can avoid rebuilding the output mesh if the input mesh topology didnt change

// for this reason, we want to maintain a mapping between plugs and our meshCloner
// objects. 
// If there is a meshCloner corresponding to a plug we update it with the plug's meshObject
// If there is a plug with no meshCloner, we create one and then update it with the plug's meshObject
// If there is a meshCloner and no corresponding plug exists, we erase it.
// By the end of this function there should be a one-to-one mapping of plugs to meshCloners.
// The key to the map will be the logical plugId.
// The meshCloners will know if the object changed since the last evaluation


MStatus clonerStack::conform(const MObject &node, MArrayDataHandle & ah, unsigned int schoolSize){
//	cerr <<	"conform" << endl;
	MStatus st = MS::kSuccess;
	
	m_schoolSizeChanged = (m_schoolSize != schoolSize) ;
	m_schoolSize = schoolSize;
	
	unsigned n = ah.elementCount();
	MFnDependencyNode nodeFn(node);
	MPlug inMeshesPlug = nodeFn.findPlug(flockShape::aInMeshes);

	for(unsigned i = 0;i < n; i++, ah.next()) {
		
	//	cerr <<	"conform " << n << endl;
		
		int el = ah.elementIndex(&st);
		if (st.error()) continue;
		MDataHandle hComp = ah.inputValue(&st );if (st.error()) continue;
		bool active = hComp.child(flockShape::aMeshActive).asBool();	
		if (!active) continue;
		
		MDataHandle h = hComp.child( flockShape::aMesh );
	    MObject  d = h.data();
		
		if (!( d.hasFn(MFn::kMesh))) continue;
		
		if (m_clonerMap.find(el) == m_clonerMap.end()) {
			MPlug elPlug = inMeshesPlug.elementByLogicalIndex(el,&st);

			//cerr << "el " << el << endl;
			if (st.error()) continue;
			MPlug meshPlug = elPlug.child(flockShape::aMesh);
			// meshCloner * p = new meshCloner(elPlug);
			// m_clonerMap[el] = meshCloner(elPlug);
			//cerr << "here m_clonerMap"  << endl;
			m_clonerMap[el] = new meshCloner(meshPlug);
		} 
		(m_clonerMap[el])->update(d, m_schoolSizeChanged);
	}
	
	// remove items from the map for which there is no corresponding plug
	// or for which the plug is not active
	
//	cerr << "here removing"  << endl;
	meshClonerMap::iterator it  = m_clonerMap.begin();
	for (;it != m_clonerMap.end();it++) {
		st = ah.jumpToElement(it->first);

		bool removeIt = false;
		if (st.error()) {
			removeIt = true;
		} else  {
			MDataHandle hComp = ah.inputValue(&st);
			if (st.error()) {
				removeIt = true;
			} else {
				bool active = hComp.child(flockShape::aMeshActive).asBool();
				if (! active ) removeIt = true;
			}
		}
	//	cerr << "removeIt "  << removeIt << endl;
		if (removeIt) {
			if (it->second) {
				delete it->second;
				it->second = 0;
			}
			m_clonerMap.erase(it);
		}	
	}
	return MS::kSuccess;
}


void clonerStack::pushPoints(){
	//MStatus st = MS::kSuccess;
	meshClonerMap::iterator it  = m_clonerMap.begin();
	for (;it !=m_clonerMap.end();it++) {
		it->second->pushPoints();
	}
}


void clonerStack::prepareOutGeometry(){
	meshClonerMap::iterator it  = m_clonerMap.begin();
	for (;it !=m_clonerMap.end();it++) {
		it->second->prepOutGeom( unsigned(m_schoolSize) );
	}
}


void clonerStack::doOutput(MArrayDataBuilder & b) {
	meshClonerMap::const_iterator it  = m_clonerMap.begin();
	for (;it !=m_clonerMap.end();it++) {
		MDataHandle h = b.addElement(it->first);
		/////////////////////////////////////////////////////////////// 
		h.set(it->second->geometry());
	}
}


//clonerStack::clear(){
//  meshClonerMap::iterator it  = m_clonerMap.begin();
//  for (;it !=m_clonerMap.end();it++) {
//  	delete it->second;
//  	it->second=0;
//  	it.erase();
//  }
//}
