#ifndef hexapodDrawOverride_H
#define hexapodDrawOverride_H

#include <stdlib.h>

#include <maya/MDagPath.h>

#include <maya/MDrawRegistry.h>
#include <maya/MPxDrawOverride.h>
#include <maya/MUserData.h>
#include <maya/MDrawContext.h>
#include <maya/MSelectionList.h>
#include <maya/MBoundingBox.h>


#include <maya/MViewport2Renderer.h>
#include <maya/MStateManager.h>
#include <maya/MShaderManager.h>
#include <assert.h>

#include "hexapodDrawData.h"
#include "hexapod.h"


class hexapodDrawOverride : public MHWRender::MPxDrawOverride {
public:

	static MHWRender::MPxDrawOverride *Creator(const MObject &obj)
	{
		return new hexapodDrawOverride(obj);
	}

	virtual ~hexapodDrawOverride();

	virtual MHWRender::DrawAPI supportedDrawAPIs() const;

	virtual bool isBounded(
	  const MDagPath &objPath,
	  const MDagPath &cameraPath) const;

	virtual MBoundingBox boundingBox(
	  const MDagPath &objPath,
	  const MDagPath &cameraPath) const;

	virtual MUserData *prepareForDraw(
	  const MDagPath &objPath,
	  const MDagPath &cameraPath,
	  const MHWRender::MFrameContext &frameContext,
	  MUserData *oldData);

	virtual bool hasUIDrawables() const { return true; }

	virtual void addUIDrawables(
	  const MDagPath &objPath,
	  MHWRender::MUIDrawManager &drawManager,
	  const MHWRender::MFrameContext &frameContext,
	  const MUserData *data);

	virtual bool traceCallSequence() const
	{
		return false;
	}

	virtual void handleTraceMessage( const MString &message ) const
	{
		MGlobal::displayInfo("hexapodDrawOverride: " + message);
		cerr <<  "hexapodDrawOverride: " << message.asChar() << endl;
	}

	void drawShaded(
	  MHWRender::MUIDrawManager &drawManager,
	const MHWRender::MFrameContext &context,
	  const hexapodDrawData *cdata);
	  
private:


	hexapodDrawOverride(const MObject &obj);

	static void markDirty(void *clientData);

	hexapod  *fHexapodNode;

	MCallbackId fModelEditorChangedCbId;
	MCallbackId fTimeChangedCbId;
};


#endif
