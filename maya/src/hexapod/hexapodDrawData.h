

#ifndef hexapodDrawData_H
#define hexapodDrawData_H

#include <maya/MPxDrawOverride.h>
#include <maya/MUserData.h>
#include <maya/MDrawRegistry.h>
// #include <maya/MViewport2Renderer.h>
// #include <maya/MMatrix.h>
// #include <maya/MColor.h>
// #include <maya/MStringArray.h>
// #include "paintingGeom.h"
// #include "paint.h"

class hexapodDrawData : public MUserData
{
public:
	hexapodDrawData();
	virtual ~hexapodDrawData();

 
	// std::map<int, Paint> palette;
	// float xPosition;
	// float yPosition;
	// float width;
	// float height;
	// bool displayId;
	// bool displayName;
	
 
};

#endif
 