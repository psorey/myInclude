// Copyright Paul Sorey 2000
// "myInclude/PEntity.cpp"

#include "stdafx.h"
#include <PEntity.h>
#include <PEntityMgr.h>
#include <PManip.h>

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

///////////////////////////////////////////////////////////////////////////////////////
// class PState		current material (line, point, surface), drawStyle, shapeHints, 
//							materialBinding,   

 
///////////////////////////////////////////////////////////////////////////////////////
// class PEntity 

#define BY_ANGLE 0
#define BY_LENGTH 1

SbColor	PEntity::sUnHighlightColor = SbColor(.4, .7, .5);
SbColor	PEntity::sHighlightColor = SbColor(1.0 ,1.0 ,1.0);

// for nurbs curve polyline generation...
int		PEntity::sPolylineGenerationStyle = BY_LENGTH;
float	PEntity::sPolylineGenerationDistance = .4;
int		PEntity::sPolylineDivisions = 200;   // if using BY_NUMBER_DIVISIONS for polyline generation
bool    PEntity::sShowPolylineNotNurbs = FALSE;
int     PEntity::sPolylinePointSize = 3;


PEntity::PEntity(void)
{

}


void PEntity::Create(void)
{
	TRACE("PEntity::CreateBasics\n");
	fSeparator = new SoSeparator;
	// the name of this separator is set by PEntityMgr when adding it to the graph
	// and is used to store the entity's "handle".
	fSeparator->ref();
	SoPickStyle* pick = new SoPickStyle;
    pick->setName("linePick");
	pick->style.setValue(SoPickStyle::UNPICKABLE);
	fTransform = new SoTransform;
	fTransform->setName("Transform");
	fSeparator->addChild(fTransform);
	fCoords = new SoCoordinate3;
	fCoords->setName("Coords");
	fCoords->point.deleteValues(0, -1);
	fConnectedCoords = NULL;
	fConnectedCoords2 = NULL;
	fSeparator->addChild(fCoords);
	fDrawStyle = new SoDrawStyle;
	fDrawStyle->setName("DrawStyle");
	fMaterial = new SoMaterial;
	fMaterial->setName("Material");
	fMaterial->diffuseColor.setValue(sHighlightColor);
	fSeparator->addChild(fMaterial);
	fDrawStyle->pointSize.setValue(7);
	fSeparator->addChild(fDrawStyle);
	fSeparator->addChild(pick);
	SoSeparator* pointSep = new SoSeparator;
	SoPickStyle* pointPick = new SoPickStyle;
    pointPick->setName("pointPick");
	
	fPointSet = new SoPointSet;
	fPointSet->setName("PointSet");
	fPointSet->setOverride(TRUE);
	fSeparator->addChild(pointSep);
	pointSep->addChild(pointPick);
	pointSep->addChild(fPointSet);
	fLineSet = new SoLineSet;             // !!!
	fSeparator->addChild(fLineSet);      // !!!
	//fLineSet = NULL;	            
}


void	PEntity::SetName(CString name)
{
	fName = name;
	fSeparator->setName(LPCTSTR(name));
}


const char*	PEntity::GetName(void)
{
	return LPCTSTR(fName);
}

int PEntity::GetType(void)
{
   return fEntityType;
}

void PEntity::SetLinesPickable(bool TRUEFALSE)
{
    // do nothing -- it's for polylines only right now...
}

void PEntity::DeletePoint(int whichPoint)
{
	fCoords->point.deleteValues(whichPoint,1);
}


void PEntity::AddPoint(SbVec3f point)
{
	TRACE("PEntity::AddPoint()\n");
	int num = fCoords->point.getNum();
	fCoords->point.set1Value(num, point);
}


SoSeparator* PEntity::GetNode(void)
{
	return fSeparator;
}


void PEntity::Highlight(void)
{
	fDrawStyle->linePattern.setValue(0xCCCC);
	fMaterial->diffuseColor.setValue(sHighlightColor);
	fPointSet->setOverride(FALSE);
}


void PEntity::UnHighlight(void)
{
	fDrawStyle->linePattern.setValue(0xFFFF);
	fMaterial->diffuseColor.setValue(sUnHighlightColor);
	fPointSet->setOverride(TRUE);
}


void PEntity::Update(void)
{
	
}

