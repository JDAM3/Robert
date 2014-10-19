// RailDiagDoc.cpp : implementation of the CRailDiagDoc class
//
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include "stdafx.h"
#include "RailDiag.h"
#include "DlgProgress.h"

#include "ComLib.h"
#include "ComDiag.h"
#include "InOutList.h"
#include "MainFrm.h"
#include "RailDiagDoc.h"
#include "ErrListView.h"
#include "StateView.h"

#include "Util.h"
#include "FileIo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRailDiagDoc

IMPLEMENT_DYNCREATE(CRailDiagDoc, CDocument)

BEGIN_MESSAGE_MAP(CRailDiagDoc, CDocument)
	//{{AFX_MSG_MAP(CRailDiagDoc)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT, OnUpdateFileExport)
	ON_COMMAND(ID_FILE_EXPORT, OnFileExport)
	ON_COMMAND(ID_RELOAD_DATA, OnReloadData)
	ON_UPDATE_COMMAND_UI(ID_RELOAD_DATA, OnUpdateReloadData)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRailDiagDoc construction/destruction

CRailDiagDoc::CRailDiagDoc()
{
	// TODO: add one-time construction code here
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	int				i;

	m_bUpdateView = FALSE;
	m_iLang = pApp->m_iLang;
	m_bNewColumnHeader = FALSE;
	m_bCancel = FALSE;

	// statische Texte übernehmen
	m_stStatText				= pApp->m_stStatText;
	m_stStatBinary.strTitle		= pApp->m_stStatBinary.strTitle;
	m_stStatAnalog.strTitle		= pApp->m_stStatAnalog.strTitle;
	m_stStatAlarm.strTitle		= pApp->m_stStatAlarm.strTitle;
	m_stStatHistory.strTitle	= pApp->m_stStatHistory.strTitle;
	m_stStatHistory2.strTitle	= pApp->m_stStatHistory2.strTitle;
	m_stStatDuration.strTitle	= pApp->m_stStatDuration.strTitle;
	m_stStatDurationFleet.strTitle	= pApp->m_stStatDurationFleet.strTitle;

	for (i = 0; i < pApp->m_stStatBinary.strarrRow.GetSize(); i++)
	{
		m_stStatBinary.strarrRow.Add(pApp->m_stStatBinary.strarrRow.GetAt(i));
	}

	for (i = 0; i < pApp->m_stStatAnalog.strarrRow.GetSize(); i++)
	{
		m_stStatAnalog.strarrRow.Add(pApp->m_stStatAnalog.strarrRow.GetAt(i));
	}

	for (i = 0; i < pApp->m_stStatAlarm.strarrRow.GetSize(); i++)
	{
		m_stStatAlarm.strarrRow.Add(pApp->m_stStatAlarm.strarrRow.GetAt(i));
	}

	for (i = 0; i < pApp->m_stStatDuration.strarrRow.GetSize(); i++)
	{
		m_stStatDuration.strarrRow.Add(pApp->m_stStatDuration.strarrRow.GetAt(i));
	}

	for (i = 0; i < pApp->m_stStatDurationFleet.strarrRow.GetSize(); i++)
	{
		m_stStatDurationFleet.strarrRow.Add(pApp->m_stStatDurationFleet.strarrRow.GetAt(i));
	}

	for (i = 0; i < pApp->m_stStatHistory.strarrRow.GetSize(); i++)
	{
		m_stStatHistory.strarrRow.Add(pApp->m_stStatHistory.strarrRow.GetAt(i));
	}

	for (i = 0; i < pApp->m_stStatHistory2.strarrRow.GetSize(); i++)
	{
		m_stStatHistory2.strarrRow.Add(pApp->m_stStatHistory2.strarrRow.GetAt(i));
	}

	m_iLineStart = 0;

	// Printinfo initialisieren
	m_stPrintInfo.bBeginPrinting = FALSE;
	m_stPrintInfo.bPrinting = FALSE;
	m_stPrintInfo.pftHeader = NULL;
	m_stPrintInfo.pftTitle = NULL;
	m_stPrintInfo.pftDescription = NULL;
	m_stPrintInfo.pftDescriptionBld = NULL;
	m_stPrintInfo.pftList = NULL;
	m_stPrintInfo.pftListBld = NULL;
	m_stPrintInfo.pftFooter = NULL;
	m_stPrintInfo.pcbLine = NULL;
	m_stPrintInfo.iBorderLeft = 350;
	m_stPrintInfo.iBorderTop = 200;
	m_stPrintInfo.iBorderRight = 150;
	m_stPrintInfo.iBorderBottom = 200;
}

CRailDiagDoc::~CRailDiagDoc()
{
	CMainFrame		*pMainFrame;
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();

	pMainFrame = (CMainFrame*)pApp->GetMainWnd();

	pMainFrame->SetDoc(NULL);

	if (m_stPrintInfo.pftHeader != NULL)
	{
		m_stPrintInfo.pftHeader->DeleteObject();
		delete m_stPrintInfo.pftHeader;
		m_stPrintInfo.pftHeader = NULL;
	}

	if (m_stPrintInfo.pftTitle != NULL)
	{
		m_stPrintInfo.pftTitle->DeleteObject();
		delete m_stPrintInfo.pftTitle;
		m_stPrintInfo.pftTitle = NULL;
	}

	if (m_stPrintInfo.pftDescription != NULL)
	{
		m_stPrintInfo.pftDescription->DeleteObject();
		delete m_stPrintInfo.pftDescription;
		m_stPrintInfo.pftDescription = NULL;
	}

	if (m_stPrintInfo.pftDescriptionBld != NULL)
	{
		m_stPrintInfo.pftDescriptionBld->DeleteObject();
		delete m_stPrintInfo.pftDescriptionBld;
		m_stPrintInfo.pftDescriptionBld = NULL;
	}

	if (m_stPrintInfo.pftList != NULL)
	{
		m_stPrintInfo.pftList->DeleteObject();
		delete m_stPrintInfo.pftList;
		m_stPrintInfo.pftList = NULL;
	}

	if (m_stPrintInfo.pftListBld != NULL)
	{
		m_stPrintInfo.pftListBld->DeleteObject();
		delete m_stPrintInfo.pftListBld;
		m_stPrintInfo.pftListBld = NULL;
	}

	if (m_stPrintInfo.pftFooter != NULL)
	{
		m_stPrintInfo.pftFooter->DeleteObject();
		delete m_stPrintInfo.pftFooter;
		m_stPrintInfo.pftFooter = NULL;
	}

	if (m_stPrintInfo.pcbLine != NULL)
	{
		m_stPrintInfo.pcbLine->DeleteObject();
		delete m_stPrintInfo.pcbLine;
		m_stPrintInfo.pcbLine = NULL;
	}
}

BOOL CRailDiagDoc::NewData4Document(void)
{
	TCHAR			tszSendMsg[CD_MAX_MSG_LEN];
	DWORD			dwMsgChars;
	CMainFrame		*pMainFrame;
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();

	pMainFrame = (CMainFrame*)pApp->GetMainWnd();

	CD_GenerateDataHeader(tszSendMsg, &dwMsgChars, CD_MSG_STATE);
	CD_GenerateDataTail(tszSendMsg, &dwMsgChars);

	pMainFrame->ResetDataCount();
	pMainFrame->SendSerialData(tszSendMsg);

	return TRUE;
}

