//  Copyright 2000 Paul Sorey.
//  "myInclude/GeoGraphic.cpp"

#include "stdafx.h"
#include <GeoGraphic.h>
#include <Graphfix.h>
//#include "GetDoubleDialog.h"
#include <Inventor/Win/viewers/SoWinExaminerViewer.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoSeparator.h>
#include <ReadDXF.h>
#include <Inventor/SbLinear.h>
#include <Inventor/elements/SoCacheElement.h>
//#include "MessageBox.h"

#include <GL/gl.h>
#include <GL/glu.h>


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
		CString errorMsg = "can't find ";
		errorMsg += LPCTSTR(fFilename);

		TRACE("can't find %s\n", errorMsg);
//		CMessageBox msg(LPCTSTR(errorMsg));
//		msg.DoModal();
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
	graphfix.setBuffer(fImageBuffer);
	graphfix.readTarga();
	graphfix.close();
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
	gluScaleImage(GL_RGB, fWidth, fHeight ,GL_UNSIGNED_BYTE, (unsigned char*)fImageBuffer, 
			newWidth, newHeight, GL_UNSIGNED_BYTE, (unsigned char*)tempBuffer);
	RgbImage* scaledCopy = new RgbImage(0, CString(""), newWidth, newHeight);
	scaledCopy->SetBuffer(tempBuffer);

	// test ..... !!!
	/*
	Graphfix graphfix((unsigned char*)tempBuffer);
	CString testName;
	testName = LPCTSTR(fFilename);
	testName += ".tga";
	graphfix.setName(LPCTSTR(testName));
	graphfix.setSize(newWidth, newHeight);	
	graphfix.saveTarga(newWidth, newHeight, LPCTSTR(testName));			
	*/
	return scaledCopy;
}


GeoGraphic::GeoGraphic(int imageHandle, 
							  SbVec3f origin, 
							  float scale,
							  float rotation,
							  int width,
							  int height,
							  SoCoordinate3* perimeterCoords)
{
	fImageHandle = imageHandle;
	fOrigin = origin;
	fScale = scale;
	fRotation = rotation;
	fWidth = width;
	fHeight = height;
	fPerimeterCoords = new SoCoordinate3;
	fPerimeterCoords->ref();
	fPerimeterCoords->copyFieldValues(perimeterCoords);


}


GeoGraphic::~GeoGraphic()
{
	fPerimeterCoords->unref();
}


GeoGraphicMgr::GeoGraphicMgr(SoWinExaminerViewer* viewer)
{
	fViewer = viewer;
	fGeoGraphicList = new SbPList;
	fRgbImageList = new SbPList;
	fPerimeter = new SoCoordinate3;
	fPerimeter->ref();
	fCompositeImage = NULL;
	//fDisplayImage = NULL;
	fTestRgbImage = NULL;
}


GeoGraphicMgr::~GeoGraphicMgr()
{
	fPerimeter->unref();
	if (fCompositeImage != NULL)
	{
		delete [] fCompositeImage;
		fCompositeImage = NULL;
	}
}


