//  Copyright 2000 Paul Sorey.
//  "myInclude/GeoGraphic.cpp"

#include "stdafx.h"
#include <GeoGraphic.h>
#include <Graphfix.h>
#include <Inventor/Win/viewers/SoWinExaminerViewer.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoSeparator.h>
#include <ReadDXF.h>
#include <Inventor/SbLinear.h>
#include <Inventor/elements/SoCacheElement.h>

//#include <GL/gl.h>
//#include <GL/glu.h>


RgbImage::RgbImage(int handle, CString filename, float width, float height)
{
	fImageHandle = handle;
	fFilename = filename;
	fWidth = width;
	fHeight = height;
	fImageBuffer = NULL;
}


RgbImage::~RgbImage(void)
{
	if (NULL != fImageBuffer)
	{
		delete [] fImageBuffer;
		fImageBuffer = NULL;
	}
}


void RgbImage::CreateBuffer(void)
{
	fImageBuffer = new Rgb[fWidth * fHeight];
}


bool RgbImage::LoadImage(void)
{
	Graphfix graphfix;
	if ((graphfix.openTarga((char *)LPCTSTR(fFilename))) == FALSE) 
	{
		CString errorMsg = LPCTSTR("can't find ");
		errorMsg += LPCTSTR(fFilename);

		TRACE("can't find %s\n", errorMsg);
		return FALSE;
	}
	graphfix.getSize(fWidth, fHeight);
	TRACE("size = %d   %d\n", fWidth, fHeight);

	if (NULL != fImageBuffer)
	{
		delete [] fImageBuffer;
		fImageBuffer = NULL;
	}
	fImageBuffer = new Rgb[fWidth * fHeight];  // !!! right?
	if (fImageBuffer == 0)
	{
		char message[100];
		sprintf(message, "not enough memory for %s to be loaded", fFilename);
		AfxMessageBox(LPCTSTR(message));
		return FALSE;
	}
	graphfix.setBuffer(fImageBuffer);
	graphfix.readTarga();
	graphfix.close();


	// adjust width to multiple of 4...
	int remainder = fWidth % 4;
	if (remainder != 0)
	{
		// there's something weird about this....!!!
		RgbImage* copy = this->MakeScaledCopy(1.0);
		delete [] fImageBuffer;
		fImageBuffer = NULL;
		fWidth = copy->GetWidth();
		fHeight = copy->GetHeight();
		fImageBuffer = copy->GetImageBuffer();
	}
	return TRUE; // !!! need error checking...
}


void RgbImage::UnloadImage(void)
{
	if (NULL != fImageBuffer)
	{
		delete [] fImageBuffer;
		fImageBuffer = NULL;
	}	
}


RgbImage* RgbImage::MakeScaledCopy(float scaleFactor)
{
	TRACE("RgbImage::MakeScaledCopy()\n");
	TRACE("fWidth = %d    fHeight = %d\n", fWidth, fHeight);
	int newWidth = (int)((float)fWidth / scaleFactor);   /// !!!
	int newHeight = (int)(((float)fHeight/(float)fWidth) * newWidth);
	
	// adjust width to multiple of 4...
	int remainder = newWidth % 4;
	if (remainder != 0)
		newWidth +=  (4 - remainder);
	TRACE("newWidth = %d\n", newWidth);

	Rgb* tempBuffer = new Rgb[(newWidth) * (newHeight)];
	//gluScaleImage(GL_RGB, fWidth, fHeight ,GL_UNSIGNED_BYTE, (unsigned char*)fImageBuffer, 
	//		newWidth, newHeight, GL_UNSIGNED_BYTE, (unsigned char*)tempBuffer);
	RgbImage* scaledCopy = new RgbImage(0, CString(""), newWidth, newHeight);
	scaledCopy->SetBuffer(tempBuffer);
	return scaledCopy;
}

GeoGraphic::GeoGraphic(void)
{
	//fRawPerimeterCoords = new SoCoordinate3;
	//fRawPerimeterCoords->ref();
	// the above are created and ref()'d in loading the DXF file...

	// should we instead create the GeoGraphic and pass a pointer to the dxf loader? YES!!!
	TRACE("did default geographic constructor\n");
}

