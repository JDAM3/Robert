// PhoneBookEntry.cpp : implementation file
//

#include "stdafx.h"
#include "raildiag.h"
#include "PhoneBookEntry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPhoneBookEntry dialog


CPhoneBookEntry::CPhoneBookEntry(CWnd* pParent /*=NULL*/)
	: CDialog(CPhoneBookEntry::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPhoneBookEntry)
	m_strDescription = _T("");
	m_strNumber = _T("");
	m_strDescriptionSt = _T("");
	m_strNumberSt = _T("");
	m_strHelpNumberSt = _T("");
	//}}AFX_DATA_INIT
}


void CPhoneBookEntry::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPhoneBookEntry)
	DDX_Control(pDX, IDOK, m_CtrlOk);
	DDX_Control(pDX, IDCANCEL, m_CtrlCancel);
	DDX_Text(pDX, IDC_DESC_ED, m_strDescription);
	DDX_Text(pDX, IDC_NUMBER_ED, m_strNumber);
	DDX_Text(pDX, IDC_DESC_ST, m_strDescriptionSt);
	DDX_Text(pDX, IDC_NUMBER_ST, m_strNumberSt);
	DDX_Text(pDX, IDC_ST_HELP_NUMBER, m_strHelpNumberSt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPhoneBookEntry, CDialog)
	//{{AFX_MSG_MAP(CPhoneBookEntry)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPhoneBookEntry message handlers

BOOL CPhoneBookEntry::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	
	m_strDescriptionSt = pApp->m_stDlgPhoneBookEntry.strDesc;
	m_strHelpNumberSt = pApp->m_stDlgPhoneBookEntry.strHelpNumber;
	m_strNumberSt = pApp->m_stDlgPhoneBookEntry.strNumber;

	SetWindowText(pApp->m_stDlgPhoneBookEntry.strTitle);

	m_CtrlOk.SetWindowText(pApp->m_stDlgPhoneBookEntry.strOk);

	m_CtrlCancel.SetWindowText(pApp->m_stDlgPhoneBookEntry.strCancel);

	UpdateData(FALSE);	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
