

#include <maya/M3dView.h>
#include <maya/MGlobal.h>
#include <maya/MFnPluginData.h>
#include <maya/MEventMessage.h>
#include "errorMacros.h"

#include "hexapodDrawOverride.h"


hexapodDrawOverride::hexapodDrawOverride(const MObject &obj)
	: MHWRender::MPxDrawOverride(obj, NULL, false)
{

	fModelEditorChangedCbId = MEventMessage::addEventCallback(
		"modelEditorChanged", markDirty, this);

	fTimeChangedCbId = MEventMessage::addEventCallback(
		"timeChanged", markDirty, this);

	MStatus st;
	MFnDependencyNode node(obj, &st);
	fHexapodNode = st ? dynamic_cast<hexapod *>(node.userNode()) : NULL;
}

hexapodDrawOverride::~hexapodDrawOverride()
{
	fHexapodNode = NULL;
	if (fModelEditorChangedCbId != 0)
	{
		MMessage::removeCallback(fModelEditorChangedCbId);
		fModelEditorChangedCbId = 0;
	}

	if (fTimeChangedCbId != 0)
	{
		MMessage::removeCallback(fTimeChangedCbId);
		fTimeChangedCbId = 0;
	}
}

void hexapodDrawOverride::markDirty(void *clientData)
{
	// Mark the node as being dirty so that it can update on display appearance
	// switch among wireframe and shaded.
	hexapodDrawOverride *ovr = static_cast<hexapodDrawOverride *>(clientData);
	if (ovr && ovr->fHexapodNode)
	{
		MHWRender::MRenderer::setGeometryDrawDirty(ovr->fHexapodNode->thisMObject());
	}
}

MHWRender::DrawAPI hexapodDrawOverride::supportedDrawAPIs() const
{
	return (MHWRender::kOpenGL | MHWRender::kDirectX11 | MHWRender::kOpenGLCoreProfile);
}

bool hexapodDrawOverride::isBounded(const MDagPath &objPath,
									 const MDagPath &cameraPath) const
{
	return false;
}

MBoundingBox hexapodDrawOverride::boundingBox(
	const MDagPath &objPath,
	const MDagPath &cameraPath) const
{
	return MBoundingBox();
}

MUserData *hexapodDrawOverride::prepareForDraw(
	const MDagPath &objPath,
	const MDagPath &cameraPath,
	const MHWRender::MFrameContext &frameContext,
	MUserData *oldData)
{
	MStatus st;

	hexapodDrawData *data = dynamic_cast<hexapodDrawData *>(oldData);
	if (!data)
	{
		data = new hexapodDrawData();
	}

	MObject hexapodNodeObj = objPath.node(&st);
	if (st.error())
	{
		return NULL;
	}


	// MPlug plugPaletteData(hexapodNodeObj, paletteNode::aOutput);
	// MObject dPaletteData;
	// st = plugPaletteData.getValue(dPaletteData);
	// MFnPluginData fnPaletteData(dPaletteData);
	// paletteData *pdata = (paletteData *)fnPaletteData.data();
	// if (!pdata)
	// {
	// 	return NULL;
	// }

	// data->palette = *(pdata->fGeometry);

	// MObject thisNode = thisMObject();

	// DisplayMask mask;

	// MPlug plug(thisNode, aDisplayPlants);
	// plug.getValue(mask.displayPlants);

	// plug.setAttribute(aDisplayHome);
	// plug.getValue(mask.displayHome);

	// plug.setAttribute(aDisplayFootPosition);
	// plug.getValue(mask.displayFootPosition);

	// plug.setAttribute(aDisplayId);
	// plug.getValue(mask.displayId);

	// plug.setAttribute(aDisplaySpeed);
	// plug.getValue(mask.displaySpeed);

	// plug.setAttribute(aDisplayFootLocal);
	// plug.getValue(mask.displayFootLocal);

	// plug.setAttribute(aDisplayAgentMatrix);
	// plug.getValue(mask.displayAgentMatrix);






	// MPlug(paletteNodeObj, paletteNode::aXPos).getValue(data->xPosition);
	// MPlug(paletteNodeObj, paletteNode::aYPos).getValue(data->yPosition);
	// MPlug(paletteNodeObj, paletteNode::aWidth).getValue(data->width);
	// MPlug(paletteNodeObj, paletteNode::aHeight).getValue(data->height);

	// MPlug(paletteNodeObj, paletteNode::aDisplayId).getValue(data->displayId);
	// MPlug(paletteNodeObj, paletteNode::aDisplayName).getValue(data->displayName);

	return data;
}

