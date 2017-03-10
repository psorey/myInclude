#pragma

#ifndef _GCODE_H
#define _GCODE_H

class SoSeparator;
class SoIndexedLineSet;
class SoCoordinate3;

class GCode
{
public:
	SoSeparator*		fParent;
	SoIndexedLineSet*   fIndexedLines;
	SoCoordinate3*		fCoordinate3;
	int					fPauseSeconds;

	int fGCodeLineCount;

	FILE* fDxfInFile;
	FILE* fGCodeOutFile;

	GCode(SoIndexedLineSet* lines, SoCoordinate3* coords);
	~GCode(void);

	// inline functions...
	void SetOutputFile(FILE* fp) { fGCodeOutFile = fp; }
	void SetIndexedLine(SoIndexedLineSet* indexedLine) { fIndexedLines = indexedLine; }
	void SetCoordinate3(SoCoordinate3* coordinate3) { fCoordinate3 = coordinate3; }
	void SetPauseBetweenEntities(int seconds) { fPauseSeconds = seconds; }
	void WriteGCode(FILE* fp);
};



#endif