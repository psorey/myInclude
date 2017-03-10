#ifndef _IV_GRAPH_FIXER_H
#define _IV_GRAPH_FIXER_H

class IfFixer;
class SoSeparator;

class IvGraphFixer
{
protected:
	// convenience members for file handling... 
	//CString	fInFilename;
	//FILE*		fInFp;
	//CString	fOutFilename;
	//FILE*		fOutFp;

	SoSeparator*	fInRoot;
	SoSeparator*	fOutRoot;
	IfFixer*			fIfFixer;
	int				fMode;

public:
	//void	SetInGraph(SoSeparator* inRoot) { fInRoot = inRoot; }
	//void	SetOutGraph(SoSeparator* outRoot) { fOutRoot = outRoot; }
	IvGraphFixer(void);
	~IvGraphFixer(void);
	
	// routines to control ivfix...
	SoSeparator*	FixGraph(SoSeparator* inRoot);
	void				SetMode(int mode) { fMode = mode; }

protected:
	// routines for modifying the output of ivfix...
	SoSeparator*	SeparateMaterials(SoSeparator* root);
	
public:
	enum // fMode
	{
		USE_IVFIX_DEFAULT_OUTPUT,
		TRIANGLE_STRIPS_BY_MATERIAL, 
	};
};


#endif