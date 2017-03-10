#include "stdafx.h"
#include <MyTypes.h>


/////////////////////////////////////////////////////////////////////////////
//                                                                         //       
//  class DPoint                                                           // 
//                                                                         //
/////////////////////////////////////////////////////////////////////////////


DPoint::DPoint(void)
{

}

DPoint::DPoint(const SbVec3f& point)
{
   x = point[0];
   y = point[1];
   z = point[2];
}

void
DPoint::setValue(const SbVec3f& point)
{
   x = point[0];
   y = point[1];
   z = point[2];
}

void
DPoint::setValue(const double& X, const double& Y, const double& Z)
{
   x = X;
   y = Y;
   z = Z;
}

void
DPoint::setValue(const DPoint& point)
{
   x = point.x;
   y = point.y;
   z = point.z;
}

SbVec3f
DPoint::getSbVec3f(void)
{
   // need overflow checking?? !!!
   SbVec3f pt;
   pt[0] = x;
   pt[1] = y;
   pt[2] = 0.0;
   return pt;
}
