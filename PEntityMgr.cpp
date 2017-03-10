#include "stdafx.h"
#include <afx.h>
#include <PEntityMgr.h>
#include <PEntity.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/SbPList.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/actions/SoSearchAction.h>


////////////////////////////////////////////////////////////////////////////////////////////////
// class PEntityMgr 

SoSeparator*  PEntityMgr::sEntityRoot;

	// one instance only...

PEntityMgr::PEntityMgr(SoSeparator* parentRoot)
{
	TRACE("PEntityMgr constructor\n");
	sEntityRoot = new SoSeparator;
	//sEntityRoot->ref();
	sEntityRoot->setName("EntityRoot");
	parentRoot->addChild(sEntityRoot);
	fEntityList = new SbPList;
	fTime = CTime();
}

PEntityMgr::~PEntityMgr(void)
{

}


void	PEntityMgr::GetValidName(int entityType, CString& name)
{ 
	name = "";

	switch (entityType) 
	{
	case PEntity::kPolyline:
		name = "Polyline";
		break;
	case PEntity::kNurbsCurve:
		name = "NurbsCurve";
		break;
	case PEntity::kPolygon:
		name = "Polygon";
		break;
	case PEntity::kCircle:
		name = "Circle";
		break;
	default:
		name = "NoName";
	}

	// put a time-stamp on the name...

	time_t now; 
	time( &now );
   int ran = rand();
   now += ran;
	char string[100];
	_ltoa_s(now, string, 10);
	CString append = string;
	name += append;
}


PEntity* PEntityMgr::AddEntity(PEntity* newEntity, int entityType)
{
	CString name;
	this->GetValidName(entityType, name);
	newEntity->SetName(name);
	fEntityList->append(newEntity);   // !!!
	sEntityRoot->addChild(newEntity->GetNode());  // !!! was commented

	return newEntity;
}


void	PEntityMgr::RemoveEntity(PEntity* entityToRemove)
{
	//!!!!
	int n = fEntityList->find(entityToRemove);
	TRACE("entity to remove = %d\n");
	if (n == -1) return;
	fEntityList->remove(n);
	sEntityRoot->removeChild(entityToRemove->GetNode());
}


int	PEntityMgr::GetNumEntities(void)
{
	return fEntityList->getLength();
}


PEntity*	PEntityMgr::GetEntity(int whichEntity)
{
	PEntity* entity =  (PEntity *)((*(const SbPList *)fEntityList)[whichEntity]);	
	return entity;
}


PEntity* PEntityMgr::GetEntityFromName(const char* name)
{
	PEntity *entity;
	int num = fEntityList->getLength();
	for (int i = 0; i < num; i++) 
	{
		entity =  (PEntity *)((*(const SbPList *)fEntityList)[i]);
		if (strcmp(name, entity->GetName()) == 0)
			i = num;
	}
	return entity;
}

void PEntityMgr::SetEntityLinesPickable(bool TRUEFALSE)
{
	PEntity *entity;
	int num = fEntityList->getLength();
	for (int i = 0; i < num; i++)
	{
		entity =  (PEntity *)((*(const SbPList *)fEntityList)[i]);
      if(PEntity::kPolyline == entity->GetType())
         entity->SetLinesPickable(TRUE-FALSE);
	}
}

