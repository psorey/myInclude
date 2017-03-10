#include "stdafx.h"

#include <PLineManip.h>

#include <Inventor/SbLinear.h>
#include <Inventor/SbPList.h>
#include <Inventor/SoPath.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoPrimitiveVertex.h>

#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoNurbsCurve.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoVertexProperty.h>

#include <Inventor/manips/SoTransformBoxManip.h>
#include <Inventor/manips/SoHandleBoxManip.h>

#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoSearchAction.h>

#include <Inventor/details/SoDetail.h>
#include <Inventor/details/SoFaceDetail.h>
#include <Inventor/details/SoPointDetail.h>
#include <Inventor/details/SoLineDetail.h>

#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>

#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/events/SoLocation2Event.h>

#include <Inventor/misc/SoState.h>

#include <Inventor/projectors/SbPlaneProjector.h>
#include <Inventor/projectors/SbLineProjector.h>

#include <Inventor/Win/viewers/SoWinExaminerViewer.h>

///////////////////////////////////////////////////////////////////////////////////////////
// class PManip 

// one instance only of this object?...

//		kNoEdit,
//		kMoveSelectedPoints,
//		kAddPoint,
//		kDeletePoint,
//		kAddToSelectedPoints,
//		kRemoveFromSelectedPoints,
//		kAddToSelectedEntities,
//		kRemoveFromSelectedEntities,
//		kMoveSelectedEntities,
//		kRotateSelectedEntities,
//		kDeleteSelectedEntities,

// static variables...

SoSeparator*		PManip::sRoot;
SoWinViewer*		PManip::sViewer;
int					PManip::sEditType;
SbPList*				PManip::sSelectedEntityList;		// list of strings with names of selected entities.
SoMFInt32			PManip::sSelectedPointList;
int					PManip::sPointType;
int					PManip::sMouseButton;
SbPlaneProjector*	PManip::sPlaneProjector;
SbLineProjector*	PManip::sLineProjector;
SbPlane				PManip::sProjectionPlane;
SbLine				PManip::sProjectionLine;
SbViewportRegion	PManip::sVpRegion;
SoCoordinate3*		PManip::sGhostCoords;
PEntityMgr*			PManip::sEntityMgr;
PEntity*				PManip::sSelectedEntity;
SoNode*				PManip::sSelectedNode;
int					PManip::sSelectedCoord;
SoTransform*		PManip::sTransform = NULL;
SoTransformBoxManip* PManip::sTransformBoxManip = NULL;
SoHandleBoxManip*	PManip::sHandleBoxManip = NULL;
SoPath*				PManip::sTransformPath = NULL;
bool					PManip::sShowNurbsPolylines = FALSE;

PManip::PManip(SoSeparator* parentRoot, SoWinViewer* viewer)
{
	sRoot = parentRoot;
	sViewer = viewer;
	sEntityMgr = NULL;
	sEditType = kSelectEntity;

	SoEventCallback *myEventCallback = new SoEventCallback;
	sRoot->addChild(myEventCallback);

	myEventCallback->addEventCallback(
	SoMouseButtonEvent::getClassTypeId(), 
	SetButtonDown, this);

	myEventCallback->addEventCallback(
	SoMouseButtonEvent::getClassTypeId(), 
	SetButtonUp, this);

	myEventCallback->addEventCallback(
	SoLocation2Event::getClassTypeId(), 
	MouseMoved, this);

	// create default projection parameters...
   sPlaneProjector = new SbPlaneProjector(FALSE);
	sProjectionPlane = SbPlane(SbVec3f(0,0,1),0);
   sPlaneProjector->setPlane(sProjectionPlane);
   sLineProjector = new SbLineProjector();
	sProjectionLine = SbLine(SbVec3f(0,0,0), SbVec3f(0,0,1));
   sLineProjector->setLine(sProjectionLine);

	// create a ghost line to aid in drawing entities...
	fGhostSeparator = new SoSeparator;
	fGhostSeparator->setName("GhostingSeparator");
	fGhostLine = new SoLineSet;
	sGhostCoords = new SoCoordinate3;
	sGhostCoords->point.deleteValues(0,-1);
	sRoot->addChild(fGhostSeparator);
	SoPickStyle* gPick = new SoPickStyle;
	gPick->style = SoPickStyle::UNPICKABLE;
	fGhostSeparator->addChild(gPick);
	fGhostSeparator->addChild(sGhostCoords);
	SoDrawStyle* ghostDrawStyle = new SoDrawStyle;
	ghostDrawStyle->linePattern.setValue(0xCCCC);
	fGhostSeparator->addChild(ghostDrawStyle);
	fGhostSeparator->addChild(fGhostLine);

	sSelectedEntity = NULL;
	sSelectedNode = NULL;
	sSelectedEntityList = new SbPList;
	sEntityMgr = new PEntityMgr(sRoot);
	//sEntityMgr->Create();
}


