//  DlgProgress.cpp : implementation file
// CG: This file was added by the Progress Dialog component

#include "stdafx.h"
#include "resource.h"
#include "RailDiag.h"
#include "DlgProgress.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgProgress dialog

CDlgProgress::CDlgProgress(LPTSTR lpCaption)
: m_strProgressFleet(_T(""))
{
    m_bCancel = FALSE;
    m_nLower = 0;
    m_nUpper = 100;
    m_nStep = 1;
	m_caption = lpCaption;
    //{{AFX_DATA_INIT(CDlgProgress)
	//}}AFX_DATA_INIT
    m_bParentDisabled = FALSE;
	m_strProgressFleet = _T("");
}

CDlgProgress::~CDlgProgress()
{
    if (m_hWnd != NULL)
		DestroyWindow();
}

BOOL CDlgProgress::DestroyWindow()
{
    ReEnableParent();
    return CDialog::DestroyWindow();
}

void CDlgProgress::ReEnableParent()
{
    if (m_bParentDisabled && (m_pParentWnd != NULL))
		m_pParentWnd->EnableWindow(TRUE);
    m_bParentDisabled = FALSE;
}

BOOL CDlgProgress::Create(CWnd *pParent)
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
    // Get the true parent of the dialog
    m_pParentWnd = CWnd::GetSafeOwner(pParent);

    // m_bParentDisabled is used to re-enable the parent window
    // when the dialog is destroyed. So we don't want to set
    // it to TRUE unless the parent was already enabled.

    if ((m_pParentWnd != NULL) && m_pParentWnd->IsWindowEnabled())
    {
		m_pParentWnd->EnableWindow(FALSE);
		m_bParentDisabled = TRUE;
    }

    if (!CDialog::Create(CDlgProgress::IDD, pParent))
    {
		ReEnableParent();
		return FALSE;
    }

#ifndef _DEBUG
	if (pApp->m_bAuto)
	{
		this->ShowWindow(SW_HIDE);
	}
#endif

    return TRUE;
}

void CDlgProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProgress)
	DDX_Control(pDX, IDCANCEL, m_CtrlCancel);
	DDX_Control(pDX, IDC_PROG_PROGRESS, m_progress);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_ST_PROGRESS_FLEET, m_strProgressFleet);
	DDX_Control(pDX, IDC_PROG_FLEET, m_progressFleet);
	DDX_Control(pDX, IDC_ST_PROGRESS, m_ctrlStProgress);
}

BEGIN_MESSAGE_MAP(CDlgProgress, CDialog)
    //{{AFX_MSG_MAP(CDlgProgress)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CDlgProgress::OnCancel()
{
    m_bCancel=TRUE;
}

void CDlgProgress::SetRange(int nLower, int nUpper)
{
    m_nLower = nLower;
    m_nUpper = nUpper;
    m_progress.SetRange32(nLower, nUpper);
}
  
void CDlgProgress::SetRangeFleet(int nLower, int nUpper)
{
    m_nLowerFleet = nLower;
    m_nUpperFleet = nUpper;
    m_progressFleet.SetRange32(nLower, nUpper);
}
  
int CDlgProgress::SetPos(int nPos)
{
    PumpMessages();
    int iResult = m_progress.SetPos(nPos);
    UpdatePercent(nPos);
    return iResult;
}

int CDlgProgress::SetStep(int nStep)
{
    m_nStep = nStep; // Store for later use in calculating percentage
    return m_progress.SetStep(nStep);
}

int CDlgProgress::SetStepFleet(int nStep)
{
    m_nStepFleet = nStep; // Store for later use in calculating percentage
    return m_progressFleet.SetStep(nStep);
}

int CDlgProgress::OffsetPos(int nPos)
{
    PumpMessages();
    int iResult = m_progress.OffsetPos(nPos);
    UpdatePercent(iResult + nPos);
    return iResult;
}

