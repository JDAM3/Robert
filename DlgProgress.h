// DlgProgress.h : header file
// CG: This file was added by the Progress Dialog component

/////////////////////////////////////////////////////////////////////////////
// CDlgProgress dialog

#ifndef __DLGPROGRESS_H__
#define __DLGPROGRESS_H__
#include "afxcmn.h"
#include "afxwin.h"

class CDlgProgress : public CDialog
{
// Construction / Destruction
public:
    CDlgProgress(LPTSTR lpCaption);   // standard constructor
    ~CDlgProgress();

    BOOL Create(CWnd *pParent=NULL);

    // Checking for Cancel button
    BOOL CheckCancelButton();
    // Progress Dialog manipulation
    void SetRange(int nLower, int nUpper);
    void SetRangeFleet(int nLower, int nUpper);
    int  SetStep(int nStep);
    int  SetStepFleet(int nStep);
    int  SetPos(int nPos);
    int  OffsetPos(int nPos);
    int  StepIt();
    int  StepItFleet();
	void SetNameFleet(CString strName);
        
// Dialog Data
    //{{AFX_DATA(CDlgProgress)
	enum { IDD = IDD_PROGRESS };
	CButton	m_CtrlCancel;
	CProgressCtrl	m_progress;
	//}}AFX_DATA

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDlgProgress)
    public:
    virtual BOOL DestroyWindow();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    int m_nLower;
    int m_nUpper;
    int m_nStep;
    int m_nLowerFleet;
    int m_nUpperFleet;
    int m_nStepFleet;
	CString	m_caption;
    
    BOOL m_bCancel;
    BOOL m_bParentDisabled;

    void ReEnableParent();

    virtual void OnCancel();
    virtual void OnOK() {}; 
    void UpdatePercent(int nCurrent);
    void PumpMessages();

    // Generated message map functions
    //{{AFX_MSG(CDlgProgress)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
	CString m_strProgressFleet;
	CProgressCtrl m_progressFleet;
	CStatic m_ctrlStProgress;
};

#endif // __DLGPROGRESS_H__
