// Copyright 2000 Paul Sorey
// "myInclude/GeoGraphic.h"

#include <Inventor/SbLinear.h>
#include <Inventor/fields/SoMFVec3f.h>
#include <Inventor/SbViewportRegion.h>
#include <GL/glu.h>

#ifndef _GEO_GRAPHIC_H
#define _GEO_GRAPHIC_H

class SoWinExaminerViewer;
class Rgb;
class Rgba;
class SoCoordinate3;
class SbPList;


class	RgbImage
// represents the raw image...
// an RgbImage should have the origin at lower left (pixel 0,0). 
{
public:
	Rgb*		fImageBuffer;
	int		fImageHandle;
	CString	fFilename;
	int		fWidth;
	int		fHeight;

	RgbImage(int handle, CString filename, float width, float height);
	~RgbImage(void);
	void			CreateBuffer(void);
	Rgb*			GetImageBuffer(void) { return fImageBuffer; }
	void			SetBuffer(Rgb* buf) { fImageBuffer = buf; }
	int			GetImageHandle(void) { return fImageHandle; }
	int			GetWidth(void) { return fWidth; }
	int			GetHeight(void) { return fHeight; }
	bool			LoadImage(void);
	void			UnloadImage(void);
	
	RgbImage*	MakeScaledCopy(float scaleFactor);
	// makes a new RgbImage object and creates an image buffer 	
	// containing the image at a new scale (but no handle or filename).
};

	

class GeoGraphic {
	// represents a geo-referenced RgbImage (located in space).
	// highest resolution raster image with geographic location information...
	// used as source for generating display images at different scales...
public:

	// the image as loaded from an image file (.tga, etc.)...
	int				fRawImageHandle;			// handle of RgbImage. (There may be several SourceGeoGraphic objects 
														// that use a single RgbImage)...
	float				fRawScale;					// distanceUnits / pixel of loaded rgb image...
	Rgb*           fRawImage;
	SbVec3f			fRawOrigin;
	int				fRawWidth;
	int				fRawHeight;
	float				fRawRotation;				// rotation from north (get this from dxf file...)

	// the formatted image is the raw image rotated and clipped with alpha channel, 
	Rgba*				fFormattedImage;			// clipped, rotated image, with alpha channel mask...
	SoCoordinate3*	fRawPerimeterCoords;
	SbVec3f			fFormattedOrigin;			// x,y of top left pixel.
	int				fFormattedWidth;			// width of entire image...
	int				fFormattedHeight;			// height of entire image...
	float          fFormattedScale;
	SoCoordinate3* fFormattedPerimeterCoords;

	//float				fBrightness;   // factor to multiply each pixel in RgbImage to get display image brightness.
	//float				fContrast;		// factor to increase contrast of RgbImage for display image.

	GeoGraphic(void);
	GeoGraphic( int imageHandle, 
					SbVec3f rawOrigin, 
					float rawScale, float rawRotation, int rawWidth, int rawHeight, 
					SoCoordinate3* rawPerimeterCoords);  
	~GeoGraphic(void);

	void GetDisplayRectangleParams(SbVec2s iDesiredImageSize, SbVec2f iImageLocation, double iScale, 
					SbVec2s& oSubImageSize, SbVec2s& oSkipPixels,
					int& oTotalRowLength, SbVec2f& oRasterPosition, float& oPixelZoom);
	// input:  window size, model space x-y coordinates of window origin, & scale of the window view,
	// output: subImage size, skip pixels and row length for use as arguments in the
	//				glDrawPixels(), plus new raster position for glRasterPosition()...

	SbVec2s GetFormattedPixelLocation(SbVec2f pixelLocationXY);

	SbVec3f			GetRawOrigin(void) { return fRawOrigin; }
	int				GetRawWidth(void) { return fRawWidth; }
	int				GetRawHeight(void) { return fRawHeight; }
	float				GetRawScale(void) { return fRawScale; }
	SoCoordinate3*	GetRawPerimeter(void) { return fRawPerimeterCoords; }
	int				GetRawImageHandle(void) { return fRawImageHandle; }
	Rgb*				GetRawImage(void);

	SbVec3f			GetFormattedOrigin(void) { return fFormattedOrigin; }
	int				GetFormattedWidth(void) { return fFormattedWidth; }
	int				GetFormattedHeight(void) { return fFormattedHeight; }
	float				GetFormattedScale(void) { return fFormattedScale; }
	SoCoordinate3*	GetFormattedPerimeter(void) { return fFormattedPerimeterCoords; }
	Rgba*				GetFormattedImage(void) { return fFormattedImage; }

	bool				CreateFormattedImage(float desiredRotation);

};


class GeoGraphicMgr
// manages a group of geoGraphic images, and controls their display at proper 
// scale and location, (manages loading and unloading of image buffers for
// efficient memory management ???), calculates the appropriate display mask for
// fast display updates...
{
public:
	SoWinExaminerViewer* fViewer;					// Pointer to the orthographic viewer.
	SbPList*				fGeoGraphicList;			// List of spatially located images that make up
															//		the display image.
	SbPList*				fRgbImageList;				// List of RgbImage objects -- there may be
															//		several GeoGraphic objects that
															//		refer to a single RgbImage object.

	GeoGraphicMgr(SoWinExaminerViewer* viewer);
	~GeoGraphicMgr();
	
	int			GetNumGraphics(void) { return fGeoGraphicList->getLength(); }
	GeoGraphic* GetGraphic(int i) { return (GeoGraphic*)((*(const SbPList*)fGeoGraphicList)[i]); }

	SbVec2s GetCompositePixelLocation(SbVec2f pixelLocationXY);
	SbVec2f GetCompositePixelLocation(SbVec2s pixel);
	// returns the pixel corresponding to an x-y coordinate in model space
	

	//	int	ReadDxfImageInfo(FILE* fp); (this is in ReadDXF)
	// Calls the ReadDXF utility that
	//	reads image information from a dxf file.  It creates the list of
	// GeoGraphic objects and list of RgbImage objects.
	// Returns number of GeoGraphic objects added.

	void	SetDisplayImageScale(float scale);
	// Called when camera moves in z-axis toward or away from x-y plane...
	//	places a copy of the source images at the new scale into fDisplayImage, using fBrightness
	// and fContrast to modify it if desired. (and fRotation ?)...
	
	void Add(GeoGraphic* newGeoGraphic);
	void Add(RgbImage* newRgbImage);
	
	bool CreateFormattedImages(void);
	// asks each GeoGraphic object to create its own formatted image from the raw rgb image,
	// given the parameters contained in the GeoGraphic object (and adding to it the view rotation???)
	
	bool LoadRgbImages(void);

	RgbImage* GetRgbImage(GeoGraphic* graphic);
	// finds the RgbImage named in the GeoGraphic object and returns a pointer 
	// to it.

	void GetOrderedRowPerimeterCrossings(float y, const SoCoordinate3* scaledPerimeter, 
																	 SoCoordinate3* orderedCrossings);
	// utility to find pixels within perimeter.


};

#endif