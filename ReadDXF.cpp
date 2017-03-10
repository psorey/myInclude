#include <stdafx.h>
#include <afx.h>
#include <afxext.h>
#include "ReadDXF.h"
#include "GeoGraphic.h"

#define MY_PI 3.141592653589793238462643

ReadDXF::ReadDXF( SoSeparator *_parent)
{
	SoDB::init();
	fLayerList = NULL;
	coordIndexNum = 0;
	vertexNum = 0;
	parent = _parent;
	topSep = new SoSeparator;
	topSep->ref();
	theLines = new SoIndexedLineSet;
	theCoords = new SoCoordinate3;
	theCoords->point.deleteValues(0, -1);
	topSep->addChild(theCoords);
	topSep->addChild(theLines);
}

ReadDXF::~ReadDXF()
{	
	topSep->unref();
}

bool ReadDXF::ReadLayerTable()
{
	fLayerList = new SbPList;
	// look for the tables section...
	if (ReadDXF::FindNot("TABLES", "EOF", ifp) == FALSE) return FALSE;
	while (ReadDXF::FindNot("TABLE", "ENDSEC", ifp))
	{
		// look for the layer table...
		if (ReadDXF::FindNot("LAYER", "ENDTAB", ifp)) 
		{
			// find all the layer entries...
			while(ReadDXF::FindNot("LAYER", "ENDTAB", ifp))
			{
				CString layerName;
				int layerColor;
				char layerFlags;

				ASSERT(ReadDXF::GetVariable("  2", layerName, ifp));
				ASSERT(ReadDXF::GetVariable(" 70", layerFlags, ifp));
				ASSERT(ReadDXF::GetVariable(" 62", layerColor, ifp));
			}
		}
	}
	if (fLayerList != NULL && fLayerList->getLength() < 1)
	{
		delete fLayerList;
		fLayerList = NULL;
	}
	return TRUE;
}

bool ReadDXF::FindNot(const char* findString, const char* notString, FILE* fp)
{
	char line[100];
	CString findStr(findString);
	CString notStr(notString);
	findStr += "\n";
	notStr += "\n";
	while (strcmp((fgets(line,100,fp)), LPCTSTR(findStr)))
	{
		if (line == NULL)
			return FALSE;
		if (strcmp(line, (const char *)LPCTSTR(notStr)) == 0)
			return FALSE;
	}
	return TRUE;
}


bool ReadDXF::GetVariable(const char* str, float& var, FILE* fp)
{
	if (FindNot(str, "ENDSEC", fp))
	{
		fscanf(fp, "%f", &var);
		return TRUE;
	}
	else return FALSE;
}


bool ReadDXF::GetVariable(const char* str, int& var, FILE* fp)
{
	if (FindNot(str, "ENDSEC", fp))
	{
		fscanf(fp, "%d", &var);
		//TRACE("var = %d\n", var);
		return TRUE;
	}
	else return FALSE;
}


bool ReadDXF::GetVariable(const char* str, char& var, FILE* fp)
{
	int iVar;
	if (FindNot(str, "ENDSEC", fp))
	{
		fscanf(fp, "%d", &iVar);
		var = (char) iVar;
		return TRUE;
	}
	else return FALSE;
}


bool ReadDXF::GetHexVariable(const char* str, int& var, FILE* fp)
{
	if (FindNot(str, "ENDSEC", fp))
	{
		fscanf(fp, "%x", &var);
		//TRACE("var = %d\n", var);
		return TRUE;
	}
	else return FALSE;
}


bool ReadDXF::GetVariable(const char* str, CString& var, FILE* fp)
{
	if (FindNot(str, "ENDSEC", fp))
	{
		char line[100];
		fgets(line, 100, fp);
		var = CString(line);
		var.TrimRight();
		//TRACE("var = %s\n", var);
		return TRUE;
	}
	else return FALSE;
}

