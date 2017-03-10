//////////////////////////////////////////////////////////////////////////
//
//   grid.cpp
//
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "grid.h"
#include "Interface.h"

#include <Inventor/projectors/SbPlaneProjector.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoTransform.h>


Grid::Grid()
{
}

Grid::Grid(SoGroup * parent, Interface *interfaceObj)
{
    theInterface = interfaceObj;
    gridCoords  = new SoCoordinate3;
    gridLine    = new SoLineSet;
    gridSep     = new SoSeparator;
	 SoMaterial* fineGridMaterial = new SoMaterial;
	 fineGridMaterial->diffuseColor.setValue(SbColor(.05, .25, .05));
	 fineGridCoords = new SoCoordinate3;
	 fineGridLine = new SoLineSet;
    SoPickStyle *gridPick = new SoPickStyle;
    gridPick->style.setValue(SoPickStyle::UNPICKABLE);
    gridSep->addChild(gridPick);
	 gridSep->addChild(fineGridMaterial);
	 gridSep->addChild(fineGridCoords);
	 gridSep->addChild(fineGridLine);
	 SoMaterial* gridMaterial = new SoMaterial;
	 gridMaterial->diffuseColor.setValue(SbColor(.5, .2, 0));
	 gridSep->addChild(gridMaterial);
    gridTextSep = new SoSeparator;
	 SoMaterial* textMaterial = new SoMaterial;
	 textMaterial->diffuseColor.setValue(SbColor(.8, .5, 0));
    SoFont *gridTextFont = new SoFont;
    gridTextFont->size.setValue(14);
    parent ->addChild(gridSep);
    gridSep->addChild(gridCoords);
    gridSep->addChild(gridLine);
 	 gridSep->addChild(textMaterial);
	gridSep->addChild(gridTextFont);
    gridSep->addChild(gridTextSep);
}

