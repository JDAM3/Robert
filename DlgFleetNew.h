#pragma once
#include "afxwin.h"


// DlgFleetNew dialog

class CDlgFleetNew : public CDialog
{
	DECLARE_DYNAMIC(CDlgFleetNew)

public:
	CDlgFleetNew(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFleetNew();

// Dialog Data
	enum { IDD = IDD_FLEET_NEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strFleetName;
	virtual BOOL OnInitDialog();
	CButton m_ctrlOk;
	CButton m_ctrlCancel;
	afx_msg void OnBnClickedOk();

	CStringArray	m_strarrFleetName;
};
