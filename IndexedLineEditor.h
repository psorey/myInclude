

#ifndef _INDEXED_LINE_EDITOR_H
#define _INDEXED_LINE_EDITOR_H

class SoMFVec3f;
class SoMFInt32;


class IndexedLineEditor
{

public:
	
	static int DeleteSegment(SoMFInt32& indices, int segmentBeginIndex);
	static int DeletePoint(SoMFVec3f& coords, SoMFInt32& indices, int whichIndex);

};



#endif