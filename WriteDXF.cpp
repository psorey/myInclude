#include "stdafx.h"
#include <WriteDXF.h>
#include <ReadDXF.h>

//#include <iostream>
//#include <string>
//#include <fstream>

//using namespace std;

#include <ctime>

#ifndef MY_PI
#define MY_PI 3.141592653589793238462643
#endif 

WriteDXF::WriteDXF(int test)
{
	fFp = NULL;
}

WriteDXF::WriteDXF(FILE* fp)  // assumes this file is open
{
	fFp = fp;
    /* initialize random seed: */
    srand ( time(NULL) );
}


void WriteDXF::WriteCoords(SoCoordinate3 * coords) {
	WriteBeginDXF();
	int numVertices = coords->point.getNum();
	TRACE("num vertices = %d\n", numVertices);
	char layer[2] = "0";
	// _itoa( j, layer, 10);
	//writeDXF->WriteLWPOLYLINEHeader(layer, numVertices); 
	Write3dPOLYLINEHeader(layer, numVertices);
	for (int i = 0; i < numVertices; i++) {
		SbVec3f point = coords->point[i];
		Write3dPOLYLINEPoint(point);
	}
	WriteSEQUEND();
	WriteDXFEndsec();
	WriteEndDXF();
}



void WriteDXF::WriteBeginDXF(void)
{
    if(NULL == (fBeginFile = fopen("c:\\bin\\begin_dxf.txt", "r"))) {
        ::AfxMessageBox("'c:\bin\begin_dxf.txt' not found",MB_OK|MB_ICONSTOP);
        return;
    }       
    char line[1500];
    while(fgets(line, 1500, fBeginFile ) != NULL )  {
      fprintf(fFp, "%s", line);
    }
    fclose(fBeginFile);
}


void WriteDXF::WriteEndDXF(void) 
{
    if(NULL == (fEndFile = fopen("c:\\bin\\end_dxf.txt", "r"))) {
        ::AfxMessageBox(" 'c:\bin\end_dxf.txt' not found",MB_OK|MB_ICONSTOP);
        return;
    }
    char line[200];
    while ( fgets( line, 200, fEndFile ) != NULL )  {
       fprintf(fFp, "%s", line);
    }
    fclose(fEndFile);
}


void WriteDXF::WriteZero(void)
{
    fprintf(fFp, "%s\n", "  0");
}


void WriteDXF::Write3dPOLYLINEHeader(const char* layer, int numVertices, bool isClosed /* = 0 */ )
{       
    int entity_stamp = this->GetTimestamp();
    TRACE("entity_stamp 3d polyline header = %d\n", entity_stamp);
    char line[100];
    fprintf(fFp,"POLYLINE\n  5\n%d\n100\nAcDbEntity\n  8\n%s\n100\n", entity_stamp, layer);
    fprintf(fFp,"AcDb3dPolyline\n 66\n     1\n 10\n0.0\n 20\n0.0\n 30\n0.0\n 70\n     8\n  0\n");
}


void WriteDXF::Write3dPOLYLINE(SoCoordinate3 * coords, char * layer , int mark_interval)
{
	TRACE("write 3dPolyline\n");
    mark_interval = 10;
    int numCoords = coords->point.getNum();
    if(numCoords < 2) return;
    float width = 0.0;
    for(int i=0; i<numCoords; i++) {
        SbVec3f point = coords->point[i];
        if (mark_interval > 0) {
            if(i % mark_interval == 0) {
                width = 0.2;
            }
        }
        this->Write3dPOLYLINEPoint(point, width);
    }
    fprintf(fFp, "SEQEND\n");
}

void WriteDXF::WriteSEQUEND() {
    int entity_stamp = this->GetTimestamp();
	fprintf(fFp, "SEQEND\n  5\n");
    fprintf(fFp,"%d\n", entity_stamp);
	//fprintf(fFp, "330\n16C\n");
    fprintf(fFp,"100\nAcDbEntity\n  8\n0\n  0\n");
}


