#include "stdafx.h"

#include <GCode.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedLineSet.h>

#define ENDJOG .1  // the distance for the last command.

//	SoSeparator* fEntitiesSep;
//	FILE* fDxfInFile;
//	FILE* fGCodeOutFile;


GCode::GCode(SoIndexedLineSet* lines, SoCoordinate3* coords)
{
	fIndexedLines = lines;
	fIndexedLines->ref();
	fCoordinate3 = coords;
	fCoordinate3->ref();
	fGCodeOutFile = NULL;
	fDxfInFile = NULL;
}

//	SetOutputFile(FILE* fp) { fGCodeOutFile = fp; }
//	SetIndexedLine(SoIndexedLineSet* indexedLine) { fIndexedLines = indexedLine; }
//	SetCoordinate3(SoCoordinate3* coordinate3) { fCoordinate3 = coordinate3; }

GCode::~GCode()
{
	fIndexedLines->unref();
	fCoordinate3->unref();
}

void GCode::WriteGCode(FILE* fp)
{
	fGCodeOutFile = fp;
	ASSERT(fIndexedLines != NULL);
	ASSERT(fCoordinate3 != NULL);
	ASSERT(fGCodeOutFile != NULL);

	fGCodeLineCount = 0;
	
	int startNewEntity = TRUE;
	//fprintf(fGCodeOutFile, "%%\n");
	fprintf(fGCodeOutFile, "G17 G20 G40 G49 G54 G80 G90 G94\n", fGCodeLineCount++);
	fprintf(fGCodeOutFile, "F30.0\n");
	int numIndices = fIndexedLines->coordIndex.getNum();
	SbVec3f lastPoint;
    int i;
	for (i = 0; i < numIndices; i++) {
		if (fIndexedLines->coordIndex[i] == -1) {
			startNewEntity = TRUE;
			continue;
		} else {
			int index = fIndexedLines->coordIndex[i];
			if (startNewEntity == TRUE) {
				
				fprintf(fGCodeOutFile, "M100\n");                                 // turn off plasma torch
				fprintf(fGCodeOutFile,"G0 X%f Y%f Z%f\n",                         // rapid move to first point in polyline
						fCoordinate3->point[index][0], 
						fCoordinate3->point[index][1], 
						fCoordinate3->point[index][2]);
				                                                                  // at beginning of new cut, pause for
				                                                                  // 12 seconds to let plasma cutter reset
				fprintf(fGCodeOutFile, "G4 P%f\n", 3.0);                          // pause
				fprintf(fGCodeOutFile, "M101 \n");                                // turn on plasma torch 
				fprintf(fGCodeOutFile, "G4 P%f\n", 0.7);                          // and pause .9 sec
				startNewEntity = FALSE;
			} else { // continue current entity...
				fprintf(fGCodeOutFile,"G1 X%f Y%f Z%f\n",
					fCoordinate3->point[index][0], fCoordinate3->point[index][1], fCoordinate3->point[index][2]);
			}
		}
	}
	fprintf(fGCodeOutFile, "M100\n");    // turn off plasma torch
	fprintf(fGCodeOutFile, "M100\n");    // turn off plasma torch - to be sure?
	fprintf(fGCodeOutFile, "M0\n");
}