int CDlgProgress::StepIt()
{
    PumpMessages();
	if (this->m_hWnd)
	{
		int iResult = m_progress.StepIt();
		UpdatePercent(iResult + m_nStep);
		return iResult;
	}
	else
	{
		return 0;
	}
}

int CDlgProgress::StepItFleet()
{
    PumpMessages();
	if (this->m_hWnd)
	{
		int iResult = m_progressFleet.StepIt();
		return iResult;
	}
	else
	{
		return 0;
	}
}

void CDlgProgress::PumpMessages()
{
    // Must call Create() before using the dialog
    ASSERT(m_hWnd!=NULL);

    MSG msg;
    // Handle dialog messages
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
		if (!IsDialogMessage(&msg))
		{
	        TranslateMessage(&msg);
			DispatchMessage(&msg);  
		}
    }
}

void CDlgProgress::SetNameFleet(CString strName)
{
	//UpdateData(FALSE);

	//m_strProgressFleet = strName;

	//UpdateData(TRUE);
    CWnd *pWndName = GetDlgItem(IDC_ST_PROGRESS_FLEET);
	pWndName->SetWindowText(strName);
}

BOOL CDlgProgress::CheckCancelButton()
{
    // Process all pending messages
    PumpMessages();

    // Reset m_bCancel to FALSE so that
    // CheckCancelButton returns FALSE until the user
    // clicks Cancel again. This will allow you to call
    // CheckCancelButton and still continue the operation.
    // If m_bCancel stayed TRUE, then the next call to
    // CheckCancelButton would always return TRUE

    BOOL bResult = m_bCancel;
    m_bCancel = FALSE;

    return bResult;
}

void CDlgProgress::UpdatePercent(int nNewPos)
{
    CWnd *pWndPercent = GetDlgItem(IDC_ST_PROGRESS);
    int nPercent;
    
    int nDivisor = m_nUpper - m_nLower;
    ASSERT(nDivisor > 0);  // m_nLower should be smaller than m_nUpper

    int nDividend = (nNewPos - m_nLower);
    ASSERT(nDividend >= 0);   // Current position should be greater than m_nLower

    nPercent = nDividend * 100 / nDivisor;

    // Since the Progress Control wraps, we will wrap the percentage
    // along with it. However, don't reset 100% back to 0%
    if (nPercent != 100)
		nPercent %= 100;

    // Display the percentage
    CString strBuf;
    strBuf.Format(_T("%d%c"), nPercent, _T('%'));

	CString strCur; // get current percentage
    pWndPercent->GetWindowText(strCur);

	if (strCur != strBuf)
		pWndPercent->SetWindowText(strBuf);
}
    
/////////////////////////////////////////////////////////////////////////////
// CDlgProgress message handlers

BOOL CDlgProgress::OnInitDialog() 
{
    CDialog::OnInitDialog();
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();

    m_progress.SetRange(m_nLower, m_nUpper);
    m_progress.SetStep(m_nStep);
    m_progress.SetPos(m_nLower);

    SetWindowText(m_caption);

	m_CtrlCancel.SetWindowText(pApp->m_stDlgProg.strCancel);

	if (pApp->m_iFleetMgrState == EN_FLEET_ON)
	{
		m_progressFleet.ShowWindow(SW_SHOW);
	}
	else
	{
		RECT	rect;

		UpdateData(FALSE);
		m_progressFleet.ShowWindow(SW_HIDE);
		m_progress.GetWindowRect(&rect);
		rect.top -= 110;
		rect.bottom -= 110;
		rect.left -= 7;
		rect.right -= 7;
		m_progress.MoveWindow(&rect);

		m_ctrlStProgress.GetWindowRect(&rect);
		rect.top -= 110;
		rect.bottom -= 110;
		rect.left -= 7;
		rect.right -= 7;
		m_ctrlStProgress.MoveWindow(&rect);
		UpdateData(TRUE);
	}

    return TRUE;  
}
