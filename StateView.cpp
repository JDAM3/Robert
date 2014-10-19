// StateView.cpp : implementation file
//

#include "stdafx.h"
#include "RailDiag.h"
#include "DlgProgress.h"

#include "ComLib.h"
#include "ComDiag.h"
#include "RailDiagDoc.h"
#include "InOutList.h"
#include "MainFrm.h"
#include "StateView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStateView

IMPLEMENT_DYNCREATE(CStateView, CFormView)

CStateView::CStateView()
	: CFormView(CStateView::IDD)
{
	//{{AFX_DATA_INIT(CStateView)
	m_strCompany = _T("");
	m_strLocNr = _T("");
	m_strLocType = _T("");
	m_strMotorNr = _T("");
	m_strMotorType = _T("");
	m_strDurCount = _T("0");
	m_strMode = _T("");
	m_strDiagDateTime = _T("");
	m_strTermDateTime = _T("");
	m_strStCompany = _T("");
	m_strStDiagDateTime = _T("");
	m_strStDurCount = _T("");
	m_strStLocNr = _T("");
	m_strStLocType = _T("");
	m_strStMode = _T("");
	m_strStMotorNr = _T("");
	m_strStMotorType = _T("");
	m_strStMtu1 = _T("");
	m_strStMtu2 = _T("");
	m_strStTermDateTime = _T("");
	//}}AFX_DATA_INIT
}

CStateView::~CStateView()
{
}

void CStateView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStateView)
	DDX_Text(pDX, IDC_ED_COMPANY, m_strCompany);
	DDX_Text(pDX, IDC_ED_LOCNR, m_strLocNr);
	DDX_Text(pDX, IDC_ED_LOCTYPE, m_strLocType);
	DDX_Text(pDX, IDC_ED_MOTORNR, m_strMotorNr);
	DDX_Text(pDX, IDC_ED_MOTORTYPE, m_strMotorType);
	DDX_Text(pDX, IDC_ED_DURCOUNT, m_strDurCount);
	DDX_Text(pDX, IDC_ED_MODE, m_strMode);
	DDX_Text(pDX, IDC_ED_DIAG_DATETIME, m_strDiagDateTime);
	DDX_Text(pDX, IDC_ED_TERM_DATETIME, m_strTermDateTime);
	DDX_Text(pDX, IDC_ST_COMPANY, m_strStCompany);
	DDX_Text(pDX, IDC_ST_DIAG_DATETIME, m_strStDiagDateTime);
	DDX_Text(pDX, IDC_ST_DURCOUNT, m_strStDurCount);
	DDX_Text(pDX, IDC_ST_LOCNR, m_strStLocNr);
	DDX_Text(pDX, IDC_ST_LOCTYPE, m_strStLocType);
	DDX_Text(pDX, IDC_ST_MODE, m_strStMode);
	DDX_Text(pDX, IDC_ST_MOTORNR, m_strStMotorNr);
	DDX_Text(pDX, IDC_ST_MOTORTYPE, m_strStMotorType);
	DDX_Text(pDX, IDC_ST_MTU1, m_strStMtu1);
	DDX_Text(pDX, IDC_ST_MTU2, m_strStMtu2);
	DDX_Text(pDX, IDC_ST_TERM_DATETIME, m_strStTermDateTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStateView, CFormView)
	//{{AFX_MSG_MAP(CStateView)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStateView diagnostics

#ifdef _DEBUG
void CStateView::AssertValid() const
{
	CFormView::AssertValid();
}

void CStateView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CStateView message handlers

void CStateView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();

	CRailDiagDoc	*pDoc = GetDocument();

	// static texts
	m_strStMtu1			= _T("")   + pDoc->m_stStatText.strHeadName + _T(" ") + pDoc->m_stStatText.strHeadCity;
	m_strStMtu2			= _T(" ");
	m_strStCompany		= _T("\n") + pDoc->m_stStatText.strCompany;
	m_strStLocType		= _T("\n") + pDoc->m_stStatText.strLocType;
	m_strStLocNr		= _T("\n") + pDoc->m_stStatText.strLocNumber;
	m_strStMotorType	= _T("\n") + pDoc->m_stStatText.strMotorType;
	m_strStMotorNr		= _T("\n") + pDoc->m_stStatText.strMotorNumber;
	m_strStDurCount		= _T("\n") + pDoc->m_stStatText.strDurationCount;
	m_strStDiagDateTime = _T("\n") + pDoc->m_stStatText.strDiag;
	m_strStTermDateTime = _T("\n") + pDoc->m_stStatText.strTerminal;
	m_strStMode			= _T("\n") + pDoc->m_stStatText.strMode;

	// values
	m_strCompany		= pDoc->m_stLocSettings.strCompany;
	m_strLocType		= pDoc->m_stLocSettings.strLocType;
	m_strLocNr			= pDoc->m_stLocSettings.strLocNumber;
	m_strMotorType		= pDoc->m_stLocSettings.strMotorType;
	m_strMotorNr		= pDoc->m_stLocSettings.strMotorNumber;
	m_strDurCount		= pDoc->m_strDurationCount;
	m_strDiagDateTime	= pDoc->m_strDiagDateTime;
	m_strTermDateTime	= pDoc->m_strTerminalDateTime;
	if (pDoc->m_strMode.IsEmpty())
	{
		m_strMode = _T("");
	}
	else
	{
		if (pDoc->m_strMode == CString(_T("0")))
		{
			m_strMode		= pDoc->m_stStatText.strMode0;
		}
		else
		{
			m_strMode		= pDoc->m_stStatText.strMode1;
		}
	}

	UpdateData(FALSE);	

}

