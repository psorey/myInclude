#include "stdafx.h"

#include <stdio.h>
#include <string.h>

#include <Inventor/SbLinear.h>
#include <SoGraphUtils.h>

SoNode *
SoGraphUtils::FindNodeByName(SoGroup *parent, char *name)
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

int       
SoGraphUtils::GetFileRoot(char *return_fileroot, char *filename){	

  char *fileroot;
  char tempfilename[100];
  
  ////// get the simple filename and file root name.....	
	
  fileroot = strrchr(filename, (int)'/');
  fileroot++;
  TRACE("filename = %s  fileroot = %s\n", filename, fileroot);

  strcpy(tempfilename, fileroot);
  char period[]=".";
  int strln = strcspn(fileroot, period);
  int i;
  for(i=0; i<strln; i++){
    fileroot[i] = tempfilename[i];
  }
  fileroot[strln]='\0';
  //  printf("filename = %s  fileroot = %s\n", filename, fileroot);

  strcpy(return_fileroot, fileroot);
  return 1;
}

///// find first instance of a type under parent
SoNode *
SoGraphUtils::FindChildOfType(SoGroup * parent, SoType typeId)
{
    SoSearchAction search;
    SoPath *path;
    SoNode *node;
    
    search.setType(typeId, FALSE);
    search.setInterest(SoSearchAction::FIRST);
    search.apply(parent);
    
    path = search.getPath();
    if (path == NULL) return NULL;
    node = path->getTail();
    return node;
}

SoPath *
SoGraphUtils::FindPathToNode(SoNode *fromNode, SoNode *node)
{
    SoSearchAction search;
    SoPath *path;
    
    search.setNode(node);
    search.setInterest(SoSearchAction::FIRST);
    search.apply(fromNode);
    
    path = search.getPath();
    if(path != NULL)
	path->ref();
    return path;
}

/*
void
itoa (int n, char s[])
{
    int i, j, c, sign;
    
    if ((sign = n) < 0)
	n = -n;
    i=0;
    do {
	s[i++] = n % 10 + '0';
    } while ((n /= 10 ) > 0);
    
    if (sign < 0 )
	s[i++] = '-';
    s[i] = '\0';
    
    for (i=0, j = strlen(s) -1; i < j; i++, j--){
	c    = s[i];
	s[i] = s[j];
	s[j] = c;
    }
}

// get_vector_point: find the endpoint of a vector given length & angle 

SbVec3f 
get_vector_point (SbVec3f pt, float length, float theta)
{
	SbVec3f temp;
	
	temp[0] = pt[0] + length * cos (theta);
	temp[1] = pt[1] + length * sin (theta);
	temp[2] = 0.0;
	return temp;
}

// find the crossing point of two vectors 

SbVec3f    
findcross (SbVec3f pt1, float theta1, SbVec3f pt2, float theta2)
{
	float       c1, c2, slope1, slope2;
	SbVec3f    temp;
	
	slope1 = tan (theta1);
	slope2 = tan (theta2);
	
	c1 = pt1[1] - (slope1 * pt1[0]);  
	c2 = pt2[1] - (slope2 * pt2[0]);

	temp[0] = (c2 - c1) / (slope1 - slope2);
	temp[1] = slope1 * temp[0]+ c1;
	temp[2] = 0.0;
	return temp;    
}

float 
get_theta ( SbVec3f pt1, SbVec3f pt2 )
{
	return  atan2 (( pt2[1] - pt1[1]) , ( pt2[0] - pt1[0] ));   
}

float
dist (SbVec3f pt1, SbVec3f pt2)
{
	float   distance;
	
	distance = sqrt((pt2[0] - pt1[0])*(pt2[0] - pt1[0]) \
		  +(pt2[1] - pt1[1])*(pt2[1] - pt1[1]));
	return distance;
}

  */
