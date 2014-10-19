// CDlgFleetNew.cpp : implementation file
//

#include "stdafx.h"
#include "RailDiag.h"
#include "DlgFleetNew.h"


// CDlgFleetNew dialog

IMPLEMENT_DYNAMIC(CDlgFleetNew, CDialog)

CDlgFleetNew::CDlgFleetNew(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFleetNew::IDD, pParent)
	, m_strFleetName(_T(""))
{

}

CDlgFleetNew::~CDlgFleetNew()
{
}

void CDlgFleetNew::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FLEET_NAME, m_strFleetName);
	DDX_Control(pDX, IDOK, m_ctrlOk);
	DDX_Control(pDX, IDCANCEL, m_ctrlCancel);
}


BEGIN_MESSAGE_MAP(CDlgFleetNew, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgFleetNew::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgFleetNew message handlers

BOOL CDlgFleetNew::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();

	//texts
	SetWindowText(pApp->m_stDlgFleetEdit.strFleetName);

	m_ctrlOk.SetWindowText(pApp->m_stDlgFleetEdit.strOk);
	m_ctrlCancel.SetWindowText(pApp->m_stDlgFleetEdit.strCancel);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFleetNew::OnBnClickedOk()
{
	int	i;
	bool found = false;

	UpdateData(TRUE);

	for (i = 0; !found && i < m_strarrFleetName.GetCount(); i++)
	{
		if (0 == m_strFleetName.CompareNoCase(m_strarrFleetName.GetAt(i)))
		{
			found = true;
		}
	}

	if (!found)
	{
		OnOK();
	}
	else
	{
		CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
		AfxMessageBox(pApp->m_stDlgFleetEdit.strMess2);
	}
}
