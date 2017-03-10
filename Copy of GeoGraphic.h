// Copyright 2000 Paul Sorey
// "myInclude/GeoGraphic.h"

#include <Inventor/SbLinear.h>
#include <Inventor/fields/SoMFVec3f.h>
#include <Inventor/SbViewportRegion.h>


#ifndef _GEO_GRAPHIC_H
#define _GEO_GRAPHIC_H

class SoWinExaminerViewer;
class Rgb;
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
	int				fImageHandle;	// handle of RgbImage. (There may be several SourceGeoGraphic objects 
											// that use a single RgbImage)...
	float				fScale;			// distanceUnits / pixel.
	SbVec3f			fOrigin;			// x,y of top left pixel.
	float				fWidth;			// width of entire image...
	float				fHeight;			// height of entire image...
	float				fRotation;		// rotation from north.
	SoCoordinate3*	fPerimeterCoords;		// list of corner points.
	int				fPriority;		// 1 = on top, higher number = let other image overlay it.
	float				fBrightness;   // factor to multiply each pixel in RgbImage to get display image brightness.
	float				fContrast;		// factor to increase contrast of RgbImage for display image.

	//GeoGraphic(void);
	GeoGraphic( int imageHandle, 
					SbVec3f origin, 
					float scale, float rotation, int width, int height, 
					SoCoordinate3* perimeterCoords);  
	~GeoGraphic(void);
	SbVec3f			GetOrigin(void) { return fOrigin; }
	int				GetWidth(void) { return fWidth; }
	int				GetHeight(void) { return fHeight; }
	float				GetScale(void) { return fScale; }
	SoCoordinate3*	GetPerimeter(void) { return fPerimeterCoords; }
	int				GetImageHandle(void) { return fImageHandle; }

};


class GeoGraphicMgr
// manages a group of geoGraphic images, and controls their display at proper 
// scale and location, (manages loading and unloading of image buffers for
// efficient memory management ???), calculates the appropriate display mask for
// fast display updates...
{
public:
	RgbImage*			fTestRgbImage;
	SoWinExaminerViewer* fViewer;					// Pointer to the orthographic viewer.
	Rgb*					fCompositeImage;			// The highest-resolution composite image from which
															//    lower-resolution fDisplayImage images will be copied.
	int					fCompositeImageWidth;
	int					fCompositeImageHeight;
	double					fCompositeImageScale;

	SbPList*				fGeoGraphicList;			// List of spatially located images that make up
															//		the display image.
	SbPList*				fRgbImageList;				// List of RgbImage objects -- there may be
															//		several GeoGraphic objects that
															//		refer to a single RgbImage object.
	SoCoordinate3*		fPerimeter;					// The series of line segments that form the
															//		perimeter of the composite display image.
	SbVec3f				fCompositeImageOrigin;	// lower left pixel location in fCompositeImage.
	SbViewportRegion	fVpRegion;

	GeoGraphicMgr(SoWinExaminerViewer* viewer);
	~GeoGraphicMgr();
	
	SbVec2s GetCompositePixelLocation(SbVec2f pixelLocationXY);
	SbVec2f GetCompositePixelLocation(SbVec2s pixel);
	// returns the pixel corresponding to an x-y coordinate in model space
	
	void GetDisplayRectangleParams(SbVec2s iDesiredImageSize, SbVec2f iImageLocation, double iScale, 
					SbVec2s& oSubImageSize, SbVec2s& oSkipPixels,
					int& oTotalRowLength, SbVec2f& oRasterPosition, float& oPixelZoom);
	// input:  window size, model space x-y coordinates of window origin, & scale of the window view,
	// output: subImage size, skip pixels and row length for use as arguments in the
	//				glDrawPixels(), plus new raster position for glRasterPosition()...

	//	int	ReadDxfImageInfo(FILE* fp); (this is in ReadDXF)
	// Calls the ReadDXF utility that
	//	reads image information from a dxf file.  It creates the list of
	// GeoGraphic objects and list of RgbImage objects.
	// Returns number of GeoGraphic objects added.

	void	SetDisplayImageScale(float scale);
	// Called when camera moves in z-axis toward or away from x-y plane...
	//	places a copy of the source images at the new scale into fDisplayImage, using fBrightness
	// and fContrast to modify it if desired. (and fRotation ?)...
	
	bool	GetDisplayImage(Rgb* resultImage, SbVec3f origin, float widthPixels, float heightPixels);
	// called when camera moves in x-y plane...or in response to foreground changes in the model...
	// returns a portion of the scaled fImageBuffer image within the given rectangle...

	void Add(GeoGraphic* newGeoGraphic);
	void Add(RgbImage* newRgbImage);
	// void GetNewView(void);
	// recalculates fUpperLeftPixel, fScale, (fRotation) and calls SetDisplayImageScale(),
	// GetDisplayImage(), if required.
	
	void GetViewportParams(void);
	bool CreateCompositeImage(void);
	// makes an image buffer -- the original image from which scaled composite images are
	// made.  Default resolution is highest resolution of the source images, or user can
	// select a lower resolution for faster scale changes.
	
	RgbImage* GetRgbImage(GeoGraphic* graphic);
	// finds the RgbImage named in the GeoGraphic object and returns a pointer 
	// to it.

	void GetOrderedRowPerimeterCrossings(float y, const SoCoordinate3* scaledPerimeter, 
																	 SoCoordinate3* orderedCrossings);
	// utility to find pixels within perimeter.

	RgbImage* GetNewView(void);  // { return fCompositeImage; }
	// returns a pointer to 
};

#endif