#if !defined(AFX_MODEMSETTINGS_H__71964FA6_B9FE_4D28_9032_835AEFD213A9__INCLUDED_)
#define AFX_MODEMSETTINGS_H__71964FA6_B9FE_4D28_9032_835AEFD213A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModemSettings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModemSettings dialog

class CModemSettings : public CDialog
{
// Construction
public:
	CModemSettings(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModemSettings)
	enum { IDD = IDD_MODEM_SETTINGS };
	CButton	m_CtrlSettings;
	CButton	m_CtrlOk;
	CButton	m_CtrlCancel;
	CString	m_ModemNumber;
	CString	m_StModemNumber;
	CString	m_ModemDesc;
	CString	m_StModemDesc;
	//}}AFX_DATA

	CString	m_ModemName;
	CString	m_ModemPin;
	BOOL	m_bSavePin;
	CString	m_ModemPuk;
	BOOL	m_bDialPrefix;
	CString	m_DialPrefix;
	CString m_DialPrefixView;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModemSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModemSettings)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendokCmConnName();
	afx_msg void OnConnPhoneBook();
	afx_msg void OnChangeEdConnNumber();
	afx_msg void OnSettings();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODEMSETTINGS_H__71964FA6_B9FE_4D28_9032_835AEFD213A9__INCLUDED_)
