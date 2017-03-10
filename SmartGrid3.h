//////////////////////////////////////////////////////////////////////////
//
//   SmartGrid3.h
//
///////////////////////////////////////////////////////////////////////////

#include <Inventor/nodes/SoLOD.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/Win/viewers/SoWinViewer.h>

#ifndef _SMART_GRID_3_H
#define _SMART_GRID_3_H

class Interface;

class SmartGrid3 {

public:
    SoSeparator *gridSep;
    SoSeparator *gridTextSep;

	 SoIndexedLineSet* fXLine;
	 SoIndexedLineSet* fYLine;
	 SoIndexedLineSet* fZLine;
	 
	 SoIndexedLineSet* fXLineFine;
	 SoIndexedLineSet* fYLineFine;
	 SoIndexedLineSet* fZLineFine;

    SoCoordinate3*	fFineCoords;
    SoCoordinate3*	fCoords;

	 float	fNearDistance;
	 float	fFarDistance;

    int     fXLo, fXHi, fYLo, fYHi;
    int     fSpacing, fFineSpacing;
	 SbBool	fUseStationLabels;

    Interface   *theInterface;

   SmartGrid3();
   SmartGrid3(SoGroup *parent, Interface *theInterface, SbBool useStationLabels);
    
   void    DrawGrid();
   void    DrawGrid(int xhii, int xloo, int yhii, int yloo, int spacing);
	void		ToggleX(void);
	void		ToggleY(void);
	void		ToggleZ(void);
	
};

#endif  // _SMART_GRID_3_H