BOOL CRailDiagDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	TCHAR			tszSendMsg[CD_MAX_MSG_LEN];
	DWORD			dwMsgChars;
	CMainFrame		*pMainFrame;
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();

	m_iLang	= pApp->m_iLang;
	CTime ctCurrentTime = CTime::GetCurrentTime();

	m_strDiagDateTime.Format(_T("%2.2d.%2.2d.%2.2d   %2.2d:%2.2d:%2.2d"), 
		ctCurrentTime.GetDay(), ctCurrentTime.GetMonth(), ctCurrentTime.GetYear()%100, 
		ctCurrentTime.GetHour(), ctCurrentTime.GetMinute(), ctCurrentTime.GetSecond());

	if (pApp->m_eDataType == EN_DATA_TYPE_DURATION_FLEET)
	{
		m_stLocSettings.strCompany			= _T("---");
		m_stLocSettings.strLocType			= _T("---");
		m_stLocSettings.strLocNumber		= _T("---");
		m_stLocSettings.strMotorType		= _T("---");
		m_stLocSettings.strMotorNumber		= _T("---");
		m_stLocSettings.strMtuOrderNumber	= _T("---");
	}
	else
	{
		m_stLocSettings.strCompany			= pApp->m_stLocSettingsLoad.strCompany;
		m_stLocSettings.strLocType			= pApp->m_stLocSettingsLoad.strLocType;
		m_stLocSettings.strLocNumber		= pApp->m_stLocSettingsLoad.strLocNumber;
		m_stLocSettings.strMotorType		= pApp->m_stLocSettingsLoad.strMotorType;
		m_stLocSettings.strMotorNumber		= pApp->m_stLocSettingsLoad.strMotorNumber;
		m_stLocSettings.strMtuOrderNumber	= pApp->m_stLocSettingsLoad.strMtuOrderNumber;
	}
	m_eDataType = pApp->m_eDataType;

	pMainFrame = (CMainFrame*)pApp->GetMainWnd();
	pMainFrame->PostMessage(WM_USER_NEW_PROCESS_DIALOG_START);

	CD_GenerateDataHeader(tszSendMsg, &dwMsgChars, CD_MSG_STATE);
	CD_GenerateDataTail(tszSendMsg, &dwMsgChars);

	CString strDataTypeName;

	switch(pApp->m_eDataType)
	{
	case EN_DATA_TYPE_BIN:
		strDataTypeName = m_stStatBinary.strTitle;
		break;
	case EN_DATA_TYPE_ANA:
		strDataTypeName = m_stStatAnalog.strTitle;
		break;
	case EN_DATA_TYPE_ALARM:
		strDataTypeName = m_stStatAlarm.strTitle;
		break;
	case EN_DATA_TYPE_DURATION:
		strDataTypeName = m_stStatDuration.strTitle;
		break;
	case EN_DATA_TYPE_DURATION_FLEET:
		strDataTypeName = m_stStatDurationFleet.strTitle;
		break;
	case EN_DATA_TYPE_HIST:
	case EN_DATA_TYPE_HIST20:
	case EN_DATA_TYPE_HIST100:
		strDataTypeName = m_stStatHistory.strTitle;
		break;
	case EN_DATA_TYPE_HIST2:
		strDataTypeName = m_stStatHistory2.strTitle;
		break;
	}

	CString strTitle;
	if (pApp->m_iFleetMgrState == EN_FLEET_OFF)
	{
		strTitle = m_stLocSettings.strLocNumber + _T("_") + strDataTypeName + _T("_???");
	}
	else
	{
		if (pApp->m_eDataType == EN_DATA_TYPE_DURATION_FLEET)
		{
			// duration list over all locomotivs in fleet
			strTitle = pApp->m_strFleetName + _T("_") + 
				strDataTypeName + _T("_") +pApp->m_strFleetDateTime;
		}
		else
		{
			strTitle = pApp->m_strFleetName + _T("_") + m_stLocSettings.strLocNumber + _T("_") + 
				strDataTypeName + _T("_") +pApp->m_strFleetDateTime;
		}
	}
	SetTitle(strTitle);

	CString strFile;

	strFile = pApp->m_strInitialDirectory + _T("\\") + strTitle + _T(".rdi");
	SetPathName(strFile);

	pMainFrame->SetDoc(this);
	pMainFrame->ResetDataCount();
	pMainFrame->SendSerialData(tszSendMsg);

	SetCursor(pApp->LoadStandardCursor(IDC_WAIT));

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

void CRailDiagDoc::ReloadData()
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	DWORD			dwMsgChars;
	TCHAR			tszSendMsg[CD_MAX_MSG_LEN];
	CMainFrame		*pMainFrame;
	int				i;

	pMainFrame = (CMainFrame*)pApp->GetMainWnd();

	if (pMainFrame->m_pDoc != this)
	{
		pMainFrame->SetDoc(this);
	}

	// bei sprachwechsel auch statische texte austauschen
	if (m_iLang != pApp->m_iLang)
	{
		// statische Texte übernehmen
		m_stStatText				= pApp->m_stStatText;
		m_stStatBinary.strTitle		= pApp->m_stStatBinary.strTitle;
		m_stStatAnalog.strTitle		= pApp->m_stStatAnalog.strTitle;
		m_stStatAlarm.strTitle		= pApp->m_stStatAlarm.strTitle;
		m_stStatHistory.strTitle	= pApp->m_stStatHistory.strTitle;
		m_stStatHistory2.strTitle	= pApp->m_stStatHistory2.strTitle;

		// Spaltenüberschriften
		m_stStatBinary.strarrRow.RemoveAll();
		for (i = 0; i < pApp->m_stStatBinary.strarrRow.GetSize(); i++)
		{
			m_stStatBinary.strarrRow.Add(pApp->m_stStatBinary.strarrRow.GetAt(i));
			TRACE(_T("CRailDiagDoc::NewStateData %s\n"), pApp->m_stStatBinary.strarrRow.GetAt(i));
		}

		m_stStatAnalog.strarrRow.RemoveAll();
		for (i = 0; i < pApp->m_stStatAnalog.strarrRow.GetSize(); i++)
		{
			m_stStatAnalog.strarrRow.Add(pApp->m_stStatAnalog.strarrRow.GetAt(i));
		}

		m_stStatAlarm.strarrRow.RemoveAll();
		for (i = 0; i < pApp->m_stStatAlarm.strarrRow.GetSize(); i++)
		{
			m_stStatAlarm.strarrRow.Add(pApp->m_stStatAlarm.strarrRow.GetAt(i));
		}

		m_stStatHistory.strarrRow.RemoveAll();
		for (i = 0; i < pApp->m_stStatHistory.strarrRow.GetSize(); i++)
		{
			m_stStatHistory.strarrRow.Add(pApp->m_stStatHistory.strarrRow.GetAt(i));
		}

		m_stStatHistory2.strarrRow.RemoveAll();
		for (i = 0; i < pApp->m_stStatHistory2.strarrRow.GetSize(); i++)
		{
			m_stStatHistory2.strarrRow.Add(pApp->m_stStatHistory2.strarrRow.GetAt(i));
		}

		m_bNewColumnHeader = TRUE;

		// neue Titel
		switch(m_eDataType)
		{
		case EN_DATA_TYPE_BIN:
			SetTitle(m_stLocSettings.strLocNumber + _T("_") + m_stStatBinary.strTitle + _T("_???"));
			break;
		case EN_DATA_TYPE_ANA:
			SetTitle(m_stLocSettings.strLocNumber + _T("_") + m_stStatAnalog.strTitle + _T("_???"));
			break;
		case EN_DATA_TYPE_ALARM:
			SetTitle(m_stLocSettings.strLocNumber + _T("_") + m_stStatAlarm.strTitle + _T("_???"));
			break;
		case EN_DATA_TYPE_DURATION:
			SetTitle(m_stLocSettings.strLocNumber + _T("_") + m_stStatDuration.strTitle + _T("_???"));
			break;
		case EN_DATA_TYPE_DURATION_FLEET:
			SetTitle(m_stLocSettings.strLocNumber + _T("_") + m_stStatDurationFleet.strTitle + _T("_???"));
			break;
		case EN_DATA_TYPE_HIST:
		case EN_DATA_TYPE_HIST20:
		case EN_DATA_TYPE_HIST100:
			SetTitle(m_stLocSettings.strLocNumber + _T("_") + m_stStatHistory.strTitle + _T("_???"));
			break;
		case EN_DATA_TYPE_HIST2:
			SetTitle(m_stLocSettings.strLocNumber + _T("_") + m_stStatHistory2.strTitle + _T("_???"));
			break;
		}

		m_iLang = pApp->m_iLang;
	}

	
	switch(m_eDataType)
	{
	case EN_DATA_TYPE_BIN:
	case EN_DATA_TYPE_ANA:
	case EN_DATA_TYPE_ALARM:
	case EN_DATA_TYPE_DURATION:
	case EN_DATA_TYPE_DURATION_FLEET:
	case EN_DATA_TYPE_HIST:
	case EN_DATA_TYPE_HIST20:
	case EN_DATA_TYPE_HIST100:
	case EN_DATA_TYPE_HIST2:
		NewErrorHistStart();
		 
		CD_GenerateDataHeader(tszSendMsg, &dwMsgChars, CD_MSG_STATE);
		CD_GenerateDataTail(tszSendMsg, &dwMsgChars);

		pMainFrame->ResetDataCount();
		pMainFrame->SendSerialData(tszSendMsg);
		break;

	}

}


