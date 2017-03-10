#ifndef _MY_TYPES_H
#define _MY_TYPES_H

#include <Inventor/SbLinear.h>


class DPoint 
{
public:
   double x;
   double y;
   double z;
public:
   DPoint(void);
   DPoint(const SbVec3f& point);
   void setValue(const SbVec3f& value);
   void setValue(const DPoint& dValue);
   void setValue(const double& x, const double& y, const double& z);
   SbVec3f getSbVec3f(void);
};


#endif