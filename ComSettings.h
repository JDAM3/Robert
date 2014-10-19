#if !defined(AFX_COMSETTINGS_H__59B8E399_CC4D_458C_959D_BB171CFD70CC__INCLUDED_)
#define AFX_COMSETTINGS_H__59B8E399_CC4D_458C_959D_BB171CFD70CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComSettings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CComSettings dialog

class CComSettings : public CDialog
{
// Construction
public:
	CComSettings(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CComSettings)
	enum { IDD = IDD_COM_SETTINGS };
	CButton	m_CtrlOk;
	CButton	m_CtrlCancel;
	CString	m_ComBits;
	CString	m_ComBps;
	CString	m_ComControl;
	CString	m_ComParity;
	CString	m_ComPort;
	CString	m_ComStopBits;
	CString	m_StComBits;
	CString	m_StComBps;
	CString	m_StComControl;
	CString	m_StComParity;
	CString	m_StComPort;
	CString	m_StComStopBits;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CComSettings)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMSETTINGS_H__59B8E399_CC4D_458C_959D_BB171CFD70CC__INCLUDED_)
