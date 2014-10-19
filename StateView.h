#if !defined(AFX_STATEVIEW_H__894E54F6_3D96_475E_B680_C37EB03F7CD0__INCLUDED_)
#define AFX_STATEVIEW_H__894E54F6_3D96_475E_B680_C37EB03F7CD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StateView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStateView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CStateView : public CFormView
{
protected:
	CStateView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CStateView)

// Form Data
public:
	//{{AFX_DATA(CStateView)
	enum { IDD = IDD_DLG_STATE };
	CString	m_strCompany;
	CString	m_strLocNr;
	CString	m_strLocType;
	CString	m_strMotorNr;
	CString	m_strMotorType;
	CString	m_strDurCount;
	CString	m_strMode;
	CString	m_strDiagDateTime;
	CString	m_strTermDateTime;
	CString	m_strStCompany;
	CString	m_strStDiagDateTime;
	CString	m_strStDurCount;
	CString	m_strStLocNr;
	CString	m_strStLocType;
	CString	m_strStMode;
	CString	m_strStMotorNr;
	CString	m_strStMotorType;
	CString	m_strStMtu1;
	CString	m_strStMtu2;
	CString	m_strStTermDateTime;
	//}}AFX_DATA

// Attributes
public:
	CRailDiagDoc* GetDocument()
			{
				ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRailDiagDoc)));
				return (CRailDiagDoc*) m_pDocument;
			}

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStateView)
	public:
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CStateView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CStateView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATEVIEW_H__894E54F6_3D96_475E_B680_C37EB03F7CD0__INCLUDED_)
