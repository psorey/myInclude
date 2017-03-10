/* usage: tricut [filename.d] [maskfile.d] output: cut.d & edgetopo.d
 *   
 * removes polygons from a .d (flymax) file with one or more 
 * vertices inside area outlined in maskfile.d. (For pointfiles  
 * use grid_reduce.)
 *
 * author: Paul Sorey
 *         4/95
 */
#include "stdafx.h"

#include "Tricut.h"
#include <Triangulator.h>



Tricut::Tricut(FILE* triFile, FILE* areaFile, FILE* outputTriFile, FILE* outputEdgeFile)
{
	fOutputTriFile = outputTriFile;
	fOutputEdgeFile = outputEdgeFile;
	fInputAreaFile = areaFile;
	fInputTriFile = triFile;

	fInputTriCoords = NULL;
	fInputTriFaces = NULL;
	fInputAreaCoords = NULL;

	fOutputTriCoords = NULL;
	fOutputEdgeCoords = NULL;
	fOutputTriFaces = NULL;

	fTriangulatedAreaCoords = NULL;
	//fTriangulatedAreaFaces = NULL;

	fCurrentInputTriFacesIndex = 0;
}

Tricut::Tricut(const char* trifile, const char * areafile, const char * outputtrifile, const char * outputedgefile)
{
	fOutputTriFile = fopen(outputtrifile, "w");
	fOutputEdgeFile = fopen(outputedgefile, "w");
	fInputAreaFile = fopen(areafile, "r");
	fInputTriFile = fopen(trifile, "r");

	fInputTriCoords = NULL;
	fInputTriFaces = NULL;
	fInputAreaCoords = NULL;

	fOutputTriCoords = NULL;
	fOutputEdgeCoords = NULL;
	fOutputTriFaces = NULL;

	fTriangulatedAreaCoords = NULL;
	//fTriangulatedAreaFaces = NULL;

	fCurrentInputTriFacesIndex = 0;
}

Tricut::Tricut(CString trifile, CString areafile, CString outputtrifile, CString outputedgefile)
{
    fTriIn = trifile;
    fAreaIn = areafile;
    fTriOut = outputtrifile;
    fEdgeOut = outputedgefile;
}


Tricut::~Tricut(void)
{
	
}

bool Tricut::ReadInputFiles(void)
{

	ASSERT (fInputTriFile != NULL);
	ASSERT (fInputAreaFile != NULL);

	if (fInputTriFile == NULL || fInputAreaFile == NULL) return FALSE;
	
	SoSeparator* tempSep = new SoSeparator;
	tempSep->ref();

	SoInput myInput;

	// get input trifile data...
	myInput.openFile((const char *) fTriIn.GetString()) ;
	tempSep->addChild(SoDB::readAll(&myInput));
	myInput.closeFile();
    //fclose(fInputTriFile);

	SoCoordinate3* tempCoords = NULL;
	tempCoords = (SoCoordinate3*)FindChildOfType(tempSep, SoCoordinate3::getClassTypeId());

	ASSERT(tempCoords != NULL);
	
	if (tempCoords == NULL) return FALSE;

	fInputTriCoords = new SoCoordinate3;
	fInputTriCoords->point.deleteValues(0,-1);
	fInputTriCoords->copyFieldValues(tempCoords);
	
	SoIndexedFaceSet* tempFaces = (SoIndexedFaceSet*)FindChildOfType(
		tempSep, SoIndexedFaceSet::getClassTypeId());

	ASSERT(tempFaces != NULL);
	if (tempFaces == NULL) return FALSE;

	fInputTriFaces = new SoIndexedFaceSet;
	fInputTriFaces->copyFieldValues(tempFaces);

	tempSep->removeAllChildren();

	// get input areafile data...
	myInput.openFile((const char *) fAreaIn.GetString());
	tempSep->addChild(SoDB::readAll(&myInput));
	myInput.closeFile();	
	//fclose(fInputAreaFile);

	tempCoords = NULL;
	tempCoords = (SoCoordinate3*)FindChildOfType(
		tempSep, SoCoordinate3::getClassTypeId());

	if (tempCoords == NULL) return FALSE;

	fInputAreaCoords = new SoCoordinate3;
	fInputAreaCoords->point.deleteValues(0,-1);
	fInputAreaCoords->copyFieldValues(tempCoords);

	tempSep->removeAllChildren();
	tempSep->unref();

	if (fInputTriCoords == NULL ||
		 fInputTriFaces == NULL ||
		 fInputAreaCoords == NULL)
	{
		return FALSE;
	}
	else return TRUE;
}

