class PEntityMgr;
class SoEvent;

///////////////////////////////////////////////////////////////////////////////////////////
class PManip {

	// one instance only of this object...

public:
	enum {		// editTypes...
		kNoEdit,
		kSelectEntity,
		kMovePoint,
		kAddPoint,
		kDeletePoint,
		kAddToSelectedPoints,
		kRemoveFromSelectedPoints,
		kAddToSelectedEntities,
		kRemoveFromSelectedEntities,
		kMoveSelectedEntities,
		kRotateSelectedEntities,
		kDeleteSelectedEntities,
		kCreatePolyline,
		kCreateNurbsCurve,
		kCreateCircle,
		kDeleteEntity,
		kTransformBox,
		kHandleBox,
		
	};

public:

	static	SoSeparator*		sRoot;						// root node of scene graph... we attach "this" as first child
	static	SoWinViewer*		sViewer;						// pointer to the viewer...
	static	SbPList*				sSelectedEntityList;		// list of strings with names of selected entities.
	static	SoMFInt32			sSelectedPointList;		// list of points selected in a single entity.
	static	int					sPointType;					// entity's enum for point type.
	static	int					sWhichCoord;				// which coordinate point on entity is selected.
	static	int					sEditType;					// currently selected operation.
	static	int					sMouseButton;				// which mouse button was pressed. (0 == none pressed).
	static	SbPlaneProjector* sPlaneProjector;
	static	SbLineProjector*	sLineProjector;
	static	SbPlane				sProjectionPlane;
	static	SbLine				sProjectionLine;
	static	SbViewportRegion	sVpRegion;
	static	SbViewVolume		sViewVolume;
	static	SoCoordinate3*		sGhostCoords;
	static	PEntityMgr*			sEntityMgr;
	static	bool					sShowNurbsPolylines;

	// current selection storage...
	static	PEntity*				sSelectedEntity;
	static	SoNode*				sSelectedNode;
	static	int					sSelectedCoord;
	static	SoTransform*		sTransform;
	static	SoTransformBoxManip* sTransformBoxManip;
	static   SoHandleBoxManip*	sHandleBoxManip;
	static	SoPath*				sTransformPath;

				SoSeparator*		fGhostSeparator;
				SoLineSet*			fGhostLine;

public:

// methods:
										PManip(SoSeparator* parentRoot, SoWinViewer* viewer);		
				void					SetEditType(int editType);
				bool					DoEdit(void);						// execute the edit -- return result: successful or not.
	
	static	void					MouseMoved(void *, SoEventCallback *);
	static	void					SetButtonDown(void *, SoEventCallback *);
	static	void					SetButtonUp(void *, SoEventCallback *eventCB);

				void					ProjectMouseToPlane(const SoEvent* ev, SbVec3f& result);
				void					ProjectMouseToLine(const SoEvent* ev, SbVec3f& result);
				void					ExtractViewingParams(void);
				void					LoadEntitiesFromIvFile(CString filename);
				PEntity*				GetSelectedEntity(void);

};


