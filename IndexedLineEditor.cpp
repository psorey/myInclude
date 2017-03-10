#include "stdafx.h"

#include <IndexedLineEditor.h>

#include <Inventor/fields/SoMFVec3f.h>
#include <Inventor/fields/SoMFInt32.h>




int 
IndexedLineEditor::DeleteSegment(SoMFInt32& indices, int segmentBeginIndex)
{
	int deltaCurrentIndex = 0;

	// remove segment by inserting a -1 in the coordIndex list.
	int numIndices = indices.getNum();
	int deltaIndices = 0;
	indices.insertSpace(segmentBeginIndex + 1, 1);
	indices.set1Value(segmentBeginIndex + 1, -1);
	deltaIndices++;

	// clean up...

	// this must go first because it doesn't change anything critical ???
	if (indices[segmentBeginIndex + 3] == -1)
	{
		// get rid of the dangling point...
		indices.deleteValues(segmentBeginIndex + 2, 2);
		deltaIndices -= 2;
	}
	
	if (segmentBeginIndex == 0)  
	{
		// it's at the beginning of a segment
		indices.deleteValues(segmentBeginIndex, 2);
		deltaIndices -= 2;
	}

	else if (indices[segmentBeginIndex - 1] == -1)
	{
		indices.deleteValues(segmentBeginIndex - 1, 2);
		deltaIndices -= 2;
		deltaCurrentIndex--;
	}

	indices.setNum(numIndices + deltaIndices);
	return deltaCurrentIndex;
}

int
IndexedLineEditor::DeletePoint(SoMFVec3f& coords, SoMFInt32& indices, int whichIndex)
{
	int deltaCurrentIndex = 0;
	int whichCoordToDelete = indices[whichIndex];

	// delete the segment ahead if exists...
	if (indices[whichIndex + 1] != -1)
		deltaCurrentIndex += IndexedLineEditor::DeleteSegment(indices, whichIndex);
	// and the one behind if it exists...
	if (whichIndex > 0 && indices[whichIndex - 1] != -1)
		deltaCurrentIndex += IndexedLineEditor::DeleteSegment(indices, (whichIndex - 1) + deltaCurrentIndex);

	int numCoords = coords.getNum();

	coords.deleteValues(whichCoordToDelete, 1);
	numCoords--;
	coords.setNum(numCoords);

	// now deal with any line indices that refer to the point...
	int numCoordIndices = indices.getNum();
	int k;
	
	// remove coordIndex references...
	for(k = 0; k < numCoordIndices; k++) 
	{
		if(indices[k] == whichCoordToDelete) 
		{
			 indices.deleteValues(k--, 1);
			 numCoordIndices--;
		}
	}

	// then subtract 1 from all indices greater than whichIndex...
	for(k = 0; k < numCoordIndices; k++) 
	{
		if(indices[k] > whichCoordToDelete) 
		{
			 indices.set1Value(k, indices[k] - 1);
		}
	}
	indices.setNum(numCoordIndices);
	return deltaCurrentIndex;

	// then clean up all the {-1, -1,} instances, and {-1, validIndex, -1} instances
	// ...or did we take care of this in DeleteSegment() ?  !!!!
}