void PManip::SetEditType(int editType)
{
	if (sSelectedEntity != NULL)
	{
		sSelectedEntity->UnHighlight();
		sSelectedEntity = NULL;
		sSelectedNode = NULL;
	}
	sEditType = editType;
}


bool PManip::DoEdit(void)
{						
	return TRUE;
}


void PManip::SetButtonDown(void *data, SoEventCallback *eventCB)
{
   const SoEvent *ev = eventCB->getEvent();

	if (FALSE == (ev->isOfType(SoMouseButtonEvent::getClassTypeId())))
		return;	

   const SoPickedPoint *picked_point; 
	PManip* theManip = (PManip*) data;

	if (sEditType == kNoEdit)
	{
		eventCB->setHandled();
		return;
	}

	if(SO_MOUSE_PRESS_EVENT(ev, BUTTON1)) 
	{
		sMouseButton = 1;
	}

	else if (SO_MOUSE_PRESS_EVENT(ev, BUTTON2)) 
	{
		sMouseButton = 2;
	}

	else 
	{
		eventCB->setHandled();
		return; /// test this...
	}
	
	SbVec3f projectedPosition;
	theManip->ProjectMouseToPlane(ev, projectedPosition);

	// Nothing was picked.....
	if ((picked_point = eventCB->getPickedPoint()) == NULL  || sEditType == kAddPoint)  
	{ 
		// nothing was picked...
		switch (sEditType)
		{
		case kCreatePolyline:
			{
				if (sMouseButton == 1)
				{
					sSelectedEntity = sEntityMgr->AddEntity(new PPolyline, PEntity::kPolyline);
					sSelectedEntity->AddPoint(projectedPosition);
					sEditType = kAddPoint;
					sGhostCoords->point.setValue(projectedPosition);
				}
				else if (sMouseButton == 2)
				{ } // ignore the mouse2 press... 
			}
			break;
		case kCreateNurbsCurve:
			{
				if (sMouseButton == 1)
				{
					sSelectedEntity = sEntityMgr->AddEntity(new PNurbsCurve, PEntity::kNurbsCurve);
					sSelectedEntity->AddPoint(projectedPosition);
					sEditType = kAddPoint;
					sGhostCoords->point.setValue(projectedPosition);
				}
				else if (sMouseButton == 2)
				{ } // ignore the mouse2 press... 
			}
			break;
		case kAddPoint:
			{
				if (sMouseButton == 1)
				{
					if (sSelectedEntity != NULL)
					{
						sSelectedEntity->AddPoint(projectedPosition);
						sGhostCoords->point.setValue(projectedPosition);
					}
					else
					{
						//sEditType = kSelectEntity;
					}	
				}
				else if (sMouseButton == 2)
				{
					// end the current polyline and start a new one... NO !!! !!!
					sEditType = kNoEdit;
					//sEditType = kSelectEntity; !!!
					if (sSelectedEntity != NULL)
					{
						sSelectedEntity->UnHighlight();
						sSelectedEntity = NULL;
						sGhostCoords->point.deleteValues(0, -1);
					}
				}
			}
			break;
		case kMovePoint:
			{
				//SbVec3f projectedPosition;
				//theManip->ProjectMouse(ev, projectedPosition);
				//if (sSelectedEntity != NULL)
				//{
				//	sSelectedEntity->UnHighlight();
				//	sSelectedEntity = NULL;
				//}
			}
			break;
		case kTransformBox:
			{
				if (sTransformBoxManip != NULL)
				{
					sTransformBoxManip->replaceManip(sTransformPath, sTransform);
					sTransformBoxManip = NULL;
					sTransformPath = NULL;
					sSelectedEntity->TransformCoordinates();
				}
			}
			break;
		case kHandleBox:
			{
				if (sHandleBoxManip != NULL)
				{
					sHandleBoxManip->replaceManip(sTransformPath, sTransform);
					sHandleBoxManip = NULL;
					sTransformPath = NULL;
					sSelectedEntity->TransformCoordinates();
				}
			}
			break;
		default:
			{
				// nothing was picked so UNSELECT everything...
				TRACE("we're unselecting\n");
				if (sSelectedEntity != NULL)
				{
					sSelectedEntity->UnHighlight();
					sSelectedEntity = NULL;
					// other stuff too? ...!!!
				}
				sSelectedNode = NULL;
			}
		}
		//eventCB->setHandled();
		return;
	}  


	const		SoPath *dpath = picked_point->getPath();
	const		SoDetail *detail = picked_point->getDetail();
	SoNode	*pickedNode = dpath->getTail();
	int		pathLength = dpath->getLength();
	SoGroup	*parent = (SoGroup *)dpath->getNode(pathLength -2);

	SoNode* tNode;
	int whichNode;

	for (int i = 0; i < pathLength; i++)
	{
		tNode = dpath->getNode(i);
		TRACE("current node name is %s\n", tNode->getName().getString());
		
		if (tNode->getName() == SbName("EntityRoot"))
		{
			whichNode = i + 1;
			i = pathLength + 4;
		}
	}
	if (i == pathLength) 
	{
		eventCB->setHandled();
		return;  // we didn't pick an PEntity entity.
	}

	// We picked an entity, so....

	SoSeparator *pickedEntitySep = (SoSeparator*)dpath->getNode(whichNode);
	// do we need to check if it's really a SoSeparator node??? !!!
	if (pickedEntitySep == NULL) 
	{
		eventCB->setHandled();
		return;
	}

	const char *nodeName = pickedEntitySep->getName().getString();
	PEntity* pickedEntity = theManip->sEntityMgr->GetEntityFromName(nodeName);

	if (sSelectedEntity != NULL)
	{
		sSelectedEntity->UnHighlight();
		sSelectedEntity = NULL;
		sSelectedNode = NULL;
	}
	pickedEntity->Highlight();
	
	sSelectedEntity = pickedEntity;	// picked PEntity class object...
	sSelectedNode   = pickedNode;		// picked SoNode...
	
	switch (sEditType)
	{
	case kCreatePolyline:
		{
		}
		break;
	case kCreateNurbsCurve:
		{
		}
		break;
	case kAddPoint:
		{
		}
		break;
	case kDeleteEntity:
		{
			theManip->sEntityMgr->RemoveEntity(pickedEntity);
			eventCB->setHandled();
		}
		break;
	case kMovePoint:
		{
			// it's a point...
			if (sSelectedNode->isOfType(SoPointSet::getClassTypeId())) 
			{
				sSelectedCoord = ((SoPointDetail *)detail)->getCoordinateIndex();	
				if (sMouseButton == 2)
				{
					SbVec3f tCoord = sSelectedEntity->fCoords->point[sSelectedCoord];
					SbVec3f tCoord2 = SbVec3f (tCoord[0], tCoord[1], tCoord[2] + 10);
					sProjectionLine = SbLine(tCoord, tCoord2);
					sLineProjector->setLine(sProjectionLine);
				}
				else if (sMouseButton == 1)
				{
					SbVec3f tCoord = sSelectedEntity->fCoords->point[sSelectedCoord];
					SbVec3f tCoord2 = SbVec3f (tCoord[0] + 10, tCoord[1], tCoord[2]);
					SbVec3f tCoord3 = SbVec3f (tCoord[0] + 10, tCoord[1] + 10, tCoord[2]);
					sProjectionPlane = SbPlane(tCoord, tCoord2, tCoord3);
					sPlaneProjector->setPlane(sProjectionPlane);
				}
				eventCB->setHandled();
			}
			else sSelectedCoord = - 1;		
		}
		break;
	case kTransformBox:
		{
			if (sTransformBoxManip == NULL)
			{
				sTransformPath = PEntityMgr::FindPathToNode(sSelectedEntity->GetTransform());
				ASSERT (sTransformPath != NULL);
				sTransform = (SoTransform*)sTransformPath->getTail();
				ASSERT (sTransform != NULL);
				sTransform->ref();
				sTransformBoxManip = new SoTransformBoxManip;
				sTransformBoxManip->replaceNode(sTransformPath);
			}
		}
		break;
	case kHandleBox:
		{
			if (sHandleBoxManip == NULL)
			{
				sTransformPath = PEntityMgr::FindPathToNode(sSelectedEntity->GetTransform());
				ASSERT (sTransformPath != NULL);
				sTransform = (SoTransform*)sTransformPath->getTail();
				ASSERT (sTransform != NULL);
				sTransform->ref();
				sHandleBoxManip = new SoHandleBoxManip;
				sHandleBoxManip->replaceNode(sTransformPath);
			}
		}
		break;
	default:
		{
		}
	}
	//eventCB->setHandled();
	return;
}


