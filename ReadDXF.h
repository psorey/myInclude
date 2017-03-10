//Copyright 2000 Paul Sorey.
#pragma once
// "myInclude/ReadDXF.h"
#include <afx.h>
#ifndef _READ_DXF_H_
#define _READ_DXF_H_

#include <Inventor/SbColor.h>
#include <Inventor/fields/SoMFVec3f.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/SbPList.h>

class SoSeparator;
//class GeoGraphicMgr;

class Layer
{
	CString name;
	CString linetype;
	char flags;
	int color;

	bool IsFrozen() { return (flags & 1) == 1; }
	bool IsOff() { return color < 0; }
	CString GetName() { return name; }
	
};

class DxfColor
{
	CString name; // ???
	int color;

	static SbColor GetSbColor(int color);
};


class ReadDXF 
{
// right now this reads only line information...
// image info added 5/00
public:


protected:
	char                line[100];
	FILE                *ifp;
	long                 lnct;
	long                 count;
	long                 coordIndexNum;
	long                 vertexNum;
	SoSeparator         *parent;
	double              x,y,z,trash, x1,y1,z1,x2,y2,z2;
	SoCoordinate3       *theCoords;  
	SoIndexedLineSet*	theLines;
	SoCoordinate3*		fPointCoords;
	SoSeparator*		topSep;
	
	float				fMaxDeflection;
	float				fMinLength;
	bool				fTestLengthEnabled;
	bool				fTestDeflectionEnabled;
	bool				fEndOfLWPOLYLINE;


	CString	            inDxfFilename;
	CString	            outIvFilename;

public:
				ReadDXF(SoSeparator *_parent);
        		~ReadDXF();
	float		distance(SbVec3f pt1, SbVec3f pt2);
	void		writeIvFile(void);
	CString     GetDxfFilename() { return inDxfFilename; }
	CString	    getOutIvFilename(void);
	void		reducePoints(float minLength);

	SoIndexedLineSet*   GetIndexedLines(void) { return theLines; }
	SoCoordinate3*		GetCoords(void) { return theCoords; }

	void		setMaxDeflection(float deflection) { fMaxDeflection = deflection; }
	void		setMinLength(float minLength) { fMinLength = minLength; }
	void		setTestLengthEnabled(bool enabled) { fTestLengthEnabled = enabled; }
	void		setTestDeflectionEnabled(bool enabled) { fTestDeflectionEnabled = enabled; }
	SbBool	    readFile();
	SbBool      readFile(FILE * fp, CString filename);

	// static utilities not requiring a ReadDXF object...
	
	static	bool	FindNot(const char* findString, const char* notString, FILE* fp);
	// moves file pointer to line after finding "findString" and returns TRUE,
	// but if it finds "notString" first (or EOF), returns FALSE...

	static	bool	GetVariable(const char* str, float& var, FILE* fp);
	static	bool	GetVariable(const char* str, int& var, FILE* fp);
	static  bool    GetVariable(const char* str, char& var, FILE* fp);
	static	bool	GetVariable(const char* str, CString& var, FILE* fp);
	static	bool	GetHexVariable(const char* str, int& var, FILE* fp);
	// returns the value after the line containing "str"...
	
//	static	int	ReadImageInfo(GeoGraphicMgr* theGeoGraphicMgr, FILE* fp);
	// populates a GeoGraphicMgr object with GeoGraphic objects -- all fields filled in
	// except for loading the image file...

	bool	    ReadLayerTable(void);
	SbPList*	fLayerList;

protected:	
	void		get_group ( char group_number[5] );
	void		get_xyz( void );
	void		getPOINT(void);
	void		getLINE(void);
	void		getPOLYLINE(void);
	void		getLWPOLYLINE(void);
	void        getARC(void);
	void        getCIRCLE(void);
	SbVec3f	    getWorld(SbVec3f pointVector /*point to transform*/, SbVec3f az /*arbitrary z-axis*/);
	double	    calcTheta(SbVec3f pt1, SbVec3f pt2);
	SbVec3f	    calcVectorPoint (SbVec3f pt, double length, double theta);
	double	    RadiansToDegrees(double radians);
	double	    DegreesToRadians(double degrees);

};

#endif