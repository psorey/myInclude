#ifndef SO_GRAPH_UTILS_H
#define SO_GRAPH_UTILS_H

#include <Inventor/SbLinear.h>
#include <Inventor/SbColor.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTexture2Transform.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/nodes/SoTextureCoordinateBinding.h>
#include <Inventor/SbLinear.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/nodes/SoTextureCoordinatePlane.h>
#include <Inventor/SoInput.h>
#include <Inventor/nodes/SoFile.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/actions/SoGetMatrixAction.h>

#include <Inventor/manips/SoHandleBoxManip.h>
#include <Inventor/manips/SoTrackballManip.h>
#include <Inventor/manips/SoTransformBoxManip.h>
#include <Inventor/nodekits/SoWrapperKit.h>
#include <Inventor/Xt/SoXtMaterialEditor.h>
#include <Inventor/Xt/SoXtMaterialList.h>
#include <Inventor/nodes/SoEventCallback.h>

#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/draggers/SoDragger.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/engines/SoCompose.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/details/SoPointDetail.h>
#include <Inventor/details/SoDetail.h>
#include <Inventor/SoPath.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoDirectionalLight.h>
//#include <Inventor/Xt/SoXtDirectionalLightEditor.h>
#include <Inventor/nodekits/SoCameraKit.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/sensors/SoTimerSensor.h>

#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/win/viewers/SoWinExaminerViewer.h>
#include <Inventor/SoOffscreenRenderer.h>
#include <Inventor/SoInteraction.h>

//////////// these are a few of my ///////////////
////////////   favorite utilities  ///////////////

class SoGraphUtils
{

public:

	static	SoNode*	FindNodeByName(SoGroup *parent, char *name);
	static	int		GetFileRoot(char *return_fileroot, char *filename);	

	// find first instance of a type under parent
	static	SoNode*	FindChildOfType(SoGroup * parent, SoType typeId);
	static	SoPath*	FindPathToNode(SoNode *fromNode, SoNode *node);

};

	//#define dprint(expr) printf(#expr " = %g\n", (float)expr)
	//#define sprint(expr) printf(#expr "\n")

	/// Vector Geometry...

	//SbVec3f	    get_vector_point (SbVec3f pt, float length, float theta);
	//SbVec3f	    findcross (SbVec3f pt1, float theta1, SbVec3f pt2, float theta2);
	//float	    get_theta ( SbVec3f pt1, SbVec3f pt2 );
	//float	    dist (SbVec3f pt1, SbVec3f pt2);

#endif
