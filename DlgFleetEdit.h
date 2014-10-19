#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CDlgFleetEdit dialog

class CDlgFleetEdit : public CDialog
{
	DECLARE_DYNAMIC(CDlgFleetEdit)

public:
	CDlgFleetEdit(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFleetEdit();
	
public:
	CStringArray	m_strarrDescription;
	CStringArray	m_strarrNumber;

	CStringArray	m_strarrFleetDesc;
	CPtrArray		m_ptrarrFleetElem;



// Dialog Data
	enum { IDD = IDD_FLEET_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()


public:
	CButton m_CtrlAddLok;
	CButton m_CtrlDelLok;
	CButton m_CtrlDelFleet;
	CButton m_CtrlNewFleet;
	CComboBox m_CtrlComboFleetName;
	CListCtrl m_CtrlListPhoneBook;
	CListCtrl m_CtrlListFleet;
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedNew();
	afx_msg void OnBnClickedSetLok();
	afx_msg void OnBnClickedResetLok();
	afx_msg void OnBnClickedDel();

	afx_msg void OnCbnCloseupFleetName();
	afx_msg void OnCbnSelchangeFleetName();

	void RenumFleetList();
	void UpdateFleetList();
	afx_msg void OnBnClickedFleetImport();
	afx_msg void OnBnClickedFleetExport();
	CButton m_CtrlOk;
	CButton m_CtrlCancel;
	CButton m_CtrlImport;
	CButton m_CtrlExport;
	CString m_strFleetPhoneBookSt;
	CString m_strFleetFleetSt;
};
