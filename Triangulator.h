//
// A little class that encapsulates the functionality of triangulating.
//

#ifndef TRIANGL__H
#define TRIANGL__H

#include <Inventor/SbLinear.h>
class SoMFVec3f;
class SoMFInt32;
class SoIndexedFaceSet;
class SoCoordinate3;

class Triangulator
{
public:

   static SbBool clockWiseTest( const SoMFVec3f &coords, 
			 const SoMFInt32 &indices, int startVert, int numVerts );
   static SbBool triangEdgeTest( const SoMFVec3f &coords, int e0p0, int e0p1,
				int e1p0, int e1p1 );
   static SbBool triangInsideTest( const SoMFVec3f &coords, 
				    int testPt, int pt0, int pt1, int pt2 );
    // this one uses z = 0...
   static SbBool triangulate( SoMFVec3f &coords, const SoMFInt32 &input,
				SoMFInt32 &output );

	// My user-friendly interfaces...
   // this one uses z = 0...
	
	static SbBool TriangleInsideTest( const SoMFVec3f &coords, 
				    SbVec3f testPt, int pt0, int pt1, int pt2 );

	static SoIndexedFaceSet* Triangulate( SoCoordinate3* coords );

	// test to see if testPoint is inside any of the triangles in coords/indices...
	static	bool IsInside(SbVec3f testPoint, const SoMFVec3f& coords, const SoMFInt32& indices);
	// returns FALSE if they intersect !!!
	static	SbBool Intersect(SbVec3f pt1, SbVec3f pt2, SbVec3f pt3, SbVec3f pt4);
};

#endif