bool GeoGraphicMgr::CreateCompositeImage(void)
// this one is not speed-critical...
{

		//// test //////// !!!
	SoSeparator* tempSep = new SoSeparator;
	tempSep->ref();
	for(int i = 0; i < fGeoGraphicList->getLength(); i++)
	{
		GeoGraphic* graphic = (GeoGraphic*)((*(const SbPList*)fGeoGraphicList)[i]);
		SoSeparator* subSep = new SoSeparator;
		SoLineSet* tempLines = new SoLineSet;
		SoTransform* tempTrans = new SoTransform;
		subSep->addChild(tempTrans);
		tempTrans->translation.setValue(graphic->fOrigin);
		subSep->addChild(graphic->fPerimeterCoords);
		subSep->addChild(tempLines);
		tempSep->addChild(subSep);
	}
	SoWriteAction wa;
	wa.getOutput()->openFile("perimeter.iv");
	wa.apply(tempSep);
	wa.getOutput()->closeFile();
	tempSep->unref();
	///////////////////////////

	int numImages = fRgbImageList->getLength();
	for (i = 0; i < numImages; i++)
	{
		RgbImage* image = (RgbImage*)((*(const SbPList*)fRgbImageList)[i]);
		if (FALSE == (image->LoadImage())) return FALSE;
	}
	int numGeoGraphics = fGeoGraphicList->getLength();
	
	float lowestX = 100000000;
	float lowestY = 100000000;
	float highestX = -100000000;
	float highestY = -100000000;
	float lowestScale = 10000000;

	for (i = 0; i < numGeoGraphics; i++)
	{
		TRACE("Num geoGraphics = %d\n", numGeoGraphics);
		GeoGraphic* graphic = (GeoGraphic*)((*(const SbPList*)fGeoGraphicList)[i]);
		SbVec3f lowerLeft = 	graphic->GetOrigin();
		float scale = graphic->GetScale();
		float width = graphic->GetWidth();
		float height = graphic->GetHeight();
		float upperRightX = lowerLeft[0] + (width * scale);
		float upperRightY = lowerLeft[1] + (height * scale);
		if (lowerLeft[0] < lowestX) lowestX = lowerLeft[0];
		if (lowerLeft[1] < lowestY) lowestY = lowerLeft[1];
		if (upperRightX > highestX) highestX = upperRightX;
		if (upperRightY > highestY) highestY = upperRightY;
		if (scale < lowestScale) lowestScale = scale;
	}

	// test
	//lowestScale = .555;

	fCompositeImageScale = lowestScale;

	TRACE("lowestX = %f   lowestY = %f   lowestScale = %f \n", lowestX, lowestY, lowestScale); 
	TRACE("highestX = %f   highestY = %f  \n", highestX, highestY); 
	fCompositeImageOrigin = SbVec3f(lowestX, lowestY, 0.0); // origin of fCompositeImage...in distance units

	int xPixels = (int)(highestX - lowestX) / lowestScale;
	int yPixels = (int)(highestY - lowestY) / lowestScale;
	
	int remainder = xPixels % 4;
	if (remainder != 0)
		xPixels += (4 - remainder);

	// change the size of the composite image if desired...
	// otherwise the scale = lowestScale (highest resolution)...
	int imageSize = xPixels * yPixels;
	int kImageSize = (int) (imageSize / 1000.0);
	char str[30];
	sprintf(str, "%d", kImageSize);
	strcat(str, "K");

//	CGetDoubleDialog dlg("Select a scale for the composite image", "default image size =", str);
//	dlg.fDouble = lowestScale;
//	int ret = dlg.DoModal();
//	if (ret == IDOK)
//		lowestScale = dlg.fDouble;
	

	TRACE("width = %d    height = %d\n", xPixels, yPixels);

	// create the composite image buffer and initialize with the viewer
	// background color...
	if (NULL != fCompositeImage)
	{
		delete [] fCompositeImage;
		fCompositeImage = NULL;
	}
	fCompositeImage = new Rgb[(xPixels + 2) * (yPixels + 2)]; // adequate size always?? !!!
	SbColor viewerBackgroundColor = fViewer->getBackgroundColor();
	Rgb defaultColor(viewerBackgroundColor[0], viewerBackgroundColor[1], viewerBackgroundColor[2]);
	int numPixels = xPixels * yPixels;
	for (i = 0; i < numPixels; i++)
	{
		fCompositeImage[i] = defaultColor;
	}

	// make a scaled copy for each GeoGraphic object, align it with the
	// fCompositeImage space, and write the appropriate pixels,
	// then destroy the scaled copy...
	for (i = 0; i < numGeoGraphics; i++)
	{
		GeoGraphic* graphic = (GeoGraphic*)((*(const SbPList*)fGeoGraphicList)[i]);
		RgbImage* rgbImage = this->GetRgbImage(graphic);
		float originalScale = graphic->GetScale();
		float scaleFactor = lowestScale / originalScale;
		RgbImage* scaledRgbImage = rgbImage->MakeScaledCopy(scaleFactor);
		
		// calculate the pixel location (in fCompositeImage) of the
		// origin pixel (of the GeoGraphic object image)...
		int xOriginPixel;
		int yOriginPixel;
		float xDistanceToOrigin = graphic->GetOrigin()[0] - fCompositeImageOrigin[0];
		xOriginPixel = xDistanceToOrigin / lowestScale;
		float yDistanceToOrigin = graphic->GetOrigin()[1] - fCompositeImageOrigin[1];
		yOriginPixel = yDistanceToOrigin / lowestScale;
		TRACE("xy origin pixels = %d   %d\n", xOriginPixel, yOriginPixel);

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
	
	/*
	Graphfix graphfix((unsigned char*)fCompositeImage);
	graphfix.setName("compositeImage.tga");
	graphfix.setSize(xPixels, yPixels);
	graphfix.saveTarga(xPixels, yPixels, "compositeImage.tga");	
	*/

	fCompositeImageWidth = xPixels;
	fCompositeImageHeight = yPixels;
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
	int imageHandle = graphic->GetImageHandle();
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


RgbImage* GeoGraphicMgr::GetNewView(void)
{
	//RgbImage* rgbImage = (RgbImage*)((*(const SbPList*)fRgbImageList)[0]);
	this->GetViewportParams();
	return fTestRgbImage;

	/*
	this->GetViewportParams();

	double objx, objy, objz;  
	double winx = 1;
	double winy = 1;
	double winz = .9999999;

	double modelMatrix[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);

	double projMatrix[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	int atest = gluUnProject(winx, winy, winz, modelMatrix, projMatrix, viewport,
				&objx, &objy, &objz);
	TRACE("objectCoords = %f  %f  %f\n", objx, objy, objz);
	//if(objz > 1) objz = 1;
	glRasterPos3f(objx,objy,objz);

	glDrawPixels(fCompositeWidth,fCompositeHeight,GL_RGB, GL_UNSIGNED_BYTE, fDisplayImage);
	glFlush();

	//SoCacheElement::invalidate(action->getState());
	*/
}

/*
void GeoGraphicMgr::SetDisplayImageScale(float scale)
{
	if (fabs(scale - fDisplayImageScale) < .0001)
		return; // don't change it...

	fDisplayImageScale = scale;
	if (fDisplayImage != NULL)
	{
		delete [] fDisplayImage;
		fDisplayImage = NULL;
	}
	float scaleFactor = fCompositeImageScale / fDisplayImageScale;
	
	TRACE("fDisplayImageScale = %f\n", fDisplayImageScale );
	// scale the image from fCompositeImage...
	fDisplayImageWidth = (int)((float)fCompositeImageWidth * scaleFactor);   
	fDisplayImageHeight = (int)(((float)fCompositeImageHeight/(float)fCompositeImageWidth) * fDisplayImageWidth);
	TRACE("fDisplayImageWidth = %d\n", fDisplayImageWidth );
	TRACE("fDisplayImageHeight = %d\n", fDisplayImageHeight );
	
	// adjust width to multiple of 4...
	int remainder = fDisplayImageWidth % 4;
	if (remainder != 0)
		fDisplayImageWidth +=  (4 - remainder);


	fDisplayImage = new Rgb[(fDisplayImageWidth) * (fDisplayImageHeight)];
	gluScaleImage(GL_RGB, fCompositeImageWidth, fCompositeImageHeight ,GL_UNSIGNED_BYTE, (unsigned char*)fCompositeImage, 
			fDisplayImageWidth, fDisplayImageHeight, GL_UNSIGNED_BYTE, (unsigned char*)fDisplayImage);

}
*/

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

	//oSubImageSize[0] = iDesiredImageSize[0] / scaleRatio;
	//oSubImageSize[1] = iDesiredImageSize[1] / scaleRatio;
	
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


void GeoGraphicMgr::GetViewportParams(void)
{
	SoCamera *camera = fViewer->getCamera();
	fVpRegion = fViewer->getViewportRegion(); 
	float aR = fVpRegion.getViewportAspectRatio();
	SbViewVolume vV = camera->getViewVolume(aR);
	SbVec2s originPixels = fVpRegion.getViewportOriginPixels();
	SbVec2s sizePixels = fVpRegion.getViewportSizePixels();
	TRACE("origin = %d  %d   size = %d  %d\n", originPixels[0], originPixels[1],
		sizePixels[0], sizePixels[1]);
}