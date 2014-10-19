// splitter.cpp : implementation file
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "RailDiag.h"

#include "RailDiagDoc.h"
#include "RailDiagView.h"
#include "StateView.h"
#include "ErrListView.h"
#include "splitter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplitterFrame

// Create a splitter window which splits an output text view and an input view
//                           |
//    TEXT VIEW (CTextView)  | INPUT VIEW (CInputView)
//                           |

IMPLEMENT_DYNCREATE(CSplitterFrame, CMDIChildWnd)

CSplitterFrame::CSplitterFrame()
{
}

CSplitterFrame::~CSplitterFrame()
{
}


BOOL CSplitterFrame::OnCreateClient(LPCREATESTRUCT,
	 CCreateContext* pContext)
{
	// create a splitter with 2 row, 1 columns
	if (!m_wndSplitter.CreateStatic(this, 2, 1))
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}
/*
	// add the first splitter pane - the default view in row 1
	if (!m_wndSplitter.CreateView(1, 0,
		pContext->m_pNewViewClass, CSize(130, 50), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}
*/

	// add the first splitter pane - the default view in row 1
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();

	if (pApp->m_eDataType == EN_DATA_TYPE_DURATION_FLEET)
	{
		if (!m_wndSplitter.CreateView(0, 0,
			RUNTIME_CLASS(CStateView), CSize(250, 50), pContext))
		{
			TRACE0("Failed to create first pane\n");
			return FALSE;
		}
	}
	else
	{
		if (!m_wndSplitter.CreateView(0, 0,
			RUNTIME_CLASS(CStateView), CSize(200, 180), pContext))
		{
			TRACE0("Failed to create first pane\n");
			return FALSE;
		}
	}

	// add the second splitter pane - an input view in row 0
	if (!m_wndSplitter.CreateView(1, 0,
		RUNTIME_CLASS(CErrListView), CSize(250, 50), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}

	// activate the list view
	SetActiveView((CView*)m_wndSplitter.GetPane(1,0));

	return TRUE;
}

BEGIN_MESSAGE_MAP(CSplitterFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CSplitterFrame)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CSplitterFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();

	if (pApp->m_eDataType == EN_DATA_TYPE_DURATION_FLEET)
	{
		cs.cx = 947;
	}
	else
	{
		cs.cx = 927;
	}

	cs.style = cs.style | WS_MAXIMIZE;
//	cs.style &= ~WS_MAXIMIZEBOX;
	
	return CMDIChildWnd::PreCreateWindow(cs);
}


void CSplitterFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	TRACE(_T("CSplitterFrame::OnSize nType %d, cx %d, cy %d\n"), nType, cx, cy);
	
}
