#if !defined(AFX_ERRLISTVIEW_H__97EE912E_14A2_4FFE_A628_89BF8F27ACC3__INCLUDED_)
#define AFX_ERRLISTVIEW_H__97EE912E_14A2_4FFE_A628_89BF8F27ACC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ErrListView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CErrListView view

class CErrListView : public CListView
{
protected:
	void CheckNewData(void);
//	void SeparateComma(CString string, CStringArray &stringArr);
	CErrListView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CErrListView)

// Attributes
public:
	int			m_iUpdate;

protected:
	int			m_iEntries;
	BOOL		m_bInitDone;
//	CImageList m_imageList;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CErrListView)
	public:
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CErrListView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	void UpdateColumnHeader();
	//{{AFX_MSG(CErrListView)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ERRLISTVIEW_H__97EE912E_14A2_4FFE_A628_89BF8F27ACC3__INCLUDED_)