void CStateView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	TRACE(_T("CStateView::OnSize nType %d, cx %d, cy %d\n"), nType, cx, cy);
	
}


BOOL CStateView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	cs.cx = 920;
	
	return CFormView::PreCreateWindow(cs);
}

void CStateView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CRailDiagDoc	*pDoc = GetDocument();

	m_strCompany	= pDoc->m_stLocSettings.strCompany;
	m_strLocType	= pDoc->m_stLocSettings.strLocType;
	m_strLocNr		= pDoc->m_stLocSettings.strLocNumber;
	m_strMotorType	= pDoc->m_stLocSettings.strMotorType;
	m_strMotorNr	= pDoc->m_stLocSettings.strMotorNumber;
	m_strDurCount	= pDoc->m_strDurationCount;
	m_strDiagDateTime = pDoc->m_strDiagDateTime;
	m_strTermDateTime = pDoc->m_strTerminalDateTime;
//	m_strMode		= pDoc->m_strMode;
	if (pDoc->m_strMode == CString(_T("0")))
	{
		m_strMode		= pDoc->m_stStatText.strMode0;
	}
	else
	{
		m_strMode		= pDoc->m_stStatText.strMode1;
	}

	// statische Texte auch austauschen
	// static texts
	m_strStMtu1			= _T("")   + pDoc->m_stStatText.strHeadName + _T(" ") + pDoc->m_stStatText.strHeadCity;
	m_strStMtu2			= _T(" ");
	m_strStCompany		= _T("\n") + pDoc->m_stStatText.strCompany;
	m_strStLocType		= _T("\n") + pDoc->m_stStatText.strLocType;
	m_strStLocNr		= _T("\n") + pDoc->m_stStatText.strLocNumber;
	m_strStMotorType	= _T("\n") + pDoc->m_stStatText.strMotorType;
	m_strStMotorNr		= _T("\n") + pDoc->m_stStatText.strMotorNumber;
	m_strStDurCount		= _T("\n") + pDoc->m_stStatText.strDurationCount;
	m_strStDiagDateTime = _T("\n") + pDoc->m_stStatText.strDiag;
	m_strStTermDateTime = _T("\n") + pDoc->m_stStatText.strTerminal;
	m_strStMode			= _T("\n") + pDoc->m_stStatText.strMode;

	UpdateData(FALSE);	
}

BOOL CStateView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// TODO: call DoPreparePrinting to invoke the Print dialog box
	BOOL bRet;

	bRet = DoPreparePrinting(pInfo);
	return bRet;
//	return CFormView::OnPreparePrinting(pInfo);
}

void CStateView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	CRailDiagDoc *pDoc = (CRailDiagDoc*)GetDocument();
	pDoc->DoBeginPrinting(pDC, pInfo);
	
//	CFormView::OnBeginPrinting(pDC, pInfo);
}

void CStateView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	CRailDiagDoc *pDoc = (CRailDiagDoc*)GetDocument();
	pDoc->DoPrepareDC(pDC, pInfo);
	
//	CFormView::OnPrepareDC(pDC, pInfo);
}

void CStateView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	CRailDiagDoc *pDoc = (CRailDiagDoc*)GetDocument();
	pDoc->DoPrint(pDC, pInfo);
	
//	CFormView::OnPrint(pDC, pInfo);
}

void CStateView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	CRailDiagDoc *pDoc = (CRailDiagDoc*)GetDocument();
	pDoc->DoEndPrinting(pDC, pInfo);
	
//	CFormView::OnEndPrinting(pDC, pInfo);
}


void CStateView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	TRACE(_T("CStateView::OnKeyDown nChar %d\n"), nChar);
	
	CFormView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CStateView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (bActivate)
	{
		TRACE(_T("CStateView::OnActivateView\n"));
		CRailDiagDoc	*pDoc = (CRailDiagDoc*)GetDocument();
		CView			*pView = NULL;

		pView = pDoc->GetErrListView();
		if (pView)
		{
//			pView->SetFocus();
//			pView->SetCapture();
			PostMessage(WM_USER_ACTIVATE_LIST_VIEW);
		}
		
	}
	
	CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}