void
Grid::drawGrid()
{
    int x,y,i,j;

    //// get area in view...
    SoCamera *camera = 
        (SoCamera *)theInterface->getCamera();
	SbViewportRegion myRegion = theInterface->getViewportRegion(); 
    float aR = myRegion.getViewportAspectRatio();
    //aR*=4;
    SbViewVolume vV = camera->getViewVolume(aR);
	
    SoSFVec3f pos = camera->position;
    SbVec3f position = pos.getValue();
    
    SbVec3f LL = vV.getPlanePoint(position[2],SbVec2f(0,0));
    SbVec3f UR = vV.getPlanePoint(position[2],SbVec2f(1,1));
    
    SbVec2f scale = theInterface->getWindowScale();
    int H_SCALE = (int)scale[0];
    int V_SCALE = (int)scale[1];

	 //int H_SCALE = 1;
	 //int V_SCALE = 1;

    xlo = LL[0] / H_SCALE;
    xhi = UR[0] / H_SCALE;
    ylo = LL[1] / V_SCALE;
    yhi = UR[1] / V_SCALE;
    //TRACE("H_SCALE = %f    V_SCALE = %f\n", H_SCALE, V_SCALE);

    i=0;
    j=0;
    int hspacing;
    int hspan = xhi - xlo;
    int logten = int(log((double)(/*2 * */ hspan)) );
    
    if(logten > 0) logten--;
    switch(logten) {
    case 0:
        hspacing = 1;
        break;
    case 1:
        hspacing = 2;
        break;
    case 2:
        hspacing = 5;
        break;
    case 3:
        hspacing = 10;
        break;
    case 4:
        hspacing = 20;
        break;
    case 5:
        hspacing = 50;
        break;
    case 6:
        hspacing = 100;
        break;
    case 7:
        hspacing = 200;
        break;
    case 8:
        hspacing = 500;
        break;
    case 9:
        hspacing = 1000;
        break;
    case 10:
        hspacing = 2000;
        break;
    case 11:
        hspacing = 5000;
        break;
    case 12:
        hspacing = 10000;
        break;
    case 13:
        hspacing = 20000;
        break;
    default:
        hspacing = 50000;
        break;
    }
	 
    while(xlo % hspacing != 0) {
        xlo++;
    }
    
	 int vspacing;
    int vspan = yhi - ylo;
    logten = int(log((double)(2 * vspan)) );
    if(logten > 0) logten--;

    switch(logten) {
    case 0:
        vspacing = 1;
        break;
    case 1:
        vspacing = 2;
        break;
    case 2:
        vspacing = 5;
        break;
    case 3:
        vspacing = 10;
        break;
    case 4:
        vspacing = 20;
        break;
    case 5:
        vspacing = 50;
        break;
    case 6:
        vspacing = 100;
        break;
    case 7:
        vspacing = 200;
        break;
    case 8:
        vspacing = 500;
        break;
    case 9:
        vspacing = 1000;
        break;
    case 10:
        vspacing = 2000;
        break;
    case 11:
        vspacing = 5000;
        break;
    case 12:
        vspacing = 10000;
        break;
    case 13:
        vspacing = 20000;
        break;
    default:
        vspacing = 50000;
        break;
    }
    while(ylo % vspacing != 0) {
        ylo--;
    }    
    while(ylo % vspacing != 0) {
        ylo++;
    }

    // adjust grid larger than screen...
    float textY = ylo;
    float textX = xlo;

    ylo -= vspacing * 3;
    yhi += vspacing * 3;
    xlo -= hspacing * 3;
    xhi += hspacing * 3;
    gridTextSep->removeAllChildren();
    
	 int numIntermediateLines = 5;

	 int n = 0;
	 int m = 0;

	 // draw vertical lines...
    for(x = xlo; x <= xhi; x += hspacing) {
			for( int xx = 1; xx < numIntermediateLines; xx++)
			{
				float xxx = (float)x + (xx * ((float)vspacing / numIntermediateLines));
				fineGridCoords->point.set1Value(m++, xxx, ylo, 0.0);
				fineGridCoords->point.set1Value(m++, xxx, yhi, 0.0);
				fineGridLine->numVertices.set1Value(n++, 2);
			}
	    gridCoords->point.set1Value(i++, x, ylo, 0.0);
	    gridCoords->point.set1Value(i++, x, yhi, 0.0);
	    gridLine->numVertices.set1Value(j++, 2);
        
        /// label vertical line...
        SoSeparator *tempSep   = new SoSeparator;
        SoTransform *tempTrans = new SoTransform;
        SbVec3f temp = SbVec3f((float)x, 
                                textY + .36 * vspacing, 0.0);
        tempTrans->translation.setValue(temp);
        SoText2 *tempText = new SoText2;
		  tempText->justification.setValue(SoText2::CENTER);
        char astring[30];
        tempText->string.setValue(_itoa(x,astring,10));
        tempSep->addChild(tempTrans);
        tempSep->addChild(tempText);
        gridTextSep->addChild(tempSep);
    }

    // draw horizontal lines...
    for(y = ylo; y <= yhi; y += vspacing) {
		for( int yy = 1; yy < numIntermediateLines; yy++)
		{
			float yyy = (float)y + (yy * ((float)hspacing / numIntermediateLines));
			fineGridCoords->point.set1Value(m++, xlo, yyy, 0.0);
			fineGridCoords->point.set1Value(m++, xhi, yyy, 0.0);
			fineGridLine->numVertices.set1Value(n++, 2);
		}
	    gridCoords->point.set1Value(i++, xlo, y, 0.0);
	    gridCoords->point.set1Value(i++, xhi, y, 0.0);
	    gridLine->numVertices.set1Value(j++, 2);

        /// label horizontal line...
        SoSeparator *tempSep   = new SoSeparator;
        SoTransform *tempTrans = new SoTransform;
        SbVec3f temp = SbVec3f(textX  + .5 * hspacing , (float)y - .05 * hspacing, 0.0);
			
			tempTrans->translation.setValue(temp);
        SoText2 *tempText = new SoText2;
		  tempText->justification.setValue(SoText2::CENTER);
        char astring[30];
        tempText->string.setValue(_itoa(y,astring,10));
        tempSep->addChild(tempTrans);
        tempSep->addChild(tempText);
        gridTextSep->addChild(tempSep);
    } 
    gridCoords->point.deleteValues(i, -1);
    gridLine->numVertices.deleteValues(j, -1);
	 fineGridCoords->point.deleteValues(m, -1);
	 fineGridLine->numVertices.deleteValues(n, -1);
}