void PEntity::AddCoordsToFile(const SoWriteAction wa)
{
	// do nothing:  we only want to write nurbs polylines...
}

void PEntity::WriteCoordsToFile(CString filename, float beginElev, float endElev)
{
	int num = fCoords->point.getNum() - 1;
	TRACE("writing extracted coords to a file\n");
	for (int i = 0; i < num; i++)
	{
		if (fabs(fCoords->point[i][0] - fCoords->point[i + 1][0]) < .001)
		{	
			SbVec3f point = fCoords->point[i + 1];
			point[0] += .001;
			fCoords->point.set1Value(i + 1, point);
			i--;
			TRACE("fixed a segment\n");
		}
	}
	SoWriteAction wa;
	wa.getOutput()->openFile(filename);
	wa.apply(fCoords);
	wa.getOutput()->closeFile();
}




SoCoordinate3* PEntity::GetCoords()
{
	return fCoords;
}



void PEntity::SetConnectedCoords(SoCoordinate3* outsideCoords1, SoCoordinate3* outsideCoords2)
{
	//fConnectedCoords = outsideCoords1;
	//fConnectedCoords2 = outsideCoords2;
}



float PEntity::GetLength()
{
	return 0.0;  // if GetLength() is undefined for entity type...
}


void	PEntity::TransformCoordinates() // MoveEntity() ???
{
	int numCoords = fCoords->point.getNum();
	SbMatrix matrix;
	matrix.setTransform(fTransform->translation.getValue(), fTransform->rotation.getValue(), 
		fTransform->scaleFactor.getValue(), fTransform->scaleOrientation.getValue(), 
		fTransform->center.getValue());
	for (int i = 0; i < numCoords; i++)
	{
		SbVec3f tempCoord = fCoords->point[i];
		SbVec3f newCoord;
		matrix.multVecMatrix(tempCoord, newCoord);
		fCoords->point.set1Value(i, newCoord);
	}
	matrix.makeIdentity();
	fTransform->setMatrix(matrix);
	this->Update();
	//// if we need to get the world space coordinates of the points...    
	//SoWinExaminerViewer * eViewer = (SoWinExaminerViewer *)sViewer;
	//SbViewportRegion myRegion = eViewer->getViewportRegion(); 
	//SoGetMatrixAction matrixAction(myRegion);
	//matrixAction.apply((SoPath *)dpath);
	//SbMatrix matrix = matrixAction.getMatrix();
	//SbVec3f tempPoint = picked_point->getObjectPoint();
	//matrix.multVecMatrix(tempPoint, currentLocation);
}

void PEntity::CreateFromShapeCoords(SoCoordinate3* coords)
{

}

////////////////////////////////////////////////////////////////////////////////////////
// class PPolyline : public PEntity 

PPolyline::PPolyline(void)
{
	TRACE("PPolyline constructor\n");
	PEntity::Create();
	fEntityType = kPolyline;
	fLineSet = new SoLineSet;
	fLineSet->setName("LineSet");
	fSeparator->addChild(fLineSet);
}


// create a new entity from a special IV file... 
PPolyline::PPolyline(SoSeparator* entitySep)
{
	TRACE("Ppolyline alternate constructor .. no base class constructor!!\n");
	fSeparator = entitySep;
	fCoords = (SoCoordinate3*)PEntityMgr::FindNodeByName(entitySep, "Coords");
	fTransform = (SoTransform*)PEntityMgr::FindNodeByName(entitySep, "Transform");
	if (fTransform == NULL)
	{
		fTransform = new SoTransform;
		fTransform->setName("Transform");
		fSeparator->insertChild(fTransform,1);
	}
	fPointSet = (SoPointSet*)PEntityMgr::FindNodeByName(entitySep, "PointSet");
	fLineSet = (SoLineSet*)PEntityMgr::FindNodeByName(entitySep, "LineSet");
	fMaterial = (SoMaterial*)PEntityMgr::FindNodeByName(entitySep, "Material");
	fDrawStyle = (SoDrawStyle*)PEntityMgr::FindNodeByName(entitySep, "DrawStyle");
	fEntityType = kPolyline;
}


PPolyline::~PPolyline(void)
{
	if (fSeparator != NULL)
	{
		fSeparator->removeAllChildren();
		fSeparator->unref();
	}
}


