//////////////////////////////////////////////////////////////////////////
//
//   grid.h
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

#ifndef GRID_H
#define GRID_H

class Interface;

class Grid {

public:
    SoSeparator *gridSep;
    SoSeparator *gridTextSep;

    SoLineSet *gridLine;
    SoLineSet *fineGridLine;

    SoCoordinate3 *gridCoords;
    SoCoordinate3 *fineGridCoords;

    int     xlo, xhi, ylo, yhi;
    int     spacing, fineSpacing;

    Interface   *theInterface;

    Grid();
    Grid(SoGroup *parent, Interface *theInterface);
    
    void    drawGrid();
    void    drawGrid(int xhii, int xloo, int yhii, int yloo, int spacing);
//    void    setLabels();
};

#endif