/*
bool Tricut::ReadInputFiles(void)
{

	ASSERT (fInputTriFile != NULL);
	ASSERT (fInputAreaFile != NULL);

	if (fInputTriFile == NULL || fInputAreaFile == NULL) return FALSE;
	
	SoSeparator* tempSep = new SoSeparator;
	tempSep->ref();

	SoInput myInput;

	// get input trifile data...
	myInput.setFilePointer(fInputTriFile);
	tempSep->addChild(SoDB::readAll(&myInput));
	//myInput.closeFile();
    fclose(fInputTriFile);

	SoCoordinate3* tempCoords = NULL;
	tempCoords = (SoCoordinate3*)FindChildOfType(tempSep, SoCoordinate3::getClassTypeId());

	ASSERT(tempCoords != NULL);
	
	if (tempCoords == NULL) return FALSE;

	fInputTriCoords = new SoCoordinate3;
	fInputTriCoords->point.deleteValues(0,-1);
	fInputTriCoords->copyFieldValues(tempCoords);
	
	SoIndexedFaceSet* tempFaces = (SoIndexedFaceSet*)FindChildOfType(
		tempSep, SoIndexedFaceSet::getClassTypeId());

	ASSERT(tempFaces != NULL);
	if (tempFaces == NULL) return FALSE;

	fInputTriFaces = new SoIndexedFaceSet;
	fInputTriFaces->copyFieldValues(tempFaces);

	tempSep->removeAllChildren();

	// get input areafile data...
	myInput.setFilePointer(fInputAreaFile);
	tempSep->addChild(SoDB::readAll(&myInput));
	//myInput.closeFile();	
	fclose(fInputAreaFile);

	tempCoords = NULL;
	tempCoords = (SoCoordinate3*)FindChildOfType(
		tempSep, SoCoordinate3::getClassTypeId());


	if (tempCoords == NULL) return FALSE;

	fInputAreaCoords = new SoCoordinate3;
	fInputAreaCoords->point.deleteValues(0,-1);
	fInputAreaCoords->copyFieldValues(tempCoords);

	tempSep->removeAllChildren();
	tempSep->unref();

	if (fInputTriCoords == NULL ||
		 fInputTriFaces == NULL ||
		 fInputAreaCoords == NULL)
	{
		return FALSE;
	}
	else return TRUE;
}
*/

