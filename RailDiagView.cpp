// RailDiagView.cpp : implementation of the CRailDiagView class
//

#include "stdafx.h"
#include "RailDiag.h"

#include "RailDiagDoc.h"
#include "RailDiagView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRailDiagView

IMPLEMENT_DYNCREATE(CRailDiagView, CView)

BEGIN_MESSAGE_MAP(CRailDiagView, CView)
	//{{AFX_MSG_MAP(CRailDiagView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRailDiagView construction/destruction

CRailDiagView::CRailDiagView()
{
	// TODO: add construction code here

}

CRailDiagView::~CRailDiagView()
{
}

BOOL CRailDiagView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CRailDiagView drawing

void CRailDiagView::OnDraw(CDC* pDC)
{
	CRailDiagDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CRailDiagView printing

BOOL CRailDiagView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CRailDiagView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CRailDiagView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CRailDiagView diagnostics

#ifdef _DEBUG
void CRailDiagView::AssertValid() const
{
	CView::AssertValid();
}

void CRailDiagView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRailDiagDoc* CRailDiagView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRailDiagDoc)));
	return (CRailDiagDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRailDiagView message handlers
