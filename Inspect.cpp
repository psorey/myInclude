#include "stdafx.h"
#include "Inspect.h"
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/nodes/SoLineSet.h>


#define DO_TRACE

void Inspect::Coordinate3(char * label, SoCoordinate3 * coords)
{
  #ifdef DO_TRACE
    TRACE("%s\n", label);
    for(int i = 0; i < coords->point.getNum(); i++) {
        SbVec3f point = coords->point[i];
        TRACE("point %d  x: %f  y: %f  z: %f\n", i, point[0], point[1], point[2]); }
  #endif 
}


void Inspect::Vec3f(char * label, SbVec3f point)
{
  #ifdef DO_TRACE
    TRACE("%s\n", label);
    TRACE("SbVec3f  x: %f  y: %f  z: %f\n", point[0], point[1], point[2]);
  #endif 
}


void Inspect::MFVec3f(SoMFVec3f point_list)
{
    int num = point_list.getNum();
    for(int i = 0; i < num; i++) {
        SbVec3f point = point_list[i];
        char lab[500];
        sprintf(lab, "point[%d]", i);
        Inspect::Vec3f(lab, point_list[i]);
    }
}


void Inspect::Matrix(char * label, SbMatrix matrix)
{
  #ifdef DO_TRACE
    SbVec3f vector = SbVec3f(1,1,1);
    SbVec3f point;
    matrix.multVecMatrix(vector, point);
    TRACE("%s\n", label);
    TRACE(" x: %f  y: %f  z: %f\n", point[0], point[1], point[2]); 
  #endif 
}

void Inspect::WriteCoordinate3(char * filename, SoCoordinate3 * coords)
{
    SoWriteAction wa;
	wa.getOutput()->openFile(filename);
    coords->ref();
	wa.apply(coords);
    SoLineSet * lineSet = new SoLineSet;
    lineSet->ref();
    wa.apply(lineSet);
    lineSet->unref();
	wa.getOutput()->closeFile();
}

void Inspect::WriteTree(char * filename, SoSeparator * tree)
{
    SoWriteAction wa;
	wa.getOutput()->openFile(filename);
    //tree->ref();
	wa.apply((SoNode *) tree);
	wa.getOutput()->closeFile();
}