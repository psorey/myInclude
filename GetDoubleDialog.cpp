// GetDoubleDialog.cpp : implementation file
//

#include "stdafx.h"
#include "proad.h"
#include <GetDoubleDialog.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetDoubleDialog dialog


CGetDoubleDialog::CGetDoubleDialog(CString title,
	CString upperLabel, CString lowerLabel)
	: CDialog(CGetDoubleDialog::IDD)
{
	//{{AFX_DATA_INIT(CGetIntegerDialog)
	fDouble = 0;
	//}}AFX_DATA_INIT
	fTitle = title;
	fUpperLabel = upperLabel;
	fLowerLabel = lowerLabel;
}

void CGetDoubleDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetDoubleDialog)
	DDX_Text(pDX, IDC_EDIT1, fDouble);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGetDoubleDialog, CDialog)
	//{{AFX_MSG_MAP(CGetDoubleDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetDoubleDialog message handlers

BOOL CGetDoubleDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetDlgItemText(IDC_STATIC_TOP, LPCTSTR(fUpperLabel));
	SetDlgItemText(IDC_STATIC_BOTTOM, LPCTSTR(fLowerLabel));
	this->SetWindowText(fTitle);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