/////////////////////////////////////////////////////////////////////////////
// CRailDiagDoc serialization

void CRailDiagDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CRailDiagDoc diagnostics

#ifdef _DEBUG
void CRailDiagDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRailDiagDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRailDiagDoc commands

BOOL CRailDiagDoc::NewErrorHistData(TCHAR *ptszData, int *piLineCount)
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	BOOL	bRet = TRUE;
	CString strNew = ptszData;
	CString strLine;
	CString strLineData;
	CString	strLineCount;
	int	iAdded;
	int	iPos;
	int	iCount;
	CErrListView *pErrListView;

	iPos = strNew.Find(',', 0);
	if (iPos > 0)
	{
		strLineData = strNew.Right(strNew.GetLength() - (iPos+1));
		strLineCount = strNew.Left(iPos);

		iCount = (int)_tcstol(strLineCount, NULL, 10);

		if (iCount <= 0) // um alte Version ohne Zeilennummer richtig zu unterstützen
		{
			strLineData = strNew;
			iCount = *piLineCount;
		}

		*piLineCount = iCount;

		strLineCount.Format(_T("%4.4d,"), iCount);
		strLine = strLineCount + strLineData;

		if (m_eDataType == EN_DATA_TYPE_DURATION_FLEET)
		{
			iCount = m_iLineStart + m_strarrListData.GetSize() + 1; // Zeile immer fix hinzufügen !!!
			strLineCount.Format(_T("%4.4d,"), iCount);
			iPos = strLineData.Find(',', 0);
			strLine = strLineCount + strLineData.Left(iPos) + 
				_T(",") + pApp->m_stLocSettingsLoad.strLocNumber + 
				_T(",") + pApp->m_stLocSettingsLoad.strMotorNumber + 
				strLineData.Right(strLineData.GetLength() - iPos);
		}

		if (iCount - 1 == m_iLineStart + m_strarrListData.GetSize())
		{
			iAdded = m_strarrListData.Add(strLine);
			TRACE(_T("String added at %d\n"), iAdded);
		} 
		else if (iCount - 1 - m_iLineStart<  m_strarrListData.GetSize())
		{
			m_strarrListData.SetAt(iCount - 1 - m_iLineStart, strLine);
			TRACE(_T("String updated at %d\n"), iCount - 1 - m_iLineStart);
			pErrListView = (CErrListView*)GetErrListView();
			pErrListView->m_iUpdate = iCount - 1 - m_iLineStart;	
		}
		else
		{
			for (int i = m_iLineStart + m_strarrListData.GetSize(); i < iCount - 1; i++)
			{
				strLineCount.Format(_T("%4.4d,"), i);
				iAdded = m_strarrListData.Add(strLineCount);
				TRACE(_T("Empty line added at %d\n"), iAdded);
			}
			iAdded = m_strarrListData.Add(strLine);
			TRACE(_T("String added at %d\n"), iAdded);
		}
	}
	else
	{
		CMainFrame		*pMainFrame;

		pMainFrame = (CMainFrame*)pApp->GetMainWnd();

		pMainFrame->PostMessage(WM_USER_RELOAD_DATA);
		bRet = FALSE;
	}
	
//	UpdateAllViews(NULL);
	return bRet;
}

void CRailDiagDoc::NewErrorHistStart()
{
	m_strarrListData.RemoveAll();
	UpdateAllViews(NULL);
}

void CRailDiagDoc::NewErrorHistUpdate()
{
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = GetNextView(pos);

		if (pView->IsKindOf(RUNTIME_CLASS(CErrListView)))
		{
			pView->UpdateWindow();
		}
	}   
}

CView * CRailDiagDoc::GetStaticView()
{
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = GetNextView(pos);

		if (pView->IsKindOf(RUNTIME_CLASS(CStateView)))
		{
			return pView;
		}
	}   
	return NULL;
}


CView * CRailDiagDoc::GetErrListView()
{
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = GetNextView(pos);

		if (pView->IsKindOf(RUNTIME_CLASS(CErrListView)))
		{
			return pView;
		}
	}   
	return NULL;
}


void CRailDiagDoc::NewStateData(TCHAR *ptszData)
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CString			strNew;
	CStringArray	astrNew;
	CUtil			Util;

	strNew = ptszData;
	Util.SeparateComma(strNew, astrNew);

	if (pApp->m_eDataType == EN_DATA_TYPE_DURATION_FLEET)
	{
		m_strDurationCount = _T("---");
		m_strTerminalDateTime = _T("---");
		m_strMode = _T("");
	}
	else
	{
		if (astrNew.GetSize() >= 3)
		{
			m_strDurationCount = astrNew.GetAt(0);
			m_strTerminalDateTime = astrNew.GetAt(1);
			m_strMode = astrNew.GetAt(2);
		}
	}

	CTime ctCurrentTime = CTime::GetCurrentTime();

	m_strDiagDateTime.Format(_T("%2.2d.%2.2d.%2.2d   %2.2d:%2.2d:%2.2d"), 
		ctCurrentTime.GetDay(), ctCurrentTime.GetMonth(), ctCurrentTime.GetYear()%100, 
		ctCurrentTime.GetHour(), ctCurrentTime.GetMinute(), ctCurrentTime.GetSecond());

	if (pApp->m_eDataType == EN_DATA_TYPE_DURATION_FLEET)
	{
		m_stLocSettings.strCompany			= _T("---");
		m_stLocSettings.strLocType			= _T("---");
		m_stLocSettings.strLocNumber		= _T("---");
		m_stLocSettings.strMotorType		= _T("---");
		m_stLocSettings.strMotorNumber		= _T("---");
		m_stLocSettings.strMtuOrderNumber	= _T("---");
	}
	else
	{
		m_stLocSettings.strCompany			= pApp->m_stLocSettingsLoad.strCompany;
		m_stLocSettings.strLocType			= pApp->m_stLocSettingsLoad.strLocType;
		m_stLocSettings.strLocNumber		= pApp->m_stLocSettingsLoad.strLocNumber;
		m_stLocSettings.strMotorType		= pApp->m_stLocSettingsLoad.strMotorType;
		m_stLocSettings.strMotorNumber		= pApp->m_stLocSettingsLoad.strMotorNumber;
		m_stLocSettings.strMtuOrderNumber	= pApp->m_stLocSettingsLoad.strMtuOrderNumber;
	}
}

BOOL CRailDiagDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	BOOL		bRet = TRUE;
	CStdioFile	file;
	CWaitCursor	wait;

    if (file.Open(lpszPathName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
	{
		bRet = SaveDocData(file);
		file.Close();
	}
	else
		bRet = FALSE;

	if (!bRet)
	{
		CString	csMsg;

		csMsg.Format(_T("Save file failed: %s"), lpszPathName);
		AfxMessageBox(csMsg);
	}

	return bRet;
}

BOOL CRailDiagDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	BOOL		bRet = TRUE;
	CStdioFile	file;
	CWaitCursor	wait;

    if (file.Open(lpszPathName, CFile::modeRead | CFile::typeBinary))
	{
		bRet = ReadDocData(file);
		file.Close();

	}
	else
		bRet = FALSE;

	if (!bRet)
	{
		CString	csMsg;

		csMsg.Format(_T("Open file failed: %s"), lpszPathName);
		AfxMessageBox(csMsg);
	}
	
	return bRet;
}