/*
int ReadDXF::ReadImageInfo(GeoGraphicMgr* theGeoGraphicMgr, FILE* fp)
{
	int numImages = 0;

	if (ReadDXF::FindNot("ENTITIES", "EOF", fp) == FALSE) return 0;
	
	SoCoordinate3* perimeterCoords = new SoCoordinate3;
	perimeterCoords->ref();
	
	while (ReadDXF::FindNot("AcDbRasterImage", "ENDSEC", fp))
	{	
		SbVec3f origin;
		ASSERT(ReadDXF::GetVariable(" 10", origin[0], fp));
		ASSERT(ReadDXF::GetVariable(" 20", origin[1], fp));
		ASSERT(ReadDXF::GetVariable(" 30", origin[2], fp));
		float scaleX1 = 0.0;
		float scaleX2 = 0.0;
		float scaleY1 = 0.0;
		float scaleY2 = 0.0;
		ASSERT(ReadDXF::GetVariable(" 11", scaleX1, fp));
		ASSERT(ReadDXF::GetVariable(" 21", scaleX2, fp));
		ASSERT(ReadDXF::GetVariable(" 12", scaleY1, fp));
		ASSERT(ReadDXF::GetVariable(" 22", scaleY2, fp));
		float rotation;
		float scale;
		if (scaleX2 == 0.0)
		{
			rotation = 0.0;
			scale = scaleX1;
		}
		else
		{
			rotation = atan(scaleX2 / scaleX1);
			TRACE("rotation = %f\n", rotation);
			scale = sqrt((scaleX2 * scaleX2) + (scaleX1 * scaleX1));
		}
		float width = 0.0;
		ASSERT(ReadDXF::GetVariable(" 13", width, fp));
		float height = 0.0;
		ASSERT(ReadDXF::GetVariable(" 23", height, fp));
		int imageHandle = 0;
		ASSERT(ReadDXF::GetHexVariable("340", imageHandle, fp));
		int numVertices = 0;
		ASSERT(ReadDXF::GetVariable(" 91", numVertices, fp));
		//TRACE("numVertices = %d\n", numVertices);
		SoCoordinate3* perimeterCoords = new SoCoordinate3;
		perimeterCoords->ref();

		perimeterCoords->point.deleteValues(0,-1);
		for (int i = 0; i < numVertices; i++)
		{
			SbVec3f point;
			ASSERT(ReadDXF::GetVariable(" 14", point[0], fp));
			ASSERT(ReadDXF::GetVariable(" 24", point[1], fp));
			// flip the perimeter coords so that 0,0 is lower left instead of upper left...
			point[1] = (height - 1) - point[1];
			perimeterCoords->point.set1Value(i, point);
			TRACE("perimeterCoords[%d] = %f  %f\n", i, point[0], (height - 1) - point[1]);
		}
		// if there are only 2 perimeter coords (lower left and upper right)
		// convert them to 4 corners of a rectangle...
		if (perimeterCoords->point.getNum() == 2)
		{
			//TRACE("coord 0 = %f  %f\n", perimeterCoords->point[0][0], perimeterCoords->point[0][1]);
			//TRACE("coord 1 = %f  %f\n\n", perimeterCoords->point[1][0], perimeterCoords->point[1][1]);
			perimeterCoords->point.insertSpace(1, 1);
			perimeterCoords->point.set1Value(1, SbVec3f(
				perimeterCoords->point[0][0], perimeterCoords->point[2][1], 0.0));
			perimeterCoords->point.set1Value(3, SbVec3f(
				perimeterCoords->point[2][0], perimeterCoords->point[0][1], 0.0));
			//TRACE("coord 0 = %f  %f\n", perimeterCoords->point[0][0], perimeterCoords->point[0][1]);
			//TRACE("coord 1 = %f  %f\n", perimeterCoords->point[1][0], perimeterCoords->point[1][1]);
			//TRACE("coord 2 = %f  %f\n", perimeterCoords->point[2][0], perimeterCoords->point[2][1]);
			//TRACE("coord 1 = %f  %f\n\n", perimeterCoords->point[3][0], perimeterCoords->point[3][1]);
		}
		GeoGraphic* newGeoGraphic = new GeoGraphic(imageHandle, 
					origin, scale, rotation, (int)width, (int)height, perimeterCoords);  
		
		theGeoGraphicMgr->Add(newGeoGraphic);
		numImages++;
	}
	perimeterCoords->unref();

	while (ReadDXF::FindNot("IMAGEDEF", "ENDSEC", fp))
	{
		int imageHandle = 0;
		ASSERT(ReadDXF::GetHexVariable("  5", imageHandle, fp));
		CString filename;
		ASSERT(ReadDXF::GetVariable("  1", filename, fp));
		float width;
		ASSERT(ReadDXF::GetVariable(" 10", width, fp));
		float height;
		ASSERT(ReadDXF::GetVariable(" 20", height, fp));
		//TRACE("handle = %d\n", imageHandle);
		//CString add = "\"";
		//add += filename;
		//add += "\"";
		//filename = add;
		//TRACE("filename = %s\n", (LPCTSTR)add);
		//TRACE("width = %d   height = %d\n", (int)width, (int)height);
		RgbImage* newImage = new RgbImage(imageHandle, filename, (int)width, (int)height);
		theGeoGraphicMgr->Add(newImage);
	}
	return numImages;
}

*/