void PPolyline::GetPolylineCoords(SoMFVec3f& coords)
{
	coords.deleteValues(0, -1);
	int num = fCoords->point.getNum();
	
	// use the field method???..
	for (int i = 0; i < num; i++)
	{
		coords.set1Value(i, fCoords->point[i]);
	}
}


void PPolyline::SetLinesPickable(bool TRUEFALSE)
{
   SoSearchAction search;
   SoPath*      path;

   search.setName(SbName("linePick"));
   search.setInterest(SoSearchAction::FIRST);
   search.apply(fSeparator); // the top node of the entity

   path = search.getPath();
   SoPickStyle* pickNode = (SoPickStyle *)path->getTail();
   if (TRUEFALSE == TRUE)
      pickNode->style = (SoPickStyle::SHAPE);
   else
      pickNode->style = (SoPickStyle::UNPICKABLE);
}


////////////////////////////////////////////////////////////////////////////////////////
// class PNurbsCurve : public PEntity 

SbVec3f PNurbsCurve::sPoint1;
SbVec3f PNurbsCurve::sPoint2;
SbVec3f PNurbsCurve::sPoint3;
SbVec3f PNurbsCurve::sPoint4;
int		PNurbsCurve::sCount;


PNurbsCurve::PNurbsCurve()
{
	TRACE("PNurbsCurve::PNurbsCurve\n");
	PEntity::Create(); // call base class constructor...
	fEntityType = kNurbsCurve;
	fComplexity = new SoComplexity;
	fShowNurbs = FALSE;
	fShowPolyline = TRUE;
	fShowMarkers = FALSE;

	fComplexity = new SoComplexity;          // !!! these were commented out...
	fComplexity->setName("Complexity");
	fComplexity->value.setValue(.99999999);
	fSeparator->addChild(fComplexity);
    //

	fNurbsCurve = new SoNurbsCurve;
	fNurbsCurve->setName("NurbsCurve");
	fNurbsCurve->knotVector.set1Value(0,0);
	fNurbsCurve->knotVector.set1Value(1,0);
	fNurbsCurve->knotVector.set1Value(2,0);
	fNurbsCurve->knotVector.set1Value(3,0);
	fNurbsCurve->knotVector.set1Value(4,1);
	fNurbsCurve->knotVector.set1Value(5,1);
	fNurbsCurve->knotVector.set1Value(6,1);
	fNurbsCurve->knotVector.set1Value(7,1);

	fSeparator->addChild(fNurbsCurve);
	fPolylineSeparator = new SoSeparator;
	fPolylineSeparator->setName("PolylineSeparator");
	fNurbsPolylineDrawStyle = new SoDrawStyle;
	fNurbsPolylineDrawStyle->pointSize.setValue(sPolylinePointSize);
	fNurbsPolylineMaterial = new SoMaterial;
	fNurbsPolylineMaterial->setName("NurbsPolylineMaterial");
	fNurbsPolylineMaterial->diffuseColor.setValue(1,.2,0);
	fSeparator->addChild(fNurbsPolylineMaterial);
	fSeparator->addChild(fNurbsPolylineDrawStyle);
	fSeparator->addChild(fPolylineSeparator);
	
	//fPolylineLines = NULL;
	//fPolylinePoints = NULL;
	//fPolylineCoords = NULL;

	fPolylineLines = new SoLineSet;
	fPolylinePoints = new SoPointSet;
	fPolylineCoords = new SoCoordinate3;

	fPolylineLines->setName("PolylineLines");
	fPolylinePoints->setName("PolylinePoints");
	fPolylineCoords->setName("PolylineCoords");
	fPolylineSeparator->addChild(fPolylineCoords);
	fPolylineSeparator->addChild(fPolylineLines);
	fPolylineSeparator->addChild(fPolylinePoints);
}