BOOL CRailDiagDoc::SaveDocData(CStdioFile &file)
{
	BOOL	bRet = TRUE;
	CString	buf;

	file.WriteString(_T("[GLOBAL]\r\n"));
    buf.Format(_T("FILE_VERSION = 0001\r\n"));
    file.WriteString(buf);
    buf.Format(_T("LANG = %2.2d\r\n"), m_iLang);
    file.WriteString(buf);
	switch(m_eDataType)
	{
	case EN_DATA_TYPE_BIN:
		buf.Format(_T("DATA_TYPE = BINARY\r\n"));
		break;
	case EN_DATA_TYPE_ANA:
		buf.Format(_T("DATA_TYPE = ANALOG\r\n"));
		break;
	case EN_DATA_TYPE_ALARM:
		buf.Format(_T("DATA_TYPE = ALARM\r\n"));
		break;
	case EN_DATA_TYPE_DURATION:
		buf.Format(_T("DATA_TYPE = DURATION\r\n"));
		break;
	case EN_DATA_TYPE_DURATION_FLEET:
		buf.Format(_T("DATA_TYPE = DURATION_FLEET\r\n"));
		break;
	case EN_DATA_TYPE_HIST:
		buf.Format(_T("DATA_TYPE = HISTORY\r\n"));
		break;
	case EN_DATA_TYPE_HIST20:
		buf.Format(_T("DATA_TYPE = HIST20\r\n"));
		break;
	case EN_DATA_TYPE_HIST100:
		buf.Format(_T("DATA_TYPE = HIST100\r\n"));
		break;
	case EN_DATA_TYPE_HIST2:
		buf.Format(_T("DATA_TYPE = HIST_2\r\n"));
		break;
	}
    file.WriteString(buf);
    file.WriteString(_T("\r\n"));

	// beschreibung ausgeben
	file.WriteString(_T("[DESCRIPTION]\r\n"));
    buf.Format(_T("COMPANY = %s\r\n"), m_stLocSettings.strCompany);
    file.WriteString(buf);
    buf.Format(_T("LOC_TYPE = %s\r\n"), m_stLocSettings.strLocType);
    file.WriteString(buf);
    buf.Format(_T("LOC_NUMBER = %s\r\n"), m_stLocSettings.strLocNumber);
    file.WriteString(buf);
    buf.Format(_T("MOTOR_TYPE = %s\r\n"), m_stLocSettings.strMotorType);
    file.WriteString(buf);
    buf.Format(_T("MOTOR_NUMBER = %s\r\n"), m_stLocSettings.strMotorNumber);
    file.WriteString(buf);
    buf.Format(_T("MTU_ORDER_NUMBER = %s\r\n"), m_stLocSettings.strMtuOrderNumber);
    file.WriteString(buf);
    buf.Format(_T("DURATION_COUNT = %s\r\n"), m_strDurationCount);
    file.WriteString(buf);
    buf.Format(_T("DIAG_DATE_TIME = %s\r\n"), m_strDiagDateTime);
    file.WriteString(buf);
    buf.Format(_T("TERMINAL_DATE_TIME = %s\r\n"), m_strTerminalDateTime);
    file.WriteString(buf);
    buf.Format(_T("MODE = %s\r\n"), m_strMode);
    file.WriteString(buf);
    file.WriteString(_T("\r\n"));


	// datenzeilen ausgeben
	file.WriteString(_T("[DATA]\r\n"));
	buf.Format(_T("LINES = %4.4d\r\n"), m_strarrListData.GetSize());
	file.WriteString(buf);

	for (int i = 0; i < m_strarrListData.GetSize(); i++)
	{
		buf.Format(_T("DATA_LINE = %s\r\n"), m_strarrListData[i]);
		file.WriteString(buf);
	}

    file.WriteString(_T("\r\n"));

	return bRet;
}

