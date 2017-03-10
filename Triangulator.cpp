
//
// Code for a Generalized Cylinder class.  This is not really very
// robust or well-designed (or complete or...).
//

#include "stdafx.h"

#include <stdio.h>
#include <Inventor/SbLinear.h>
//#include <Inventor/fields/SoMFInt32.h>
#include <Inventor/fields/SoMFInt32.h>
#include <Inventor/fields/SoMFVec3f.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>


#include "Triangulator.h"



//**********************************************************************
//                                                                      
// Returns FALSE if the two line segments intersect each other at only  
// one point.                                                            
// Returns TRUE if they are parallel or if they intersect at a point    
//    external to either line segment.                                    
//                                                                      
//    Assumes that the four points lie in the y = 0 plane.                
//                                                                      
//**********************************************************************


/*
SbBool
Triangulator::SegmentIntersectTest(SoMFVec3f &areaCoords, SbVec3f pt1, SbVec3f pt2)
{
	int numAreaCoords = areaCoords.getNum();
	for (int i = 0; i < numAreaCoords; i++)
	{
		
	}
}
*/

SbBool
Triangulator::Intersect(SbVec3f pt1, SbVec3f pt2, SbVec3f pt3, SbVec3f pt4)
{
    float x00 = pt1[0];
    float x10 = pt3[0];
    float z00 = pt1[1];
    float z10 = pt3[1];

    float dx0 = pt2[0] - x00;
    float dx1 = pt4[0] - x10;
    float dz0 = pt2[1] - z00;
    float dz1 = pt4[1] - z10;

    float diffx = x10 - x00;
    float diffz = z10 - z00;

	 TRACE("pt1 = %f  %f  pt2 = %f  %f  pt3 = %f  %f  pt4 = %f  %f\n",
		 pt1[0], pt1[1],  pt2[0], pt2[1],  pt3[0], pt3[1],  pt4[0], pt4[1]);

    float determ = dx0 * -dz1 - dz0 * -dx1;
	 

    if (determ == 0) // lines are parallel 
        return( TRUE );
    
    // otherwise, solve for intersection point 
    // first, solve for parametric length on edge 1 from e0p0 to e0p1 
    float parametric_val = (-dz1 * diffx + dx1 * diffz) / determ;
	
	 TRACE("parametricVal = %f\n", parametric_val);

#define SMALL_VAL        0.00001

    if (parametric_val > 1.0 - SMALL_VAL  || parametric_val < SMALL_VAL)
        // value lies outside line segment 
        return( TRUE );

    // next, solve for parametric length on edge 1 from e1p0 to e1p1 
    parametric_val = (-dz0 * diffx + dx0 * diffz) / determ;
	
	 TRACE("parametricVal2 = %f\n", parametric_val);

    if (parametric_val > 1.0 - SMALL_VAL || parametric_val < SMALL_VAL)
        // value lies outside line segment 
        return( TRUE );

#undef SMALL_VAL

    // otherwise, intersection point lies within both segments 
    return(FALSE);
}	



// for y = 0;
SbBool    
Triangulator::triangEdgeTest(const SoMFVec3f &coords, 
				int e0p0, int e0p1,  int e1p0,  int e1p1 )
{
    float x00 = coords[(int) e0p0][0];
    float x10 = coords[(int) e1p0][0];
    float z00 = coords[(int) e0p0][2];
    float z10 = coords[(int) e1p0][2];

    float dx0 = coords[(int) e0p1][0] - x00;
    float dx1 = coords[(int) e1p1][0] - x10;
    float dz0 = coords[(int) e0p1][2] - z00;
    float dz1 = coords[(int) e1p1][2] - z10;

    float diffx = x10 - x00;
    float diffz = z10 - z00;

    float determ = dx0 * -dz1 - dz0 * -dx1;

    if (determ == 0) // lines are parallel 
        return( TRUE );
    
    // otherwise, solve for intersection point 
    // first, solve for parametric length on edge 1 from e0p0 to e0p1 
    float parametric_val = (-dz1 * diffx + dx1 * diffz) / determ;

#define SMALL_VAL        0.00001

    if (parametric_val > 1.0 - SMALL_VAL  || parametric_val < SMALL_VAL)
        // value lies outside line segment 
        return( TRUE );

    // next, solve for parametric length on edge 1 from e1p0 to e1p1 
    parametric_val = (-dz0 * diffx + dx0 * diffz) / determ;

    if (parametric_val > 1.0 - SMALL_VAL || parametric_val < SMALL_VAL)
        // value lies outside line segment 
        return( TRUE );

#undef SMALL_VAL

    // otherwise, intersection point lies within both segments 
    return(FALSE);
}