bool	Tricut::CutTrifile(void)
{
				int            discard = 0, m, i, j, k, qty;
				int            save_k[7], erase[5];
				int				count, count2;
				SoMFVec3f   	entity;
				SbVec3f			cross_pt[5];
				SbVec3f    		hold_point[10];
				SbVec3f			hold;
				int            cutline_print;

	//step through entities in datafile searching for entities that
	// have points inside the areafile entity 
   
	fCurrentInputTriFacesIndex = 0;
	fCurrentInputTriFacesIndex = 0;

	fOutputTriCoords = new SoCoordinate3;
	fOutputTriFaces = new SoIndexedFaceSet;
	fOutputEdgeCoords = new SoCoordinate3;
	fOutputEdgeLines = new SoIndexedLineSet;

	fOutputTriCoords->ref();
	fOutputTriFaces->ref();
	fOutputEdgeCoords->ref();
	fOutputEdgeLines->ref();
	fOutputTriCoords->point.deleteValues(0,-1);
	fOutputTriFaces->coordIndex.deleteValues(0,-1);
	fOutputEdgeCoords->point.deleteValues(0,-1);
	fOutputEdgeLines->coordIndex.deleteValues(0,-1);

	while( TRUE == GetInputTrifileEntity(entity)) 
	{		
		// how many points are inside the area, if any?      

		discard = 0;
		int numEntityPoints = entity.getNum();    

		for (int vertex = 0; vertex < numEntityPoints; vertex++)
		{
			if ( IsInside ( entity[vertex][0], entity[vertex][1], fInputAreaCoords->point ))
			{
				discard++;
			}
		}
	   
		// if none of the points are inside the cut area, save the triangle...
		if (discard == 0) 
		{
			int pointNumber;
			for (k = 0; k < numEntityPoints; k++)
			{
				pointNumber = fOutputTriCoords->point.getNum();
				fOutputTriCoords->point.set1Value(pointNumber, entity[k]);
				fOutputTriFaces->coordIndex.set1Value(fOutputTriFaces->coordIndex.getNum(), pointNumber);
			}
			fOutputTriFaces->coordIndex.set1Value(fOutputTriFaces->coordIndex.getNum(), -1);
		}
		
		/* if some of the points are inside, trim the 
		 * edge entities at the area line -- add two 
		 * coplanar points, delete the inside points,
		 * and fprintf the new entity, making sure the new
		 * polygon has a counterclockwise orientation. */
		
		
		else if (discard < 3)
		{
			count2 = 0;

			cutline_print = 1;
			k = 0;
			j = 0;
			m = 0;
			for ( i = 0; i < numEntityPoints -1; i++) // !!!
			{ 
				
				/* if the current line segment crosses an 
				 * area_entity line segment, add the cross point
				 * to the hold_point list  */
				
				hold_point[k] = entity[i];
				if (IsInside (hold_point[k][0], hold_point[k][1], fInputAreaCoords->point))
					erase[m++] = k;
				k++;
				if ((Findcross (hold, entity[i], entity[i+1], fInputAreaCoords->point)))
				{
					save_k[j++] = k;
					hold_point[k++] = hold;
					
					/* if the next point is inside, this
					 * crossing point must be the "first",
					 * so write it to the cutline file */

					if (IsInside (entity[i+1][0], entity[i+1][1], fInputAreaCoords->point))
					{
						int coordNumber = fOutputEdgeCoords->point.getNum();
						fOutputEdgeCoords->point.set1Value(coordNumber, hold_point[k-1]);
						fOutputEdgeLines->coordIndex.set1Value(fOutputEdgeLines->coordIndex.getNum(),
							coordNumber);

						if (j == 1)
							cutline_print = 0;
					}    
					if (j > 1 && cutline_print == 0) 
					{
						int coordNumber = fOutputEdgeCoords->point.getNum();
						fOutputEdgeCoords->point.set1Value(coordNumber, hold_point[k-1]);
						fOutputEdgeLines->coordIndex.set1Value(fOutputEdgeLines->coordIndex.getNum(),
							coordNumber);
					}
					if (j > 1 && cutline_print == 1) 
					{
						int coordNumber = fOutputEdgeCoords->point.getNum();
						fOutputEdgeCoords->point.set1Value(coordNumber, hold_point[save_k[0]]);
						fOutputEdgeLines->coordIndex.set1Value(fOutputEdgeLines->coordIndex.getNum(),
							coordNumber);
					}
				}
			}
			hold_point[k] = entity[i];
			if (IsInside (hold_point[k][0], hold_point[k][1], fInputAreaCoords->point))
				erase[m++] = k;
			k++;
			if (Findcross (hold, entity[numEntityPoints - 1], entity[0], fInputAreaCoords->point))
			{
				save_k[j] = k;
				hold_point[k] = hold;
				++k;
				
				/* otherwise, this must be the one we want 
				 *for the cutline file, by default */
				
				int coordNumber = fOutputEdgeCoords->point.getNum();
				fOutputEdgeCoords->point.set1Value(coordNumber, hold_point[k-1]);
				fOutputEdgeLines->coordIndex.set1Value(fOutputEdgeLines->coordIndex.getNum(),
						coordNumber);

				if (cutline_print == 1)
				{
					int coordNumber = fOutputEdgeCoords->point.getNum();
					fOutputEdgeCoords->point.set1Value(coordNumber, hold_point[save_k[0]]);
					fOutputEdgeLines->coordIndex.set1Value(fOutputEdgeLines->coordIndex.getNum(),
						coordNumber);
				}
			}
			fOutputEdgeLines->coordIndex.set1Value(fOutputEdgeLines->coordIndex.getNum(), -1);

			/* ok, now that we have all 5 points we need to
			 * get rid of the inside points while keeping
			 * the remaining points in order. */
			
			count = 0;
			m = 0;
			qty = (numEntityPoints - discard) + 2;
			for (i = 0; i < k; i++)
			{
				if (erase[m] == i)
					m++;
				else
				{
					int pointNumber = fOutputTriCoords->point.getNum();
					fOutputTriCoords->point.set1Value(pointNumber, hold_point[i]);
					fOutputTriFaces->coordIndex.set1Value(fOutputTriFaces->coordIndex.getNum(), pointNumber);
				}
			}
			fOutputTriFaces->coordIndex.set1Value(fOutputTriFaces->coordIndex.getNum(), -1);			    
			for (m = 0; m < 4; m++)
				erase[m] = 0;
		}
		// otherwise, discard the entity
	}

	SoWriteAction wa;
	wa.getOutput()->openFile((const char *)fTriOut.GetString());
	wa.apply(fOutputTriCoords);
	wa.apply(fOutputTriFaces);
	wa.getOutput()->closeFile();

	wa.getOutput()->openFile((const char *) fEdgeOut.GetString());
	wa.apply(fOutputEdgeCoords);
	wa.apply(fOutputEdgeLines);
	wa.getOutput()->closeFile();

	fOutputEdgeCoords->unref();
	fOutputTriCoords->unref();
	fOutputTriFaces->unref();
	fOutputEdgeCoords = NULL;
	fOutputTriCoords = NULL;
	fOutputTriFaces = NULL;

	return TRUE;
}


