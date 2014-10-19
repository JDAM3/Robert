#include "afxwin.h"
#if !defined(AFX_MODEMSETTINGS2_H__6ADF6ED3_9C35_4816_B8A6_C4C4B6EFEE71__INCLUDED_)
#define AFX_MODEMSETTINGS2_H__6ADF6ED3_9C35_4816_B8A6_C4C4B6EFEE71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModemSettings2.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModemSettings2 dialog

class CModemSettings2 : public CDialog
{
// Construction
public:
	CModemSettings2(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModemSettings2)
	enum { IDD = IDD_MODEM_SETTINGS2 };
	CEdit	m_CtrlModemPuk;
	CButton	m_CtrlSavePin;
	CButton	m_CtrlOk;
	CButton	m_CtrlCancel;
	CComboBox	m_CntrlModemName;
	CString	m_ModemPin;
	CString	m_ModemPuk;
	BOOL	m_bSavePin;
	CString	m_StModemName;
	CString	m_StModemPin;
	CString	m_StModemPuk;
	CString	m_ModemName;
	CString m_StHelpDialPrefix;
	CString m_StDialPrefix;
	CButton	m_CtrlDialPrefix;
	BOOL	m_bDialPrefix;
	CString m_DialPrefix;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModemSettings2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModemSettings2)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEdConnPin();
	afx_msg void OnChangeEdConnPuk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedDialPrefix();
	CEdit m_CtrlEdDialPrefix;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODEMSETTINGS2_H__6ADF6ED3_9C35_4816_B8A6_C4C4B6EFEE71__INCLUDED_)