// create a new entity from a special IV file... 
PNurbsCurve::PNurbsCurve(SoSeparator* entitySep)
{
	TRACE("PNurbsCurve alternate constructor .. no base class constructor!!\n");
	fSeparator = entitySep;

	fCoords = (SoCoordinate3*)PEntityMgr::FindNodeByName(entitySep, "Coords");
	fTransform = (SoTransform*)PEntityMgr::FindNodeByName(entitySep, "Transform");
	
	// temporary ???
	if (fTransform == NULL)
	{
		fTransform = new SoTransform;
		fTransform->setName("Transform");
		fSeparator->insertChild(fTransform, 1);
	}
	fPointSet  = (SoPointSet*)PEntityMgr::FindNodeByName(entitySep, "PointSet");
	fLineSet   = (SoLineSet*)PEntityMgr::FindNodeByName(entitySep, "LineSet");
	fDrawStyle = (SoDrawStyle*)PEntityMgr::FindNodeByName(entitySep, "DrawStyle");
	fMaterial  = (SoMaterial*)PEntityMgr::FindNodeByName(entitySep, "Material");

	fNurbsCurve = (SoNurbsCurve*)PEntityMgr::FindNodeByName(entitySep, "NurbsCurve");
	fComplexity = (SoComplexity*)PEntityMgr::FindNodeByName(entitySep, "Complexity");

	fPolylineSeparator = (SoSeparator*)PEntityMgr::FindNodeByName(entitySep, "PolylineSeparator");
	fPolylineCoords = (SoCoordinate3*)PEntityMgr::FindNodeByName(entitySep, "PolylineCoords");
	fPolylinePoints = (SoPointSet*)PEntityMgr::FindNodeByName(entitySep, "PolylinePoints");
	fPolylineLines = (SoLineSet*)PEntityMgr::FindNodeByName(entitySep, "PolylineLines");
	fNurbsPolylineMaterial = (SoMaterial*)PEntityMgr::FindNodeByName(entitySep, "NurbsPolylineMaterial");
	fNumPolylineDivisions = 222; // !!! just added this
	//fShowNurbs = TRUE;
	//fShowPolyline = FALSE;
	fEntityType = kNurbsCurve;
}


PNurbsCurve::~PNurbsCurve(void)
{

}

void PNurbsCurve::DeletePoint(int whichPoint)
{
	PEntity::DeletePoint(whichPoint);

	int numCoords = fCoords->point.getNum();

	// update the nurbs curve fields...
	//if (numCoords > 5) 
	{
		fNurbsCurve->knotVector.setValue(0);
		fNurbsCurve->knotVector.set1Value(0,0);
		fNurbsCurve->knotVector.set1Value(1,0);
		fNurbsCurve->knotVector.set1Value(2,0);
		fNurbsCurve->knotVector.set1Value(3,0);
		int i;
		for (i = 1; i <= numCoords - 3; i++) 
		{
			fNurbsCurve->knotVector.set1Value(i + 3, i);
		}
		fNurbsCurve->knotVector.set1Value(i + 3, i - 1);
		fNurbsCurve->knotVector.set1Value(i + 4, i - 1);
		fNurbsCurve->knotVector.set1Value(i + 5, i - 1);
		fNurbsCurve->numControlPoints.setValue(numCoords);

		this->GetPolylineCoords();    
	}	
}

void PNurbsCurve::AddPoint(SbVec3f point)
{
	PEntity::AddPoint(point);
	TRACE("PNurbsCurve::AddPoint()\n");

	int numCoords = fCoords->point.getNum();

	// update the nurbs curve fields...
	if (numCoords > 5) 
	{
		fNurbsCurve->knotVector.setValue(0);
		fNurbsCurve->knotVector.set1Value(0,0);
		fNurbsCurve->knotVector.set1Value(1,0);
		fNurbsCurve->knotVector.set1Value(2,0);
		fNurbsCurve->knotVector.set1Value(3,0);
		int i;
		for (i = 1; i <= numCoords - 3; i++) 
		{
			fNurbsCurve->knotVector.set1Value(i + 3, i);
		}
		fNurbsCurve->knotVector.set1Value(i + 3, i - 1);
		fNurbsCurve->knotVector.set1Value(i + 4, i - 1);
		fNurbsCurve->knotVector.set1Value(i + 5, i - 1);
		fNurbsCurve->numControlPoints.setValue(numCoords);

		this->GetPolylineCoords();    
	}	
}

void PNurbsCurve::Update(void)
{
	int tempMouseButton = PManip::sMouseButton;  // fool the next function...
	PManip::sMouseButton = 0;
	
	this->GetPolylineCoords();
	
	PManip::sMouseButton = tempMouseButton;

}


void PNurbsCurve::DisplayAsPolyline(int numPoints)
{

}