void
PManip::SetButtonUp(void *data, SoEventCallback *eventCB)
{
	const SoEvent *ev = eventCB->getEvent();
	if (ev == NULL) return;

	if (FALSE == (ev->isOfType(SoMouseButtonEvent::getClassTypeId())))
		return;	
	
	PManip* theManip = (PManip*) data;
	if(SO_MOUSE_RELEASE_EVENT(ev, BUTTON1)) 
	{
		//if (sMouseButton != 0)
		//{
			sMouseButton = 0;
			if (sEditType == kMovePoint)
			{
				// unselect everything...but not the selection set if we've got one...
				if (sSelectedNode != NULL)
				{
					//sSelectedEntity->UnHighlight();
					//sSelectedCoord = -1;
					//sSelectedNode = NULL;
					//sSelectedEntity = NULL;
				}
			}
		//}
   }
	else if (SO_MOUSE_RELEASE_EVENT(ev, BUTTON2)) 
	{
		//if (sMouseButton != 0)
		//{
			sMouseButton = 0;
			if (sEditType == kMovePoint)
			{
				// unselect everything...but not the selection set if we've got one...
				// sSelectedCoord = -1;
				// sSelectedNode = NULL;
				// sSelectedEntity = NULL;
			}
		//}
   }
	if (sSelectedEntity != NULL)
		sSelectedEntity->Update();
}


