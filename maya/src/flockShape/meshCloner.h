#ifndef meshCloner_H
#define meshCloner_H


#include <vector>

#include <maya/MDataBlock.h>
#include <maya/MObject.h>
#include <maya/MIntArray.h>
#include <maya/MFloatArray.h>
#include <maya/MPlug.h>

#include <maya/MFloatPointArray.h>



class meshCloner{
	public:
		
		meshCloner();
		
	 	meshCloner(const MPlug &plug);
	
		~meshCloner();
		
		void update(const MObject &d, bool schoolSizeChanged);

		void pushPoints();
		
		const MObject& geometry() const;
		
		MStatus prepOutGeom(unsigned schoolSize);
		
	private:


	// keep hold of the output object
	
	bool m_needsRebuild;
	MPlug m_plug;
	MObject m_outGeom;	
		
		

	//MFloatPointArray m_points;
	unsigned m_numVertices;
	MFloatPointArray m_vertices;
	MIntArray m_counts;
	MIntArray m_connects;
	
	
	
	// UV mapping for one fish
	///////////////////////
	MFloatArray m_Us;
	MFloatArray m_Vs;
	
	MIntArray m_uvCounts;
	MIntArray m_uvIds;
	///////////////////////
		
};



#endif