GeoGraphic::GeoGraphic(int imageHandle, 
							  SbVec3f rawOrigin, 
							  float rawScale,
							  float rawRotation,
							  int rawWidth,
							  int rawHeight,
							  SoCoordinate3* rawPerimeterCoords)
{
	fRawImageHandle = imageHandle;
	fRawOrigin = rawOrigin;
	fRawScale = rawScale;
	fRawWidth = rawWidth;
	fRawHeight = rawHeight;
	fRawRotation = rawRotation;
	fRawPerimeterCoords = rawPerimeterCoords;
	int numPerimeterPoints = fRawPerimeterCoords->point.getNum();
	TRACE("\nnumPerimeterPoints = %d\n", numPerimeterPoints);
	TRACE("fRawPerimeterCoords = %d\n\n", fRawPerimeterCoords);
	fFormattedPerimeterCoords = new SoCoordinate3;
	fFormattedPerimeterCoords->ref();
	fRawImage = NULL;
	fFormattedImage = NULL;
}


GeoGraphic::~GeoGraphic()
{
	fRawPerimeterCoords->unref();
	fFormattedPerimeterCoords->unref();
}


bool GeoGraphic::CreateFormattedImage(float viewRotation)
{
	fFormattedScale = fRawScale;
	// total rotation = viewRotation + fRotation
	float rotation = viewRotation + fRawRotation;

	// create a rotation matrix...
	SbRotation sbRotation = SbRotation(SbVec3f(0, 0, 1), rotation);

	// transform perimeter & find bounding rectangle...
	TRACE("fRawPerimeterCoords = %d\n\n", fRawPerimeterCoords);
	int numPerimeterPoints = fRawPerimeterCoords->point.getNum();
	fFormattedPerimeterCoords->point.deleteValues(0, -1);
    int i;
	for (i = 0; i < numPerimeterPoints; i++)
	{
		SbVec3f sourcePoint;
		SbVec3f destinationPoint;
		sbRotation.multVec(fRawPerimeterCoords->point[i], destinationPoint);
		fFormattedPerimeterCoords->point.set1Value(i, destinationPoint);
	}

	SbVec2f lowerLeft = SbVec2f(1000000, 10000000);
	SbVec2f upperRight = SbVec2f(-10000000, -10000000);
	for (i = 0; i < numPerimeterPoints; i++)
	{
		if (fFormattedPerimeterCoords->point[i][0] < lowerLeft[0]) 
			lowerLeft[0] = fFormattedPerimeterCoords->point[i][0];
		if (fFormattedPerimeterCoords->point[i][1] < lowerLeft[1]) 
			lowerLeft[1] = fFormattedPerimeterCoords->point[i][1];
		if (fFormattedPerimeterCoords->point[i][0] > upperRight[0]) 
			upperRight[0] = fFormattedPerimeterCoords->point[i][0];
		if (fFormattedPerimeterCoords->point[i][1] > upperRight[1]) 
			upperRight[1] = fFormattedPerimeterCoords->point[i][1];
	}
	// get the integer values of the new rectangle...
	SbVec2s LLeft = SbVec2s(floor(lowerLeft[0]), floor(lowerLeft[1]));
	SbVec2s URight = SbVec2s(floor(upperRight[0]) + 1, floor(upperRight[1]) + 1);
	fFormattedWidth = URight[0] - LLeft[0];
	fFormattedHeight = URight[1] - LLeft[1];

   
	// adjust width to multiple of 4...
	int remainder = fFormattedWidth % 4;
	if (remainder != 0)
		fFormattedWidth +=  (4 - remainder);
	TRACE("newWidth = %d\n", fFormattedWidth);

	// get the fFormattedOrigin point...

	SbVec2s pixelOffset = SbVec2s (-(LLeft[0]), -(LLeft[1]));
	fFormattedOrigin[0] = fRawOrigin[0] - (pixelOffset[0] * fRawScale);
	fFormattedOrigin[1] = fRawOrigin[1] - (pixelOffset[1] * fRawScale);

	// create an RGBA bitmap at the size of the new rectangle...
	if (fFormattedImage != NULL)
	{
		delete [] fFormattedImage;
		fFormattedImage = NULL;
	}
	int formattedSize = fFormattedWidth * fFormattedHeight;
	fFormattedImage = new Rgba[formattedSize]; 
	
	// fill it with transparent pixels...
	for (i = 0; i < formattedSize; i++)
	{
		fFormattedImage[i].a = 0;
	}

	if (fRawImage == NULL)
		TRACE("fRawImage == NULL !!!\n");

	// step through pixels in original and transform them to new locations in fFormattedImage...
	for (int y = 0; y < fRawHeight; y++)
	{
		int formattedX;
		int formattedY;
		Rgb rawPixel;
		for (int x = 0; x < fRawWidth; x++)
		{
			rawPixel = fRawImage[y * fRawWidth + x];
			SbVec3f formattedPixelLocation;
			sbRotation.multVec(SbVec3f(x, y, 0), formattedPixelLocation);
			//formattedX = formattedPixelLocation[0] - LLeft[0];
			formattedX = formattedPixelLocation[0] + pixelOffset[0];
			//formattedY = formattedPixelLocation[1] - LLeft[1];
			formattedY = formattedPixelLocation[1] + pixelOffset[1];
			if (formattedX < 0)
			{
				TRACE("formattedX = %d  rawX = %d\n", formattedX, x);
				formattedX = 0;
			}
			if (formattedX >= fFormattedWidth)
			{
				TRACE("formattedX = %d  rawX = %d\n", formattedX, x);
				formattedX = fFormattedWidth - 1;
			}
			if (formattedY < 0)
			{
				TRACE("formattedY = %d  rawY = %d\n", formattedY, y);
				formattedY = 0;
			}
			if (formattedY >= fFormattedHeight)
			{
				TRACE("formattedY = %d  rawY = %d\n", formattedY, y);
				formattedY = fFormattedHeight - 1;
			}
			fFormattedImage[formattedY * fFormattedWidth + formattedX] = 
				Rgba(rawPixel.r, rawPixel.g, rawPixel.b, 1);
			fFormattedImage[formattedY * fFormattedWidth + formattedX +1] = 
				Rgba(rawPixel.r, rawPixel.g, rawPixel.b, 1);
		}
	}
	return TRUE;
}