BOOL CRailDiagDoc::ReadDocData(CStdioFile &file)
{
	BOOL		bRet = TRUE;
	CString		key, param;
	int			i, j;

    if (FindSection(file, _T("GLOBAL")))
    {

		while (bRet && TranslateNextLine(file, key, param))
        {
			if (key == _T("FILE_VERSION"))
			{
				if (param != _T("0001"))
				{
					bRet = FALSE;
				}
			}
			else if (key == _T("LANG"))
			{
				m_iLang = _tcstol(param, NULL, 10);
			}
			else if (key == _T("DATA_TYPE"))
			{
				if (param == _T("BINARY"))
				{
					m_eDataType = EN_DATA_TYPE_BIN;
				}
				else if (param == _T("ANALOG"))
				{
					m_eDataType = EN_DATA_TYPE_ANA;
				}
				else if (param == _T("ALARM"))
				{
					m_eDataType = EN_DATA_TYPE_ALARM;
				}
				else if (param == _T("DURATION"))
				{
					m_eDataType = EN_DATA_TYPE_DURATION;
				}
				else if (param == _T("DURATION_FLEET"))
				{
					m_eDataType = EN_DATA_TYPE_DURATION_FLEET;
				}
				else if (param == _T("HISTORY"))
				{
					m_eDataType = EN_DATA_TYPE_HIST;
				}
				else if (param == _T("HIST20"))
				{
					m_eDataType = EN_DATA_TYPE_HIST20;
				}
				else if (param == _T("HIST100"))
				{
					m_eDataType = EN_DATA_TYPE_HIST100;
				}
				else if (param == _T("HIST_2"))
				{
					m_eDataType = EN_DATA_TYPE_HIST2;
				}
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet && FindSection(file, _T("DESCRIPTION")))
    {

		while (bRet && TranslateNextLine(file, key, param))
        {
			if (key == _T("COMPANY"))
			{
				m_stLocSettings.strCompany = param;
			}
			else if (key == _T("LOC_TYPE"))
			{
				m_stLocSettings.strLocType = param;
			}
			else if (key == _T("LOC_NUMBER"))
			{
				m_stLocSettings.strLocNumber = param;
			}
			else if (key == _T("MOTOR_TYPE"))
			{
				m_stLocSettings.strMotorType = param;
			}
			else if (key == _T("MOTOR_NUMBER"))
			{
				m_stLocSettings.strMotorNumber = param;
			}
			else if (key == _T("MTU_ORDER_NUMBER"))
			{
				m_stLocSettings.strMtuOrderNumber = param;
			}
			else if (key == _T("DURATION_COUNT"))
			{
				m_strDurationCount = param;
			}
			else if (key == _T("DIAG_DATE_TIME"))
			{
				m_strDiagDateTime = param;
			}
			else if (key == _T("TERMINAL_DATE_TIME"))
			{
				m_strTerminalDateTime = param;
			}
			else if (key == _T("MODE"))
			{
				m_strMode = param;
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet && FindSection(file, _T("DATA")))
    {
		i = 0;
		while (bRet && TranslateNextLine(file, key, param))
        {
			if (key == _T("LINES"))
			{
				j = _tcstol(param, NULL, 10);
			}
			else if (key == _T("DATA_LINE"))
			{
				i++;
				m_strarrListData.Add(param);
			}
		}

		if (i != j)
		{
			bRet = FALSE;
		}
	}
	else
	{
		bRet = FALSE;
	}

	return bRet;
}

void CRailDiagDoc::DoBeginPrinting(CDC *pDC, CPrintInfo *pInfo)
{
	TRACE(_T("CRailDiagDoc::DoBeginPrinting\n"));
	m_stPrintInfo.bPrinting = TRUE;
	m_stPrintInfo.bBeginPrinting = TRUE;
	m_stPrintInfo.uiDataLine = 0;
	if (m_stPrintInfo.uiarrDataLine.GetSize() < 1)
	{
		m_stPrintInfo.uiarrDataLine.Add(m_stPrintInfo.uiDataLine);
	}

	PrCreateFonts(pDC);
	PrHeader(pDC, pInfo);
	PrFooter(pDC, pInfo);
	PrTitle(pDC, pInfo);

	if (pInfo != NULL && pInfo->m_nCurPage == 1)
	{
		PrDescription(pDC, pInfo);
	}
	else
	{
		m_stPrintInfo.rcDescription.SetRect(0,0,0,0);
	}

	PrList(pDC, pInfo);

	m_stPrintInfo.bBeginPrinting = FALSE;
}

void CRailDiagDoc::DoPrepareDC(CDC *pDC, CPrintInfo *pInfo)
{
	if (pInfo != NULL)
	{
		TRACE(_T("CRailDiagDoc::DoPrepareDC Page %d\n"), pInfo->m_nCurPage);
		if (m_stPrintInfo.uiarrDataLine.GetSize() - 1 < (int)pInfo->m_nCurPage - 1)
		{
			m_stPrintInfo.uiarrDataLine.Add(m_stPrintInfo.uiDataLine);
		}
		else
		{
			m_stPrintInfo.uiDataLine = m_stPrintInfo.uiarrDataLine.GetAt(pInfo->m_nCurPage - 1);
		}

		if (pInfo->m_nCurPage >= 2)
		{
			if (m_stPrintInfo.uiDataLine >= (UINT)m_strarrListData.GetSize())
			{
				TRACE(_T("CRailDiagDoc::DoPrepareDC end printing\n"));
				pInfo->m_bContinuePrinting = FALSE;
				pInfo->SetMaxPage(pInfo->m_nCurPage - 1);
			}
		}
	}
}

void CRailDiagDoc::DoPrint(CDC *pDC, CPrintInfo *pInfo)
{
	TRACE(_T("CRailDiagDoc::DoPrint\n"));
	PrCreateFonts(pDC);
	PrHeader(pDC, pInfo);
	PrFooter(pDC, pInfo);
	PrTitle(pDC, pInfo);

	if (pInfo != NULL && pInfo->m_nCurPage == 1)
	{
		PrDescription(pDC, pInfo);
	}

	PrList(pDC, pInfo);
}

void CRailDiagDoc::DoEndPrinting(CDC *pDC, CPrintInfo *pInfo)
{
	m_stPrintInfo.uiarrDataLine.RemoveAll();
	m_stPrintInfo.bPrinting = FALSE;
}


void CRailDiagDoc::PrCreateFonts(CDC *pDC)
{
	LOGFONT lf;

	if (m_stPrintInfo.pftHeader == NULL)
	{

		m_stPrintInfo.pftHeader = new CFont;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = 110;
		_tcscpy(lf.lfFaceName, _T("Arial"));
		if (!m_stPrintInfo.pftHeader->CreatePointFontIndirect(&lf, pDC))
		{
			delete m_stPrintInfo.pftHeader;
			m_stPrintInfo.pftHeader = NULL;
		}
	}

	if (m_stPrintInfo.pftTitle == NULL)
	{
		m_stPrintInfo.pftTitle = new CFont;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = 160;
		lf.lfWeight = FW_BOLD;
		_tcscpy(lf.lfFaceName, _T("Arial"));
		if (!m_stPrintInfo.pftTitle->CreatePointFontIndirect(&lf, pDC))
		{
			delete m_stPrintInfo.pftTitle;
			m_stPrintInfo.pftTitle = NULL;
		}
	}

	if (m_stPrintInfo.pftDescription == NULL)
	{
		m_stPrintInfo.pftDescription = new CFont;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = 100;
		_tcscpy(lf.lfFaceName, _T("Arial"));
		if (!m_stPrintInfo.pftDescription->CreatePointFontIndirect(&lf, pDC))
		{
			delete m_stPrintInfo.pftDescription;
			m_stPrintInfo.pftDescription = NULL;
		}
	}

	if (m_stPrintInfo.pftDescriptionBld == NULL)
	{
		m_stPrintInfo.pftDescriptionBld = new CFont;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = 100;
		lf.lfWeight = FW_BOLD;
		_tcscpy(lf.lfFaceName, _T("Arial"));
		if (!m_stPrintInfo.pftDescriptionBld->CreatePointFontIndirect(&lf, pDC))
		{
			delete m_stPrintInfo.pftDescriptionBld;
			m_stPrintInfo.pftDescriptionBld = NULL;
		}
	}

	if (m_stPrintInfo.pftList == NULL)
	{
		m_stPrintInfo.pftList = new CFont;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = 80;
		_tcscpy(lf.lfFaceName, _T("Arial"));
		if (!m_stPrintInfo.pftList->CreatePointFontIndirect(&lf, pDC))
		{
			delete m_stPrintInfo.pftList;
			m_stPrintInfo.pftList = NULL;
		}
	}

	if (m_stPrintInfo.pftListBld == NULL)
	{
		m_stPrintInfo.pftListBld = new CFont;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = 90;
		lf.lfWeight = FW_BOLD;
		_tcscpy(lf.lfFaceName, _T("Arial"));
		if (!m_stPrintInfo.pftListBld->CreatePointFontIndirect(&lf, pDC))
		{
			delete m_stPrintInfo.pftListBld;
			m_stPrintInfo.pftListBld = NULL;
		}
	}

	if (m_stPrintInfo.pftFooter == NULL)
	{
		m_stPrintInfo.pftFooter = new CFont;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = 100;
		_tcscpy(lf.lfFaceName, _T("Arial"));
		if (!m_stPrintInfo.pftFooter->CreatePointFontIndirect(&lf, pDC))
		{
			delete m_stPrintInfo.pftFooter;
			m_stPrintInfo.pftFooter = NULL;
		}
	}

	if (m_stPrintInfo.pcbLine == NULL)
	{
		m_stPrintInfo.pcbLine = new CBrush;
		m_stPrintInfo.pcbLine->CreateSolidBrush(RGB(0,0,0));
	}
}

void CRailDiagDoc::PrHeader(CDC *pDC, CPrintInfo *pInfo)
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CFont			*pftOld;
	CBrush			*pcbOld;
	CRect			rcBox;
	CRect			rcText;
	CString			str;
	int				iHeight;
	int				iHeightTmp1, iHeightTmp2;
	int				iBmpWidth;
	CBitmap			cBmp;

	if (!m_stPrintInfo.bBeginPrinting)
	{
		rcBox.left = pInfo->m_rectDraw.left + m_stPrintInfo.iBorderLeft;
		rcBox.top  = pInfo->m_rectDraw.top + m_stPrintInfo.iBorderTop;
		rcBox.right = pInfo->m_rectDraw.right - m_stPrintInfo.iBorderRight;
	}
	else
	{
		rcBox.top  = 0 + m_stPrintInfo.iBorderTop;
	}
	iHeight = 0;

	pftOld = pDC->SelectObject(m_stPrintInfo.pftHeader);
	pcbOld = pDC->SelectObject(m_stPrintInfo.pcbLine);

	rcText.SetRectEmpty();
	str = pApp->m_strPrintHeader;
	pDC->DrawText(str, &rcText, DT_SINGLELINE | DT_LEFT | DT_CALCRECT);
	rcText.OffsetRect(rcBox.TopLeft()+CPoint(0,iHeight));

	if (!m_stPrintInfo.bBeginPrinting)
	{
		pDC->DrawText(str, &rcText, DT_SINGLELINE | DT_LEFT);
	}

	iHeightTmp1 = rcText.Height();
	iHeightTmp1 += 10;

	iHeightTmp2 = 0;

	// logo
	CDC		dcMem;
	HBITMAP	hBmp;
	BOOL	bStat;

	hBmp = (HBITMAP)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_MTU_MONO), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);

	CBitmap *pBmp = CBitmap::FromHandle(hBmp);	// MTU Bitmap
	BITMAP	bm;
	pBmp->GetBitmap(&bm);

	iHeightTmp2 = bm.bmHeight;
	iBmpWidth = bm.bmWidth;

	if (!m_stPrintInfo.bBeginPrinting)
	{
		dcMem.CreateCompatibleDC(pDC);
		CBitmap *pBmpOld;
		pBmpOld = dcMem.SelectObject(pBmp);

		bStat = pDC->StretchBlt(rcBox.right - bm.bmWidth, rcBox.top, bm.bmWidth, bm.bmHeight, 
			&dcMem, 0,0, bm.bmWidth, bm.bmHeight, SRCCOPY);

		
		dcMem.SelectObject(pBmpOld);
		dcMem.DeleteDC();
	}

	pBmp->DeleteObject();

	iHeight += max(iHeightTmp1, iHeightTmp2);

	// line
	if (!m_stPrintInfo.bBeginPrinting)
	{
		pDC->MoveTo(rcBox.left, rcBox.top+iHeight);
		pDC->LineTo(rcBox.right - iBmpWidth - 10, rcBox.top+iHeight);
	}
	iHeight += 50;

	rcBox.bottom = rcBox.top + iHeight;

	if (m_stPrintInfo.bBeginPrinting)
	{
		m_stPrintInfo.rcHeader = rcBox;
	}

	pDC->SelectObject(pftOld);
	pDC->SelectObject(pcbOld);
}

void CRailDiagDoc::PrTitle(CDC *pDC, CPrintInfo *pInfo)
{
	CFont			*pftOld;
	CRect			rcBox;
	CRect			rcText;
	CString			str;
	int				iHeight;

	if (!m_stPrintInfo.bBeginPrinting)
	{
		rcBox.left = pInfo->m_rectDraw.left + m_stPrintInfo.iBorderLeft;
		rcBox.right = pInfo->m_rectDraw.right - m_stPrintInfo.iBorderRight;
		rcBox.top  = pInfo->m_rectDraw.top + m_stPrintInfo.iBorderTop +
						m_stPrintInfo.rcHeader.Height();
	}
	else
	{
		rcBox.top  = 0 + m_stPrintInfo.iBorderTop;
	}
	iHeight = 50;

	pftOld = pDC->SelectObject(m_stPrintInfo.pftTitle);

	rcText.SetRectEmpty();

	switch(m_eDataType)
	{
	case EN_DATA_TYPE_BIN:
		str = m_stStatBinary.strTitle;
		break;
	case EN_DATA_TYPE_ANA:
		str = m_stStatAnalog.strTitle;
		break;
	case EN_DATA_TYPE_ALARM:
		str = m_stStatAlarm.strTitle;
		break;
	case EN_DATA_TYPE_DURATION:
		str = m_stStatDuration.strTitle;
		break;
	case EN_DATA_TYPE_DURATION_FLEET:
		str = m_stStatDurationFleet.strTitle;
		break;
	case EN_DATA_TYPE_HIST:
	case EN_DATA_TYPE_HIST20:
	case EN_DATA_TYPE_HIST100:
		str = m_stStatHistory.strTitle;
		break;
	case EN_DATA_TYPE_HIST2:
		str = m_stStatHistory2.strTitle;
		break;
	}

	pDC->DrawText(str, &rcText, DT_SINGLELINE | DT_LEFT | DT_CALCRECT);
	rcText.OffsetRect(rcBox.TopLeft()+CPoint(0,iHeight));

	if (!m_stPrintInfo.bBeginPrinting)
	{
		pDC->DrawText(str, &rcText, DT_SINGLELINE | DT_LEFT);
	}
	iHeight += rcText.Height();
	iHeight += 50;

	rcBox.bottom = rcBox.top + iHeight;

	if (m_stPrintInfo.bBeginPrinting)
	{
		m_stPrintInfo.rcTitle = rcBox;
	}

	pDC->SelectObject(pftOld);
}

void CRailDiagDoc::PrDescription(CDC *pDC, CPrintInfo *pInfo)
{
	CFont			*pftOld;
	CRect			rcBox;
	CRect			rcText;
	CString			str;
	int				iHeight;
	int				iY;
	UINT			uiFormat;
	int				iLineGap = 30;

	if (!m_stPrintInfo.bBeginPrinting)
	{
		rcBox.left = pInfo->m_rectDraw.left + m_stPrintInfo.iBorderLeft;
		rcBox.right = pInfo->m_rectDraw.right - m_stPrintInfo.iBorderRight;
		rcBox.top  = pInfo->m_rectDraw.top + m_stPrintInfo.iBorderTop +
						m_stPrintInfo.rcHeader.Height() +
						m_stPrintInfo.rcTitle.Height();
	}
	else
	{
		rcBox.top  = 0 + m_stPrintInfo.iBorderTop;
	}
	iHeight = iLineGap;

	pftOld = pDC->SelectObject(m_stPrintInfo.pftDescriptionBld);

/*
	// MTU Kopf
	rcText.SetRectEmpty();
	str = m_stStatText.strHeadNameShort;
	pDC->DrawText(str, &rcText, DT_SINGLELINE | DT_CENTER | DT_CALCRECT);
	rcText.OffsetRect(rcBox.TopLeft()+CPoint(0,iHeight));
	rcText.right = rcBox.right;
	if (!m_stPrintInfo.bBeginPrinting)
	{
		pDC->DrawText(str, &rcText, DT_SINGLELINE | DT_CENTER);
	}
	iHeight += rcText.Height();
	iHeight += 5;

	rcText.SetRectEmpty();
	str = m_stStatText.strHeadName;
	pDC->DrawText(str, &rcText, DT_SINGLELINE | DT_CENTER | DT_CALCRECT);
	rcText.OffsetRect(rcBox.TopLeft()+CPoint(0,iHeight));
	rcText.right = rcBox.right;
	if (!m_stPrintInfo.bBeginPrinting)
	{
		pDC->DrawText(str, &rcText, DT_SINGLELINE | DT_CENTER);
	}
	iHeight += rcText.Height();
	iHeight += 5;

	rcText.SetRectEmpty();
	str = m_stStatText.strHeadCity;
	pDC->DrawText(str, &rcText, DT_SINGLELINE | DT_CENTER | DT_CALCRECT);
	rcText.OffsetRect(rcBox.TopLeft()+CPoint(0,iHeight));
	rcText.right = rcBox.right;
	if (!m_stPrintInfo.bBeginPrinting)
	{
		pDC->DrawText(str, &rcText, DT_SINGLELINE | DT_CENTER);
	}
	iHeight += rcText.Height();
	iHeight += iLineGap;
*/

	// Eisenbanhgesellschaft
	pftOld = pDC->SelectObject(m_stPrintInfo.pftDescriptionBld);
	uiFormat = DT_SINGLELINE | DT_LEFT;
	iY = PrText(pDC, rcBox.left, rcBox.right, rcBox.top+iHeight, 0, uiFormat, m_stStatText.strCompany);
	pftOld = pDC->SelectObject(m_stPrintInfo.pftDescription);
	PrText(pDC, rcBox.left, rcBox.right, rcBox.top+iHeight, 30, uiFormat, m_stLocSettings.strCompany);
	iHeight += iY;
	iHeight += iLineGap;

	// Loktyp, LockNr
	pftOld = pDC->SelectObject(m_stPrintInfo.pftDescriptionBld);
	uiFormat = DT_SINGLELINE | DT_LEFT;
	iY = PrText(pDC, rcBox.left, rcBox.right, rcBox.top+iHeight, 0, uiFormat, m_stStatText.strLocType);
	PrText(pDC, rcBox.left, rcBox.right, rcBox.top+iHeight, 60, uiFormat, m_stStatText.strLocNumber);
	pftOld = pDC->SelectObject(m_stPrintInfo.pftDescription);
	PrText(pDC, rcBox.left, rcBox.right, rcBox.top+iHeight, 30, uiFormat, m_stLocSettings.strLocType);
	PrText(pDC, rcBox.left, rcBox.right, rcBox.top+iHeight, 80, uiFormat, m_stLocSettings.strLocNumber);
	iHeight += iY;
	iHeight += iLineGap;

	// Motortyp, Motornummer
	pftOld = pDC->SelectObject(m_stPrintInfo.pftDescriptionBld);
	uiFormat = DT_SINGLELINE | DT_LEFT;
	iY = PrText(pDC, rcBox.left, rcBox.right, rcBox.top+iHeight, 0, uiFormat, m_stStatText.strMotorType);
	PrText(pDC, rcBox.left, rcBox.right, rcBox.top+iHeight, 60, uiFormat, m_stStatText.strMotorNumber);
	pftOld = pDC->SelectObject(m_stPrintInfo.pftDescription);
	PrText(pDC, rcBox.left, rcBox.right, rcBox.top+iHeight, 30, uiFormat, m_stLocSettings.strMotorType);
	PrText(pDC, rcBox.left, rcBox.right, rcBox.top+iHeight, 80, uiFormat, m_stLocSettings.strMotorNumber);
	iHeight += iY;
	iHeight += iLineGap;

	// Laptop Datum-Uhrzeit, Terminal Datum-Uhrzeit
	pftOld = pDC->SelectObject(m_stPrintInfo.pftDescriptionBld);
	uiFormat = DT_SINGLELINE | DT_LEFT;
	iY = PrText(pDC, rcBox.left, rcBox.right, rcBox.top+iHeight, 0, uiFormat, m_stStatText.strDiag);
	PrText(pDC, rcBox.left, rcBox.right, rcBox.top+iHeight, 60, uiFormat, m_stStatText.strTerminal);
	pftOld = pDC->SelectObject(m_stPrintInfo.pftDescription);
	PrText(pDC, rcBox.left, rcBox.right, rcBox.top+iHeight, 30, uiFormat, m_strDiagDateTime);
	PrText(pDC, rcBox.left, rcBox.right, rcBox.top+iHeight, 80, uiFormat, m_strTerminalDateTime);
	iHeight += iY;
	iHeight += iLineGap;

	// Betriebsstundenzähler
	pftOld = pDC->SelectObject(m_stPrintInfo.pftDescriptionBld);
	uiFormat = DT_SINGLELINE | DT_LEFT;
	iY = PrText(pDC, rcBox.left, rcBox.right, rcBox.top+iHeight, 0, uiFormat, m_stStatText.strDurationCount);
	pftOld = pDC->SelectObject(m_stPrintInfo.pftDescription);
	PrText(pDC, rcBox.left, rcBox.right, rcBox.top+iHeight, 30, uiFormat, m_strDurationCount);
	iHeight += iY;
	iHeight += 50 * 2;

	rcBox.bottom = rcBox.top + iHeight;

	if (m_stPrintInfo.bBeginPrinting)
	{
		m_stPrintInfo.rcDescription = rcBox;
	}

	pDC->SelectObject(pftOld);
}

void CRailDiagDoc::PrList(CDC *pDC, CPrintInfo *pInfo)
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CFont			*pftOld;
	CRect			rcBox;
	CRect			rcText;
	CString			str;
	int				iHeight;
	int				iY;
	UINT			uiFormat;
	CUIntArray		uiArr;
	CUIntArray		*puiArray;
	int				iLineGap = 10;

	if (!m_stPrintInfo.bBeginPrinting)
	{
		rcBox.left = pInfo->m_rectDraw.left + m_stPrintInfo.iBorderLeft;
		rcBox.right = pInfo->m_rectDraw.right - m_stPrintInfo.iBorderRight;
		if (pInfo->m_nCurPage <= 1)
		{
			rcBox.top  = pInfo->m_rectDraw.top + m_stPrintInfo.iBorderTop +
							m_stPrintInfo.rcHeader.Height() +
							m_stPrintInfo.rcTitle.Height() +
							m_stPrintInfo.rcDescription.Height();
		}
		else
		{
			rcBox.top  = pInfo->m_rectDraw.top + m_stPrintInfo.iBorderTop +
							m_stPrintInfo.rcHeader.Height() +
							m_stPrintInfo.rcTitle.Height();
		}
	}
	else
	{
		rcBox.top  = 0 + m_stPrintInfo.iBorderTop;
	}
	iHeight = 80;

	CStringArray	*pStrArr;
	switch(m_eDataType)
	{
	case EN_DATA_TYPE_BIN:
		puiArray = &pApp->m_uiarrPrintPosBinaryList;
		pStrArr = &m_stStatBinary.strarrRow;
		break;
	case EN_DATA_TYPE_ANA:
		puiArray = &pApp->m_uiarrPrintPosAnalogList;
		pStrArr = &m_stStatAnalog.strarrRow;
		break;
	case EN_DATA_TYPE_ALARM:
		puiArray = &pApp->m_uiarrPrintPosAlarmList;
		pStrArr = &m_stStatAlarm.strarrRow;
		break;
	case EN_DATA_TYPE_DURATION:
		puiArray = &pApp->m_uiarrPrintPosDurationList;
		pStrArr = &m_stStatDuration.strarrRow;
		break;
	case EN_DATA_TYPE_DURATION_FLEET:
		puiArray = &pApp->m_uiarrPrintPosDurationFleetList;
		pStrArr = &m_stStatDurationFleet.strarrRow;
		break;
	case EN_DATA_TYPE_HIST:
	case EN_DATA_TYPE_HIST20:
	case EN_DATA_TYPE_HIST100:
		puiArray = &pApp->m_uiarrPrintPosHistoryList;
		pStrArr = &m_stStatHistory.strarrRow;
		break;
	case EN_DATA_TYPE_HIST2:
		puiArray = &pApp->m_uiarrPrintPosHistory2List;
		pStrArr = &m_stStatHistory2.strarrRow;
		break;
	}

	pftOld = pDC->SelectObject(m_stPrintInfo.pftListBld);

	// Listenüberschrift
	uiFormat = DT_SINGLELINE | DT_LEFT;
	int i;
	for (i = 0; i < pStrArr->GetSize(); i++)
	{
		iY = PrText(pDC, rcBox.left, rcBox.right, rcBox.top+iHeight, puiArray->GetAt(i), 
					uiFormat, pStrArr->GetAt(i));
	}
	iHeight += iY;
	iHeight += iLineGap / 2;

	// line
	if (!m_stPrintInfo.bBeginPrinting)
	{
		pDC->MoveTo(rcBox.left, rcBox.top+iHeight);
		pDC->LineTo(rcBox.right , rcBox.top+iHeight);
	}
	iHeight += iLineGap;

	// Listenzeilen ausgeben
	BOOL			bEnd = FALSE;
	int				iLineHeight = 0;
	CStringArray	strarrElem;
	CUtil			Util;
	UINT			j;

	pftOld = pDC->SelectObject(m_stPrintInfo.pftList);
	for (j = m_stPrintInfo.uiDataLine; !bEnd && j < (UINT)m_strarrListData.GetSize(); j++)
	{
		if (rcBox.top + iHeight + iLineHeight < 
			pInfo->m_rectDraw.bottom - m_stPrintInfo.iBorderTop - m_stPrintInfo.rcFooter.Height())
		{
			m_stPrintInfo.uiDataLine = j + 1;
			strarrElem.RemoveAll();
			Util.SeparateComma(m_strarrListData.GetAt(j), strarrElem);

			for (i = 0; i < pStrArr->GetSize() && i+1 < strarrElem.GetSize(); i++)
			{
				if (i == 0)
				{
					iLineHeight = PrText(pDC, rcBox.left, rcBox.right, rcBox.top+iHeight, puiArray->GetAt(i), 
						uiFormat, strarrElem.GetAt(i));
				}
				else
				{
					iLineHeight = PrText(pDC, rcBox.left, rcBox.right, rcBox.top+iHeight, puiArray->GetAt(i), 
						uiFormat, strarrElem.GetAt(i+1));
				}
			}
			iHeight += iLineHeight+iLineGap;
		}
		else
		{
			bEnd = TRUE;
		}
	}

	if (pInfo->m_nCurPage > (UINT)m_stPrintInfo.uiarrDataLine.GetSize())
	{
		m_stPrintInfo.uiarrDataLine.Add(m_stPrintInfo.uiDataLine);
	}

	rcBox.bottom = rcBox.top + iHeight;

	pDC->SelectObject(pftOld);
}

