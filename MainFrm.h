// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__D8E713BE_7E36_4D0A_BD23_85F76078AAB2__INCLUDED_)
#define AFX_MAINFRM_H__D8E713BE_7E36_4D0A_BD23_85F76078AAB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "MyStatusBar.h"

typedef enum
{
	EN_AUTO_DISCONNECTED = 1,
	EN_AUTO_CONNECTING,
	EN_AUTO_CONNECTED,
//	EN_AUTO_COMMAND_FIRST,
	EN_AUTO_COMMAND_RUNNING,
	EN_AUTO_COMMAND_NEXT,
	EN_AUTO_END
} AUTO_STATES;

class CRailDiagDoc;

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
protected:
	CString			m_strConnState;
	CString			m_strConnLevel;
	CString			m_strConnError;
	DWORD			m_dwLastReadTick;
	DWORD			m_dwLastWriteTick;
	DWORD			m_dwLastActiveTick;
	int				m_iRepeatCount;
	CDlgProgress	*m_pDlgProg;
	int				m_iDlgStartCount;
	int				m_iDlgCount;
	BOOL			m_bStop;
	CImageList		m_imageList;
	AUTO_STATES		m_eAutoState;
	DWORD			m_dwAutoConnectStart;
	int				m_iWriteCount;
	CUIntArray		*m_puiarrLangs;
	int				m_iReconnectStat;
	DWORD			m_dwDlgProgTickCount;
	int				m_iNoifyState;
	CInOutList		m_InOutList;


public:
	TCHAR			m_tszRead[CD_MAX_MSG_LEN];
	DWORD			m_dwPos;

	TCHAR			m_tszSendMsg[CD_MAX_MSG_LEN];
	BOOL			m_bSendMsg;						// Meldung zum senden vorhanden
	int				m_iSendType;
	int				m_iLineCountTotal;
	int				m_iLineCount;
	CRailDiagDoc	*m_pDoc;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
protected:
//		HBITMAP LoadBmp(CString strBmpFile);

public:
	void ResetDataCount(void);
	void SendDataLineCancel(void);
	void SetProgressLineCount(int iLineTotal);
	void SetConnError(CString strError);
	void ClearConnError(void);
	void SendLang(int iLang);
	void SetLangList(CUIntArray *puiarrLangSel);
	void EnableFleetList(BOOL bEnable);
	void SetFleetList(CStringArray &strarrFleetNames);
	void ReconnectReceive(void);
	BOOL SetMenuText(BOOL bDoMenuChange = FALSE);
	void SetLangImageList();
	void ConfigReceive(void);
	void ConfigInitSend(CString strCompany, CString strLocType, CString strLocNumber, 
			CString strMotorType, CString strMotorNumber, CString strMtuOrderNumber, 
			CString strDisplayTelNumber, CString strInhibitTime,
			CString strTelNumberSMS1, CString strTelNumberSMS2, CString strTelNumberSMS3,
			CString strReturnStartPageTime,
			CString strPin, CString strPuk);
	void SetDoc(CRailDiagDoc *pDoc);
	void SendSerialData(TCHAR *tszMsg, BOOL	bResetCount = TRUE);
	void NewSerialData(void);
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CToolBar    m_wndToolBar2;
	CToolBar    m_wndToolBar3;
	CReBar      m_wndReBar;
	CDialogBar  m_wndDlgBar;
	CDialogBar  m_wndDlgBarFleet;


// Generated message map functions
protected:
	void SendNextList(void);
	void SendStopCmdNoData(void);
	void SendStopCmdNoAccess(void);
	void NotifyDisconnected(void);
	void NotifyConnected(void);

	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelendokCmLanguage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg BOOL OnNotifyNeedText(UINT id, NMHDR *pNMHDR, LRESULT *result);
public:
	afx_msg void OnCbnSelendokCombExFleet();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__D8E713BE_7E36_4D0A_BD23_85F76078AAB2__INCLUDED_)
