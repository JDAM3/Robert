// DlgFleetSel.cpp : implementation file
//

#include "stdafx.h"
#include "RailDiag.h"
#include "DlgFleetSel.h"
#include "FileIO.h"


// CDlgFleetSel dialog

IMPLEMENT_DYNAMIC(CDlgFleetSel, CDialog)

CDlgFleetSel::CDlgFleetSel(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFleetSel::IDD, pParent)
{

}

CDlgFleetSel::~CDlgFleetSel()
{
}

void CDlgFleetSel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FLEET_NAME, m_CtrlComboFleetName);
}


BEGIN_MESSAGE_MAP(CDlgFleetSel, CDialog)
END_MESSAGE_MAP()


// CDlgFleetSel message handlers

BOOL CDlgFleetSel::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CString strFile;
	//texts
	SetWindowText(pApp->m_stDlgFleetEdit.strFleetName);

	//Fill List of Fleets in ComboBox
	strFile = pApp->m_strAppPath + _T("\\Fleets.ini");

	GetFleetNames(strFile, m_CtrlComboFleetName);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
