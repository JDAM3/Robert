// ErrListView.cpp : implementation file
//

#include "stdafx.h"
#include "RailDiag.h"
#include "DlgProgress.h"

#include "ComLib.h"
#include "ComDiag.h"
#include "RailDiagDoc.h"
#include "InOutList.h"
#include "MainFrm.h"
#include "ErrListView.h"

#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CErrListView

IMPLEMENT_DYNCREATE(CErrListView, CListView)

CErrListView::CErrListView()
{
	m_iEntries = 0;
	m_iUpdate = -1;
	m_bInitDone = FALSE;
}

CErrListView::~CErrListView()
{
}


BEGIN_MESSAGE_MAP(CErrListView, CListView)
	//{{AFX_MSG_MAP(CErrListView)
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CErrListView drawing

void CErrListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here

}

/////////////////////////////////////////////////////////////////////////////
// CErrListView diagnostics

#ifdef _DEBUG
void CErrListView::AssertValid() const
{
	CListView::AssertValid();
}

void CErrListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CErrListView message handlers

void CErrListView::OnInitialUpdate() 
{
	CListView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	CRailDiagDoc		*pDoc = (CRailDiagDoc*)GetDocument();
	CStringArray		colArr;
	STAT_LANG_TXT_LIST	*pStatTxtList = NULL;

	switch(pDoc->m_eDataType)
	{
	case EN_DATA_TYPE_BIN:
		pStatTxtList = &pDoc->m_stStatBinary;
		break;
	case EN_DATA_TYPE_ANA:
		pStatTxtList = &pDoc->m_stStatAnalog;
		break;
	case EN_DATA_TYPE_ALARM:
		pStatTxtList = &pDoc->m_stStatAlarm;
		break;
	case EN_DATA_TYPE_DURATION:
		pStatTxtList = &pDoc->m_stStatDuration;
		break;
	case EN_DATA_TYPE_DURATION_FLEET:
		pStatTxtList = &pDoc->m_stStatDurationFleet;
		break;
	case EN_DATA_TYPE_HIST:
	case EN_DATA_TYPE_HIST20:
	case EN_DATA_TYPE_HIST100:
		pStatTxtList = &pDoc->m_stStatHistory;
		break;
	case EN_DATA_TYPE_HIST2:
		pStatTxtList = &pDoc->m_stStatHistory2;
		break;
	}

	int i;
	for (i = 0; i < pStatTxtList->strarrRow.GetSize(); i++)
	{
		colArr.Add(pStatTxtList->strarrRow.GetAt(i));
	}

	// this code only works for a report-mode list view
	ASSERT(GetStyle() & LVS_REPORT);

	// Gain a reference to the list control itself
	CListCtrl& theCtrl = GetListCtrl();

	// The other InsertColumn() override requires an initialized
	// LVCOLUMN structure.
	LVCOLUMN col;

   	col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.cx = 80;

	for (i = 0; i < colArr.GetSize(); i++)
	{
		switch(pDoc->m_eDataType)
		{
		case EN_DATA_TYPE_BIN:
			if (i == 0)
				col.cx = 60;
			else if (i == 1)
				col.cx = 200;
			else
				col.cx = 120;
			break;
		case EN_DATA_TYPE_ANA:
			if (i == 0)
				col.cx = 60;
			else if (i == 1)
				col.cx = 200;
			else
				col.cx = 120;
			break;
		case EN_DATA_TYPE_ALARM:
			if (i == 0)
				col.cx = 60;
			else if (i == 1)
				col.cx = 200;
			else
				col.cx = 120;
			break;
		case EN_DATA_TYPE_DURATION:
			if (i == 0)
				col.cx = 60;
			else if (i == 1)
				col.cx = 200;
			else
				col.cx = 120;
			break;
		case EN_DATA_TYPE_DURATION_FLEET:
			if (i == 0)
				col.cx = 60;
			else if (i == 1)
				col.cx = 200;
			else
				col.cx = 120;
			break;
		case EN_DATA_TYPE_HIST:
		case EN_DATA_TYPE_HIST20:
		case EN_DATA_TYPE_HIST100:
			if (i == 0)
				col.cx = 60;
			else if (i == 1)
				col.cx = 200;
			else
				col.cx = 80;
			break;
		case EN_DATA_TYPE_HIST2:
			if (0 == i)
				col.cx = 60;
			else if (1 == i)
				col.cx = 200;
			else if (4 <= i && 7 >= i)
				col.cx = 90;
			else
				col.cx = 80;
			break;
		}


		col.pszText = colArr[i].GetBuffer(1);
		theCtrl.InsertColumn(i, &col);
		colArr[i].ReleaseBuffer();
	}

	m_bInitDone = TRUE;
	CheckNewData();
}

