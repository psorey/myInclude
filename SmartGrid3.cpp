//////////////////////////////////////////////////////////////////////////
//
//   SmartGrid3.cpp
//														Copyright: Paul Sorey     2001
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "grid.h"
#include "Interface.h"

#include <Inventor/projectors/SbPlaneProjector.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoTransform.h>


SmartGrid3::SmartGrid3()
{
}

SmartGrid3::SmartGrid3(SoGroup * parent, Interface *interfaceObj, SbBool useStationLabels)
{
    theInterface = interfaceObj;
    gridCoords  = new SoCoordinate3;
    gridLine    = new SoLineSet;
    gridSep     = new SoSeparator;
    SoFont *gridTextFont = new SoFont;
    gridSep->addChild(gridTextFont);
    gridTextFont->size.setValue(14);
    gridTextSep = new SoSeparator;
    SoPickStyle *gridPick = new SoPickStyle;
    gridPick->style.setValue(SoPickStyle::UNPICKABLE);
    parent ->addChild(gridSep);
    gridSep->addChild(gridPick);
    gridSep->addChild(gridCoords);
    gridSep->addChild(gridLine);
    gridSep->addChild(gridTextSep);
	 fUseStationLabels = useStationLabels;
}

void
SmartGrid3::drawGrid()
{
    int x,y,i,j;

    //// get area in view...
    SoPerspectiveCamera *camera = 
        (SoPerspectiveCamera *)theInterface->getCamera();
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

    xlo = LL[0] / H_SCALE;
    xhi = UR[0] / H_SCALE;
    ylo = LL[1] / V_SCALE;
    yhi = UR[1] / V_SCALE;

    i=0;
    j=0;
    int hspacing;
    int hspan = xhi - xlo;
    int logten = int(log((double)(2 * hspan)) );
    TRACE("hspan = %d     log10 = %d\n", hspan, logten);
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
        xlo--;
    }

    int vspacing;
    int vspan = yhi - ylo;
    //vspan /= 4;
    float temp = (log((double)(2 * vspan)) );
	 logten = (int) (temp / 1); // !!!
    TRACE("vspan = %d     log10 = %d\n\n", vspan, logten);
    switch(logten) {
    case 0:
        vspacing = 1;
        break;
    case 1:
        vspacing = 1;
        break;
    case 2:
        vspacing = 2;
        break;
    case 3:
        vspacing = 5;
        break;
    case 4:
        vspacing = 10;
        break;
    case 5:
        vspacing = 20;
        break;
    case 6:
        vspacing = 50;
        break;
    case 7:
        vspacing = 100;
        break;
    case 8:
        vspacing = 200;
        break;
    case 9:
        vspacing = 500;
        break;
    case 10:
        vspacing = 1000;
        break;
    case 11:
        vspacing = 2000;
        break;
    case 12:
        vspacing = 5000;
        break;
    case 13:
        vspacing = 10000;
        break;
    default:
        vspacing = 20000;
        break;
    }

    while(ylo % vspacing != 0) {
        ylo--;
    }    
    
	gridTextSep->removeAllChildren();
	for(x = xlo; x <= xhi; x += hspacing) {
	gridCoords->point.set1Value(i++, x, ylo, 0.0);
	gridCoords->point.set1Value(i++, x, yhi, 0.0);
	gridLine->numVertices.set1Value(j++, 2);

	/// label line...
	SoSeparator *tempSep   = new SoSeparator;
	SoTransform *tempTrans = new SoTransform;
	SbVec3f temp = SbVec3f((float)x +.05 * vspacing, 
								  ylo + 1.4*vspacing, 0.0);
	tempTrans->translation.setValue(temp);
	SoText2 *tempText = new SoText2;
	char astring[30];
	if (TRUE == fUseStationLabels)
	{
		_itoa(x/100, astring, 10);
		strcat(astring, "+00");
	}
	else
	{
		_itoa(x, astring, 10);
	}
	tempText->string.setValue(astring);

	tempSep->addChild(tempTrans);
	tempSep->addChild(tempText);
	gridTextSep->addChild(tempSep);
    }


    for(y = ylo; y <= yhi; y += vspacing) {
	    gridCoords->point.set1Value(i++, xlo, y, 0.0);
	    gridCoords->point.set1Value(i++, xhi, y, 0.0);
	    gridLine->numVertices.set1Value(j++, 2);

        /// label line...
        SoSeparator *tempSep   = new SoSeparator;
        SoTransform *tempTrans = new SoTransform;
        SbVec3f temp = SbVec3f(xlo + 1.4*hspacing, (float)y+.05*vspacing, 0.0);
        tempTrans->translation.setValue(temp);
        SoText2 *tempText = new SoText2;
        char astring[30];
        tempText->string.setValue(_itoa(y,astring,10));
        tempSep->addChild(tempTrans);
        tempSep->addChild(tempText);
        gridTextSep->addChild(tempSep);

    } 
    gridCoords->point.deleteValues(i, -1);
    gridLine->numVertices.deleteValues(j, -1);
}

