#include <afx.h>
#ifndef _P_ENTITY_MGR_H
#define _P_ENTITY_MGR_H

// class PEditableAttributeList; ???
// class PStateSaver;  ???


class SbPList;
class SoSeparator;
class PEntity;
class SoNode;
class SoPath;
class SoGroup;


////////////////////////////////////////////////////////////////////////////////////////////////
class PEntityMgr {

	// one instance only

public:
				SbPList*			fEntityList;
				int					fCurrentEntityNumber;
	static		SoSeparator*		sEntityRoot;
				CTime				fTime;

	// functions.....
									PEntityMgr(SoSeparator* root);
									~PEntityMgr(void);
				void				GetValidName(int entityType, CString& name);
				PEntity*			AddEntity(PEntity* newEntity, int entityType);
				void				RemoveEntity(PEntity* entityToRemove);
				PEntity*			GetEntityFromName(const char* name);
				int					GetNumEntities(void); // could be inline...
				PEntity*			GetEntity(int whichEntity);
	static	    SoNode*				FindNodeByName(SoGroup* searchRoot, char* name);
	static	    SoPath*				FindPathToNode(SoNode*);
				int					GetEntityTypeFromName(CString name);
				void				LoadEntitiesFromIvFile(CString filename);
				void				SetEntityLinesPickable(bool TRUEFALSE);	
	
};

#endif