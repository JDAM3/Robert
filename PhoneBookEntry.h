#if !defined(AFX_PHONEBOOKENTRY_H__BABF41C0_BA5D_4708_8CE0_9747116B342A__INCLUDED_)
#define AFX_PHONEBOOKENTRY_H__BABF41C0_BA5D_4708_8CE0_9747116B342A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PhoneBookEntry.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPhoneBookEntry dialog

class CPhoneBookEntry : public CDialog
{
// Construction
public:
	CPhoneBookEntry(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPhoneBookEntry)
	enum { IDD = IDD_PHONE_BOOK_ENTRY };
	CButton	m_CtrlOk;
	CButton	m_CtrlCancel;
	CString	m_strDescription;
	CString	m_strNumber;
	CString	m_strDescriptionSt;
	CString	m_strNumberSt;
	CString	m_strHelpNumberSt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPhoneBookEntry)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPhoneBookEntry)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PHONEBOOKENTRY_H__BABF41C0_BA5D_4708_8CE0_9747116B342A__INCLUDED_)
