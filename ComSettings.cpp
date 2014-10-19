// ComSettings.cpp : implementation file
//

#include "stdafx.h"
#include "raildiag.h"
#include "ComSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComSettings dialog

//some comment 2
CComSettings::CComSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CComSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CComSettings)
	m_ComBits = _T("8");
	m_ComBps = _T("14200");
	m_ComControl = _T("keine");
	m_ComParity = _T("keine");
	m_ComPort = _T("");
	m_ComStopBits = _T("1");
	m_StComBits = _T("");
	m_StComBps = _T("");
	m_StComControl = _T("");
	m_StComParity = _T("");
	m_StComPort = _T("");
	m_StComStopBits = _T("");
	//}}AFX_DATA_INIT
}


void CComSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CComSettings)
	DDX_Control(pDX, IDOK, m_CtrlOk);
	DDX_Control(pDX, IDCANCEL, m_CtrlCancel);
	DDX_Text(pDX, IDC_COM_BITS, m_ComBits);
	DDX_Text(pDX, IDC_COM_BPS, m_ComBps);
	DDX_Text(pDX, IDC_COM_CONTROL, m_ComControl);
	DDX_Text(pDX, IDC_COM_PARITY, m_ComParity);
	DDX_CBString(pDX, IDC_COM_PORT, m_ComPort);
	DDX_Text(pDX, IDC_COM_STOP_BITS, m_ComStopBits);
	DDX_Text(pDX, IDC_ST_COM_BITS, m_StComBits);
	DDX_Text(pDX, IDC_ST_COM_BPS, m_StComBps);
	DDX_Text(pDX, IDC_ST_COM_CONTROL, m_StComControl);
	DDX_Text(pDX, IDC_ST_COM_PARITY, m_StComParity);
	DDX_Text(pDX, IDC_ST_COM_PORT, m_StComPort);
	DDX_Text(pDX, IDC_ST_COM_STOP_BITS, m_StComStopBits);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CComSettings, CDialog)
	//{{AFX_MSG_MAP(CComSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComSettings message handlers

BOOL CComSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	
	m_StComPort = pApp->m_stDlgSerCon.strPort;
	m_StComBps = pApp->m_stDlgSerCon.strBps;
	m_StComBits = pApp->m_stDlgSerCon.strBits;
	m_StComParity = pApp->m_stDlgSerCon.strParity;

	SetWindowText(pApp->m_stDlgSerCon.strTitle);
	m_CtrlOk.SetWindowText(pApp->m_stDlgSerCon.strOk);
	m_CtrlCancel.SetWindowText(pApp->m_stDlgSerCon.strCancel);

	UpdateData(FALSE);	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