void CRailDiagDoc::PrFooter(CDC *pDC, CPrintInfo *pInfo)
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CFont			*pftOld;
	CBrush			*pcbOld;
	CRect			rcBox;
	CRect			rcText;
	CString			str;
	int				iHeight;

	if (!m_stPrintInfo.bBeginPrinting)
	{
		rcBox.left = pInfo->m_rectDraw.left + m_stPrintInfo.iBorderLeft;
		rcBox.bottom  = pInfo->m_rectDraw.bottom - m_stPrintInfo.iBorderBottom;
		rcBox.right = pInfo->m_rectDraw.right - m_stPrintInfo.iBorderRight;
	}
	else
	{
		rcBox.bottom  = 4676 - m_stPrintInfo.iBorderBottom;
	}
	iHeight = 0;

	pftOld = pDC->SelectObject(m_stPrintInfo.pftFooter);
	pcbOld = pDC->SelectObject(m_stPrintInfo.pcbLine);

	// text
	rcText.SetRectEmpty();
	str = pApp->m_strPrintFooter;
	pDC->DrawText(str, &rcText, DT_SINGLELINE | DT_LEFT | DT_CALCRECT);
	rcText.OffsetRect(rcBox.left, rcBox.bottom-iHeight-rcText.Height());

	if (!m_stPrintInfo.bBeginPrinting)
	{
		pDC->DrawText(str, &rcText, DT_SINGLELINE | DT_LEFT);
	}

	// Pagenumber
	rcText.SetRectEmpty();
	str.Format(_T("%d"), pInfo->m_nCurPage);
	pDC->DrawText(str, &rcText, DT_SINGLELINE | DT_RIGHT | DT_CALCRECT);
	rcText.OffsetRect(rcBox.right-rcText.Width(), rcBox.bottom-iHeight-rcText.Height());

	if (!m_stPrintInfo.bBeginPrinting)
	{
		pDC->DrawText(str, &rcText, DT_SINGLELINE | DT_RIGHT);
	}
	iHeight += rcText.Height();
	iHeight += 10;
	if (!m_stPrintInfo.bBeginPrinting)
	{
		pDC->MoveTo(rcBox.left, rcBox.bottom-iHeight);
		pDC->LineTo(rcBox.right, rcBox.bottom-iHeight);
	}
	iHeight += 50;

	rcBox.top = rcBox.bottom - iHeight;

	if (m_stPrintInfo.bBeginPrinting)
	{
		m_stPrintInfo.rcFooter = rcBox;
	}

	pDC->SelectObject(pftOld);
	pDC->SelectObject(pcbOld);
}