BOOL CErrListView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= LVS_SHOWSELALWAYS | LVS_REPORT;
	
	return CListView::PreCreateWindow(cs);
}

void CErrListView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: Add your specialized code here and/or call the base class
	CRailDiagDoc *pDoc = (CRailDiagDoc*)GetDocument();
	pDoc->m_bUpdateView = FALSE;

	if (pDoc->m_bNewColumnHeader)
	{
		UpdateColumnHeader();
		pDoc->m_bNewColumnHeader = FALSE;
	}

	CheckNewData();
}

void CErrListView::CheckNewData(void)
{
	CRailDiagDoc *pDoc = (CRailDiagDoc*)GetDocument();

	if (!m_bInitDone)
		return;

//	TRACE(_T("CErrListView::CheckNewData\n"));
	if (m_iUpdate != -1 && m_iEntries <= pDoc->m_strarrListData.GetSize())
	{
		CStringArray	strarrSubItems;
		CUtil			cUtil;
		int				j;
		CListCtrl&		theCtrl = GetListCtrl();

		for (int i = m_iUpdate; i < m_iEntries; i++)
		{

			strarrSubItems.RemoveAll();
			cUtil.SeparateComma(pDoc->m_strarrListData[i], strarrSubItems);
			int	iElem = strarrSubItems.GetSize();
			for ( j = 0; j < min(9, iElem-1); j++)  
			{
				if (j == 0)
				{
					theCtrl.SetItemText(i, j, strarrSubItems[j].GetBuffer(1));
					strarrSubItems[j].ReleaseBuffer();
				}
				else // Variablenname übergehen daher j = 1
				{
					theCtrl.SetItemText(i, j, strarrSubItems[j+1].GetBuffer(1));
					strarrSubItems[j+1].ReleaseBuffer();
				}
			}
		}
		m_iUpdate = -1;
	}

	if (pDoc->m_strarrListData.GetSize() != m_iEntries)
	{
		CListCtrl& theCtrl = GetListCtrl();
		LV_ITEM		lvi;

		if (pDoc->m_strarrListData.GetSize() == 0)
		{
			theCtrl.DeleteAllItems();
			m_iEntries = 0;
		}
		else
		{
			CStringArray	strarrSubItems;
			CUtil			cUtil;
			int				j;

//			TRACE(_T("CErrListView::OnUpdate Entries %d, strarrErrorHist Size %d\n"), m_iEntries, pDoc->m_strarrErrorHist.GetSize());
			for (int i = m_iEntries; i < pDoc->m_strarrListData.GetSize(); i++)
			{
				lvi.mask = LVIF_TEXT | LVIF_PARAM;
				lvi.iItem = i;
				lvi.iSubItem = 0;
				lvi.pszText = _T("???");
				lvi.lParam = NULL;
				theCtrl.InsertItem(&lvi);
				m_iEntries ++;

//				TRACE(_T("Draw Line i %d %s\n"), i, pDoc->m_strarrErrorHist[i]);

				strarrSubItems.RemoveAll();
				cUtil.SeparateComma(pDoc->m_strarrListData[i], strarrSubItems);
				int	iElem = strarrSubItems.GetSize();
				for ( j = 0; j < min(9, iElem-1); j++)  
				{
					if (j == 0)
					{
						theCtrl.SetItemText(i, j, strarrSubItems[j].GetBuffer(1));
						strarrSubItems[j].ReleaseBuffer();
					}
					else // Variablenname übergehen daher j = 1
					{
						theCtrl.SetItemText(i, j, strarrSubItems[j+1].GetBuffer(1));
						strarrSubItems[j+1].ReleaseBuffer();
					}
				}
			}
		}
		theCtrl.Scroll(CSize(0, 50000));
	}
}

void CErrListView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CListView::OnTimer(nIDEvent);
}