void hexapodDrawOverride::addUIDrawables(
	const MDagPath &objPath,
	MHWRender::MUIDrawManager &drawManager,
	const MHWRender::MFrameContext &context,
	const MUserData *data)
{


	hexapodDrawData *cdata = (hexapodDrawData *)data;
	if (!cdata)
	{
		return;
	}
	// if (!cdata->palette.size())
	// {
	// 	return;
	// }

	const unsigned int displayStyle = context.getDisplayStyle();

	if ((displayStyle & MHWRender::MFrameContext::kGouraudShaded) || 
		 (displayStyle & MHWRender::MFrameContext::kFlatShaded))
	{
		drawShaded(drawManager, context, cdata);
	}
	if (displayStyle & MHWRender::MFrameContext::kWireFrame)
	{
		return;
	}
}




void hexapodDrawOverride::drawShaded(
	MHWRender::MUIDrawManager &drawManager,
	const MHWRender::MFrameContext &context,
	const hexapodDrawData *cdata
	)
{

	//Get the view attributes that impact drawing
    // int oX,oy,portWidth,portHeight;
    // context.getViewportDimensions(oX,oy,portWidth,portHeight);

	drawManager.beginDrawable();
 
	// float fScaleX = cdata->width * portWidth * 0.5 ;
	// float fScaleY = cdata->height * portHeight * 0.5;
	// float fXPosition = cdata->xPosition * portWidth;
	// float fYPosition = cdata->yPosition * portHeight;

	// float centerX = fScaleX + fXPosition;
 
	// int y=fYPosition + fScaleY;

	// drawManager.setFontSize(11);

	// float h,s,v;
	// MColor textColor;

    // for (std::map<int, Paint>::const_iterator iter = cdata->palette.begin();
    //      iter != cdata->palette.end();
    //      iter++)
    // {
    //     int paintId = iter->first;
    //     const Paint & p = iter->second;
	// 	const MColor &c = p.color();
	// 	drawManager.setColor(p.color());
	// 	MPoint center(centerX,y,0.5);

	// 	drawManager.rect2d(center, MVector::yAxis, fScaleX, fScaleY, true);
	// 	y = y + fScaleY + fScaleY+4;

	// 	if (cdata->displayId || cdata->displayName) {
	// 		int red = c.r *255;
	// 		int green = c.g *255;
	// 		int blue = c.b *255;

	// 		MString txt = "";
	// 		if (cdata->displayId) {
	// 			txt+=paintId;
	// 			txt+=" ";
	// 		}
	// 		if (cdata->displayName) {
	// 			txt+=p.name() ;
	// 		}
	// 		p.color().get(MColor::kHSV,h,s,v);
	// 		textColor = v > 0.68 ? MColor(0,0,0) : MColor(1,1,1);
	// 		MPoint textPos(center + MVector(-fScaleX,0,-0.1));
	// 		MPoint colorPos(center + MVector(fScaleX,-fScaleY+2,-0.1));
			
	// 		MString coltx = "(";
	// 		coltx+=red;coltx+=",";
	// 		coltx+=green;coltx+=",";
	// 		coltx+=blue;coltx+=")";


	// 		drawManager.setColor(textColor);
	// 		drawManager.text2d(textPos,txt)	;
	// 		drawManager.text2d(colorPos,coltx, MUIDrawManager::kRight)	;



	// 	}
    // }

	drawManager.endDrawable();
}
