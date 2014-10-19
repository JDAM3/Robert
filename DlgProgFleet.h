#pragma once


// CDlgProgFleet dialog

class CDlgProgFleet : public CDialog
{
	DECLARE_DYNAMIC(CDlgProgFleet)

public:
	CDlgProgFleet(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgProgFleet();

// Dialog Data
	enum { IDD = IDD_PROGRESS_FLEET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
