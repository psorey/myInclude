#pragma once
// "myInclude/pEntity.h"
#include <afx.h>
#include <Inventor/SbLinear.h>
#include <Inventor/fields/SoMFVec3f.h>
#include <Inventor/projectors/SbPlaneProjector.h>
#include <Inventor/projectors/SbLineProjector.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/SbColor.h>
#include <Inventor/Actions/SoWriteAction.h>



class SoSeparator;
class SoCoordinate3;
class SoMaterial;
class SoMaterialBinding;
class SoDrawStyle;
class	SoIndexedFaceSet;
class SoLineSet;
class	SoPointSet;
class	SoMFInt32;
class SoWinViewer;
class SoEventCallback;
class SoCallbackAction;
class SoWriteAction;
class SoHandleEventAction;
class SoGroup;
class SoPrimitiveVertex;
class SoTransform;
class SoTransformBoxManip;
class SoHandleBoxManip;


///////////////////////////////////////////////////////////////////////////////////////
class PEntity {

public:

	enum {
		kPolyline,
		kNurbsCurve,
		kCircle,
		kArc,
		kRectangle,
		kPolygon,
		kExtrusion,
	};

public:

	CString					fName;
	int						fEntityType;
	int						fShow;
	SoSeparator*			fSeparator;
	SoTransform*			fTransform;
	SoCoordinate3*			fCoords;
	SoCoordinate3*          fConnectedCoords;
	SoCoordinate3*          fConnectedCoords2;
	SoMaterial*				fMaterial;
	SoMaterialBinding*		fMaterialBinding;
	SoLineSet*				fLineSet;
	SoDrawStyle*			fDrawStyle;
	SoIndexedFaceSet*		fIndexedFaceSet;
	SoPointSet*				fPointSet;
	SoMFInt32*				fSelectedPoints;		// list of entity's currently selected points.
	
	static	SbColor			sHighlightColor;
	static	SbColor			sUnHighlightColor;
	static	int				sPolylineGenerationStyle;  // BY_LENGTH or BY_ANGLE
	static  float			sPolylineGenerationDistance;
	static	int				sPolylineDivisions;        // number of segments in polyline
	static  bool			sShowPolylineNotNurbs;
	static  int				sPolylinePointSize;

							PEntity(void);
	virtual	SoCoordinate3*	GetCoords(void);
	virtual void			SetConnectedCoords(SoCoordinate3 *, SoCoordinate3 * );
			void			Create(void);
			void			SetName(CString name);
  virtual   void            SetLinesPickable(bool TRUEFALSE);
	const	char*			GetName(void);
			SoSeparator*	GetNode(void);
            int             GetType(void);
	virtual	void			AddPoint(SbVec3f point);
	virtual void			DeletePoint(int whichPoint);
	virtual float			GetLength(void);
	//									InsertPoint(int beforeWhich);
	//									DeletePoint(int whichPoint);
	//									TransformPoint(int whichPoint, const SbMatrix& deltaMatrix);
	//									TransformEntity(const SbMatrix& deltaMatrix);
	//						
	virtual	void			Update(void);
	//									UpdateInterfaceGraphic();
	//									UpdateInternalDataPerInterface();
	virtual	void			Highlight(void);
	virtual	void			UnHighlight(void);
	virtual  void			AddCoordsToFile(const SoWriteAction wa);
	virtual	void			WriteCoordsToFile(CString filename, float fBeginElev = NULL, float fEndElev = NULL);
	//virtual	void		WriteCoordsToFile(CString filename, float beginElev, float endElev);
	virtual	SoTransform*	GetTransform(void) { return fTransform; }
	virtual	void			TransformCoordinates(void);
   virtual  void            CreateFromShapeCoords(SoCoordinate3* shapeCoords);
};


///////////////////////////////////////////////////////////////////////////////////////
class PPolyline : public PEntity {

public:
							PPolyline(void);
							PPolyline(SoSeparator* subGraphRoot);
							~PPolyline(void);

		    void		    GetPolylineCoords(SoMFVec3f& coords);
   virtual  void			SetLinesPickable(bool TRUEFALSE);
};


class SoNurbsCurve;
class SoComplexity;

////////////////////////////////////////////////////////////////////////////////////////
class PNurbsCurve : public PEntity {

public:

	SoComplexity*			fComplexity;
	SoNurbsCurve*			fNurbsCurve;
	SoSeparator*			fPolylineSeparator;
	SoLineSet*				fPolylineLines;
	SoPointSet*				fPolylinePoints;
	SoCoordinate3*			fPolylineCoords;		// separate coordinates for polyline representation.
	SoCoordinate3*			fMarkerCoords;
	SoSeparator*			fMarkerSep;
	SoPointSet*				fMarkerPoints;
	SoMaterial*				fNurbsPolylineMaterial;
	SoDrawStyle*            fNurbsPolylineDrawStyle;

	bool					fShowNurbs;
	bool					fShowPolyline;
	bool					fShowMarkers;
	int                     fNumPolylineDivisions;
	float                   fPolylineGenerationDistance;
	//float					fPolylineInterval;

	static	int				sCount;
	static	SbVec3f			sPoint1;
	static	SbVec3f			sPoint2;
	static	SbVec3f			sPoint3;
	static	SbVec3f			sPoint4;

							PNurbsCurve(void);
							PNurbsCurve(SoSeparator* subGraphRoot);
							~PNurbsCurve(void);

			void			DisplayAsPolyline(int numPoints);
			void			GetPolylineCoords(void);
			void			GetPolylineCoords(int divisions);
	virtual	SoCoordinate3*	GetCoords(void); // this one returns the splinePolylineCoords...
	virtual	void			AddPoint(SbVec3f point);
	virtual void			DeletePoint(int whichPoint);
	virtual float			GetLength(void);
	static	void			NurbsCurveLineSegCB(void *, SoCallbackAction *action, const SoPrimitiveVertex *v1, const SoPrimitiveVertex *v2);
			double			Distance3d(SbVec3f pt1, SbVec3f pt2);
			float			Get3dSegmentAngle(void);
			float			Angle3d(SbVec3f pt1, SbVec3f pt2, SbVec3f testPt);
			void			Update(void);
	virtual void			AddCoordsToFile(const SoWriteAction wa);
//	virtual	void			WriteCoordsToFile(CString filename);
	virtual	void			WriteCoordsToFile(CString filename, float fBeginElev = NULL, float fEndElev = NULL);
    virtual void			CreateFromShapeCoords(SoCoordinate3* shapeCoords);
	//virtual	void				TransformCoordinates(void);
};