int CRailDiagDoc::PrText(CDC *pDC, int iLeft, int iRight, int iTop, int iPosRel, UINT uiFormat, CString str)
{
	CRect	rcText;

	rcText.SetRectEmpty();
	pDC->DrawText(str, &rcText, uiFormat | DT_CALCRECT);
	rcText.OffsetRect(iLeft + (iRight-iLeft)*iPosRel/100, iTop);
	if (!m_stPrintInfo.bBeginPrinting)
	{
		pDC->DrawText(str, &rcText, uiFormat);
	}
	return rcText.Height();
}

BOOL CRailDiagDoc::IsPrinting()
{
	return 	m_stPrintInfo.bPrinting;
}


void CRailDiagDoc::OnUpdateFileExport(CCmdUI* pCmdUI) 
{
/*
	switch(m_eDataType)
	{
	case EN_DATA_TYPE_HIST:
	case EN_DATA_TYPE_HIST20:
	case EN_DATA_TYPE_HIST100:
		pCmdUI->Enable(TRUE);
		break;
	default:
		pCmdUI->Enable(FALSE);
		break;
	}
*/
}

void CRailDiagDoc::OnFileExport() 
{
	// TODO: Add your command handler code here
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CString			strFile;
	int				iPos;

//	strFile = _T("Export file");
	strFile = GetTitle();
	iPos = strFile.Find(_T(".rdi"), 0);
	if (iPos > 0)
	{
		strFile.SetAt(iPos, '\0');
		strFile.Insert(iPos, _T(".csv"));
	}
	else
	{
		strFile += _T(".csv");
	}

	CFileDialog	FileDialog(FALSE, _T("csv"), strFile, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
						   _T("Export Files (*.csv)|*.csv|All Files (*.*)|*.*||"));
	int			iStatus;
	CString		strPath;

	FileDialog.m_ofn.lpstrInitialDir = pApp->m_strInitialDirectory;

//	FileDialog.m_ofn.lpstrTitle = _T("Export");
	iStatus = FileDialog.DoModal();
	if (iStatus == IDOK)
	{
//		TRACE(_T("Exporting file \n"));
		strPath = FileDialog.GetPathName();

		BOOL		bRet = TRUE;
		CStdioFile	file;
		CWaitCursor	wait;

		if (file.Open(strPath, CFile::modeWrite | CFile::modeCreate | CFile::typeText))
		{
			bRet = ExportDocData(file);
			file.Close();
		}
		else
			bRet = FALSE;

		if (!bRet)
		{
			CString	csMsg;

			csMsg.Format(_T("Eporting file failed: %s"), strPath);
			AfxMessageBox(csMsg);
		}
	}

}