void
PManip::MouseMoved(void *data, SoEventCallback *eventCB)
{
	const SoEvent *ev = eventCB->getEvent();
		
	if (FALSE == (ev->isOfType(SoLocation2Event::getClassTypeId())))
		return;

	PManip* theManip = (PManip*) data;
	
	SbVec3f projectedPosition;
	theManip->ProjectMouseToPlane(ev, projectedPosition);	
	
	// in case we're working in the Z-axis...
	//SbVec3f aboveMousePosition( projectedPosition[0], projectedPosition[1], projectedPosition[2] + 10);
	//sProjectionLine = SbLine(projectedPosition, aboveMousePosition);
   //sLineProjector->setLine(sProjectionLine);
	
	switch (sEditType)
	{
	case kAddPoint:
		if (sSelectedEntity != NULL)
		{
			sGhostCoords->point.setValue(sSelectedEntity->fCoords->point[sSelectedEntity->fCoords->point.getNum() - 1]);
			sGhostCoords->point.set1Value(1, projectedPosition);
		}
		break;
	case kMovePoint:
		if (sSelectedEntity != NULL && sMouseButton == 1)
		{
			SbVec3f newPosition;
			theManip->ProjectMouseToPlane(ev, newPosition);
			sSelectedEntity->fCoords->point.set1Value(sSelectedCoord, newPosition);
			sSelectedEntity->Update();		
		}
		else if (sSelectedEntity != NULL && sMouseButton == 2)
		{
			SbVec3f vertPosition;
			theManip->ProjectMouseToLine(ev, vertPosition);
			sSelectedEntity->fCoords->point.set1Value(sSelectedCoord, vertPosition);
			sSelectedEntity->Update();		
		}
		break;
	default:
		{
		
		}
	}
}


