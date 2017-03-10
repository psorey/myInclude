#if !defined(AFX_GETDOUBLEDIALOG_H__E07A87E1_701F_11D3_905B_00609724AD23__INCLUDED_)
#define AFX_GETDOUBLEDIALOG_H__E07A87E1_701F_11D3_905B_00609724AD23__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GetDoubleDialog.h : header file
//

//#ifdef _PROAD_APP
#include "c:\5000 SoftwareDev\pRoad\resource.h"
//#endif

/////////////////////////////////////////////////////////////////////////////
// CGetDoubleDialog dialog

class CGetDoubleDialog : public CDialog
{
// Construction
public:
	CGetDoubleDialog(CString title,
		CString upperLabel, CString lowerLabel);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGetDoubleDialog)
	enum { IDD = IDD_GET_DOUBLE_DIALOG };
	double	fDouble;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetDoubleDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CString fTitle;
	CString fUpperLabel;
	CString fLowerLabel;
	// Generated message map functions
	//{{AFX_MSG(CGetDoubleDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETDOUBLEDIALOG_H__E07A87E1_701F_11D3_905B_00609724AD23__INCLUDED_)
