// ModemSettings.cpp : implementation file
//

#include "stdafx.h"
#include "raildiag.h"
#include "ModemSettings.h"
#include "Line.h"
#include "PhoneBook.h"
#include "ModemSettings2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModemSettings dialog


CModemSettings::CModemSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CModemSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModemSettings)
	m_ModemNumber = _T("");
	m_StModemNumber = _T("");
	m_ModemDesc = _T("");
	m_StModemDesc = _T("");
	//}}AFX_DATA_INIT

	m_ModemName = _T("");
	m_ModemPin = _T("");
	m_bSavePin = FALSE;
	m_ModemPuk = _T("");
	m_bDialPrefix = FALSE;
	m_DialPrefix = _T("");
	m_DialPrefixView = _T("");

}


void CModemSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModemSettings)
	DDX_Control(pDX, IDC_SETTINGS, m_CtrlSettings);
	DDX_Control(pDX, IDOK, m_CtrlOk);
	DDX_Control(pDX, IDCANCEL, m_CtrlCancel);
	DDX_Text(pDX, IDC_ED_CONN_NUMBER, m_ModemNumber);
	DDX_Text(pDX, IDC_ST_CONN_NUMBER, m_StModemNumber);
	DDX_Text(pDX, IDC_ED_CONN_DESC, m_ModemDesc);
	DDX_Text(pDX, IDC_ST_CONN_DESC, m_StModemDesc);
	DDX_Text(pDX, IDC_DIAL_PREFIX_VIEW, m_DialPrefixView);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModemSettings, CDialog)
	//{{AFX_MSG_MAP(CModemSettings)
	ON_CBN_SELENDOK(IDC_CM_CONN_NAME, OnSelendokCmConnName)
	ON_BN_CLICKED(IDC_CONN_PHONE_BOOK, OnConnPhoneBook)
	ON_EN_CHANGE(IDC_ED_CONN_NUMBER, OnChangeEdConnNumber)
	ON_BN_CLICKED(IDC_SETTINGS, OnSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModemSettings message handlers

BOOL CModemSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();

	m_StModemDesc = pApp->m_stDlgModemCon.strDesc;
	m_StModemNumber = pApp->m_stDlgModemCon.strNumber;

	SetWindowText(pApp->m_stDlgModemCon.strTitle);

	if (pApp->m_bUserConnect)
	{
		m_CtrlOk.SetWindowText(pApp->m_stDlgModemCon.strConnect);
	}
	else
	{
		m_CtrlOk.SetWindowText(pApp->m_stDlgModemCon.strOk);
	}

	m_CtrlCancel.SetWindowText(pApp->m_stDlgModemCon.strCancel);
	m_CtrlSettings.SetWindowText(pApp->m_stDlgModemCon.strSettings);

	if (m_bDialPrefix)
	{
		m_DialPrefixView = m_DialPrefix;
	}
	else
	{
		m_DialPrefix = _T("");
	}

	UpdateData(FALSE);	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CModemSettings::OnSelendokCmConnName() 
{
	// TODO: Add your control notification handler code here
	
}

void CModemSettings::OnConnPhoneBook() 
{
	// TODO: Add your control notification handler code here
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CPhoneBook		PhoneBook;

	PhoneBook.m_bSelect = TRUE;
	
	for (int i = 0; i < pApp->m_strarrPhoneBookDesc.GetSize(); i++)
	{
		PhoneBook.m_strarrDescription.Add(pApp->m_strarrPhoneBookDesc.GetAt(i));
		PhoneBook.m_strarrNumber.Add(pApp->m_strarrPhoneBookNumber.GetAt(i));
	}

	if (PhoneBook.DoModal() == IDOK)
	{
		if (PhoneBook.m_nItem != -1 && PhoneBook.m_nItem < PhoneBook.m_strarrDescription.GetSize())
		{
			m_ModemDesc = PhoneBook.m_strarrDescription.GetAt(PhoneBook.m_nItem);
			m_ModemNumber = PhoneBook.m_strarrNumber.GetAt(PhoneBook.m_nItem);

			UpdateData(FALSE);	
		}
	}
	
}


void CModemSettings::OnChangeEdConnNumber() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	TRACE(_T("CModemSettings::OnChangeEdConnNumber Telephonenumber changed\n"));
	UpdateData(TRUE);
	
	m_ModemDesc = _T("");

	UpdateData(FALSE);	
	
}

void CModemSettings::OnSettings() 
{
	// TODO: Add your control notification handler code here
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CModemSettings2	ModemSettings2;

	ModemSettings2.m_bSavePin = m_bSavePin;
	ModemSettings2.m_ModemPin = m_ModemPin;
	ModemSettings2.m_ModemPuk = m_ModemPuk;
	ModemSettings2.m_ModemName = m_ModemName;
	ModemSettings2.m_bDialPrefix = m_bDialPrefix;
	ModemSettings2.m_DialPrefix = m_DialPrefix;

	if (ModemSettings2.DoModal() == IDOK)
	{
		m_bSavePin = ModemSettings2.m_bSavePin;
		m_ModemPin = ModemSettings2.m_ModemPin;
		m_ModemPuk = ModemSettings2.m_ModemPuk;
		m_ModemName = ModemSettings2.m_ModemName;
		m_bDialPrefix = ModemSettings2.m_bDialPrefix;
		m_DialPrefix = ModemSettings2.m_DialPrefix;

		if (m_bDialPrefix)
		{
			m_DialPrefixView = m_DialPrefix;
		}
		else
		{
			m_DialPrefixView = _T("");
		}

		UpdateData(FALSE);
	}
}