SbBool 
Triangulator::clockWiseTest( const SoMFVec3f &coords, 
			 const SoMFInt32 &indices, int startVert, int numVerts )
{
    // ARE THE COORDS GIVEN BY THIS POLYGON CLOCKWISE ORDERED WHEN VIEWED
    // FROM ABOVE?  
    // ASSUME THAT ALL THE POINTS LIE IN THE Y=0 PLANE.
    // How do we know? Well, the total of the interior angles 
    // for an n-sided polygon is:                                
    //         interior_angles = (PI * n) - (2 * PI)                
    // while:                                                    
    //        exterior_angles = (PI * n) + (2 * PI)                
    // So we total up the interior angles, and reverse the
    // order of the vertices if it is the wrong amount
    float angle_total = 0;
    for( int vNum = 0; vNum < numVerts; vNum++ ) {
	int nextP = (vNum == numVerts - 1) ? 0 : vNum + 1;
	int prevP = (vNum == 0) ? numVerts - 1 : vNum - 1;
	SbVec3f nextEdge = coords[(int) indices[startVert + nextP]] -
			   coords[(int) indices[startVert + vNum]];
	SbVec3f prevEdge = coords[(int) indices[startVert + prevP]] -
			   coords[(int) indices[startVert + vNum]];
	nextEdge.normalize();
	prevEdge.normalize();
	
	// WE WANT ALL ANGLES IN THE SENSE OF ROTATING ABOUT THE 
	// POSITIVE Y AXIS 
	SbVec3f crossProd = prevEdge.cross( nextEdge );
	if (crossProd[1] > 0.0) {
	    angle_total += acos( prevEdge.dot( nextEdge ) );
	}
	else {
	    angle_total += 2 * M_PI - acos( prevEdge.dot( nextEdge ) );
	}
    }
    if (  angle_total > M_PI * numVerts )
	return FALSE;
    else
	return TRUE;
}

//**********************************************************************
//                                                                      
// Returns TRUE if the first point lies inside the triangle formed by   
//    the other three.                                                     
//                                                                      
// Assumes that all of the points lie in the plane y = 0                
// Assumes that the triangle is ordered clockwise when viewed from above
// when travelling in the order: pt0 to pt1 to pt2                        
//                                                                      
// Performs the check by doing a cross product between edges of the     
// triangle and vectors connecting the vertices to the test point.      
// If the point is inside the triangle, the cross products are positive.
// If any are negative, the point lies outside the triangle.            
// We only need to check the y values of the cross product, since the   
// points lie in a horizontal plane.                                    
//**********************************************************************

SbBool    
Triangulator::triangInsideTest( const SoMFVec3f &coords, 
				 int testPt, int pt0, int pt1, int pt2)
{
    SbVec3f edge0 = coords[(int)pt1] - coords[(int)pt0];
    SbVec3f edge1 = coords[(int)pt2] - coords[(int)pt1];
    SbVec3f edge2 = coords[(int)pt0] - coords[(int)pt2];

    SbVec3f ptEdge0 = coords[(int)testPt] - coords[(int)pt0];
    SbVec3f ptEdge1 = coords[(int)testPt] - coords[(int)pt1];
    SbVec3f ptEdge2 = coords[(int)testPt] - coords[(int)pt2];

#define TEENY_VAL        0.0000001

    if (     ptEdge0[2] * edge0[0] - ptEdge0[0] * edge0[2] > TEENY_VAL
      &&    ptEdge1[2] * edge1[0] - ptEdge1[0] * edge1[2] > TEENY_VAL
      &&    ptEdge2[2] * edge2[0] - ptEdge2[0] * edge2[2] > TEENY_VAL)
        return( TRUE );
    else 
        return( FALSE );

#undef TEENY_VAL
}

