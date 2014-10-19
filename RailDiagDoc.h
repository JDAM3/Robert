// RailDiagDoc.h : interface of the CRailDiagDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAILDIAGDOC_H__2568267B_DA8A_45BE_A081_04435DF9B6F3__INCLUDED_)
#define AFX_RAILDIAGDOC_H__2568267B_DA8A_45BE_A081_04435DF9B6F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct _tab_PRINT_INFO
{
	BOOL		bBeginPrinting;
	BOOL		bPrinting;
	int			iBorderTop;
	int			iBorderLeft;
	int			iBorderBottom;
	int			iBorderRight;
	UINT		uiDataLine;
	CUIntArray	uiarrDataLine;
	CRect		rcHeader;
	CRect		rcTitle;
	CRect		rcDescription;
	CRect		rcFooter;
	CFont		*pftHeader;
	CFont		*pftTitle;
	CFont		*pftDescription, *pftDescriptionBld;
	CFont		*pftList, *pftListBld;
	CFont		*pftFooter;
	CBrush		*pcbLine;
} PRINT_INFO;

class CRailDiagDoc : public CDocument
{
protected: // create from serialization only
	CRailDiagDoc();
	DECLARE_DYNCREATE(CRailDiagDoc)

// Attributes
public:
	CStringArray			m_strarrListData;
	BOOL					m_bUpdateView;
	int						m_iLineStart;
	BOOL					m_bCancel;

public:
	LOC_SETTINGS			m_stLocSettings;
	CString					m_strDurationCount;
	CString					m_strDiagDateTime;
	CString					m_strTerminalDateTime;
	CString					m_strMode;
	EN_DATA_TYPE			m_eDataType;
	int						m_iLang;
	BOOL					m_bNewColumnHeader;


public:
	STAT_LANG_TXT_DOC		m_stStatText;
	STAT_LANG_TXT_LIST		m_stStatBinary;
	STAT_LANG_TXT_LIST		m_stStatAnalog;
	STAT_LANG_TXT_LIST		m_stStatAlarm;
	STAT_LANG_TXT_LIST		m_stStatDuration;
	STAT_LANG_TXT_LIST		m_stStatDurationFleet;
	STAT_LANG_TXT_LIST		m_stStatHistory;
	STAT_LANG_TXT_LIST		m_stStatHistory2;

protected:
	PRINT_INFO				m_stPrintInfo;

// Operations
public:
	BOOL SaveDocData(CStdioFile &file);
	BOOL ReadDocData(CStdioFile &file);
	BOOL ExportDocData(CStdioFile &file);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRailDiagDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void SetTitle(LPCTSTR lpszTitle);
	//}}AFX_VIRTUAL

// Implementation
public:
	void ReloadData(void);
	BOOL IsPrinting(void);
	void DoPrepareDC(CDC* pDC, CPrintInfo* pInfo);
	void DoEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	void DoBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	void DoPrint(CDC* pDC, CPrintInfo* pInfo);
	void NewStateData(TCHAR *ptszData);
	CView * GetStaticView(void);
	CView * GetErrListView(void);
	void NewErrorHistUpdate(void);
	void NewErrorHistStart(void);
	BOOL NewErrorHistData(TCHAR *ptszData, int *piLineCount);
	virtual ~CRailDiagDoc();
	BOOL NewData4Document(void);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void PrCreateFonts(CDC *pDC);
	void PrHeader(CDC *pDC, CPrintInfo *pInfo);
	void PrTitle(CDC *pDC, CPrintInfo *pInfo);
	void PrDescription(CDC *pDC, CPrintInfo *pInfo);
	void PrList(CDC *pDC, CPrintInfo *pInfo);
	void PrFooter(CDC *pDC, CPrintInfo *pInfo);
	int  PrText(CDC *pDC, int iLeft, int iRight, int iTop, int iPosRel, UINT uiFormat, CString str);

// Generated message map functions
protected:
	//{{AFX_MSG(CRailDiagDoc)
	afx_msg void OnUpdateFileExport(CCmdUI* pCmdUI);
	afx_msg void OnFileExport();
	afx_msg void OnReloadData();
	afx_msg void OnUpdateReloadData(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveAs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RAILDIAGDOC_H__2568267B_DA8A_45BE_A081_04435DF9B6F3__INCLUDED_)