SbBool 
ReadDXF::readFile(FILE *ifp, CString filename) 
{
	this->ifp = ifp;
	if(ifp == NULL) return FALSE;

	char layer[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
	/* find the beginning of the ENTITIES section */
	count = x1 = y1 = z1 = x2 = y2 = z2 = 0;  
		while(strcmp((fgets(line,100,ifp)),"ENTITIES\n"))
		lnct++;
	while ((fgets(line,100,ifp)) != NULL )
	{
		//TRACE("line = %s\n", line);
		lnct++;		
		if((strcmp( line, "LINE\n")) == 0 )
			getLINE();
		else if((strcmp( line, "LWPOLYLINE\n")) == 0 )
			getLWPOLYLINE();
		else if((strcmp(line,  "POLYLINE\n")) == 0) 
			getPOLYLINE();
		// added 2/13/01...
		else if((strcmp(line,  "ARC\n")) == 0)
			getARC();
		else if((strcmp(line,  "POINT\n")) == 0)
			getPOINT();
		else {}
	}
	fclose(ifp);
	char nwfile[150];
	for(int n = 1; n < 150; n++)
		nwfile[n] = '\0';

	strncpy( nwfile, LPCTSTR(filename), (strcspn(LPCTSTR(filename),"." )));
	strcat( nwfile, ".iv");
	outIvFilename = CString(nwfile);

	if (theLines->coordIndex[0] == -1)
		theLines->coordIndex.deleteValues(0, 1);	
	
	if((theCoords->point.getNum() > 1) && parent != NULL) {
		parent->addChild(theCoords);
		parent->addChild(theLines);
	}
	return TRUE;
}

SbBool 
ReadDXF::readFile() 
{
	lnct  = 0;
	count = 0;
	long n;

	BOOL        bAddFileDialog = TRUE;
	LPCTSTR     lpszFilter = NULL;
	LPCTSTR     lpszDefExt = LPCTSTR("dxf");
	LPCTSTR     lpszFileName = LPCTSTR("*.dxf");
	DWORD       dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CWnd        *pParentWnd = NULL;

	CFileDialog addFileDialog(bAddFileDialog,
	  lpszDefExt, lpszFileName, dwFlags,
	  lpszFilter, pParentWnd);

	addFileDialog.m_ofn.lpstrTitle = LPCTSTR("select a DXF file... ");
	int nModal = addFileDialog.DoModal();
	CString m_strAddFile;

	if (nModal != IDOK) return FALSE;
	m_strAddFile = addFileDialog.GetFileName();
	inDxfFilename = m_strAddFile;
	TRACE("inDxfFilename  =  %s\n", m_strAddFile);
	ifp = fopen(LPCTSTR(m_strAddFile), "r");
	if(ifp == NULL){
		TRACE("can't open %s\n", LPCTSTR(m_strAddFile));
		return FALSE;
	}	  
	SbBool ret = this->readFile(ifp, LPCTSTR(m_strAddFile));
	return ret;
}

void ReadDXF::writeIvFile(void)
{
	TRACE ("made it here\n");
	TRACE("writeIV  %d\n", theCoords->point.getNum());

	if (theCoords->point.getNum() > 1) 
	{
		//SoSeparator *topSep = new SoSeparator;
		topSep->ref();
		//topSep->addChild(theCoords);
		//topSep->addChild(theLines);

	//	if (fPointCoords->point.getNum() > 0)
	//	{
	//		topSep->addChild(fPointCoords);
	//		SoPointSet* points = new SoPointSet;
	//		SoDrawStyle* style = new SoDrawStyle;
	//		style->pointSize.setValue(3);
	//		topSep->addChild(points);
	//	}
		SoWriteAction wa;
		wa.getOutput()->openFile(LPCTSTR(outIvFilename));
		wa.apply(topSep);
		wa.getOutput()->closeFile();
		topSep->unref();        
	}
}

CString ReadDXF::getOutIvFilename(void)
{
	return outIvFilename;
}

void 
ReadDXF::get_group ( char group_number[5] )
{
	while ( strcmp (( fgets( line, 50, ifp)), group_number) != 0 ) 
		lnct++;
	get_xyz();
}

void 
ReadDXF::get_xyz( void )
{
	fscanf ( ifp, "%lf", &x );
    lnct++;
	fscanf ( ifp, "%f", &trash );
    lnct++;
	fscanf ( ifp, "%lf", &y );
    lnct++;
	fscanf ( ifp, "%f", &trash );
    lnct++;
	fscanf ( ifp, "%lf", &z );
    lnct++;
}

void
ReadDXF::getLINE(void)
{ 
	/*
	get_group( " 10\n" );
	x1=x;
	y1=y;
	z1=z;
	if (( (int)x2 != (int)x1) || ((int)y2 != (int)y1) || ((int)z2 != (int)z1 )) 
	{
		theLines->coordIndex.set1Value(coordIndexNum++, -1);
		//theVP->vertex.set1Value(vertexNum++, (SbVec3f(x,y,z)));
		theCoords->point.set1Value(vertexNum++, (SbVec3f(x,y,z)));
		theLines->coordIndex.set1Value(coordIndexNum++, vertexNum-1);
	}
	get_group( " 11\n" );
	x2=x;
	y2=y;
	z2=z;

	//theVP->vertex.set1Value(vertexNum++, (SbVec3f(x,y,z)));
	theCoords->point.set1Value(vertexNum++, (SbVec3f(x,y,z)));
	theLines->coordIndex.set1Value(coordIndexNum++, vertexNum-1);

	TRACE("xyz = %f  %f  %f\n", x, y, z);
		*/
        
	get_group(" 10\n");
	theCoords->point.set1Value(vertexNum++, (SbVec3f(x,y,z)));
	theLines->coordIndex.set1Value(coordIndexNum++, vertexNum - 1);
 
	get_group(" 11\n");
	theCoords->point.set1Value(vertexNum++, (SbVec3f(x,y,z)));
	theLines->coordIndex.set1Value(coordIndexNum++, vertexNum - 1);
	theLines->coordIndex.set1Value(coordIndexNum++, -1);

}

void
ReadDXF::getPOINT(void)
{
	get_group(" 10\n");
	int numPoints = fPointCoords->point.getNum();
	fPointCoords->point.set1Value(numPoints, SbVec3f(x,y,z));
}

void
ReadDXF::getPOLYLINE(void)
{
   int closed = 0;
   int rclosed = 0;
   //SbVec3f savedPoint;
   int savedIndex =0;
    count = 0;
    while((fgets(line,100,ifp))!= NULL)
	 {
	 //TRACE("line =  %s\n", line);
      lnct++;
      if((strcmp(line, " 70\n")) == 0) 
		{
			fgets(line,100,ifp);
			lnct++;
			sscanf(line,"%d", &rclosed);
			if(count == 1 && rclosed == 9) /// !!! could be other values too!
         {
				 closed = 1;   
         }
			//else closed = 0;
         TRACE ("closed = %d\n", closed);
			continue;
		}
        if((strcmp(line," 10\n")) == 0)
		  {
            fgets(line,100,ifp);
            lnct++;
            sscanf(line,"%lf", &x);
            fgets(line,100,ifp);
            lnct++;
            fgets(line,100,ifp);
            lnct++;
            sscanf(line,"%lf", &y);
            fgets(line,100,ifp);
            lnct++;
            fgets(line,100,ifp);
            lnct++;
            sscanf(line,"%lf", &z);
			//TRACE("xyz = %f   %f   %f\n", x,y,z);
            if(count != 0) 
				{
                //theVP->vertex.set1Value(vertexNum++, (SbVec3f(x,y,z)));
                theCoords->point.set1Value(vertexNum++, (SbVec3f(x,y,z)));
                theLines->coordIndex.set1Value(coordIndexNum++, vertexNum-1);
                if(count == 1)
                  // savedPoint = SbVec3f(x,y,z);
                  savedIndex = vertexNum-1;
                count++;
            }
            else count++;
        }
        if((strcmp(line,"SEQEND\n")) == 0) 
		  {
           // theVP->vertex.set1Value(vertexNum++, (SbVec3f(x,y,z)));
           // theCoords->point.set1Value(vertexNum++, (SbVec3f(x,y,z)));
           // theLines->coordIndex.set1Value(coordIndexNum++, vertexNum-1);
            if(closed == 1)
            {
               //theCoords->point.set1Value(vertexNum++, savedPoint);
               theLines->coordIndex.set1Value(coordIndexNum++, savedIndex);
            }
            theLines->coordIndex.set1Value(coordIndexNum++, -1);
            count--;
            return;
        }
    }
}

void
ReadDXF::getARC(void)
{
	SbVec3f	centerPoint;
	double	radius;
	int		numSegments = 24; // !!! control this from preferences...
	double  beginAngle;
	double	endAngle;

	while((fgets(line,100,ifp))!= NULL)
	{
		lnct++;

		// get x and y coordinates of center point...
		if((strcmp(line," 10\n")) == 0)
		{
			SbVec3f temp;
			fgets(line,100,ifp);
			lnct++;
			sscanf(line,"%lf", &x);
			centerPoint[0] = x;

			fgets(line,100,ifp);
			lnct++;
			if((strcmp(line," 20\n")) != 0) continue;
			
			fgets(line,100,ifp);
			lnct++;
			sscanf(line,"%lf", &y);
			centerPoint[1] = y;

			fgets(line,100,ifp);
			lnct++;
			if((strcmp(line," 30\n")) != 0) continue;

			fgets(line,100,ifp);
			lnct++;
			sscanf(line,"%lf", &z);
			centerPoint[2] = z;
			
			fgets(line,100,ifp);
			lnct++;
			if((strcmp(line," 40\n")) != 0) continue;
			
			fgets(line,100,ifp);
			lnct++;
			sscanf(line,"%lf", &radius);

			//TRACE("x, y, z = %f  %f  %f \n", x, y, z);
			
			continue;
		}

		// get beginning and ending angle...
		if((strcmp(line," 50\n")) == 0)
		{
			fgets(line,100,ifp);
			lnct++;
			sscanf(line,"%lf", &beginAngle);

			fgets(line,100,ifp);
			lnct++;
			if((strcmp(line," 51\n")) != 0) continue;
			fgets(line,100,ifp);
			lnct++;
			sscanf(line,"%lf", &endAngle);

			//TRACE("begin and end = %f  %f\n", beginAngle, endAngle);
			// finish up the calculations...

			float totalAngle = endAngle - beginAngle;
			if (totalAngle < 0)
			{
				// we're crossing the 360 so fix it...
				float tempBeginAngle = beginAngle - 360;
				totalAngle = endAngle - tempBeginAngle;
			}
			float angleIncrement = totalAngle / numSegments;
			for (int i = 0; i <= numSegments; i++)
			{
				float newAngle = i * angleIncrement + beginAngle;
				if (newAngle >= 360)
					newAngle = newAngle - 360;
				SbVec3f newCoord = calcVectorPoint(centerPoint, radius, (newAngle / 180) * MY_PI );
				theCoords->point.set1Value(vertexNum++, newCoord[0], newCoord[1], z); 
				theLines->coordIndex.set1Value(coordIndexNum++, vertexNum - 1);
				count++;
			}
			theLines->coordIndex.set1Value(coordIndexNum++, -1);
			return;
		}
		
	}
	return;
}


void
ReadDXF::getLWPOLYLINE(void)
{
	TRACE("getting LWPOLYLINE\n");
	SoMFVec3f coords;
	SbVec3f   az;
	z=0.0;
	int closed = 0;
	int acount = 0;
	while((fgets(line,100,ifp))!= NULL)
	{
		lnct++;
		//get elevation...
		if((strcmp(line, " 70\n")) == 0) 
		{
			fgets(line,100,ifp);
			lnct++;
			sscanf(line,"%d", &closed);
			if(closed == 1 || closed == 129)
				 closed = 1;
			else closed = 0;
			continue;
		}
		if((strcmp( line, " 38\n")) == 0 )
		{
			fgets(line,100,ifp);
			lnct++;
			sscanf(line,"%lf", &z);
			continue;
		}
		// get x and y coordinates...
		if ((strcmp(line," 10\n")) == 0)
		{
			//TRACE("found 10\n");
			SbVec3f temp;
			fgets(line, 100, ifp);
			lnct++;
			sscanf(line,"%lf", &x);
			temp[0] = x;
			fgets(line, 100, ifp);
			lnct++;
			if((strcmp(line," 20\n")) != 0) continue;
			fgets(line, 100, ifp);
			lnct++;
			sscanf(line,"%lf", &y);
			temp[1] = y;
			temp[2] = z;
			coords.set1Value(acount++, temp);
			continue;
		}
		if ((strcmp(line," 42\n")) == 0)
		{
			TRACE("found an arc\n");
			// next segment is an arc, so get the arc segments...
			// save the last point...
			SbVec3f beginPoint = SbVec3f(x, y, z);		// begin point of arc
			SbVec3f endPoint;									// end point of arc

			// get the "bulge" of the arc...
			fgets(line, 100, ifp);
			double bulgeRatio;
			sscanf(line, "%lf", &bulgeRatio);

			// Get the next point...
			// get the " 10"
			fgets(line, 100, ifp);
			// there's a 42 at the end, which needs to be ignored... (why??)
			if ((strcmp(line,"  0\n")) == 0)
				goto ENDLINE;
			if ((strcmp(line," 10\n")) != 0)
				continue;  // need to un fgets so we're able to read the ..0 at the end of the LWP !!!
			fgets(line, 100, ifp);
			sscanf(line, "%lf", &x);
			// get the " 20"
			fgets(line, 100, ifp);
			fgets(line, 100, ifp);
			sscanf(line, "%lf", &y);
			endPoint = SbVec3f(x, y, z);
			
			// calculate the third point on the circle...
			float halfLC = .5 * (distance(beginPoint, endPoint)); 
			float bulge = fabs(bulgeRatio * halfLC);
			float theta = calcTheta(beginPoint, endPoint);	// radians...
			SbVec3f midChord = calcVectorPoint(beginPoint, halfLC, theta);	// radians...
			SbVec3f thirdPoint;
			if (bulgeRatio > 0)
				thirdPoint = calcVectorPoint(midChord, bulge, theta - DegreesToRadians(90)); 
			else
				thirdPoint = calcVectorPoint(midChord, bulge, theta + DegreesToRadians(90)); 
			
			// get radius and center point from the three-point construction of a circle...
			float bx = beginPoint[0];
			float by = beginPoint[1];
			float cx = endPoint[0];
			float cy = endPoint[1];
			float dx = thirdPoint[0];
			float dy = thirdPoint[1];

			float temp = cx * cx + cy * cy;
			float bc = (bx * bx + by * by - temp) / 2.0;
			float cd = (temp - dx * dx - dy * dy) / 2.0;
			float det = (bx - cx) * (cy - dy) - (cx - dx) * (by - cy);
			SbVec3f centerPoint;
			if (fabs(det) < 1.0e-6)
			{
				centerPoint = SbVec3f(1.0, 1.0, 0.0);
				return;
			}
			det = 1 / det;
			centerPoint[0] = (bc * (cy - dy) - cd * (by - cy)) * det;
			centerPoint[1] = ((bx - cx) * cd - (cx - dx) * bc) * det;
			cx = centerPoint[0];
			cy = centerPoint[1];
			float radius = sqrt((cx - bx) * (cx - bx) + (cy - by) * (cy - by));

			// now draw the arc as a series of segments,
			// making sure they're drawn in the correct order...

			int numSegments = 25;
			float beginAngle = RadiansToDegrees(calcTheta(centerPoint, beginPoint));
			float endAngle = RadiansToDegrees(calcTheta(centerPoint, endPoint));
			
			if (bulgeRatio < 0)
			{
				// need to go CW instead of CCW...
				float temp = beginAngle;
				beginAngle = endAngle;
				endAngle = temp;
			}
			
			float totalAngle = endAngle - beginAngle;
			if (totalAngle < 0)
			{
				// we're crossing the 360 so fix it...
				float tempBeginAngle = beginAngle - 360;
				totalAngle = endAngle - tempBeginAngle;
			}			
			float angleIncrement = totalAngle / numSegments;
			for (int i = 0; i <= numSegments; i++)
			{
				float newAngle;
				if (bulgeRatio > 0)
				{
					newAngle = (i * angleIncrement) + beginAngle;
					if (newAngle >= 360)
						newAngle = newAngle - 360;
					SbVec3f newCoord = calcVectorPoint(centerPoint, radius, DegreesToRadians(newAngle));
					coords.set1Value(acount++, newCoord);
				}
				else // negative bulge...
				{
					newAngle = endAngle - (i * angleIncrement);
					if (newAngle >= 360)
						newAngle = newAngle - 360;
					SbVec3f newCoord = calcVectorPoint(centerPoint, radius, DegreesToRadians(newAngle));
					coords.set1Value(acount++, newCoord);
				}
			}
			continue;
		}

		// get ocs z-axis
		if ((strcmp(line,"210\n")) == 0)
		{
			fgets(line, 100, ifp);
			lnct++;
			sscanf(line,"%lf", &x);
			az[0] = x;
			fgets(line, 100, ifp);
			lnct++;
			if((strcmp(line,"220\n")) != 0) continue;
			fgets(line,100,ifp);
			lnct++;
			sscanf(line,"%lf", &y);
			az[1] = y;
			fgets(line,100,ifp);
			lnct++;
			fgets(line,100,ifp);
			lnct++;
			sscanf(line,"%lf", &z);
			az[2] = z;

			int j=coords.getNum();   
         TRACE("ReadDXF::numCoords = %d\n", j);

			SbVec3f result;
			for (int i=0; i<j; i++) 
			{
				result = getWorld(coords[i],az);
				theCoords->point.set1Value(vertexNum++, result[0],result[1],result[2]);
				theLines->coordIndex.set1Value(coordIndexNum++, vertexNum-1);
				count++;
			}

			if(!closed) 
			{
				 theLines->coordIndex.set1Value(coordIndexNum++, -1);
			}
			else 
			{
				int k;
				int newValue;
            for(k = coordIndexNum; k >= 0; k--) 	 
				{
					if (*theLines->coordIndex.getValues(k) == -1) 
					{
						newValue = *theLines->coordIndex.getValues(k+1);
                  k = -1;
					}
					if(k==0) 
						newValue = *theLines->coordIndex.getValues(k);
				}
				theLines->coordIndex.set1Value(coordIndexNum++, newValue);
				theLines->coordIndex.set1Value(coordIndexNum++, -1);
			}
         return;
		}
ENDLINE:
		if((strcmp(line,"  0\n")) == 0)
		{
			int j = coords.getNum();            
         SbVec3f result;
         for (int i = 0; i < j; i++) 
			{
				result = getWorld(coords[i],az);
				//theVP->vertex.set1Value(vertexNum++, coords[i][0],coords[i][1],coords[i][2]);
				theCoords->point.set1Value(vertexNum++, coords[i][0],coords[i][1],coords[i][2]);
				theLines->coordIndex.set1Value(coordIndexNum++, vertexNum-1);
			}
			if (!closed) 
			{
				theLines->coordIndex.set1Value(coordIndexNum++, -1);
			}
         else 
			{
				int k;
            int newValue;
            for(k = coordIndexNum; k>=0; k--) 
				{
					if (*theLines->coordIndex.getValues(k) == -1) 
					{
						newValue = *theLines->coordIndex.getValues(k+1);
                  k = -1;
					}
					if(k == 0) 
					newValue = *theLines->coordIndex.getValues(k);
				}
				theLines->coordIndex.set1Value(coordIndexNum++, newValue);
				theLines->coordIndex.set1Value(coordIndexNum++, -1);
			}
			return;
		}
	}
	return;
}

SbVec3f
ReadDXF::getWorld(SbVec3f pointVector /*point to transform*/, SbVec3f az /*arbitrary z-axis*/)
{

    //find arbitrary x-axis...
    SbVec3f ax;
    SbVec3f wx = SbVec3f(1.0,0.0,0.0);       //world x
    SbVec3f wy = SbVec3f(0.0,1.0,0.0);       //world y
    SbVec3f wz = SbVec3f(0.0,0.0,1.0);       //world z
    if((fabs(az[0])) < .015625 && (fabs(az[1])) < .015625) {
        ax = wy.cross(az);
        //fprintf(ofp,"near z\n");
    }
    else {
        ax = wz.cross(az);
    }
    ax.normalize();

    //find arbitrary y-axis...
    SbVec3f ay = az.cross(ax);
    ay.normalize();
    
    //make the transformation matrix and multiply it by input point...
    SbVec3f  sum;
    float m[4][4] = {ax[0],ay[0],az[0], 0,
                     ax[1],ay[1],az[1], 0,
                     ax[2],ay[2],az[2], 0,
                         0,    0,    0, 1};
    int disp = 0;
    SbVec3f bp = pointVector;
    SbVec3f ap;
    int i;
    for (i=0; i<3; i++)
        sum[i] = m[i][0]*bp[0]+m[i][1]*bp[1]+m[i][2]*bp[2]
              +(disp ? 0.0 : m[i][3]);
    for (i=0; i<3; i++) ap[i] = sum[i];
    //TRACE("%f  %f  %f\n", ap[0],ap[1],ap[2]);
    return ap;
}

void ReadDXF::reducePoints(float MIN_LENGTH)
{
	SoIndexedLineSet *reducedLines = new SoIndexedLineSet;
	SoCoordinate3 *reducedCoords = new SoCoordinate3;

	int numIndices = theLines->coordIndex.getNum();
	SbVec3f currentTestPoint;
	SbVec3f lastTestPoint;
	SbVec3f lastSavedPoint;
	int index;
	int reducedCoordsCount = 0;
	int reducedIndicesCount = 0;
	bool includeThisPoint = FALSE;
	bool addNewCurvePoint = FALSE;

	// !!!
	fTestLengthEnabled = TRUE;
	fTestDeflectionEnabled = TRUE;


	//TRACE("Min Length = %f\n", MIN_LENGTH);

	// begin next polyline...
	for (long i = 0; i < numIndices; i++)
	{
		index = theLines->coordIndex[i];
		lastTestPoint = theCoords->point[index];
		lastSavedPoint = theCoords->point[index];
		reducedCoords->point.set1Value(reducedCoordsCount, lastTestPoint);
		reducedLines->coordIndex.set1Value(reducedIndicesCount, reducedCoordsCount);
		reducedCoordsCount++;
		reducedIndicesCount++;
		float totalLength = 0.0;
		
		i++;  // now we're working with the next point....
	
		// find the remaining points in the polyline...
		while (-1 != (index = theLines->coordIndex[i]) && i < numIndices)
		{
			currentTestPoint = theCoords->point[index];
			float segmentLength = this->distance(lastTestPoint, currentTestPoint);
			totalLength += segmentLength;
			//TRACE("segment length = %f     total length  = %f\n", segmentLength, totalLength);
			if (totalLength > MIN_LENGTH && fTestLengthEnabled)
			{
				includeThisPoint = TRUE;
			}

			
			// test deflection...
			if (fTestDeflectionEnabled == TRUE && reducedIndicesCount > 1 && 
				reducedLines->coordIndex[reducedIndicesCount - 2] != -1)
			{
				SbVec3f pt1 = reducedCoords->point[reducedLines->coordIndex[reducedIndicesCount - 2]];
				SbVec3f pt2 = reducedCoords->point[reducedLines->coordIndex[reducedIndicesCount - 1]];
				float currentSegmentAngle = this->calcTheta(pt1, pt2);
				// project the current segment to a point totalLength distance
				SbVec3f currentSegmentProjectionPoint = this->calcVectorPoint(
					lastTestPoint, currentSegmentAngle, totalLength);
				float deflectionDistance = this->distance(currentSegmentProjectionPoint, currentTestPoint);				
				
				if (fabs(deflectionDistance) > 1.0 ) //MaxDeflection) !!!
					includeThisPoint = TRUE;
			
			}
			
		
			if (TRUE == includeThisPoint)
			{
				//TRACE("includedPoint\n");
				reducedCoords->point.set1Value(reducedCoordsCount, currentTestPoint);
				reducedLines->coordIndex.set1Value(reducedIndicesCount++, reducedCoordsCount++);
				totalLength = 0;
				includeThisPoint = FALSE;
				lastSavedPoint = currentTestPoint;
			}
			lastTestPoint = currentTestPoint;
			i++;
		}
		if (reducedCoords->point[reducedCoords->point.getNum() - 1] != currentTestPoint)
		{
			reducedCoords->point.set1Value(reducedCoordsCount, currentTestPoint);
			reducedLines->coordIndex.set1Value(reducedIndicesCount++, reducedCoordsCount++);
		}
		reducedLines->coordIndex.set1Value(reducedIndicesCount++, -1);
	}
	theCoords = reducedCoords;
	theLines = reducedLines;
}

float ReadDXF::distance(SbVec3f pt1, SbVec3f pt2)
{
    float val;
    val = fabs((pt2[0]-pt1[0])*(pt2[0]-pt1[0]))+fabs((pt2[1]-pt1[1])*(pt2[1]-pt1[1]));
    return sqrt(val);
}

double
ReadDXF::calcTheta(SbVec3f pt1, SbVec3f pt2)
{
	return  atan2 (( pt2[1] - pt1[1]) , ( pt2[0] - pt1[0] ));   
}

SbVec3f 
ReadDXF::calcVectorPoint (SbVec3f pt, double length, double theta)
{
	SbVec3f temp;
	
	temp[0] = pt[0] + length * cos(theta);
	temp[1] = pt[1] + length * sin(theta);
	temp[2] = 0.0;
	return temp;
}

double
ReadDXF::RadiansToDegrees(double radians)
{
	double degrees = (radians / MY_PI) * 180;
	if (degrees < 0)
		degrees = 360 + degrees;
	return degrees;
}

double
ReadDXF::DegreesToRadians(double degrees)
{
	double radians = (degrees / 180) * MY_PI;
	if (radians > MY_PI)
		radians = radians - (2 * MY_PI);
	return radians;
}