void PNurbsCurve::NurbsCurveLineSegCB(void *data, SoCallbackAction *action, const SoPrimitiveVertex *v1,
          const SoPrimitiveVertex *v2)
{
	sPolylineGenerationStyle = BY_LENGTH;
	sPolylineGenerationDistance = .4;

	PNurbsCurve* theNurbsCurve = (PNurbsCurve*)data;
	SbVec3f point = v1->getPoint();
	sCount++;
	TRACE("count = \n", sCount);

	int count = theNurbsCurve->fPolylineCoords->point.getNum();
	
	if (sCount == 1)
	{
		theNurbsCurve->fPolylineCoords->point.set1Value(count, point);
		sPoint1 = point;
		return;
	}
	
	if (sPolylineGenerationStyle == BY_LENGTH) {
		float length = theNurbsCurve->GetLength();
   	    PEntity::sPolylineGenerationDistance = length / PEntity::sPolylineDivisions;
   	    theNurbsCurve->Update();
		float dist = theNurbsCurve->Distance3d(point, theNurbsCurve->fPolylineCoords->point[count - 1]);
		if (dist > theNurbsCurve->fPolylineGenerationDistance)
		{
			TRACE("wanted = %f     distance = %.4f\n", theNurbsCurve->fPolylineGenerationDistance, dist);
			theNurbsCurve->fPolylineCoords->point.set1Value(count, point);
		}
		return;
	}
	
	
	else if (sPolylineGenerationStyle == BY_ANGLE)
	{
		if (sCount == 2)
		{
			//theNurbsCurve->fPolylineCoords->point.set1Value(count,point);
			sPoint2 = point;
		}

		else if (sCount == 3)
			sPoint3 = point;
		
		else
		{
			sPoint4 = point;
			float angle = theNurbsCurve->Get3dSegmentAngle();
			TRACE("segment angle = %f\n", angle);

			if (fabs(angle) > .04)
			{
				theNurbsCurve->fPolylineCoords->point.set1Value(count, point);
				sPoint1 = sPoint3;
				sPoint2 = sPoint4;
				sPoint3 = sPoint4;
			}
			else
			{
				sPoint3 = sPoint4;
			}
		}
	}
}

void PNurbsCurve::GetPolylineCoords(void)
{
	//if (PManip::sMouseButton != 0) return;
	//if (PManip::sShowNurbsPolylines == FALSE) return;
   float length = this->GetLength();
	TRACE("length = %f\n", length);
	fNumPolylineDivisions = 100; // !!!
	TRACE("divisions desired = %d\n", fNumPolylineDivisions);
   fPolylineGenerationDistance = length / fNumPolylineDivisions; // PEntity::sPolylineDivisions;
   // this->Update();
	PEntity::sPolylineGenerationDistance = 2.1;
	SoCallbackAction ca;  
	ca.addLineSegmentCallback(SoNurbsCurve::getClassTypeId(), NurbsCurveLineSegCB, this);	
	

	sCount = 0;
	ca.apply(fSeparator);
	
	// if we're connected to the outside world, update it...

	/*
	if(fConnectedCoords != NULL)
	{
		fConnectedCoords->point.deleteValues(0, -1);
		int num = fPolylineCoords->point.getNum();
		for(int i = 0; i < num; i++)
		{
			TRACE("adding a polyline coordinate\n");
			fConnectedCoords->point.set1Value(i, fPolylineCoords->point[i]);  // will this trigger a callback each time.???
		}
	}
	if(fConnectedCoords2 != NULL)
	{
		int num = fCoords->point.getNum();
		for(int i = 0; i < num; i++)
		{
			fConnectedCoords2->point.set1Value(i, fCoords->point[i]);  // will this trigger a callback each time.???
		}
	}
	*/
}




void PNurbsCurve::AddCoordsToFile(const SoWriteAction wa)
{
	int num = fPolylineCoords->point.getNum() - 1;
	TRACE("writing extracted coords to a file\n");
	for (int i = 0; i < num; i++)
	{
		if (fabs(fPolylineCoords->point[i][0] - fPolylineCoords->point[i + 1][0]) < .001)
		{	
			SbVec3f point = fPolylineCoords->point[i + 1];
			point[0] += .001;
			fPolylineCoords->point.set1Value(i + 1, point);
			i--;
			TRACE("fixed segment %d\n", i);
		}
	}
	//wa.apply(fPolylineCoords);
}