// this one uses z = 0;
SbBool    
Triangulator::TriangleInsideTest( const SoMFVec3f &coords, 
				 SbVec3f testPt, int pt0, int pt1, int pt2)
{
    SbVec3f edge0 = coords[(int)pt1] - coords[(int)pt0];
    SbVec3f edge1 = coords[(int)pt2] - coords[(int)pt1];
    SbVec3f edge2 = coords[(int)pt0] - coords[(int)pt2];

    SbVec3f ptEdge0 = testPt - coords[(int)pt0];
    SbVec3f ptEdge1 = testPt - coords[(int)pt1];
    SbVec3f ptEdge2 = testPt - coords[(int)pt2];

#define TEENY_VAL        0.0000001

    if (     ptEdge0[1] * edge0[0] - ptEdge0[0] * edge0[1] > TEENY_VAL
      &&    ptEdge1[1] * edge1[0] - ptEdge1[0] * edge1[1] > TEENY_VAL
      &&    ptEdge2[1] * edge2[0] - ptEdge2[0] * edge2[1] > TEENY_VAL)
        return( TRUE );
    else 
        return( FALSE );

#undef TEENY_VAL
}

//**********************************************************************
//                                                                      
// 1] assume all points lie in the y = 0 plane 
// 2] perform triangulation on the projected polygon, copy the order later 
// 3] for each point, find two others that form a triangle which meets 
//      the following conditions:                                         
//        a] Doesn't cut through any other edges                            
//        b] Any new edges are on the interior                            
//                                                                      
//**********************************************************************

SoIndexedFaceSet* 
Triangulator::Triangulate(SoCoordinate3* coords )
{
	SoMFInt32 inVals;
	SoMFInt32 outVals;
    int i;

	for (i = 0; i < coords->point.getNum(); i++)
		inVals.set1Value(i, i);

	int test = Triangulator::triangulate(coords->point, inVals, outVals);
	if (test == FALSE) return NULL;
	SoIndexedFaceSet* faces = new SoIndexedFaceSet;
	int numIndices = outVals.getNum();
	for (i = 0; i < numIndices; i++)
		faces->coordIndex.set1Value(i, outVals[i]);
	return faces;
}

bool Triangulator::IsInside(SbVec3f testPoint, const SoMFVec3f& coords, const SoMFInt32& indices)
{
	// step through cut area triangles...
	ASSERT( indices.getNum() >= 4); // we need at least 1 triangle...

	int numIndices = indices.getNum();
	
	for (int i = 0; i < numIndices; i += 4)
	{
		int pt0 = indices[i];
		int pt1 = indices[i + 1];
		int pt2 = indices[i + 2];

		testPoint[2] = 0.0; // necessary ???
		bool isInside = Triangulator::TriangleInsideTest(coords, testPoint, pt0, pt1, pt2);
		if (TRUE == isInside)
		{
			TRACE("isInside\n");
			return TRUE;
		}
	}
	return FALSE;
}




