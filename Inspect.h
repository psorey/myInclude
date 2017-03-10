#pragma once

#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/SbMatrix.h>
#include <Inventor/fields/SoMFVec3f.h>


class Inspect
{
public:
    static void Coordinate3(char * label, SoCoordinate3 * coords);
    static void Vec3f(char * label, SbVec3f point);
    static void MFVec3f(SoMFVec3f point_list);
    static void Matrix(char * label, SbMatrix matrix);
    static void WriteCoordinate3(char * filename, SoCoordinate3 * coords);
    static void WriteTree(char * filename, SoSeparator * tree);
};