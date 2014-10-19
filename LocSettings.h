#include "afxwin.h"
#if !defined(AFX_LOCSETTINGS_H__D465D148_E917_42DD_BB46_89A19F37BD03__INCLUDED_)
#define AFX_LOCSETTINGS_H__D465D148_E917_42DD_BB46_89A19F37BD03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LocSettings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLocSettings dialog

class CLocSettings : public CDialog
{
// Construction
public:
	void SettingsSet(void);
	void ConnectionLost(void);
	CLocSettings(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLocSettings)
	enum { IDD = IDD_LOC_SETTINGS };
	CButton	m_CtrlSend;
	CButton	m_CtrlOk;
	CButton	m_CtrlCancel;
	CString	m_strCompany;
	CString	m_strLocNumber;
	CString	m_strLocType;
	CString	m_strMotorNumber;
	CString	m_strMotorType;
	CString	m_strMtuOrderNumber;
	CString	m_strStCompany;
	CString	m_strStLocNumber;
	CString	m_strStLocType;
	CString	m_strStMotorNumber;
	CString	m_strStMotorType;
	CString	m_strStMtuOrderNumber;
	CString	m_strPin;
	CString	m_strStPin;
	CString	m_strStMessage;
	CString	m_strPuk;
	CString	m_strStPuk;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLocSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void PumpMessages();

	// Generated message map functions
	//{{AFX_MSG(CLocSettings)
	virtual BOOL OnInitDialog();
	afx_msg void OnSettingsSend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CString m_strDisplayTelNumber;
	CString m_strInhibitTime;
	CString m_strTelNumberSMS1;
	CString m_strTelNumberSMS2;
	CString m_strTelNumberSMS3;
	CString m_strStDisplayTelNumber;
	CString m_strStInhibitTime;
	CString m_strStTelNumberSMS1;
	CString m_strStTelNumberSMS2;
	CString m_strStTelNumberSMS3;
	afx_msg void OnBnClickedSettingsLoad();
	afx_msg void OnBnClickedSettingsSave();
	CButton m_CtrlLoad;
	CButton m_CtrlSave;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOCSETTINGS_H__D465D148_E917_42DD_BB46_89A19F37BD03__INCLUDED_)