void GeoGraphic::GetDisplayRectangleParams(SbVec2s iDesiredImageSize, SbVec2f iImageLocation, double iScale, 
					SbVec2s& oSubImageSize, SbVec2s& oSkipPixels,
					int& oTotalRowLength, SbVec2f& oRasterPosition, float& oPixelZoom)
{
	SbVec2s originPixel = this->GetFormattedPixelLocation(iImageLocation);
	oRasterPosition = iImageLocation;
	if (originPixel[0] < 0) // i.e., if the origin of the image is INSIDE the frame...
		oRasterPosition[0] = fFormattedOrigin[0];
	if (originPixel[1] < 0)
		oRasterPosition[1] = fFormattedOrigin[1];

	if (originPixel[0] < 0)
		oSkipPixels[0] = 0;
	else
	{
		oSkipPixels[0] = originPixel[0];
		//oRasterPosition[0] = 0; /// !!! testing this
	}
	if (originPixel[1] < 0)
		oSkipPixels[1] = 0;
	else
	{
		oSkipPixels[1] = originPixel[1];
		//oRasterPosition[1] = 0; /// !!! testing this too...
	}
	oTotalRowLength = fFormattedWidth;
	double scaleRatio = fFormattedScale / iScale;
	oPixelZoom = (float)scaleRatio;
	
	//test...
	oSubImageSize[0] = fFormattedWidth;
	oSubImageSize[1] = fFormattedHeight;

	// get the distance from raster position to upperRightCorner
	// get the distance in pixels between the two points.
	// scale by zoom factor and find number of pixels.
	// If it is less than the actual number of pixels, trim off the extras...

	// if the desired width exceeds the size of the compositeImage
	// cut it to size...
	if (oSubImageSize[0] + oSkipPixels[0] > fFormattedWidth)
		oSubImageSize[0] = fFormattedWidth - oSkipPixels[0];
	if (oSubImageSize[1] + oSkipPixels[1] > fFormattedHeight)
		oSubImageSize[1] = fFormattedHeight - oSkipPixels[1];
}