BOOL CRailDiagDoc::ExportDocData(CStdioFile &file)
{
	BOOL			bRet = TRUE;
	CString			buf, buf2;
	int				i;
	CStringArray	*pStrArr = NULL;

	// Zeilenüberschriften ausgeben
	switch(m_eDataType)
	{
	case EN_DATA_TYPE_BIN:
		pStrArr = &m_stStatBinary.strarrRow;
		break;
	case EN_DATA_TYPE_ANA:
		pStrArr = &m_stStatAnalog.strarrRow;
		break;
	case EN_DATA_TYPE_ALARM:
		pStrArr = &m_stStatAlarm.strarrRow;
		break;
	case EN_DATA_TYPE_DURATION:
		pStrArr = &m_stStatDuration.strarrRow;
		break;
	case EN_DATA_TYPE_DURATION_FLEET:
		pStrArr = &m_stStatDurationFleet.strarrRow;
		break;
	case EN_DATA_TYPE_HIST:
	case EN_DATA_TYPE_HIST20:
	case EN_DATA_TYPE_HIST100:
		pStrArr = &m_stStatHistory.strarrRow;
		break;
	case EN_DATA_TYPE_HIST2:
		pStrArr = &m_stStatHistory2.strarrRow;
		break;
	}
	if (pStrArr)
	{
		buf.Empty();
		for (i = 0; pStrArr && i < pStrArr->GetSize(); i++)
		{
			if (i == 1)
			{
				buf2.Format(_T("ID,%s,"), pStrArr->GetAt(i));
			}
			else
			{
				buf2.Format(_T("%s,"), pStrArr->GetAt(i));
			}
			buf += buf2;
		}
		buf += CString(_T("\n"));
		file.WriteString(buf);
	}

	// datenzeilen ausgeben
	for (int i = 0; i < m_strarrListData.GetSize(); i++)
	{
		buf.Format(_T("%s\n"), m_strarrListData[i]);
		file.WriteString(buf);
	}

	return bRet;
}

void CRailDiagDoc::OnReloadData() 
{
	// TODO: Add your command handler code here
	CMainFrame		*pMainFrame;
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();

	m_bCancel = FALSE;

	pMainFrame = (CMainFrame*)pApp->GetMainWnd();
	pMainFrame->PostMessage(WM_USER_NEW_PROCESS_DIALOG_START);
	ReloadData();
	
}

void CRailDiagDoc::OnUpdateReloadData(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();

	if (pApp->m_iFleetMgrState == EN_FLEET_ON)
		pCmdUI->Enable(FALSE);
	else
	{
		if (pApp->GetConnectionState() == EN_CONN_CONNECTED)
		{
			pCmdUI->Enable(TRUE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
}

BOOL CRailDiagDoc::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDocument::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CRailDiagDoc::SetTitle(LPCTSTR lpszTitle) 
{
	// TODO: Add your specialized code here and/or call the base class
/*
	CString	szPath;
	TCHAR	szCurrentDir[_MAX_PATH];

	GetCurrentDirectory(_MAX_PATH, szCurrentDir);

	szPath.Format(_T("%s\\%s"), szCurrentDir, lpszTitle);

	SetPathName((LPCTSTR)szPath, FALSE);
*/
	CDocument::SetTitle(lpszTitle);
}


void CRailDiagDoc::OnFileSaveAs() 
{
	// TODO: Add your command handler code here
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CString			strFile;
//	int			iPos;

//	strFile = GetTitle(); 
//	iPos = strFile.Find(_T(".rdi"), 0);
//	if (iPos > 0)
//	{
//		strFile.SetAt(iPos, '\0');
//	}

	CFileDialog	FileDialog(FALSE, _T("rdi"), strFile, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING, 
						   _T("RailDiag (*.rdi)|*.rdi|All Files (*.*)|*.*||"));
	int			iStatus;
	CString		strPath;

	FileDialog.m_ofn.lpstrInitialDir = pApp->m_strInitialDirectory;

	iStatus = FileDialog.DoModal();
	if (iStatus == IDOK)
	{
		strPath = FileDialog.GetPathName();

		BOOL		bRet = TRUE;
		CStdioFile	file;
		CWaitCursor	wait;

		OnSaveDocument((LPCTSTR)strPath);
		SetPathName(strPath);
		//SetTitle(FileDialog.GetFileName());
	}
}