// for loading files already set up as pDraw files...i.e, saved by pDraw...
void PEntityMgr::LoadEntitiesFromIvFile(CString filename)
{
	TRACE("load entities from file\n");
	SoInput myInput;
	myInput.openFile(filename);
	SoSeparator* tempSep = new SoSeparator;
	tempSep->ref();

	tempSep->addChild(SoDB::readAll(&myInput));
	myInput.closeFile(); 

	//SoWriteAction wa;
	//wa.getOutput()->openFile("tempSep.iv");
	//wa.apply(tempSep);
	//wa.getOutput()->closeFile();
	SoSeparator* tempSubSep;
	//
	// inconsistency in number of Separator nodes...
	// if (strcmp(tempSep->getName().getString(),"") != 0) {
	// 

	SoNode* testNode = tempSep->getChild(0);
	TRACE("test node name = %s\n", testNode->getName().getString());
	
	if (!testNode->isOfType(SoSeparator::getClassTypeId()))
		tempSubSep = tempSep;
		// TRACE("tempSep name = %s\n", tempSep->getName().getString());
	// }
	else
	{
		tempSubSep = (SoSeparator*)tempSep->getChild(0);
		TRACE("tempSubSep name = %s\n", tempSubSep->getName().getString());
	}

	int numChildren = tempSubSep->getNumChildren();
	for (int i = 0; i < numChildren; i++)
	{
		TRACE("adding %s to EntityRoot\n", tempSubSep->getChild(i)->getName().getString());
		sEntityRoot->addChild(tempSubSep->getChild(i));	
	}
	for (int i = 0; i < numChildren; i++)
	{
		PPolyline* nPolyline = NULL;
		PNurbsCurve* nNurbsCurve = NULL;
		
		CString name = tempSubSep->getChild(i)->getName().getString();
		//TRACE("name = %s\n", tempSubSep->getChild(i)->getName().getString());
		TRACE("name = %s,  index = %d\n", name, i);
		int entityType = this->GetEntityTypeFromName(name);
		CString newName = "";
		switch (entityType) 
		{
		case (PEntity::kPolyline):
			nPolyline = new PPolyline((SoSeparator*)tempSubSep->getChild(i));
			//this->GetValidName(PEntity::kPolyline, newName);
			//nPolyline->SetName(newName);
			nPolyline->SetName(name);
			fEntityList->append(nPolyline);
			break;
		case (PEntity::kNurbsCurve):
			nNurbsCurve = new PNurbsCurve((SoSeparator*)tempSubSep->getChild(i));
			//this->GetValidName(PEntity::kNurbsCurve, newName);
			//nNurbsCurve->SetName(newName);
			nNurbsCurve->SetName(name);
			fEntityList->append(nNurbsCurve);
			break;
		default:
			;
		}
	}
	tempSep->unref();
	// debug...
	for (int i = 0; i < fEntityList->getLength(); i++)
	{
		PEntity* entity =  (PEntity *)((*(const SbPList *)fEntityList)[i]);
		TRACE("Entity %d = %s   type = %d\n", i, entity->GetName(), entity->fEntityType);
	}
}


int PEntityMgr::GetEntityTypeFromName(CString name)
{
	int i = 0;
	while ((isalpha(name.GetAt(i))) && i < name.GetLength())
	{ i++; }

	if (i == name.GetLength()) return -1;
	
	CString entityName = name;
	CString typeName = entityName.Left(i);
	TRACE("type name = %s\n", typeName);
	const char* type = LPCTSTR(typeName);
	
	if	(strcmp(type, "Polyline") == 0) 
	{
		TRACE("found a polyline\n");
		return PEntity::kPolyline;
	}
	else if (strcmp(type, "NurbsCurve") == 0)
	{
		TRACE("found a nurbsCurve\n");
		return PEntity::kNurbsCurve;
	}
	else return -1;
}


SoPath* PEntityMgr::FindPathToNode(SoNode *node)
{
    SoSearchAction search;
    SoPath *path;
    
    search.setNode(node);
    search.setInterest(SoSearchAction::FIRST);
    search.apply(sEntityRoot);
    
    path = search.getPath();
    if(path != NULL)
		path->ref();
    return path;
}


SoNode* PEntityMgr::FindNodeByName(SoGroup *parent, char *name)
{
    SoSearchAction search;
    SoPath *path;
    SoNode *node;
    
    search.setName(SbName(name));
    search.setInterest(SoSearchAction::FIRST);
    search.apply(parent);
    
    path = search.getPath();
    if (path == NULL) return NULL;
    node = path->getTail();
    return node;
}