SbVec2s GeoGraphic::GetFormattedPixelLocation(SbVec2f location)
{
	SbVec2s pixel;
	pixel[0] = (location[0] - fFormattedOrigin[0]) / fFormattedScale;  
	pixel[1] = (location[1] - fFormattedOrigin[1]) / fFormattedScale; 
	return pixel;
}

////////////////////////////////////////////////////////////////////
// GeoGraphicMgr
////////////////////////////////////////////////////////////////////

GeoGraphicMgr::GeoGraphicMgr(SoWinExaminerViewer* viewer)
{
	fViewer = viewer;
	fGeoGraphicList = new SbPList;
	fRgbImageList = new SbPList;
}


GeoGraphicMgr::~GeoGraphicMgr()
{
	delete fGeoGraphicList;
	delete fRgbImageList;
}


bool GeoGraphicMgr::LoadRgbImages(void)
// this one is not speed-critical...
{
	int numImages = fRgbImageList->getLength();
    int i;
	for (i = 0; i < numImages; i++)
	{
		RgbImage* image = (RgbImage*)((*(const SbPList*)fRgbImageList)[i]);
		if (FALSE == (image->LoadImage())) return FALSE;
	}
	int numGeoGraphics = fGeoGraphicList->getLength();
	for (i = 0; i < numGeoGraphics; i++)
	{
		GeoGraphic* graphic = (GeoGraphic*)((*(const SbPList*)fGeoGraphicList)[i]);
		graphic->fRawImage = this->GetRgbImage(graphic)->GetImageBuffer();
	}
/*
		// make a scaled copy of the perimeter...
		SoCoordinate3* scaledPerimeter = new SoCoordinate3;
		scaledPerimeter->ref();
		scaledPerimeter->point.deleteValues(0, -1);
		SoCoordinate3* perimeter = graphic->GetPerimeter();
		int numCoords = perimeter->point.getNum();
		
		TRACE("\nscaled perimeter\n");
		for (int k = 0; k < numCoords; k++)
		{
			SbVec3f point = perimeter->point[k];
			point /= scaleFactor;
			scaledPerimeter->point.set1Value(k, point);
			TRACE("point[%d] = %f  %f\n", k, point[0], point[1]);
		}

		// copy pixels from the GeoGraphic into the fCompositeImage...
		int width = scaledRgbImage->GetWidth();
		int height = scaledRgbImage->GetHeight();
		TRACE("XXXXXXXXXXXXXXXXXX scaled width = %d   height = %d\n", width, height);
		Rgb* scaledImageBuffer = scaledRgbImage->GetImageBuffer();
		SoCoordinate3* orderedCrossings = new SoCoordinate3;
		orderedCrossings->ref();
		orderedCrossings->point.deleteValues(0, -1);
		for (int y = 0; y < height; y++)
		{
			//TRACE("graphic %d   height = %d\n", i, y);
			this->GetOrderedRowPerimeterCrossings((float)y, scaledPerimeter, orderedCrossings);
			int numOrderedCrossings = orderedCrossings->point.getNum();
			// do this row of pixels...
			for (int x = 0; x < width; x++)
			{
				for (int j = 0; j < numOrderedCrossings - 1; j += 2)
				{
					int compositeHeight = y + yOriginPixel;
					int compositeWidth  = x + xOriginPixel;
					int pixelNumber = compositeHeight * xPixels + compositeWidth;
					//int pixelNumber = y * width + x;

					if (x < orderedCrossings->point[j][0]) 
					{
						// write a 0,0,0 to this pixel location ( plus pixel offset )
						// so it won't be copied to fCompositeImage...
						// fCompositeImage[pixelNumber] = Rgb(0,0,0);
						j = numOrderedCrossings; // get out of loop.
					}
					else if ( x < orderedCrossings->point[j + 1][0])
					{
						// copy the pixel color here ( plus pixel offset )...
						fCompositeImage[pixelNumber] = scaledImageBuffer[y * width + x];
						j = numOrderedCrossings; // get out of loop.
					}
				}
			}
		}
		orderedCrossings->unref();
		scaledPerimeter->unref();
		delete scaledRgbImage;
	}

	fCompositeImageWidth = xPixels;
	fCompositeImageHeight = yPixels;
	return TRUE;
*/
	return TRUE;
}