BOOL CErrListView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// TODO: call DoPreparePrinting to invoke the Print dialog box
	BOOL bRet;

	bRet = DoPreparePrinting(pInfo);
	return bRet;
//	return CListView::OnPreparePrinting(pInfo);
}

void CErrListView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	CRailDiagDoc *pDoc = (CRailDiagDoc*)GetDocument();
	pDoc->DoBeginPrinting(pDC, pInfo);

//	CListView::OnBeginPrinting(pDC, pInfo);
}

void CErrListView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	CRailDiagDoc *pDoc = (CRailDiagDoc*)GetDocument();
	pDoc->DoPrepareDC(pDC, pInfo);
	
//	CListView::OnPrepareDC(pDC, pInfo);
}

void CErrListView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	CRailDiagDoc *pDoc = (CRailDiagDoc*)GetDocument();
	pDoc->DoPrint(pDC, pInfo);

//	CListView::OnPrint(pDC, pInfo);
}

void CErrListView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	CRailDiagDoc *pDoc = (CRailDiagDoc*)GetDocument();
	pDoc->DoEndPrinting(pDC, pInfo);
	
//	CListView::OnEndPrinting(pDC, pInfo);
}

void CErrListView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	TRACE(_T("CErrListView::OnKeyDown nChar %d\n"), nChar);

	CRailDiagDoc	*pDoc = (CRailDiagDoc*)GetDocument();
	if (nChar == 13) // return
	{
		CMainFrame		*pMainFrame;
		CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();

		pDoc->m_bCancel = FALSE;

		pMainFrame = (CMainFrame*)pApp->GetMainWnd();
		pMainFrame->PostMessage(WM_USER_NEW_PROCESS_DIALOG_START);

		pDoc->ReloadData();
	}

	CListView::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CErrListView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (bActivate)
	{
		TRACE(_T("CErrListView::OnActivateView\n"));

		CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
		CRailDiagDoc	*pDoc = (CRailDiagDoc*)GetDocument();
		CMainFrame		*pMainFrame;

		pMainFrame = (CMainFrame*)pApp->GetMainWnd();
		pMainFrame->SetDoc(pDoc);

	}
	
	CListView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CErrListView::UpdateColumnHeader()
{
	CRailDiagDoc		*pDoc = (CRailDiagDoc*)GetDocument();
	CStringArray		colArr;
	STAT_LANG_TXT_LIST	*pStatTxtList = NULL;
	BOOL				bStat;

	switch(pDoc->m_eDataType)
	{
	case EN_DATA_TYPE_BIN:
		pStatTxtList = &pDoc->m_stStatBinary;
		break;
	case EN_DATA_TYPE_ANA:
		pStatTxtList = &pDoc->m_stStatAnalog;
		break;
	case EN_DATA_TYPE_ALARM:
		pStatTxtList = &pDoc->m_stStatAlarm;
		break;
	case EN_DATA_TYPE_DURATION:
		pStatTxtList = &pDoc->m_stStatDuration;
		break;
	case EN_DATA_TYPE_DURATION_FLEET:
		pStatTxtList = &pDoc->m_stStatDurationFleet;
		break;
	case EN_DATA_TYPE_HIST:
	case EN_DATA_TYPE_HIST20:
	case EN_DATA_TYPE_HIST100:
		pStatTxtList = &pDoc->m_stStatHistory;
		break;
	case EN_DATA_TYPE_HIST2:
		pStatTxtList = &pDoc->m_stStatHistory2;
		break;
	}

	int i;
	for (i = 0; i < pStatTxtList->strarrRow.GetSize(); i++)
	{
		colArr.Add(pStatTxtList->strarrRow.GetAt(i));
		TRACE(_T("CErrListView::UpdateColumnHeader %s\n"), pStatTxtList->strarrRow.GetAt(i));
	}

	// this code only works for a report-mode list view
	ASSERT(GetStyle() & LVS_REPORT);

	// Gain a reference to the list control itself
	CListCtrl& theCtrl = GetListCtrl();

	// The other InsertColumn() override requires an initialized
	// LVCOLUMN structure.
	LVCOLUMN col;

   	col.mask = LVCF_TEXT;

	for (i = 0; i < colArr.GetSize(); i++)
	{
		col.pszText = colArr[i].GetBuffer(1);
		bStat = theCtrl.SetColumn(i, &col);
		colArr[i].ReleaseBuffer();
	}
}
