// ModemSettings2.cpp : implementation file
//

#include "stdafx.h"
#include "raildiag.h"
#include "Line.h"
#include "ModemSettings2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModemSettings2 dialog


CModemSettings2::CModemSettings2(CWnd* pParent /*=NULL*/)
	: CDialog(CModemSettings2::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModemSettings2)
	m_ModemPin = _T("");
	m_ModemPuk = _T("");
	m_bSavePin = FALSE;
	m_StModemName = _T("");
	m_StModemPin = _T("");
	m_StModemPuk = _T("");
	m_ModemName = _T("");
	m_StHelpDialPrefix = _T("");
	m_StDialPrefix = _T("");
	m_bDialPrefix = FALSE;
	m_DialPrefix = _T("");
	//}}AFX_DATA_INIT
}


void CModemSettings2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModemSettings2)
	DDX_Control(pDX, IDC_ED_CONN_PUK, m_CtrlModemPuk);
	DDX_Control(pDX, IDC_SAVE_PIN, m_CtrlSavePin);
	DDX_Control(pDX, IDOK, m_CtrlOk);
	DDX_Control(pDX, IDCANCEL, m_CtrlCancel);
	DDX_Control(pDX, IDC_CM_CONN_NAME, m_CntrlModemName);
	DDX_Text(pDX, IDC_ED_CONN_PIN, m_ModemPin);
	DDX_Text(pDX, IDC_ED_CONN_PUK, m_ModemPuk);
	DDX_Check(pDX, IDC_SAVE_PIN, m_bSavePin);
	DDX_Text(pDX, IDC_ST_CONN_NAME, m_StModemName);
	DDX_Text(pDX, IDC_ST_CONN_PIN, m_StModemPin);
	DDX_Text(pDX, IDC_ST_CONN_PUK, m_StModemPuk);
	DDX_CBString(pDX, IDC_CM_CONN_NAME, m_ModemName);
	DDX_Text(pDX, IDC_ST_HELP_DIAL_PREFIX, m_StHelpDialPrefix);
	DDX_Control(pDX, IDC_DIAL_PREFIX, m_CtrlDialPrefix);
	DDX_Check(pDX, IDC_DIAL_PREFIX, m_bDialPrefix);
	DDX_Text(pDX, IDC_ED_DIAL_PREFIX, m_DialPrefix);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_ED_DIAL_PREFIX, m_CtrlEdDialPrefix);
}


BEGIN_MESSAGE_MAP(CModemSettings2, CDialog)
	//{{AFX_MSG_MAP(CModemSettings2)
	ON_EN_CHANGE(IDC_ED_CONN_PIN, OnChangeEdConnPin)
	ON_EN_CHANGE(IDC_ED_CONN_PUK, OnChangeEdConnPuk)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_DIAL_PREFIX, &CModemSettings2::OnBnClickedDialPrefix)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModemSettings2 message handlers

BOOL CModemSettings2::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	TCHAR	szName[256];
	BOOL	bFound;

	Line_Init(0, 0, NULL, NULL, NULL, FALSE, 0);
	bFound = Line_GetFirstName(szName);
	while(bFound)
	{
		m_CntrlModemName.InsertString(-1, szName);
		bFound = Line_GetNextName(szName);
	}
	Line_Deinit(0, 0, FALSE);

	m_CtrlDialPrefix.SetWindowText(pApp->m_stDlgModemCon.strDialPrefix);
	m_StHelpDialPrefix = pApp->m_stDlgModemCon.strHelpDialPrefix;
	m_StModemName = pApp->m_stDlgModemCon.strConnectionName;
	m_StModemPin = pApp->m_stDlgModemCon.strPin;
	m_StModemPuk = pApp->m_stDlgModemCon.strPuk;

	SetWindowText(pApp->m_stDlgModemCon.strTitle);
	m_CtrlSavePin.SetWindowText(pApp->m_stDlgModemCon.strSavePin);

	m_CtrlOk.SetWindowText(pApp->m_stDlgModemCon.strOk);
	m_CtrlCancel.SetWindowText(pApp->m_stDlgModemCon.strCancel);

	m_CtrlEdDialPrefix.EnableWindow(m_bDialPrefix);

	if (pApp->m_bPUK)
	{
		m_CtrlModemPuk.EnableWindow(TRUE);
		m_ModemPin = _T("");
		m_CtrlOk.EnableWindow(FALSE);
	}
	else
	{
		m_CtrlModemPuk.EnableWindow(FALSE);
	}

	UpdateData(FALSE);	

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CModemSettings2::OnChangeEdConnPin() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (_tcslen(m_ModemPin) > 0 &&
		_tcslen(m_ModemPuk) > 0)
	{
		m_CtrlOk.EnableWindow(TRUE);
	}
}

void CModemSettings2::OnChangeEdConnPuk() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (_tcslen(m_ModemPin) > 0 &&
		_tcslen(m_ModemPuk) > 0)
	{
		m_CtrlOk.EnableWindow(TRUE);
	}
}

void CModemSettings2::OnBnClickedDialPrefix()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_CtrlEdDialPrefix.EnableWindow(m_bDialPrefix);

}