bool GeoGraphicMgr::CreateFormattedImages(void)

// create a new image buffer that will accomodate the formatted image,
// rotate and clip the image to its boundaries amd make pixels outside
// the perimeter transparent...
// this happens once when the images are loaded, and any time the view is rotated...
// this one is not speed-critical...
{
	//int numImages = fRgbImageList->getLength();
	//for (int i = 0; i < numImages; i++)
	//{
	//	RgbImage* image = (RgbImage*)((*(const SbPList*)fRgbImageList)[i]);
	//	if (FALSE == (image->LoadImage())) return FALSE;
	//}

	int numGeoGraphics = fGeoGraphicList->getLength();	
    int i;
	for (i = 0; i < numGeoGraphics; i++)
	{
		// tell each GeoGraphic object to create its own formatted image given the rotation...
		GeoGraphic* graphic = (GeoGraphic*)((*(const SbPList*)fGeoGraphicList)[i]);
		RgbImage* image = this->GetRgbImage(graphic);
		if (FALSE == (image->LoadImage())) return FALSE;
		graphic->fRawImage = image->GetImageBuffer();  // redundancy somewhere else???

		float viewRotation = 0; // !!! later we can add rotation of the view...
		graphic->CreateFormattedImage(viewRotation); // set scale, create buffer, set width & height
		image->UnloadImage();
		// how do I set graphic->fRawImage to NULL ?
		graphic->fRawImage = NULL;
	}

		//// test //////// !!!
	SoSeparator* tempSep = new SoSeparator;
	tempSep->ref();
	for(i = 0; i < fGeoGraphicList->getLength(); i++)
	{
		GeoGraphic* graphic = (GeoGraphic*)((*(const SbPList*)fGeoGraphicList)[i]);
		SoSeparator* subSep = new SoSeparator;
		SoLineSet* tempLines = new SoLineSet;
		SoTransform* tempTrans = new SoTransform;
		subSep->addChild(tempTrans);
		tempTrans->translation.setValue(graphic->fRawOrigin);
		tempTrans->scaleFactor.setValue(graphic->fFormattedScale, graphic->fFormattedScale, 1);
		subSep->addChild(graphic->fFormattedPerimeterCoords);
		subSep->addChild(tempLines);
		tempSep->addChild(subSep);
	}
	SoWriteAction wa;
	wa.getOutput()->openFile("perimeter.iv");
	wa.apply(tempSep);
	wa.getOutput()->closeFile();
	tempSep->unref();
	///////////////////////////

	return TRUE;
}


void GeoGraphicMgr::GetOrderedRowPerimeterCrossings(float y, const SoCoordinate3* scaledPerimeter, 
																	 SoCoordinate3* orderedCrossings)
{
	// get a list of x-values where the row of pixels crosses
	// the perimeter, and put them in ascending order...
	ASSERT(NULL != orderedCrossings);
	SoMFVec3f unorderedValues;
	unorderedValues.deleteValues(0, -1);

	int numCoords = scaledPerimeter->point.getNum();
	// orderedCrossings->point.deleteValues(0, -1);
	// get the un-ordered crossing points...
	SbVec3f ptA;
	SbVec3f ptB;
	int count = 0;
	for (int k = 0; k < numCoords; k++)
	{
		ptA = scaledPerimeter->point[k];
		if (k < numCoords - 1)
			ptB = scaledPerimeter->point[k + 1];
		else
			ptB = scaledPerimeter->point[0];

		if ((ptA[1] > y && y > ptB[1]) || (ptA[1] < y && y < ptB[1]))
		{
			float ratio = (fabs(ptA[1] - y)) / (fabs(ptA[1] - ptB[1]));
			float xCross = ptA[0] - ((ptA[0] - ptB[0]) * ratio);
			unorderedValues.set1Value(count++, SbVec3f(xCross, y, 0)); 
			TRACE("Unordered value %d   =   %f\n", count - 1, xCross);
		}
	}

	// now make sure the coordinates are in order of x value...
	
	numCoords = unorderedValues.getNum();
	int		newCoordsCount = 0;
	SbVec3f	savedPoint;
	int		whichSavedPoint;
	
	while (newCoordsCount < numCoords)
	{
		float	savedDistance = 10000000.0;
		for (int k = 0; k < unorderedValues.getNum(); k++)
		{
			if (unorderedValues[k][0] < savedDistance)
			{
				savedPoint = unorderedValues[k];
				savedDistance = unorderedValues[k][0];
				whichSavedPoint = k;
			}
		}
		orderedCrossings->point.set1Value(newCoordsCount, savedPoint);
		newCoordsCount++;
		unorderedValues.deleteValues(whichSavedPoint, 1);		
	}	
}
	

