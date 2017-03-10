#ifndef _TRICUT_H_
#define _TRICUT_H_

#include <Inventor/SbLinear.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/SoPath.h>
#include <Inventor/SoPickedPoint.h>

#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoFile.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoLightModel.h>

#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/details/SoDetail.h>
#include <Inventor/details/SoFaceDetail.h>
#include <Inventor/details/SoPointDetail.h>


class Tricut {

public:
					Tricut(SoCoordinate3* areaCoords);
					Tricut(FILE* triFile, FILE* areaFile, FILE* outputTriFile, FILE* outputEdgeFile);
					Tricut(const char* trifile, const char * areafile, const char * outputtrifile, const char * outputedgefile);
                    Tricut(CString trifile, CString areafile, CString outputtrifile, CString outputedgefile);

                    ~Tricut(void);
	bool			Findcross (SbVec3f& returnPoint, SbVec3f point1, SbVec3f point2, 
							const SoMFVec3f& entity);
	SoMFVec3f	GetEntity( FILE *fp );
	bool			ReadInputFiles(void);
	bool			GetInputTrifileEntity(SoMFVec3f& returnCoords);
	bool			IsInside( double x, double y, const SoMFVec3f& area );
	int         fLineCount;
	int			fCurrentInputTriFacesIndex;
	SoNode*		FindChildOfType(SoGroup * parent, SoType typeId);
	bool			CutTrifile(void);
	void			TriangulateCutArea(void);

	FILE*			fInputTriFile;
	FILE*			fInputAreaFile;              
	FILE*			fOutputTriFile;        
	FILE*			fOutputEdgeFile;

    CString         fTriIn;
    CString         fAreaIn;
    CString         fTriOut;
    CString         fEdgeOut;


	SoCoordinate3*		fTriangulatedAreaCoords;
	SoMFInt32	      fTriangulatedAreaIndices;

	SoCoordinate3*		fInputTriCoords;
	SoIndexedFaceSet* fInputTriFaces;
	SoCoordinate3*		fInputAreaCoords;

	SoCoordinate3*		fOutputTriCoords;
	SoIndexedFaceSet*	fOutputTriFaces;
	SoCoordinate3*		fOutputEdgeCoords;
	SoIndexedLineSet*	fOutputEdgeLines;

};


#endif