void PManip::ProjectMouseToPlane(const SoEvent* ev, SbVec3f& result)
{	
	this->ExtractViewingParams();
	result = sPlaneProjector->project(ev->getNormalizedPosition(sVpRegion));
}


void PManip::ProjectMouseToLine(const SoEvent* ev, SbVec3f& result)
{	
	this->ExtractViewingParams();
	result = sLineProjector->project(ev->getNormalizedPosition(sVpRegion));
}


void PManip::LoadEntitiesFromIvFile(CString filename)
{
	sEntityMgr->LoadEntitiesFromIvFile(filename);
	
	// debug...
	SoWriteAction wa;
	wa.getOutput()->openFile("Root.iv");
	wa.apply(sRoot);
	wa.getOutput()->closeFile();	
}


void PManip::ExtractViewingParams(void)
{
	SoCamera *camera = sViewer->getCamera();
	if (camera->isOfType(SoPerspectiveCamera::getClassTypeId()))
	{
		SoPerspectiveCamera* pCamera = (SoPerspectiveCamera *)camera;
		SoWinExaminerViewer* eViewer = (SoWinExaminerViewer *)sViewer;

		sVpRegion = eViewer->getViewportRegion(); 
		float aR = sVpRegion.getViewportAspectRatio();
		SbViewVolume vV = pCamera->getViewVolume(aR);
		sPlaneProjector->setViewVolume(vV);
		sLineProjector->setViewVolume(vV);
	}
	else if (camera->isOfType(SoOrthographicCamera::getClassTypeId()))
	{
		SoOrthographicCamera* oCamera = (SoOrthographicCamera *)camera;
		SoWinExaminerViewer* eViewer = (SoWinExaminerViewer *)sViewer;
		sVpRegion = eViewer->getViewportRegion(); 
		float aR = sVpRegion.getViewportAspectRatio();
		SbViewVolume vV = oCamera->getViewVolume(aR);
		sPlaneProjector->setViewVolume(vV);
		sLineProjector->setViewVolume(vV);
	} 

	/*
	if (ha == NULL) 
	{
		// If the action is NULL, use default values for view info.
		sViewVolume.ortho(-1,1,-1,1,1,10);
		sVpRegion = SbViewportRegion(1,1);
	}
   else 
	{
		// Get the view info from the action.
		SoState *state = ha->getState();
		sViewVolume = SoViewVolumeElement::get(state);
		sVpRegion = SoViewportRegionElement::get(state);
	}
	*/

}

PEntity*	PManip::GetSelectedEntity(void)
{
	return sSelectedEntity;
}