RgbImage* GeoGraphicMgr::GetRgbImage(GeoGraphic* graphic)
{
	int imageHandle = graphic->GetRawImageHandle();
	int numImages = fRgbImageList->getLength();

	// find RgbImage that matches GeoGraphic's fImageHandle...
	for (int i = 0; i < numImages; i++)
	{
		RgbImage* rgbImage = (RgbImage*)((*(const SbPList*)fRgbImageList)[i]);
		if (rgbImage->GetImageHandle() == imageHandle)
			return rgbImage;
	}
	return NULL;
}


void GeoGraphicMgr::Add(GeoGraphic* newGeoGraphic)
{
	fGeoGraphicList->append(newGeoGraphic);
}


void GeoGraphicMgr::Add(RgbImage* newRgbImage)
{
	fRgbImageList->append(newRgbImage);
}

/*
SbVec2s GeoGraphicMgr::GetCompositePixelLocation(SbVec2f location)
{
	SbVec2s pixel;
	pixel[0] = (location[0] - fCompositeImageOrigin[0]) / fCompositeImageScale;  
	pixel[1] = (location[1] - fCompositeImageOrigin[1]) / fCompositeImageScale; 
	return pixel;
}


SbVec2f GeoGraphicMgr::GetCompositePixelLocation(SbVec2s pixel)
{
	SbVec2f location;
	location[0] = fCompositeImageScale * pixel[0] + fCompositeImageOrigin[0];
	location[1] = fCompositeImageScale * pixel[1] + fCompositeImageOrigin[1];
	return location;
}
*/

/*
void GeoGraphicMgr::GetDisplayRectangleParams(SbVec2s iDesiredImageSize, SbVec2f iImageLocation, double iScale, 
					SbVec2s& oSubImageSize, SbVec2s& oSkipPixels,
					int& oTotalRowLength, SbVec2f& oRasterPosition, float& oPixelZoom)
{
	SbVec2s originPixel = this->GetCompositePixelLocation(iImageLocation);
	oRasterPosition = iImageLocation;
	if (originPixel[0] < 0) // i.e., if the origin of the image is INSIDE the frame...
		oRasterPosition[0] = fCompositeImageOrigin[0];
	if (originPixel[1] < 0)
		oRasterPosition[1] = fCompositeImageOrigin[1];

	if (originPixel[0] < 0)
		oSkipPixels[0] = 0;
	else
		oSkipPixels[0] = originPixel[0];
	
	if (originPixel[1] < 0)
		oSkipPixels[1] = 0;
	else
		oSkipPixels[1] = originPixel[1];
	
	oTotalRowLength = fCompositeImageWidth;
	double scaleRatio = fCompositeImageScale / iScale;
	oPixelZoom = (float)scaleRatio;
	
	//test...
	oSubImageSize[0] = fCompositeImageWidth;
	oSubImageSize[1] = fCompositeImageHeight;

	// get the distance from raster position to upperRightCorner
	// get the distance in pixels between the two points.
	// scale by zoom factor and find number of pixels.
	// If it is less than the actual number of pixels, trim off the extras...

	// if the desired width exceeds the size of the compositeImage
	// cut it to size...
	if (oSubImageSize[0] + oSkipPixels[0] > fCompositeImageWidth)
		oSubImageSize[0] = fCompositeImageWidth - oSkipPixels[0];
	if (oSubImageSize[1] + oSkipPixels[1] > fCompositeImageHeight)
		oSubImageSize[1] = fCompositeImageHeight - oSkipPixels[1];
}
*/