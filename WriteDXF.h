////////////////////////////////////////////////////////////////
//                                                            //
//  WriteDXF.h                                                //
//							       Copyright: Paul Sorey 2001 //
////////////////////////////////////////////////////////////////
#pragma once
#ifndef WRITE_DXF_H
#define WRITE_DXF_H

#include <Inventor/nodes/SoCoordinate3.h>
#include <MyTypes.h>

class ReadDXF;
class Layer;
class Color;
class SoCoordinate3;

class WriteDXF
{
protected:

public:

	enum /* Line Translation Entity Type */
	{
		kAS_POLYLINE,
		kAS_LINES,
		kAS_ARC_TAN_POLYLINE,
		kAS_NURBS_CURVE,
		kAS_EP_CONTOURS,
		kAS_MS_CONTOURS,
	};

	// we could replicate or even subclass the node hierarchy to
	// include translation flags and DXF translation routines (???)...

	char		fLine[100];
	FILE *		fFp;
    FILE *      fBeginFile;
    FILE *      fEndFile;

		       
				WriteDXF(int test);
				WriteDXF(FILE * fp);      // specify the file pointer to write to...
//	void		ReadIvFile(void);
//	CString	    GetInputIVFilename(void);
	void        WriteCoords(SoCoordinate3 * coords);

	// file writing functions....
	void        WriteSEQUEND();
//	void        WriteARC(void);

    // use these:
    void        WriteBeginDXF(void);
    void        WriteEndDXF(void);

    void        WriteLWPOLYLINE(SoCoordinate3 *coords, char * layer = "0", int marker_interval = -1 ); // this one calls the next two...
	void		WriteLWPOLYLINEHeader(const char* layer, int numVertices, bool isClosed = FALSE);
	void		WriteLWPOLYLINEPoint(SbVec3f point, float segment_width = 0.0, double bulge = 0.0, bool isAnArc = FALSE );

    void        Write3dPOLYLINE(SoCoordinate3 *coords, char * layer = "0", int marker_interval = -1 ); // this one calls the next two...
	void		Write3dPOLYLINEHeader(const char* layer, int numVertices, bool isClosed = FALSE);
	void		Write3dPOLYLINEPoint(SbVec3f point, float segment_width = 0.0, double bulge = 0.0, bool isAnArc = FALSE );

   // void        ConcatenateDXF(void);   // calls a ruby script

	static int  GetTimestamp(void);
    void        WriteZero(void);
    void        WriteDXFEndsec(void);
    void		WriteENDSEC(void);
	void		WriteEOF(void);

	
    // deprecated...
	void		WriteLWPOLYLINE(CString filename, SoCoordinate3* sequentialCoords, int temp);
	void		WriteDXFHeader(void);

	/* not yet implemented...
	void		WriteLINE(DPoint endpoint1, DPoint endpoint2);
	void		WriteLINE(SbVec3f endpoint1, SbVec33f endpoint2);
	void		WritePOLYLINE(SoCoordinate3* coords, SoIndexedLineSet* indexedLineSet);
	void		WritePOLYLINE(SoCoordinate3* coords, SoLineSet* lineSet);
	void		WritePOLYLINE(SoVertexProperty* vp);
	void		WritePOLYLINE(DPoint* sequentialDPoints);
	void     WriteCIRCLE(void);
	void		WriteLWPOLYLINE(void);
   void		WriteLAYERTable();
	*/

	// implemented in ReadDXF...
	// SbVec3f	getWorld(SbVec3f pointVector /*point to transform*/, SbVec3f az /*arbitrary z-axis*/);
	// double	calcTheta(SbVec3f pt1, SbVec3f pt2);
	// SbVec3f	calcVectorPoint (SbVec3f pt, double length, double theta);
	// double	RadiansToDegrees(double radians);
	// double	DegreesToRadians(double degrees);
	// float		Distance(SbVec3f pt1, SbVec3f pt2);
};

#endif