void WriteDXF::Write3dPOLYLINEPoint(SbVec3f point, float segment_width /* = 0 */, double bulge /* = 0.0 */, bool isAnArc /* = FALSE */)
{
    int entity_stamp = this->GetTimestamp();
    TRACE("write3dpolyline_point entity stamp = %d\n", entity_stamp);
    fprintf(fFp,"VERTEX\n  5\n");
    fprintf(fFp,"%d\n", entity_stamp);
	fprintf(fFp, "330\n22E\n");
    fprintf(fFp,"100\nAcDbEntity\n  8\n0\n100\nAcDbVertex\n100\nAcDb3dPolylineVertex\n");

    fprintf(fFp," 10\n");
    fprintf(fFp,"%f\n", point[0]);
    fprintf(fFp," 20\n");
    fprintf(fFp,"%f\n", point[1]);
    fprintf(fFp," 30\n");
    fprintf(fFp,"%f\n", point[2]);
    // spec width for each segment even if zero...
   // fprintf(fFp, " 40\n");
   // fprintf(fFp, "%f\n", segment_width);
   // fprintf(fFp, " 41\n");
   // fprintf(fFp, "%f\n", segment_width);
    fprintf(fFp," 70\n    32\n  0\n");
}




void WriteDXF::WriteLWPOLYLINE(SoCoordinate3 * coords, char * layer , int mark_interval)
{
    mark_interval = 10;
    int numCoords = coords->point.getNum();
    if(numCoords < 2) return;
    float width = 0.0;
	this->WriteLWPOLYLINEHeader("mylayer", numCoords);
    for(int i=0; i<numCoords; i++) {
        SbVec3f point = coords->point[i];

        if (mark_interval > 0) {
            if(i % mark_interval == 0) {
                width = 0.2;
            }
        }
		this->WriteLWPOLYLINEPoint(point, width);
	}
	this->WriteZero();
	TRACE("end of polyline\n");
}

int WriteDXF::GetTimestamp(void)
{
    int iSecret;
    /* generate secret number: */
    iSecret = rand() % 10000 + 250;
    return iSecret;
}


void WriteDXF::WriteDXFHeader(void)
{
	//sprintf(fDxfString,"  0\nSECTION\n  2\nENTITIES\n  0\n");
}

void WriteDXF::WriteLWPOLYLINEHeader(const char* layer, int numVertices, bool isClosed /* = 0 */ )
{ 	
    int entity_stamp = this->GetTimestamp();
    TRACE("entity_stamp = %d\n", entity_stamp);
    char line[100];
	fprintf(fFp,"LWPOLYLINE\n  5\n%d\n100\nAcDbEntity\n  8\n%s\n100\n", entity_stamp, layer);
	fprintf(fFp,"AcDbPolyline\n 90\n%9d\n 70\n     1\n 43\n0.0\n", numVertices);	
}


void WriteDXF::WriteDXFEndsec(void)
{
	//this->WriteENDSEC();
}


void WriteDXF::WriteLWPOLYLINEPoint(SbVec3f point, float segment_width /* = 0 */, double bulge /* = 0.0 */, bool isAnArc /* = FALSE */)
{
    fprintf(fFp," 10\n");
	fprintf(fFp,"%f\n", point[0]);
	fprintf(fFp," 20\n");
	fprintf(fFp,"%f\n", point[1]);

    // spec width for each segment even if zero...
    fprintf(fFp, " 40\n");
	fprintf(fFp, "%f\n", segment_width);
    fprintf(fFp, " 41\n");
	fprintf(fFp, "%f\n", segment_width);

    if (isAnArc == TRUE) {
		fprintf(fFp, " 42\n");
		fprintf(fFp, "%f\n", bulge);
	}
}


void WriteDXF::WriteENDSEC()	
{	
	fprintf(fFp,"ENDSEC\n");  // !!! the 0 should come at the end of each LWPOLYLINE...
}

void WriteDXF::WriteEOF()
{
	fprintf(fFp, "  0\nEOF\n");
}

////////////////////////////////////////

void WriteDXF::WriteLWPOLYLINE(CString filename, SoCoordinate3* sequentialCoords, int temp)
{
	//fFp = fopen(LPCTSTR(filename), "w");
	this->WriteDXFHeader();
	int numCoords = sequentialCoords->point.getNum();
	this->WriteLWPOLYLINEHeader("0", numCoords);
	for (int i=0; i<numCoords; i++) {
        SbVec3f point = sequentialCoords->point[i];
		this->WriteLWPOLYLINEPoint(point);
	}
	//this->WriteENDSEC();
	//this->WriteEOF();
	//fclose(fFp);
}