bool Tricut::Findcross (SbVec3f& returnPoint, SbVec3f point1, SbVec3f point2, 
				const SoMFVec3f& entity) 
{
	double      x1, y1, z1, x2, y2, z2, c, ac, slope, aslope;
	double      ax1, ax2, ay1, ay2;
	int         i, numEntityPoints, j;
	SbVec3f    temp;
	
	x1 = point1[0];
	x2 = point2[0];
	y1 = point1[1];
	y2 = point2[1];
	z1 = point1[2];
	z2 = point2[2];

	numEntityPoints = entity.getNum();
	if (x2 == x1)
	{
		//returnPoint[0] = x1;
		TRACE("1 divide by zero\n");
		//return FALSE;
		slope = 9999999;
	}
	else 
		slope = (y2 - y1) / (x2 - x1);
	c = y2 - (slope * x2);

	for (i = 0; i < numEntityPoints; i++) 
	{
		j=i+1;
		if (i == numEntityPoints - 1) 
			j = 0;
		ax1 = entity[i][0];
		ay1 = entity[i][1];
		ax2 = entity[j][0];
		ay2 = entity[j][1];
	
		if (ax2 == ax1)
		{
			TRACE ("2 avoiding divide by 0\n");
			//return FALSE;
			aslope = 9999999;
		}
		else 
			aslope = (ay2 - ay1) / (ax2 - ax1);
		ac = ay2 - (aslope * ax2);

		temp[0] = (ac - c) / (slope - aslope);
		temp[1] = slope * temp[0] + c;
					
		if (((x1 > temp[0]) && (temp[0] > x2)) || ((x2 > temp[0] ) && (temp[0] > x1)))
		{
			if (((ax1 > temp[0]) && (temp[0] > ax2)) ||((ax2 > temp[0] ) && (temp[0] > ax1)))
			{
				if (z2 == z1) 
					temp[2] = z2;
				else
					temp[2] = z1 + (((temp[0] - x1) / (x2 - x1)) * (z2 - z1));
				
				returnPoint = temp;
				TRACE("Findcross::returnPoint = %f   %f   %f\n", returnPoint[0],
					returnPoint[1], returnPoint[2]);
				if (returnPoint[2] == 0 || returnPoint[0] > 20000)
				{
					int kk = 0;
					int  jj = 0;
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}

bool Tricut::GetInputTrifileEntity(SoMFVec3f& returnCoords)
{
	returnCoords.deleteValues(0,-1);

	if (fCurrentInputTriFacesIndex >= fInputTriFaces->coordIndex.getNum() -1)
		return FALSE;

	int index;
	while	((index = (fInputTriFaces->coordIndex[fCurrentInputTriFacesIndex++])) != -1)
	{
		returnCoords.set1Value(returnCoords.getNum(), fInputTriCoords->point[index]);
	}
	return TRUE;
}


//void Tricut::TriangulateCutArea(SoMFVec3f inputAreaCoords



void Tricut::TriangulateCutArea(void)
{
	ASSERT( NULL != fInputAreaCoords);
	SoMFInt32 inputCoordIndex;
	inputCoordIndex.deleteValues(0, -1);

	int numCoords = fInputAreaCoords->point.getNum();
	for (int i = 0; i < numCoords; i++)
		inputCoordIndex.set1Value(i, i);
	Triangulator::triangulate(fInputAreaCoords->point, inputCoordIndex, 
		fTriangulatedAreaIndices);
	
	/*
	//test...
	SoIndexedFaceSet* faces = new SoIndexedFaceSet;
	for (i = 0; i < fTriangulatedAreaIndices.getNum(); i++)
		faces->coordIndex.set1Value(i, fTriangulatedAreaIndices[i]);
	SoSeparator* sep = new SoSeparator;
	sep->ref();
	fInputAreaCoords->ref();
	sep->addChild(fInputAreaCoords);
	sep->addChild(faces);

    SoWriteAction wa;
    wa.getOutput()->openFile("test.iv");
    wa.apply(sep); 
    wa.getOutput()->closeFile();   
	 sep->unref();
	 */
}


bool Tricut::IsInside(double x, double y, const SoMFVec3f& coords)
{
	// step through cut area triangles...
	ASSERT( fTriangulatedAreaIndices.getNum() >= 4); // we need at least 1 triangle...
	int numTriangles = fTriangulatedAreaIndices.getNum() / 4;

	int numCoords = fTriangulatedAreaIndices.getNum();
	
	for (int i = 0; i < numCoords; i += 4)
	{
		int pt0 = fTriangulatedAreaIndices[i];
		int pt1 = fTriangulatedAreaIndices[i + 1];
		int pt2 = fTriangulatedAreaIndices[i + 2];

		bool isInside = Triangulator::TriangleInsideTest(coords, SbVec3f(x, y, 0.0), pt0, pt1, pt2);
		if (TRUE == isInside)
		{
			TRACE("isInside\n");
			return TRUE;
		}
	}
	return FALSE;
}



/*
// IsInside:    returns non-zero value if coordinates are inside area 
		// delineated in areafile 

bool Tricut::IsInside( double x, double y, const SoMFVec3f& area )
{
	double      area_x1, area_x2, area_y1, area_y2;
	int         i,j,k;
	double      ycross[20];
	double		ytemp1;
	double		ytemp2;
	double		yadd;
	double		yadd0;
	double		yadd1;
	double		yadd2;
	int         num_ycross;
	int			count;
	int			count1;
	int			count2;
	int         area_n;
	
	count = 0;
	num_ycross = 0;
	++count;

	//TRACE("numAreaPoints = %d\n", area.getNum());
	area_n = fInputAreaCoords->point.getNum();

	area_x1 = fInputAreaCoords->point[0][0];
	area_y1 = fInputAreaCoords->point[0][1];  
	
	for( i = 1; i <= area_n; i++ ) {
		area_x2 = fInputAreaCoords->point[i][0];
		area_y2 = fInputAreaCoords->point[i][1];   
		++count;
   
	// IsInside:    if the line crosses the y-axis of the test point 
	//		then calculate the ycross point 

		if (( area_x2 > x && x > area_x1) || 
				( area_x2 < x && x < area_x1)) {
			
			yadd = ((fabs( x - area_x1)) / 
				(fabs( area_x2 - area_x1))) * 
				(fabs( area_y2 - area_y1 ));
			yadd0 = (fabs( x - area_x1));
			yadd1 = (fabs( area_x2 - area_x1));
			yadd2 = (fabs( area_y2 - area_y1));
			yadd = yadd0 / yadd1 * yadd2;

			if ( area_y2 > area_y1)
				ycross[num_ycross] = area_y1 + yadd;
			else 
				ycross[num_ycross] = area_y1 - yadd;
			++num_ycross;
		}
		area_x1 = area_x2;
		area_y1 = area_y2;
	}
	

	// IsInside:  sort the ycross[numEntityPoints] points in ascending order 

   ytemp1 = ytemp2 = 0;
   count2 = count1 = num_ycross;
   j=k=0;
   ytemp1 = ycross[k];

	while ( k < (num_ycross - 1)) {
		while (--count1) {
			++j;
			ytemp2 = ycross[j];

			if (ytemp2 < ytemp1) {
				ycross[k] = ytemp2;
				ycross[j] = ytemp1;
				ytemp1 = ycross[k];
			}
		}
		++k;
		j=k;
		--count2;
		count1 = count2;
		ytemp1 = ycross[k];
	}

	i=0;
	while (i < num_ycross){
		if (ycross[i] < y && y < ycross[i+1])
		{
			TRACE("isInsideOld\n");
			return TRUE;
		}
		++i;
		++i;
	}
	return FALSE;
}

*/

SoNode*	Tricut::FindChildOfType(SoGroup * parent, SoType typeId)
{
    SoSearchAction search;
    SoPath *path = NULL;
    SoNode *node = NULL;
    
    search.setType(typeId, FALSE);
    search.setInterest(SoSearchAction::FIRST);
    search.apply(parent);
    
    path = search.getPath();
    if (path == NULL) return NULL;
    node = path->getTail();
    return node;
}
	