/*
void PNurbsCurve::WriteCoordsToFile(CString filename)
{
	int num = fPolylineCoords->point.getNum() - 1;
	TRACE("writing extracted coords to a file\n");
	for (int i = 0; i < num; i++)
	{
		if (fabs(fPolylineCoords->point[i][0] - fPolylineCoords->point[i + 1][0]) < .001)
		{	
			SbVec3f point = fPolylineCoords->point[i + 1];
			point[0] += .001;
			fPolylineCoords->point.set1Value(i + 1, point);
			i--;
			TRACE("fixed segment %d\n", i);
		}
	}
	SoWriteAction wa;
	wa.getOutput()->openFile(filename);
	wa.apply(fPolylineCoords);
	wa.getOutput()->closeFile();
}
*/

void PNurbsCurve::WriteCoordsToFile(CString filename, float beginElev, float endElev)
{
	int num = fPolylineCoords->point.getNum() - 1;
	TRACE("writing extracted coords to a file\n");
	for (int i = 0; i < num; i++)
	{
		if (fabs(fPolylineCoords->point[i][0] - fPolylineCoords->point[i + 1][0]) < .001)
		{	
			SbVec3f point = fPolylineCoords->point[i + 1];
			point[0] += .001;
			fPolylineCoords->point.set1Value(i + 1, point);
			i--;
			TRACE("fixed segment %d\n", i);
		}
   }
   if (beginElev != NULL || endElev != NULL)
   {
      num = fPolylineCoords->point.getNum();
      for (int i = 0; i < num; i++)
	   {
         float ratio = (float)i / (float)num;
         float currentElev = (endElev - beginElev) * ratio + beginElev;
	   
	      SbVec3f point = fPolylineCoords->point[i];
		   point[2] += currentElev;
		   fPolylineCoords->point.set1Value(i, point);
	   }
   }
	SoWriteAction wa;
	wa.getOutput()->openFile(filename);
	wa.apply(fPolylineCoords);
	wa.getOutput()->closeFile();
}






SoCoordinate3* PNurbsCurve::GetCoords()
{
	return fPolylineCoords;
}


float PNurbsCurve::GetLength()
{
	float length = 0.0;
	if (fCoords->point.getNum() < 2)
		return 0.0;  // if GetLength() is undefined for entity type...
	else
	{
		for (int i = 0; i < fCoords->point.getNum() - 1; i++)
		{
			length += Distance3d(fCoords->point[i], fCoords->point[i+1]);
		}
	}
	return length;
}


float PNurbsCurve::Get3dSegmentAngle(void)
{
	float radians;
	SbVec3f axis;
	SbRotation rot = SbRotation(sPoint2 - sPoint1, sPoint4 - sPoint3);
	rot.getValue(axis, radians);
	return radians;
}

float PNurbsCurve::Angle3d(SbVec3f pt1, SbVec3f pt2, SbVec3f testPt)
{
	float radians;
	SbVec3f axis;
	SbRotation rot = SbRotation((pt2 - pt1), (testPt - pt2));
	rot.getValue(axis, radians);
	return radians;
}

double PNurbsCurve::Distance3d(SbVec3f pt1, SbVec3f pt2)
{
	double   distance;
	distance = sqrt((pt2[0] - pt1[0])*(pt2[0] - pt1[0]) \
		  +(pt2[1] - pt1[1])*(pt2[1] - pt1[1]));
	double distanceZ = sqrt((distance * distance) + ((pt2[2] - pt1[2]) * (pt2[2] - pt1[2])));
	return distanceZ;
}

void PNurbsCurve::CreateFromShapeCoords(SoCoordinate3* coords)
{

}

/*
void PNurbsCurve::TransformCoordinates()
{
	int numCoords = fCoords->point.getNum();
	SbMatrix matrix;
	matrix.makeIdentity();
	fTransform->multLeft(matrix);
	for (int i = 0; i < numCoords; i++)
	{
		SbVec3f tempCoord = fCoords->point[i];
		SbVec3f newCoord;
		matrix.multVecMatrix(tempCoord, newCoord);
		fCoords->point.set1Value(i, newCoord);
	}
	matrix.makeIdentity();
	fTransform->setMatrix(matrix);
	this->Update();
}
*/
