#pragma once
#include "afxwin.h"


// CDlgFleetSel dialog

class CDlgFleetSel : public CDialog
{
	DECLARE_DYNAMIC(CDlgFleetSel)

public:
	CDlgFleetSel(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFleetSel();

// Dialog Data
	enum { IDD = IDD_FLEET_SEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_CtrlComboFleetName;
	virtual BOOL OnInitDialog();
};
