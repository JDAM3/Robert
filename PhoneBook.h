#include "afxwin.h"
#if !defined(AFX_PHONEBOOK_H__61BC1678_2C67_4FC1_A1E2_02412B2C2681__INCLUDED_)
#define AFX_PHONEBOOK_H__61BC1678_2C67_4FC1_A1E2_02412B2C2681__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PhoneBook.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPhoneBook dialog

class CPhoneBook : public CDialog
{
// Construction
public:
	void UpdateList(void);
	void EditEntry();
	CPhoneBook(CWnd* pParent = NULL);   // standard constructor
	static int CALLBACK CompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	void UpdateButtons(void);

protected:
	BOOL			m_bSortAsc;
	int				m_iSortCol;

public:
	CStringArray	m_strarrDescription;
	CStringArray	m_strarrNumber;
	BOOL			m_bModified;
	BOOL			m_bSelect;
	int				m_nItem;

// Dialog Data
	//{{AFX_DATA(CPhoneBook)
	enum { IDD = IDD_PHONE_BOOK };
	CButton	m_CtrlDeleteEntry;
	CButton	m_CtrlEditEntry;
	CButton	m_CtrlNewEntry;
	CButton	m_CtrlOk;
	CButton	m_CtrlCancel;
	CListCtrl	m_ListCtrlPhone;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPhoneBook)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPhoneBook)
	virtual BOOL OnInitDialog();
	afx_msg void OnEditEntry();
	afx_msg void OnDblclkListPhone(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickListPhone(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickListPhone(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNewEntry();
	afx_msg void OnDeleteEntry();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CButton m_CtrlExport;
	CButton m_CtrlImport;
	afx_msg void OnBnClickedPhbImport();
	afx_msg void OnBnClickedPhbExport();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PHONEBOOK_H__61BC1678_2C67_4FC1_A1E2_02412B2C2681__INCLUDED_)