SbBool 
Triangulator::triangulate( 
			SoMFVec3f &coords, 
		        const SoMFInt32  &inVals, // indices describing the input
					         // polygons
		        SoMFInt32  &outVals) // indices describing the output
					   // polygons
{
	// we like to use z = 0 plane, so a temporary fix is this... !!
    int i;
	for (i = 0; i < coords.getNum(); i++)
	{
		SbVec3f tCoord = coords[i];
		tCoord[2] = tCoord[1];
		tCoord[1] = 0.0;
		coords.set1Value(i, tCoord[0], tCoord[1], tCoord[2]);
	}


    // CHECK THAT ALL COORDS LIE IN THE y = 0.0 PLANE 
    for (i = 0; i < coords.getNum(); i++ ){
        TRACE("input coords for tri %f  %f  %f\n", coords[i][0], coords[i][1], coords[i][2]);
	if ( coords[i][1] != 0.0 ) {
	    TRACE("Error in triangulation: not all points \n");
	    TRACE("  lie in the y = 0 plane\n");
	    return FALSE;
	}
    }

    // COPY THE INPUT INTO THE OUTPUT
    if ( outVals.getNum() < inVals.getNum() )
	 outVals.insertSpace( 0, inVals.getNum() - outVals.getNum());
    else if ( outVals.getNum() > inVals.getNum() )
	      outVals.deleteValues( 0, outVals.getNum() - inVals.getNum());
    outVals = inVals;
    

    int curInd0 = 0; 
    int curNumVerts = 0;

    SbBool     got_a_tri, tri_ok, keep_going;
    SbVec3f    edge2, edge0, prevEdge, nextEdge;
    int        ind0, ind1, ind2, test_ind0, test_ind1;
    int        vNum;

    //***** WALK DOWN THE LIST OF POLYS *****
    
    int shift_count = 0;

    for ( curInd0 = 0; curInd0 < outVals.getNum(); ) {
	int j;
	for( j = curInd0, curNumVerts = 0; j < outVals.getNum(); j++ ) {
	    if ( outVals[j] >= 0 )
		curNumVerts++;
	    else
		break;
	}

	// IF ANY THREE CONSECUTIVE POINTS ARE COLINEAR, REMOVE THE 
	// CENTER POINT
	// Note: we don't have to check that there are three points.
	// The algorithm will just wind up removing all the points
	// if there are two or less, or if all the points in the polygon
	// are colinear.
	for( vNum = 0; vNum < curNumVerts; vNum++ ) {
	    int nextP = (vNum == curNumVerts - 1) ? 0 : vNum +1;
	    int prevP = (vNum == 0) ? curNumVerts - 1 : vNum - 1;
	    nextEdge = coords[(int) outVals[curInd0 + nextP]] - 
		       coords[(int) outVals[curInd0 + vNum]];
	    prevEdge = coords[(int) outVals[curInd0 + prevP]] - 
		       coords[(int) outVals[curInd0 + vNum]];
	    
	    // CROSS PRODUCT IS ZERO IF THEY ARE CO-LINEAR
	    SbVec3f crossProd = prevEdge.cross( nextEdge );

#define TEENY_VAL        0.0000001
#define IS_COORD_TEENY(a)  ( (a)[0] < TEENY_VAL && (a)[1] < TEENY_VAL \
		      && (a)[2] < TEENY_VAL && (a)[0] > -TEENY_VAL \
		      && (a)[1] > -TEENY_VAL && (a)[2] > -TEENY_VAL )

	    if (IS_COORD_TEENY( crossProd) ) {
		// remove the index of this point from the poly
		outVals.deleteValues( curInd0 + vNum, 1);

		curNumVerts--;

		vNum--;  // decrement, because everyone's been shifted
			 // down by 1
	    }
#undef TEENY_VAL
#undef IS_COORD_TEENY
	}

	if ( curNumVerts == 0) {
	    // The polygon has no vertices (this could happen because the
	    // whole polygon was colinear and got removed in the step above.)
	    // Remove the '-1' index that signals the end of the polygon.
	    outVals.deleteValues( curInd0, 1 );

	    // Do not increment curInd0. By removing the '-1' entry, the
	    // next polygon moves up a slot to have *ITS* first index at
	    // curInd0.
	}
	else if ( curNumVerts <= 3) {
	    // Just move on to the next polygon.
	    // (We add an extra 1, because each polygon has a
	    // extra index entry of -1 to indicate the end of the poly
	    curInd0 += curNumVerts + 1;
	}
	else {

	    // REVERSE ORDER IF NECESSARY
	    if ( !clockWiseTest( coords, outVals, curInd0, curNumVerts) ) {
		int *outIndices = outVals.startEditing();

		int tempI, early, late;
		for ( early = 0, late = curNumVerts - 1;
		      early < late; early++, late-- ) {
		    tempI = outIndices[curInd0 + early];
		    outIndices[curInd0 + early] = outIndices[curInd0 + late];
		    outIndices[curInd0 + late] = tempI;
		}

		outVals.finishEditing();
            }
        
            // find a triangle formed by the first point and two other points
            // The new triangle must satisfy the condition that no new      
            // Edges intersect any already existing edges in the polygon
            ind0 = curInd0;
            keep_going = TRUE;
            for (ind1 = curInd0 + 1, ind2 = curInd0 + 2, got_a_tri = FALSE; 
                 got_a_tri == FALSE && keep_going == TRUE;   ) {
                // TEST EACH NEW EDGE AGAINST ALL OTHER EDGES
                tri_ok = TRUE;

                // make sure ordering of new triangle is clockwise 
                // Normal points down if (edge2 cross edge0)[1] < 0.0 
                edge2 = coords[(int)outVals[ind2]] - coords[(int)outVals[ind0]];
                edge0 = coords[(int)outVals[ind1]] - coords[(int)outVals[ind0]];
                if (edge0[0] * edge2[2] - edge2[0] * edge0[2] < 0.0)
                    tri_ok = FALSE;
                
                // make sure no other points fall inside the newly formed 
                // triangle
                if (tri_ok == TRUE) {
                    for( vNum = curInd0; vNum < curInd0 + curNumVerts; vNum++) {
                        if (vNum != ind0 && vNum != ind1 && vNum != ind2) {
                            if ( triangInsideTest(coords,outVals[vNum],outVals[ind0], outVals[ind1], outVals[ind2]))
                                tri_ok = FALSE;
                        }
                    }
                }

                // edge between ind0 and ind1
                if (ind1 != curInd0 + 1) {
                    // edge between point0 and point1 is ok, since part of 
                    // original polygon 
                    for( test_ind0 = curInd0, test_ind1 = curInd0 + 1;
                         tri_ok == TRUE && test_ind0 < curInd0 + curNumVerts;
                         test_ind0++, test_ind1++) { 
                        if (test_ind1 == curInd0 + curNumVerts)
                            test_ind1 = curInd0;
                        tri_ok = triangEdgeTest(coords, outVals[ind0], outVals[ind1], 
                                    outVals[test_ind0], outVals[test_ind1]);
                    }
                }

                // edge between ind1 and ind2 is part of the original polygon
                // except when forming tri between v0, v1, and final point 
                if (ind2 != ind1 + 1) {
                    for( test_ind0 = curInd0, test_ind1 = curInd0 + 1;
                         tri_ok == TRUE && test_ind0 < curInd0 + curNumVerts;
                         test_ind0++, test_ind1++) { 
                        if (test_ind1 == curInd0 + curNumVerts)
                            test_ind1 = curInd0;
                        tri_ok = triangEdgeTest(coords, outVals[ind1], outVals[ind2], 
                                    outVals[test_ind0], outVals[test_ind1]);
                    }
                }

                // edge between ind2 and ind0 
                if (ind2 != curInd0 + curNumVerts -1) {
                    // edge between point0 and last point in poly is ok,since
                    // part of original polygon 
                    for( test_ind0 = curInd0, test_ind1 = curInd0 + 1;
                         tri_ok == TRUE && test_ind0 < curInd0 + curNumVerts;
                         test_ind0++, test_ind1++) { 
                        if (test_ind1 == curInd0 + curNumVerts)
                            test_ind1 = curInd0;
                        tri_ok = triangEdgeTest(coords, outVals[ind2], outVals[ind0], 
                                    outVals[test_ind0], outVals[test_ind1]);
                    }
                }
                if (tri_ok == TRUE)
                    got_a_tri = TRUE;
                else {
                    if (ind1 == curInd0 + 1 && ind2 == curInd0 + curNumVerts -1)
                        keep_going = FALSE;
                    ind1++;
                    ind2++;
                    if (ind2 == curInd0 + curNumVerts) {
                        // If none others worked, try the case where the 
                        // the points are first, second, and last          
                        ind1 = curInd0 + 1;
                        ind2 = curInd0 + curNumVerts - 1;
                    }
                }
            }

            if (got_a_tri == FALSE) {
                // Could not draw a triangle using the first vertex. 
                // Shift all of the vertices in the polygon over and try again 
                shift_count++;

		int *outIndices = outVals.startEditing();

                int temp_index = outIndices[ curInd0 + 0];

                for( vNum = curInd0; vNum < curInd0 + curNumVerts - 1; vNum++ )
                    outIndices[ vNum] = outIndices[ vNum + 1];
                outIndices[ curInd0 + curNumVerts - 1] = temp_index;

		outVals.finishEditing();

                if (shift_count >= curNumVerts) {
                    printf("Error: could not triangulate this polygon\n");
                    printf("It may look funny!\n");
		    return(FALSE );
                }

		// Do not increment the value of curInd0.
		// This will mean that the 'for' loop will repeat the entire
		// process on this polygon (but THIS time, the indices have
		// been shifted in order.
            }
            else {
            
                shift_count = 0;

                // Create a new triangle which connects the first point 
                // to these two new points 
		// Add this new triangle to the end of the outVals

		int newIndices[4];
		newIndices[0] = outVals[ ind0];
		newIndices[1] = outVals[ ind1]; 
		newIndices[2] = outVals[ ind2]; 
		newIndices[3] = -1;

		outVals.setValues( outVals.getNum(), 4, newIndices );

                // Make necessary changes to the old polygon 
                if (ind1 == curInd0 + 1 && ind2 == curInd0 + curNumVerts - 1) {
                    // triangle was first, second, and last points 
                    // we need to get rid of first point in the current poly
		    outVals.deleteValues( curInd0, 1);
                    curNumVerts--;
                }
                else if (ind1 == curInd0 + 1) {
                    // triangle was first three points 
                    // we need to get rid of second point 
		    outVals.deleteValues( curInd0 + 1, 1 );
                    curNumVerts --;
                }
                else if (ind2 == curNumVerts - 1) {
                    // triangle was last three points 
		    // we need to get rid of last point
		    outVals.deleteValues( curInd0 + curNumVerts - 1, 1 );
                    curNumVerts --;
                }
                else {    
                    // we need to break the poly into 2, since we just created
                    // an interior-type triangle 
                    // The first polygon will go from ind0 to ind1 
                    // and will reside in the original polygon data structure 
                    // The second will continue from ind2 back to ind0 
                    // and will be added to the end of outVals.

		    // FIRST, ADD THE LATTER POLY TO THE END OF outVals...

		    int newPolNumVerts = curInd0 + curNumVerts - ind2 + 1;

		    // Add an extra slot for the '-1' which signifies end
		    // of polygon
		    int newPolInd0 = outVals.getNum();
		    outVals.insertSpace( newPolInd0, newPolNumVerts + 1 ); 

		    int *outIndices = outVals.startEditing();
		    // Copy the points from ind2 to the end of the poly
		    for ( int j = newPolInd0, vNum = ind2;
			  vNum < curInd0 + curNumVerts; j++, vNum++ ) {
			  outIndices[j] = outIndices[vNum];
		    }
		    // Copy the initial point...
		    outIndices[newPolInd0 + newPolNumVerts - 1] 
			= outIndices[curInd0];

		    // Add the -1 to signify the end
		    outIndices[newPolInd0 + newPolNumVerts] = -1;

		    outVals.finishEditing();


		    // NEXT, REMOVE THE LATTER HALF OF  THE FIRST POLY...
		    outVals.deleteValues( ind2, newPolNumVerts - 1 );

                    // Reduce the num of coords being used from the original
                    // polygon 
                    curNumVerts -= newPolNumVerts - 1;
                }
            }
        }
    }

    
	// we like to use z = 0 plane, so a temporary fix is this... !!
	for (i = 0; i < coords.getNum(); i++)
	{
		SbVec3f tCoord = coords[i];
		tCoord[1] = tCoord[2];
		tCoord[2] = 0.0;
		coords.set1Value(i, tCoord[0], tCoord[1], tCoord[2]);
	}


    return TRUE;
}

