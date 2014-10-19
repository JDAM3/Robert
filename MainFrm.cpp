// MainFrm.cpp : implementation of the CMainFrame class
//
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include "stdafx.h"
#include "RailDiag.h"
#include "DlgProgress.h"
#include "LocSettings.h"

#include "ComLib.h"
#include "ComDiag.h"
#include "RailDiagDoc.h"
#include "InOutList.h"
#include "MainFrm.h"
#include "Line.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define _CRT_SECURE_NO_WARNINGS

#define		_CONN_STATE_TIMER	1
//#define		NOTYFY_IMPL


extern BYTE			g_bySerReadBuf[1024];
extern DWORD		g_dwSerReadBufPos;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_CBN_SELENDOK(IDC_COMB_EX_LANG, OnSelendokCmLanguage)
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpFinder)

	ON_NOTIFY_EX(TTN_GETDISPINFO, 0, OnNotifyNeedText)
	ON_CBN_SELENDOK(IDC_COMB_EX_FLEET, &CMainFrame::OnCbnSelendokCombExFleet)
END_MESSAGE_MAP()

static UINT indicators[] =
{
//	ID_SEPARATOR,           // status line indicator
	ID_CONN_STATE,
	ID_CONN_ERROR,
	ID_CONN_LEVEL,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_bSendMsg = FALSE;
	m_iLineCountTotal = 0;
	m_iLineCount = 0;
	m_pDoc = NULL;
	m_dwLastReadTick = 0;
	m_dwLastWriteTick = 0;
	m_pDlgProg = NULL;
	m_iDlgStartCount = 0;
	m_iDlgCount = 0;
	m_bStop = FALSE;
	m_eAutoState = EN_AUTO_DISCONNECTED;
	m_dwAutoConnectStart = 0;
	m_iWriteCount = 0;
	m_puiarrLangs = NULL;
	m_iReconnectStat = 0;
	m_dwDlgProgTickCount = 0;
	m_iNoifyState = -1;
	m_iSendType = -1;
}

void CMainFrame::SetDoc(CRailDiagDoc *pDoc)
{
	m_pDoc = pDoc;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	UINT			uiID, uiStyle;
	int				iWidth;

	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	if (!m_wndToolBar2.CreateEx(this) ||
		!m_wndToolBar2.LoadToolBar(IDR_TOOLBAR2))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	if (!m_wndToolBar3.CreateEx(this) ||
		!m_wndToolBar3.LoadToolBar(IDR_TOOLBAR3))
	{
		TRACE0("Failed to create toolbar3\n");
		return -1;      // fail to create
	}

	//DJ
	//CRect rect;
	//int nIndex = m_wndToolBar.GetToolBarCtrl().CommandToIndex(ID_COMB_FLEET);
	//if (!m_combFleet.Create(CBS_DROPDOWNLIST | CBS_SORT | WS_VISIBLE |
	//	WS_TOBSTOP | WS_SCROLL, rect, &m_wndToolBar, ID_COMB_FLEET)
	//{
	//	TRACE0("Failed to create combo-box\n");
	//	return -1;		// fail to create
	//}


	if (!m_wndDlgBar.Create(this, IDR_MAINFRAME, 
		CBRS_ALIGN_TOP | CBRS_TOOLTIPS, AFX_IDW_DIALOGBAR))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;		// fail to create
	}
	if (!m_wndDlgBarFleet.Create(this, IDR_FLEET, 
		CBRS_ALIGN_TOP | CBRS_TOOLTIPS, AFX_IDW_DIALOGBAR))
	{
		TRACE0("Failed to create dialogbar fleet\n");
		return -1;		// fail to create
	}

	if (!m_wndReBar.Create(this) ||
		!m_wndReBar.AddBar(&m_wndToolBar) ||
		!m_wndReBar.AddBar(&m_wndToolBar2) ||
		!m_wndReBar.AddBar(&m_wndDlgBar) ||
		!m_wndReBar.AddBar(&m_wndToolBar3) ||
		!m_wndReBar.AddBar(&m_wndDlgBarFleet))
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.CreateEx(this, SBT_TOOLTIPS, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM, AFX_IDW_STATUS_BAR ) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.GetPaneInfo(0, uiID, uiStyle, iWidth);
	m_wndStatusBar.SetPaneInfo(0, uiID, uiStyle | SBPS_STRETCH, 100);
	m_wndStatusBar.GetPaneInfo(1, uiID, uiStyle, iWidth);
	m_wndStatusBar.SetPaneInfo(1, uiID, uiStyle | SBPS_STRETCH, 100);
	
//	int iIdx, iID, iWidth;

//	iID = ID_CONN_LEVEL;
//	iWidth = 100;
//	iIdx = m_wndStatusBar.CommandToIndex(iID);
//	m_wndStatusBar.SetPaneInfo(iIdx, iID, m_wndStatusBar.GetPaneStyle(iIdx), iWidth);

	// TODO: Remove this if you don't want tool tips for toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar2.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar3.SetBarStyle(m_wndToolBar3.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);

//	m_wndDlgBar.ShowWindow(SW_HIDE); // Dialogbar einstweilen ausbelenden
	SetLangList(&pApp->m_uiarrLangNr);
	SetFleetList(pApp->m_strarrFleetDesc);
	EnableFleetList(FALSE);

	SetMenuText(TRUE);

	SetTimer(_CONN_STATE_TIMER, 300, NULL);

#ifdef NOTYFY_IMPL
	if (m_iNoifyState == -1)
	{
		NOTIFYICONDATA	NotifyIconData;

		NotifyIconData.cbSize = sizeof(NotifyIconData);
		NotifyIconData.hWnd = this->m_hWnd;
		NotifyIconData.uID = 0;
		NotifyIconData.uFlags = NIF_ICON | NIF_TIP;
		NotifyIconData.hIcon = LoadIcon(pApp->m_hInstance, MAKEINTRESOURCE(IDI_DISCONNECT));
		_tcscpy(NotifyIconData.szTip, _T("RailDiag disonnected"));

		Shell_NotifyIcon(NIM_ADD, &NotifyIconData);

		m_iNoifyState = 1;
	}
#endif

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	RECT	rcDesk;

	CWnd *pWndDesktop = GetDesktopWindow();

	pWndDesktop->GetWindowRect(&rcDesk);

//	cs.style |= WS_MAXIMIZE;
	if (rcDesk.bottom - rcDesk.top == 600 && rcDesk.right - rcDesk.left == 800)
	{
		cs.x = 0;
		cs.y = 0;
		cs.cx = 800;
		cs.cy = 600;
		cs.style |= WS_MAXIMIZE;
	}
	else
	{
		cs.x = 0;
		cs.y = 0;
		cs.cx = 1024;
		cs.cy = 768 - 28;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::NewSerialData()
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CRailDiagDoc	*pDoc = m_pDoc;

	TCHAR			tszNewMsg[CD_MAX_MSG_LEN];
	TCHAR			tszData[CD_MAX_MSG_LEN];
	int				iType;
	BOOL			bStat;
//	int				iLen;
	DWORD			dwBytesMsg;
	CString			strCount;
	int				iTmp;
	CString			strMsg, strTmp;

	bStat = TRUE;

	pApp->m_bDataIn = TRUE;

	m_dwLastReadTick = GetTickCount();
	for (; bStat; )
	{
		//TRACE(_T("---> in ??? %d >%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x< \n"), 
		//	g_dwSerReadBufPos,
		//	g_bySerReadBuf[0], 
		//	g_bySerReadBuf[1],
		//	g_bySerReadBuf[2],
		//	g_bySerReadBuf[3],
		//	g_bySerReadBuf[4],
		//	g_bySerReadBuf[5],
		//	g_bySerReadBuf[6],
		//	g_bySerReadBuf[7],
		//	g_bySerReadBuf[8],
		//	g_bySerReadBuf[9],
		//	g_bySerReadBuf[10], 
		//	g_bySerReadBuf[11],
		//	g_bySerReadBuf[12],
		//	g_bySerReadBuf[13],
		//	g_bySerReadBuf[14],
		//	g_bySerReadBuf[15],
		//	g_bySerReadBuf[16],
		//	g_bySerReadBuf[17],
		//	g_bySerReadBuf[18],
		//	g_bySerReadBuf[19],
		//	g_bySerReadBuf[20]
		//	);

		bStat = CD_ParseMsg(g_bySerReadBuf, &g_dwSerReadBufPos, tszNewMsg, &iType);
		if (bStat)
		{
//			strMsg.Format(_T("---> in  Msg %3.3d"), iType);
			strTmp = tszNewMsg;
			strMsg.Format(_T("---> in  Msg >%s<"), strTmp.Right(strTmp.GetLength()-1));
			pApp->Write2Log(strMsg);

			TRACE(_T("---> New Command: %d, tick %x\n"), iType, GetTickCount());

			// neue Meldung
			switch(iType)
			{
			case CD_MSG_CONFIG:
				m_dwLastActiveTick = GetTickCount();
				if(CD_CheckMsg(tszNewMsg))
				{
					CString			strData;
					CUtil			Util;
					CStringArray	astrData;

					// daten in Doc übernehmen und
					// nächste Zeile anfordern
					CD_GetMsgData(tszNewMsg, tszData);
					strData = tszData;

					Util.SeparateComma(strData, astrData);

					if (astrData.GetSize() >= 6)
					{
						pApp->m_stLocSettingsLoad.strCompany		= astrData.GetAt(0);
						pApp->m_stLocSettingsLoad.strLocType		= astrData.GetAt(1);
						pApp->m_stLocSettingsLoad.strLocNumber		= astrData.GetAt(2);
						pApp->m_stLocSettingsLoad.strMotorType		= astrData.GetAt(3);
						pApp->m_stLocSettingsLoad.strMotorNumber	= astrData.GetAt(4);
						pApp->m_stLocSettingsLoad.strMtuOrderNumber	= astrData.GetAt(5);
					}
					if (astrData.GetSize() >= 12)
					{
						pApp->m_stLocSettingsLoad.strDisplayTelNumber	= astrData.GetAt(6);
						pApp->m_stLocSettingsLoad.strInhibitTime		= astrData.GetAt(7);
						pApp->m_stLocSettingsLoad.strTelNumberSMS1		= astrData.GetAt(8);
						pApp->m_stLocSettingsLoad.strTelNumberSMS2		= astrData.GetAt(9);
						pApp->m_stLocSettingsLoad.strTelNumberSMS3		= astrData.GetAt(10);
						pApp->m_stLocSettingsLoad.strReturnStartPageTime = astrData.GetAt(11);
					}
					if (astrData.GetSize() == 7) // alt
					{
						pApp->m_stLocSettingsLoad.strPin = astrData.GetAt(6);
					}
					if (astrData.GetSize() == 8) // alt
					{
						pApp->m_stLocSettingsLoad.strPin = astrData.GetAt(6);
						pApp->m_stLocSettingsLoad.strPuk = astrData.GetAt(7);
					}
					if (astrData.GetSize() == 13) // neu
					{
						pApp->m_stLocSettingsLoad.strPin = astrData.GetAt(12);
					}
					if (astrData.GetSize() == 14) // neu
					{
						pApp->m_stLocSettingsLoad.strPin = astrData.GetAt(12);
						pApp->m_stLocSettingsLoad.strPuk = astrData.GetAt(13);
					}

					m_bSendMsg = FALSE;
					if (enFleetStateStart == pApp->m_enFleetState ||
						enFleetStateInit == pApp->m_enFleetState)
					{
						pApp->m_enFleetState = enFleetStateSettings;
					}

					CD_GenerateDataHeader(m_tszSendMsg, &dwBytesMsg, CD_MSG_GET_LANGS);
					CD_GenerateDataTail(m_tszSendMsg, &dwBytesMsg);
					m_tszSendMsg[dwBytesMsg] = '\0';
					SendSerialData(m_tszSendMsg);
					m_bSendMsg = TRUE;

				}
				else
				{
					// Meldung nochmals senden
					SendSerialData(m_tszSendMsg);
					m_bSendMsg = TRUE;
				}
				break;

			case CD_MSG_INIT_CONFIG:
				m_dwLastActiveTick = GetTickCount();
				if (CD_CheckConfirmMsg2Msg(tszNewMsg, m_tszSendMsg, &iTmp))
				{
					m_bSendMsg = FALSE;
					if (pApp->m_pLocSettings)
					{
						PostMessage(WM_USER_CONFIG_OK);
					}
				}
				else
				{
					// Meldung nochmals senden
					SendSerialData(m_tszSendMsg);
					m_bSendMsg = TRUE;
				}
				break;

			case CD_MSG_GET_LANGS:
				m_dwLastActiveTick = GetTickCount();
				if(CD_CheckMsg(tszNewMsg))
				{
					CString			strData;
					CUtil			Util;
					CStringArray	astrData;
					UINT			uiTmp;

					// daten in Doc übernehmen und
					// nächste Zeile anfordern
					CD_GetMsgData(tszNewMsg, tszData);
					strData = tszData;

					Util.SeparateComma(strData, astrData);

					if (m_puiarrLangs == NULL)
					{
						m_puiarrLangs = new CUIntArray;

						if (m_puiarrLangs)
						{
							for (int i = 0; i < astrData.GetSize(); i++)
							{
								uiTmp = (UINT)_tcstol(astrData.GetAt(i), NULL, 10);
								m_puiarrLangs->Add(uiTmp);
							}
							PostMessage(WM_USER_LANGUAGES);
						}
						else
						{
							TRACE(_T("Error allocating memory for new Languages !!!\n"));
						}
					}
					m_bSendMsg = FALSE;
					if (enFleetStateSettings == pApp->m_enFleetState)
					{
						pApp->m_enFleetState = enFleetStateLangGet;
					}
				}
				else
				{
					// Meldung nochmals senden
					SendSerialData(m_tszSendMsg);
					m_bSendMsg = TRUE;
				}
				break;

			case CD_MSG_SET_LANG:
				m_dwLastActiveTick = GetTickCount();
				if(CD_CheckMsg(tszNewMsg))
				{
					m_bSendMsg = FALSE;
					if (enFleetStateLangGet == pApp->m_enFleetState)
					{
						pApp->m_enFleetState = enFleetStateLangSet;
					}
				}
				else
				{
					// Meldung nochmals senden
					SendSerialData(m_tszSendMsg);
					m_bSendMsg = TRUE;
				}
				break;

			case CD_MSG_STATE:
				m_dwLastActiveTick = GetTickCount();
				if(CD_CheckMsg(tszNewMsg))
				{
					// daten in Doc übernehmen und
					CD_GetMsgData(tszNewMsg, tszData);

					if (pDoc)
					{
						pDoc->NewStateData(tszData);
						TRACE(_T("State Data %s\n"), tszData);
						PostMessage(WM_USER_NEW_SERIAL_STATE);

						switch(pDoc->m_eDataType)
						{
						case EN_DATA_TYPE_BIN:
							CD_GenerateDataHeader(m_tszSendMsg, &dwBytesMsg, CD_MSG_BINARY_START);
							CD_GenerateDataTail(m_tszSendMsg, &dwBytesMsg);
							break;
						case EN_DATA_TYPE_ANA:
							CD_GenerateDataHeader(m_tszSendMsg, &dwBytesMsg, CD_MSG_ANALOG_START);
							CD_GenerateDataTail(m_tszSendMsg, &dwBytesMsg);
							break;
						case EN_DATA_TYPE_ALARM:
							CD_GenerateDataHeader(m_tszSendMsg, &dwBytesMsg, CD_MSG_ALARM_START);
							CD_GenerateDataTail(m_tszSendMsg, &dwBytesMsg);
							break;
						case EN_DATA_TYPE_DURATION:
						case EN_DATA_TYPE_DURATION_FLEET:
							CD_GenerateDataHeader(m_tszSendMsg, &dwBytesMsg, CD_MSG_DURATION_START);
							CD_GenerateDataTail(m_tszSendMsg, &dwBytesMsg);
							break;
						case EN_DATA_TYPE_HIST:
						case EN_DATA_TYPE_HIST20:
						case EN_DATA_TYPE_HIST100:
							CD_GenerateDataHeader(m_tszSendMsg, &dwBytesMsg, CD_MSG_HISTORY_START);
							CD_GenerateDataTail(m_tszSendMsg, &dwBytesMsg);
							break;
						case EN_DATA_TYPE_HIST2:
							CD_GenerateDataHeader(m_tszSendMsg, &dwBytesMsg, CD_MSG_HISTORY2_START);
							CD_GenerateDataTail(m_tszSendMsg, &dwBytesMsg);
							break;
						}
						m_tszSendMsg[dwBytesMsg] = '\0';
						SendSerialData(m_tszSendMsg);
						m_bSendMsg = TRUE;
					}

				}
				else
				{
					// Meldung nochmals senden
					SendSerialData(m_tszSendMsg);
					m_bSendMsg = TRUE;
				}

				break;

			case CD_MSG_BINARY_START:
			case CD_MSG_ANALOG_START:
			case CD_MSG_ALARM_START:
			case CD_MSG_DURATION_START:
			case CD_MSG_HISTORY_START:
			case CD_MSG_HISTORY2_START:
				m_dwLastActiveTick = GetTickCount();
				if (CD_CheckConfirmMsg2Msg(tszNewMsg, m_tszSendMsg, &m_iLineCountTotal))
				{
					m_bStop = FALSE;
					m_iDlgStartCount = m_iDlgCount;

					if (m_iLineCountTotal > 0)
					{
//						PostMessage(WM_USER_NEW_PROCESS_DIALOG_START);
						SetProgressLineCount(m_iLineCountTotal);

						TRACE(_T("Anzahl Zeilen Binär %d\n"), m_iLineCountTotal);

						m_iLineCount = 1;
						switch(pDoc->m_eDataType)
						{
						case EN_DATA_TYPE_HIST:
							m_iLineCount = 1;
							break;
						case EN_DATA_TYPE_HIST20:
							m_iLineCount = max(1, m_iLineCountTotal-(20-1));
							m_pDoc->m_iLineStart = m_iLineCount - 1;
							break;
						case EN_DATA_TYPE_HIST100:
							m_iLineCount = max(1, m_iLineCountTotal-(100-1));
							m_pDoc->m_iLineStart = m_iLineCount - 1;
							break;
						}

						if (pApp->m_iConnType == 0) // serielles Kabel
						{
							m_InOutList.Init(m_iLineCountTotal, 1, m_iLineCount - 1);
						}
						else
						{
							m_InOutList.Init(m_iLineCountTotal, pApp->m_iInOutMaxWaiting, m_iLineCount - 1);
						}

						switch (iType)
						{
						case CD_MSG_BINARY_START:
							m_iSendType = CD_MSG_BINARY_DATA;
							break;
						case CD_MSG_ANALOG_START:
							m_iSendType = CD_MSG_ANALOG_DATA;
							break;
						case CD_MSG_ALARM_START:
							m_iSendType = CD_MSG_ALARM_DATA;
							break;
						case CD_MSG_DURATION_START:
							m_iSendType = CD_MSG_DURATION_DATA;
							break;
						case CD_MSG_HISTORY_START:
							m_iSendType = CD_MSG_HISTORY_DATA;
							break;
						case CD_MSG_HISTORY2_START:
							m_iSendType = CD_MSG_HISTORY2_DATA;
							break;
}

						SendNextList();
						m_iLineCount = 1;
						switch(pDoc->m_eDataType)
						{
						case EN_DATA_TYPE_HIST:
							m_iLineCount = 1;
							break;
						case EN_DATA_TYPE_HIST20:
							m_iLineCount = max(1, m_iLineCountTotal-(20-1));
							m_pDoc->m_iLineStart = m_iLineCount - 1;
							break;
						case EN_DATA_TYPE_HIST100:
							m_iLineCount = max(1, m_iLineCountTotal-(100-1));
							m_pDoc->m_iLineStart = m_iLineCount - 1;
							break;
						}
					}
					else if (m_iLineCountTotal == -1)
					{
						// stop Datenzeilen senden, Fehlermeldung keine Zugriff
						SendStopCmdNoAccess();
					}
					else
					{
						// stop Datenzeilen senden, Fehlermeldung keine Daten
						SendStopCmdNoData();
					}
				}
				else
				{
					// Meldung nochmals senden
					SendSerialData(m_tszSendMsg);
					m_bSendMsg = TRUE;
				}
				break;

			case CD_MSG_BINARY_DATA:
			case CD_MSG_ANALOG_DATA:
			case CD_MSG_ALARM_DATA:
			case CD_MSG_DURATION_DATA:
			case CD_MSG_HISTORY_DATA:
			case CD_MSG_HISTORY2_DATA:
				m_dwLastActiveTick = GetTickCount();
				if(CD_CheckMsg(tszNewMsg))
				{
					BOOL	bRecive;
					BOOL	bNext;
					int		iLine = 1;

					// daten in Doc übernehmen und
					// nächste Zeile anfordern
					CD_GetMsgData(tszNewMsg, tszData);

					if (pDoc && !pDoc->m_bCancel)
					{
						iLine = m_iLineCount++;
						if (pDoc->NewErrorHistData(tszData, &iLine))
						{
							bRecive = m_InOutList.SetRecived(iLine, &bNext);
							TRACE(_T("Data %s\n"), tszData);
							PostMessage(WM_USER_NEW_SERIAL_DATA);
						}
						else
						{
							bRecive = FALSE;
						}
					}
					if (m_pDlgProg != NULL && bRecive)
					{
						m_pDlgProg->StepIt();
					}

					if (pDoc && !m_bStop && !m_InOutList.AllDataRecived())
					{
						SendNextList();
					}
					else if (pDoc && (m_bStop || m_InOutList.AllDataRecived()))
					{
						m_iSendType = 0;
						m_InOutList.DeInit();

						// stop Datenzeilen senden
						TRACE(_T("Stop Line \n"));
						CD_GenerateDataHeader(m_tszSendMsg, &dwBytesMsg, CD_MSG_STOP);
						CD_GenerateDataTail(m_tszSendMsg, &dwBytesMsg);

						SendSerialData(m_tszSendMsg);
						m_bSendMsg = TRUE;
					}
				}
				else
				{ 
					// datenzeile nochmals anfordern
					SendSerialData(m_tszSendMsg);
					m_bSendMsg = TRUE;
				}
				break;
			case CD_MSG_STOP:
				m_iSendType = 0;
				m_dwLastActiveTick = GetTickCount();
				if(CD_CheckMsg(tszNewMsg))
				{
					m_eAutoState = EN_AUTO_COMMAND_NEXT;
					if (m_pDlgProg && m_iDlgStartCount == m_iDlgCount)
					{
//						OutputDebugString(_T("---> Dialog Progress stop by Msg stop\n"));
						PostMessage(WM_USER_NEW_PROCESS_DIALOG_END);
					}
//					SetCursor(pApp->LoadStandardCursor(IDC_ARROW));

					if (enFleetStateLangSet == pApp->m_enFleetState)
					{
						pApp->m_enFleetState = enFleetStateData;
					}

					m_bSendMsg = FALSE;
				}
				else
				{ 
					// datenzeile nochmals anfordern
					SendSerialData(m_tszSendMsg);
					m_bSendMsg = TRUE;
				}
				break;

			case CD_MSG_LINE_TEST:
				if(CD_CheckMsg(tszNewMsg))
				{
					m_bSendMsg = FALSE;
				}

			default:
				break;
			}
		}
	}
}

void CMainFrame::SendSerialData(TCHAR *tszMsg, BOOL	bResetCount)
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	int				iLen;
	DWORD			dwBytesWritten = 0;
	DWORD			dwInBufLen, dwOutBufLen;
	CString			strTmp, strMsg;

	m_dwLastWriteTick = GetTickCount();
	m_dwLastReadTick = GetTickCount();

	if (bResetCount)
	{
		m_iWriteCount = 0;
	}

	iLen = _tcslen(tszMsg);
	if (iLen < CD_MAX_MSG_LEN)
	{
		_tcscpy(m_tszSendMsg, tszMsg);
		if (pApp->m_iConnType == 0)
		{
			if (0 == ComGetStatus(pApp->GetComPort(), &dwOutBufLen, &dwInBufLen))
			{
				if (dwOutBufLen > 0 || dwInBufLen > 0)
				{
					ComClear(pApp->GetComPort());
				}
			}

			ComSend(pApp->GetComPort(), m_tszSendMsg, iLen*2, &dwBytesWritten);
//			ASSERT(iLen*2 == (int)dwBytesWritten);
		}
		if (pApp->m_iConnType == 1)
		{
			Line_Send(m_tszSendMsg, iLen*2, &dwBytesWritten);
		}
		strTmp = m_tszSendMsg;
		TRACE(_T("<--- Send Line: %s\n"), strTmp.Right(strTmp.GetLength()-1));

		strMsg.Format(_T("<--- out Msg >%s<"), strTmp.Right(strTmp.GetLength()-1));
		pApp->Write2Log(strMsg);


		m_bSendMsg = TRUE;
	}
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CRailDiagDoc	*pDoc = m_pDoc;

	if (pMsg->message == (WM_USER_NEW_SERIAL_DATA))
	{
		if (m_pDoc && !m_pDoc->IsPrinting())
		{
			m_pDoc->UpdateAllViews(m_pDoc->GetStaticView());
			
			if (pDoc)
			{
				pDoc->m_bUpdateView = TRUE;
			}
		}
	}

	if (pMsg->message == (WM_USER_NEW_SERIAL_STATE))
	{
		if (m_pDoc)
		{
			m_pDoc->UpdateAllViews(m_pDoc->GetErrListView());

			if (!m_pDoc->IsPrinting())
			{
				m_pDoc->UpdateAllViews(m_pDoc->GetStaticView());
			}
		}
	}

	if (pMsg->message == (WM_USER_ACTIVATE_LIST_VIEW))
	{
		if (m_pDoc && !m_pDoc->IsPrinting())
		{
			CView	*pView = NULL;
			pView = m_pDoc->GetErrListView();
			
			if (pView)
			{
				pView->SetFocus();
			}
		}
	}

	if (pApp->m_iFleetMgrState == EN_FLEET_OFF &&
		(pMsg->message == WM_USER_NEW_PROCESS_DIALOG_END || pMsg->message == WM_USER_RELOAD_DATA))
	{
//		OutputDebugString(_T("-> Stop Progress-Dialog Start\n"));
		if (m_pDlgProg != NULL)
		{
//			OutputDebugString(_T("-> Stop Progress-Dialog destroy\n"));
			m_pDlgProg->DestroyWindow();
			delete m_pDlgProg;
			m_pDlgProg = NULL;
			m_bStop = FALSE;
		}
		m_iSendType = 0;
		m_InOutList.DeInit();
//		OutputDebugString(_T("-> Stop Progress-Dialog End\n"));
	}

	if (pApp->m_iFleetMgrState == EN_FLEET_OFF &&
		(pMsg->message == WM_USER_NEW_PROCESS_DIALOG_START || pMsg->message == WM_USER_RELOAD_DATA))
	{
//		OutputDebugString(_T("-> Start Progress-Dialog\n"));
		if (m_pDlgProg == NULL && pDoc)
		{
			int		iLineCount;

			m_bStop = FALSE;
			m_iDlgStartCount ++;

//			OutputDebugString(_T("--> Create Progress-Dialog\n"));
			m_pDlgProg = new CDlgProgress(_T("-"));
			m_pDlgProg->Create();
			m_pDlgProg->SetWindowText(pApp->m_stDlgProg.strTitle);
			m_pDlgProg->SetPos(0);

			switch (pDoc->m_eDataType)
			{
			case EN_DATA_TYPE_HIST20:
				iLineCount =  min(20, m_iLineCountTotal);
				break;
			case EN_DATA_TYPE_HIST100:
				iLineCount =  min(100, m_iLineCountTotal);
				break;
			default:
				iLineCount = m_iLineCountTotal;
				break;
			}

			if (iLineCount < 1)
			{
				iLineCount = 1;
			}
			m_pDlgProg->SetRange(0, iLineCount);
			m_pDlgProg->SetStep(1);
		}
//		OutputDebugString(_T("-> Start Progress-Dialog End\n"));
	}

	if (pApp->m_iFleetMgrState == EN_FLEET_ON &&
		(pMsg->message == WM_USER_NEW_PROCESS_DIALOG_FLEET_END))
	{
		OutputDebugString(_T("-> Stop Progress-Dialog Start\n"));
		if (m_pDlgProg != NULL)
		{
//			OutputDebugString(_T("-> Stop Progress-Dialog destroy\n"));
			m_pDlgProg->DestroyWindow();
			delete m_pDlgProg;
			m_pDlgProg = NULL;
			m_bStop = FALSE;
		}
		m_iSendType = 0;
		m_InOutList.DeInit();
//		OutputDebugString(_T("-> Stop Progress-Dialog End\n"));
		pApp->bFleetDialogEnd = FALSE;

		if (0 < pApp->m_uiarrError.GetCount())
		{
			CString message;
			int i;
			FLEET_ELEM *pFleetElem;

			pFleetElem = (FLEET_ELEM*)pApp->m_ptrarrFleetElem.GetAt(pApp->m_iFleetSelected);

			message = _T("Error transfer Fleetlist to\n");

			for (i = 0; i < pApp->m_uiarrError.GetCount(); i++)
			{
				message += pFleetElem->strarrPhoneBookDescr.GetAt(pApp->m_uiarrError.GetAt(i)) + _T(" #") +
					pFleetElem->strarrPhoneBookNumber.GetAt(pApp->m_uiarrError.GetAt(i)) + _T("\n");
			}
			AfxMessageBox(message);
		}
	}

	if (pApp->m_iFleetMgrState == EN_FLEET_ON &&
		(pMsg->message == WM_USER_NEW_PROCESS_DIALOG_FLEET_START || pMsg->message == WM_USER_RELOAD_DATA))
	{
		OutputDebugString(_T("-> Start Progress-Dialog\n"));
		if (m_pDlgProg == NULL)
		{
			int		iLineCount = 0;

			m_bStop = FALSE;
			m_iDlgStartCount ++;

			OutputDebugString(_T("--> Create Progress-Dialog\n"));
			m_pDlgProg = new CDlgProgress(_T("-"));
			m_pDlgProg->Create();
			m_pDlgProg->SetWindowText(pApp->m_stDlgProg.strTitle);
			m_pDlgProg->SetPos(0);

			if (iLineCount < 1)
			{
				iLineCount = 1;
			}
			m_pDlgProg->SetRange(0, iLineCount);
			m_pDlgProg->SetStep(1);

			{
				FLEET_ELEM *pFleetElem;

				if (pApp->m_iFleetSelected >= 0 && pApp->m_iFleetSelected < pApp->m_ptrarrFleetElem.GetCount())
				{
					pFleetElem = (FLEET_ELEM*)pApp->m_ptrarrFleetElem.GetAt(pApp->m_iFleetSelected);
					if (pFleetElem)
					{
						m_pDlgProg->SetRangeFleet(0, pFleetElem->strarrPhoneBookDescr.GetCount());
						if (0 < pFleetElem->strarrPhoneBookDescr.GetCount())
						{
							m_pDlgProg->SetNameFleet(pFleetElem->strarrPhoneBookDescr.GetAt(0));
						}
						m_pDlgProg->SetStepFleet(1);
					}
				}
			}
		}
//		OutputDebugString(_T("-> Start Progress-Dialog End\n"));
	}

	if (pApp->m_iFleetMgrState == EN_FLEET_ON &&
		pMsg->message == WM_USER_NEW_PROCESS_DIALOG_FLEET_STEP)
	{
		if (m_pDlgProg)
		{
			m_pDlgProg->StepItFleet();
		}
	}
	if (pApp->m_iFleetMgrState == EN_FLEET_ON &&
		pMsg->message == WM_USER_NEW_PROCESS_DIALOG_FLEET_NAME)
	{
		if (m_pDlgProg)
		{
			m_pDlgProg->SetNameFleet(pApp->GetDialName());
		}
	}

	if (pMsg->message == (WM_USER_CONFIG_OK))
	{
		if (pApp->m_pLocSettings)
		{
			PostMessage(WM_USER_CONFIG_OK);
			CLocSettings *pLocSettings;

			pLocSettings = (CLocSettings*)pApp->m_pLocSettings;
			pLocSettings->SettingsSet();
		}
	}

	if (pMsg->message == (WM_USER_LANGUAGES))
	{
		if(m_puiarrLangs)
		{
			SetLangList(m_puiarrLangs);
			delete m_puiarrLangs;
			m_puiarrLangs = NULL;
		}
	}

	if (pMsg->message == (WM_USER_RELOAD_DATA))
	{
		CRailDiagDoc	*pDoc = m_pDoc;

		if (pDoc)
		{
			pDoc->ReloadData();
		}
	}

	return CMDIFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CRailDiagDoc	*pDoc = m_pDoc;
	int				iIdxConnState;
	int				iIdxConnError;
	int				iIdxConnLevel;
	WORD			wLevel;
	CString			strLevel;
	CString			strError;
	CString			strErrorStatusLine;
	CString			strState;
	DWORD			dwTickCount;
	DWORD			dwBytes;
	BOOL			bMessageBox = FALSE;
	BOOL			bTimeoutOnlineTest = FALSE;

	SetMenuText();

	// automatischer modus abhandeln
	if (pApp->m_bAuto)
	{
		if (m_eAutoState == EN_AUTO_DISCONNECTED)
		{
			pApp->ComConnect();
			m_eAutoState = EN_AUTO_CONNECTING;
			m_dwAutoConnectStart = GetTickCount();
			if (pApp->m_bLogFile)
			{
				time_t t;
				time(&t);
				CString	csBuf;

				csBuf.Format(_T("Verbindung starten am: %s\n"), _wctime(&t));
				pApp->m_fLogFile.WriteString(csBuf);
			}
		}
		else if (m_eAutoState == EN_AUTO_CONNECTING)
		{
			if  (pApp->GetComStateConnect() == _STATE_CONN_CONNECTED)
			{
				m_eAutoState = EN_AUTO_CONNECTED;
				if (pApp->m_bLogFile)
				{
					time_t t;
					time(&t);
					CString	csBuf;

					csBuf.Format(_T("Verbindung hergestellt am: %s\n"), _wctime(&t));
					pApp->m_fLogFile.WriteString(csBuf);
				}
			}
			else
			{
				if (pApp->m_iConnType == 0) // serial
				{
					pApp->ComConnect();
				}

				// timeout für Verbindung
				dwTickCount = GetTickCount();
				if (dwTickCount - m_dwAutoConnectStart > 20000)
				{
					m_eAutoState = EN_AUTO_END;
					if (pApp->m_bLogFile)
					{
						time_t t;
						time(&t);
						CString	csBuf;

						csBuf.Format(_T("Verbindungsprobleme am: %s\n"), _wctime(&t));
						pApp->m_fLogFile.WriteString(csBuf);
					}
				}
			}
		}
		else if (m_eAutoState == EN_AUTO_CONNECTED ||
				 m_eAutoState == EN_AUTO_COMMAND_NEXT)
		{
			if (m_eAutoState == EN_AUTO_COMMAND_NEXT)
			{
				if (pApp->m_strarrAutoSaveFiles.GetSize() > 0)
				{
					m_pDoc->OnSaveDocument(pApp->m_strarrAutoSaveFiles.GetAt(0));
					if (pApp->m_bLogFile)
					{
						time_t t;
						time(&t);
						CString	csBuf;

						csBuf.Format(_T("Speichern in: %s, am: %s\n"), 
							pApp->m_strarrAutoSaveFiles.GetAt(0), _wctime(&t));
						pApp->m_fLogFile.WriteString(csBuf);
					}
					pApp->m_strarrAutoSaveFiles.RemoveAt(0);
					SendMessage(WM_COMMAND, ID_FILE_CLOSE, 0);
				}
			}

			if (pApp->m_uiarrAutoCommands.GetSize() > 0)
			{
				// array auswerten
				int	iCmd;
				iCmd = pApp->m_uiarrAutoCommands.GetAt(0);
				pApp->m_uiarrAutoCommands.RemoveAt(0);
				switch(iCmd)
				{
				case EN_DATA_TYPE_BIN:
					pApp->OnNewBinData();
					if (pApp->m_bLogFile)
					{
						time_t t;
						time(&t);
						CString	csBuf;

						csBuf.Format(_T("Binärdaten anfordern am: %s\n"), _wctime(&t));
						pApp->m_fLogFile.WriteString(csBuf);
					}
					break;
				case EN_DATA_TYPE_ANA:
					pApp->OnNewAnaData();
					if (pApp->m_bLogFile)
					{
						time_t t;
						time(&t);
						CString	csBuf;

						csBuf.Format(_T("Analogdaten anfordern am: %s\n"), _wctime(&t));
						pApp->m_fLogFile.WriteString(csBuf);
					}
					break;
				case EN_DATA_TYPE_ALARM:
					pApp->OnNewAlarmData();
					if (pApp->m_bLogFile)
					{
						time_t t;
						time(&t);
						CString	csBuf;

						csBuf.Format(_T("Alarmdaten anfordern am: %s\n"), _wctime(&t));
						pApp->m_fLogFile.WriteString(csBuf);
					}
					break;
				case EN_DATA_TYPE_DURATION:
				case EN_DATA_TYPE_DURATION_FLEET:
					pApp->OnNewDurationData();
					if (pApp->m_bLogFile)
					{
						time_t t;
						time(&t);
						CString	csBuf;

						csBuf.Format(_T("Betriebsstundenzälerdaten anfordern am: %s\n"), _wctime(&t));
						pApp->m_fLogFile.WriteString(csBuf);
					}
					break;
				case EN_DATA_TYPE_HIST20:
					pApp->OnNewHist20Data();
					if (pApp->m_bLogFile)
					{
						time_t t;
						time(&t);
						CString	csBuf;

						csBuf.Format(_T("History20daten anfordern am: %s\n"), _wctime(&t));
						pApp->m_fLogFile.WriteString(csBuf);
					}
					break;
				case EN_DATA_TYPE_HIST100:
					pApp->OnNewHist100Data();
					if (pApp->m_bLogFile)
					{
						time_t t;
						time(&t);
						CString	csBuf;

						csBuf.Format(_T("History100daten anfordern am: %s\n"), _wctime(&t));
						pApp->m_fLogFile.WriteString(csBuf);
					}
					break;
				case EN_DATA_TYPE_HIST:
					pApp->OnNewHistData();
					if (pApp->m_bLogFile)
					{
						time_t t;
						time(&t);
						CString	csBuf;

						csBuf.Format(_T("Historydaten anfordern am: %s\n"), _wctime(&t));
						pApp->m_fLogFile.WriteString(csBuf);
					}
					break;
				case EN_DATA_TYPE_HIST2:
					pApp->OnNewHist2Data();
					if (pApp->m_bLogFile)
					{
						time_t t;
						time(&t);
						CString	csBuf;

						csBuf.Format(_T("Historystatistikdaten anfordern am: %s\n"), _wctime(&t));
						pApp->m_fLogFile.WriteString(csBuf);
					}
					break;
				}
				m_eAutoState = EN_AUTO_COMMAND_RUNNING;
			}
			else
			{
				m_eAutoState = EN_AUTO_END;
			}
		}
		else if (m_eAutoState == EN_AUTO_END)
		{
			pApp->ComDisconnect();
			if (pApp->m_bLogFile)
			{
				time_t t;
				time(&t);
				CString	csBuf;

				csBuf.Format(_T("Verbindungs- und Programmende am: %s\n"), _wctime(&t));
				pApp->m_fLogFile.WriteString(csBuf);
			}
			PostMessage(WM_COMMAND, ID_APP_EXIT, 0);
		}
	} // auto

	if (m_pDlgProg != NULL)
	{
		if (m_pDlgProg->CheckCancelButton())
		{
//			OutputDebugString(_T("-> OnTimer Progress-Dialog cancel-Button\n"));
			m_bStop = TRUE;
			m_dwDlgProgTickCount = GetTickCount();
			SendDataLineCancel();
			pApp->m_bFleetCancel = TRUE;
		}

		if (m_bStop && GetTickCount() - m_dwDlgProgTickCount > 2000)
		{
//			OutputDebugString(_T("-> OnTimer End Progress-Dialog stop and timeout\n"));
			if (pApp->m_iFleetMgrState == EN_FLEET_ON)
			{
				pApp->m_bFleetCancel = TRUE;
				PostMessage(WM_USER_NEW_PROCESS_DIALOG_FLEET_END);
			}
			else
			{
				PostMessage(WM_USER_NEW_PROCESS_DIALOG_END);
			}
		}

		if (pApp->GetConnectionState() != EN_CONN_CONNECTED && !pApp->GetReconnect() &&
			pApp->m_iFleetMgrState == EN_FLEET_OFF)
		{
			SendDataLineCancel();
			PostMessage(WM_USER_NEW_PROCESS_DIALOG_END);
		}

	}

	// prüfuen für reconnect
	dwTickCount = GetTickCount();
	if (pApp->GetReconnect() && pApp->GetComStateConnect() == _STATE_CONN_DISCONNECTED &&
		dwTickCount - pApp->m_dwDisconnectTick > pApp->m_uiWait4Reconnect)
	{
		pApp->Write2Log(_T("Connecting after timeout"));
		pApp->ComConnect();
	}

	if ( ( (pApp->GetConnectionState() == EN_CONN_OPENING && pApp->m_iConnType == 0) ||  // serielle Schnittst.
		   (pApp->GetConnectionState() == EN_CONN_CONNECTED) )&& 
		dwTickCount - m_dwLastReadTick > 2 * pApp->m_uiMaxWait4Check)
	{
		pApp->Write2Log(_T("Disconnecting by timeout"));

		pApp->ComDisconnect();
		pApp->SetReconnect(TRUE);
	}
	else if ( ( (pApp->GetConnectionState() == EN_CONN_OPENING && pApp->m_iConnType == 0) ||  // serielle Schnittst.
				(pApp->GetConnectionState() == EN_CONN_CONNECTED) ) 
			  && 
			  ( m_bSendMsg && dwTickCount - m_dwLastReadTick > pApp->m_uiMaxWait4Data ||
			   !m_bSendMsg && dwTickCount - m_dwLastReadTick > pApp->m_uiMaxWait4Check) )
	{
		TRACE(_T("CMainFrame::OnTimer Timeout 1 \n"));
		if (m_bStop)
		{
			PostMessage(WM_USER_NEW_PROCESS_DIALOG_END);
		}

		if (dwTickCount - m_dwLastReadTick > pApp->m_uiMaxWait4Check ||
			dwTickCount - (m_dwLastReadTick - m_iWriteCount * pApp->m_uiMaxWait4Data) > pApp->m_uiMaxWait4Check)
		{
//			bTimeoutOnlineTest = TRUE;
		}

		if (m_bSendMsg)
		{
			if (m_iWriteCount > 15) // 0 entspricht 1 mal den Befehl wiederholen
			{
				pApp->Write2Log(_T("Disconnecting by retry command"));
				pApp->ComDisconnect();
				pApp->SetReconnect(TRUE);
			}
			else
			{
				SendSerialData(m_tszSendMsg, FALSE);
				m_iWriteCount ++;

				if (pApp->m_iConnType == 0 && m_iWriteCount > 2)
				{
					pApp->ComDisconnect();
					// messageBox
					strError = pApp->m_stConStatus.strSerialOff;
					bMessageBox = TRUE;
					SendDataLineCancel();
//					TRACE(_T("strError set com disconnect\n"));
				}

			}
		}
		else if (m_iSendType > 0)
		{
			int		iRetryCount = 0;

			iRetryCount = m_InOutList.SetCount4NewSend();
			TRACE(_T("SetCount4NewSend count %d\n"), iRetryCount);
			if (iRetryCount > 15)
			{
				pApp->Write2Log(_T("Disconnecting by retry transfer"));
				pApp->ComDisconnect();
				pApp->SetReconnect(TRUE);
				m_InOutList.ResetRetryCount();
			}
			else
			{
				SendNextList();
				if (pApp->m_iConnType == 0) // serielle Schnittstelle
				{
					pApp->ComDisconnect();
					// messageBox
					strError = pApp->m_stConStatus.strSerialOff;
					bMessageBox = TRUE;
					SendDataLineCancel();
//					TRACE(_T("strError set com disconnect\n"));

				}
				else
				{
					pApp->SetReconnect(TRUE);
				}
			}
		}
		else
		{
			// Verbindung testen, wenn keine Daten übertragen werden
			CD_GenerateDataHeader(m_tszSendMsg, &dwBytes, CD_MSG_LINE_TEST);
			CD_GenerateDataTail(m_tszSendMsg, &dwBytes);
			m_tszSendMsg[dwBytes] = '\0';
			SendSerialData(m_tszSendMsg);
			m_bSendMsg = TRUE;
		}
	}


	if (pDoc && pDoc->m_bUpdateView)
	{
//		PostMessage(WM_USER_NEW_SERIAL_DATA);
	}

	// Verbindungswiederholung bei Verbinden und trennen des Modems
	if (pApp->m_iConnType == 1)
	{
		EN_LINE_STATE	eLineState;
		EN_LINE_ERROR	eLineError;

		if (pApp->GetComStateConnect() == _STATE_CONN_DISCONNECTING)
		{
			pApp->ComDisconnect();
		}
		else
		{
			eLineState = Line_GetState(&eLineError);
			if ((pApp->GetReconnect() || pApp->m_bUserConnect) &&
				(eLineState == EN_LINE_STATE_INIT ||
				eLineState == EN_LINE_STATE_NOOPEN || 
				eLineState == EN_LINE_STATE_CHANGEDBAUD))
			{
				pApp->ComConnect();
			}
/*
			if (pApp->GetConnectionState() == EN_CONN_CONNECTED &&
				eLineState != EN_LINE_STATE_CONNECTED)
			{
				TRACE(_T("disconnecting GetConnectionState %d, eLineState %d\n"), pApp->GetConnectionState(), eLineState);
				pApp->ComDisconnect();

				if (!pApp->GetReconnect())
				{
					// messageBox
					strError.Format(_T("%s, # %d, # %d"), pApp->m_stConStatus.strUnknown, eLineState, pApp->GetComStateConnect());
					bMessageBox = TRUE;
					SendDataLineCancel();
				}
			}
*/

		}

/*
		if (pApp->GetComStateConnect() != _STATE_CONN_CONNECTING &&
			pApp->GetComStateConnect() != _STATE_CONN_CONNECTED &&
			pApp->GetComStateConnect() != _STATE_CONN_DISCONNECTED)
		{
			if (eLineState != EN_LINE_STATE_NOINIT)
			{
				pApp->Write2Log(_T("Reconnecting by wrong connection state"));
				pApp->ComDisconnect();

//				pApp->SetReconnect(TRUE);
				// messageBox
				strError.Format(_T("%s, # %d, # %d"), pApp->m_stConStatus.strUnknown, eLineState, pApp->GetComStateConnect());
				bMessageBox = TRUE;
				SendDataLineCancel();
			}
		}
*/
	}


	if (pApp->m_bUserInput && !bMessageBox)
	{
		strError = _T(" ");
		strErrorStatusLine = _T(" ");
//		TRACE(_T("strError clear\n"));
	}

	if (pApp->GetComStateConnect() != _STATE_CONN_CONNECTED)
	{
		if (GetTickCount() - pApp->m_dwDisconnectTick > pApp->m_uiWait4Reconnect)
		{
			strState = pApp->m_stConStatus.strDisconnected;
		}
		else
		{
			strState = pApp->m_stConStatus.strDisconnecting;
		}

		strLevel = _T("0%");
		pApp->SetConnectionState(EN_CONN_IDLE);
		NotifyDisconnected();

		EN_LINE_STATE	eLineState;
		EN_LINE_ERROR	eLineError;

		eLineState = Line_GetState(&eLineError);
		switch(eLineState)
		{
			case EN_LINE_STATE_INIT:
				pApp->SetConnectionState(EN_CONN_OPENING);
			case EN_LINE_STATE_NOOPEN:
			case EN_LINE_STATE_CHANGEDBAUD:
				strState = pApp->m_stConStatus.strInit;
				break;
			case EN_LINE_STATE_CONNECTING:
				strState = pApp->m_stConStatus.strOpening;
				break;
			default:
				break;
		}
	}
	else
	{
		if (pApp->m_iConnType == 0)
		{
			if (pApp->m_bDataIn)
			{
				if (pApp->GetConnectionState() != EN_CONN_CONNECTED)
				{
					m_dwLastReadTick = GetTickCount();
				}
				strState =  pApp->m_stConStatus.strConnected;
				strLevel = _T("100%");
				pApp->SetConnectionState(EN_CONN_CONNECTED);
				m_iReconnectStat = 0;
				if (!bMessageBox)
				{
					strError = _T(" ");
					strErrorStatusLine = _T(" ");
				}
//				TRACE(_T("strError clear\n"));
				NotifyConnected();
			}
			else
			{
				if (pApp->GetConnectionState() != EN_CONN_OPENING)
				{
					if (pApp->GetReconnect())
					{
						pApp->Write2Log(_T("Reconnected serial"));
						ReconnectReceive();
						pApp->SetReconnect(FALSE);
					}
					else
					{
						pApp->Write2Log(_T("Connected serial"));
						ConfigReceive();
					}
					m_dwLastReadTick = GetTickCount();
				}
				strState =  pApp->m_stConStatus.strOpening;
				strLevel = _T("0%");
				pApp->SetConnectionState(EN_CONN_OPENING);
				NotifyDisconnected();
			}
		}

		if (pApp->m_iConnType == 1)
		{
			BOOL			bReconnect;
			EN_LINE_STATE	eLineState;
			EN_LINE_ERROR	eLineError;

			eLineState = Line_GetState(&eLineError);
			wLevel = Line_GetSignalLevel();
			switch(eLineState)
			{
				case EN_LINE_STATE_NOINIT:
					if (GetTickCount() - pApp->m_dwDisconnectTick > pApp->m_uiWait4Reconnect)
					{
						strState = pApp->m_stConStatus.strDisconnected;
					}
					else
					{
						strState = pApp->m_stConStatus.strDisconnecting;
					}
					strLevel = _T("0%");
					pApp->SetConnectionState(EN_CONN_IDLE);
					NotifyDisconnected();
					break;
				case EN_LINE_STATE_INIT:
				case EN_LINE_STATE_NOOPEN:
				case EN_LINE_STATE_CHANGEDBAUD:
					strState = pApp->m_stConStatus.strInit;
					strLevel = _T("0%");
					pApp->SetConnectionState(EN_CONN_IDLE);
					NotifyDisconnected();
					break;
				case EN_LINE_STATE_IDLE:
					if (pApp->GetConnectionState() == EN_CONN_CONNECTED)
					{
						pApp->Write2Log(_T("Reconnected modem idle1"));
						pApp->OnComDisconnect();
						pApp->SetReconnect(TRUE);
					}
					else
					{
						if (!pApp->m_bGSM_Rail || pApp->GetComStateConnect() == _STATE_CONN_CONNECTED)
						{
							bReconnect = pApp->GetReconnect();
							if (pApp->m_bUserConnect)
							{
								if (!bReconnect)
								{
									pApp->Write2Log(_T("disonnect modem idle"));
									bMessageBox = TRUE;
									pApp->OnComDisconnect();
								}
							}
							if(bReconnect)
							{
								pApp->Write2Log(_T("Reconnected modem idle2"));
								pApp->OnComDisconnect();
								pApp->SetReconnect(TRUE);
							}
							NotifyDisconnected();
						}
					}

					strState = pApp->m_stConStatus.strIdle;
					strLevel = _T("0%");
					pApp->SetConnectionState(EN_CONN_IDLE);
					break;
				case EN_LINE_STATE_OPENING:
				case EN_LINE_STATE_CONNECTING:
					strState = pApp->m_stConStatus.strOpening;
					strLevel = _T("0%");
					pApp->SetConnectionState(EN_CONN_OPENING);
					NotifyDisconnected();
					break;
				case EN_LINE_STATE_ANSWERING:
					strState = pApp->m_stConStatus.strAnswering;
					strLevel = _T("0%");
					pApp->SetConnectionState(EN_CONN_ANSWERING);
					NotifyDisconnected();
					break;
				case EN_LINE_STATE_CONNECTED:
					m_iReconnectStat = 0;
					if (bTimeoutOnlineTest)
					{
						strError = _T("timeout online test");
					}
					else
					{
						strError = _T(" ");
					}

					strState = pApp->m_stConStatus.strConnected;
					strLevel.Format(_T("%d%%"), 100*wLevel/0xFFFF);
					if (pApp->GetConnectionState() != EN_CONN_CONNECTED)
					{
						if (pApp->GetReconnect())
						{
							pApp->Write2Log(_T("Reconnected modem"));
							ReconnectReceive();
							pApp->SetReconnect(FALSE);
						}
						else
						{
							pApp->Write2Log(_T("Connected modem"));
							ConfigReceive();
						}
						m_dwLastReadTick = GetTickCount();
						m_dwLastActiveTick = GetTickCount();
					}

					if (GetTickCount() - m_dwLastActiveTick > pApp->m_uiMaxWait4Active)						
					{
						pApp->Write2Log(_T("Disconnected during inactivity"));
						pApp->OnComDisconnect();
						pApp->SetReconnect(FALSE);
						pApp->SetConnectionState(EN_CONN_IDLE);
					}
					else
					{
						pApp->SetConnectionState(EN_CONN_CONNECTED);
					}

					NotifyConnected();
					break;
				default:
					strState = pApp->m_stConStatus.strUnknown;
					strLevel = _T("0%");
					pApp->SetConnectionState(EN_CONN_IDLE);
					m_iReconnectStat = 0;
					NotifyDisconnected();
					break;
			}

			if (eLineState != EN_LINE_STATE_OPENING)
			{
				switch(eLineError)
				{
					case EN_LINE_ERR_WRONGPUK:
						strError = pApp->m_stConStatus.strWrongPuk;
						pApp->SetConnectionState(EN_CONN_IDLE);
						pApp->m_bPUK = TRUE;
						break;

					case EN_LINE_ERR_WRONGPIN:
						strError = pApp->m_stConStatus.strWrongPin;
						pApp->SetConnectionState(EN_CONN_IDLE);
						break;

					case EN_LINE_ERR_CLOSE:
						strError = pApp->m_stConStatus.strClose;
						pApp->SetConnectionState(EN_CONN_IDLE);
						break;

					case EN_LINE_ERR_NODIALTONE:
						strError = pApp->m_stConStatus.strNoDialTone;
						pApp->SetConnectionState(EN_CONN_IDLE);
						break;

					case EN_LINE_ERR_BUSY:
						strError = pApp->m_stConStatus.strBusy;
						pApp->SetConnectionState(EN_CONN_IDLE);
						break;

					case EN_LINE_ERR_NOANSWER:
						strError = pApp->m_stConStatus.strNoAnswer;
						pApp->SetConnectionState(EN_CONN_IDLE);
						break;

					case EN_LINE_ERR_MODEM:
						strError = pApp->m_stConStatus.strModemOff;
						pApp->SetConnectionState(EN_CONN_IDLE);
						break;
				}
			}
		}

	}

	if ((pApp->GetReconnect() && m_iReconnectStat == 0) || m_iReconnectStat == 1)
	{
		m_iReconnectStat = 1;
		if (pApp->GetConnectionState() != EN_CONN_CONNECTED)
		{
			strErrorStatusLine = pApp->m_stConStatus.strReconnect;
		}
	}

//	if (m_strConnState != strState)
	{
		iIdxConnState = m_wndStatusBar.CommandToIndex(ID_CONN_STATE);
		m_wndStatusBar.SetPaneText(iIdxConnState, strState);
		m_strConnState = strState;
	}

	if (!strError.IsEmpty() && m_strConnError != strError)
	{
		iIdxConnError = m_wndStatusBar.CommandToIndex(ID_CONN_ERROR);
		m_wndStatusBar.SetPaneText(iIdxConnError, strErrorStatusLine);
		m_strConnError = strErrorStatusLine;
		pApp->m_bUserInput = FALSE;
	}

	if (m_strConnLevel != strLevel)
	{
		iIdxConnLevel = m_wndStatusBar.CommandToIndex(ID_CONN_LEVEL);
		m_wndStatusBar.SetPaneText(iIdxConnLevel, strLevel);
		m_strConnLevel = strLevel;
	}

	if (!pApp->m_bMessage && bMessageBox)
	{
		CString strMsg;

		pApp->m_bMessage = TRUE;
		strMsg.Format(_T("%s\n%s"), pApp->m_stConStatus.strMessage, strError);
		AfxMessageBox(strMsg);
	}

//	m_wndStatusBar.SetPaneText(0, _T(""));


	CMDIFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::ConfigInitSend(CString strCompany, CString strLocType, CString strLocNumber, 
					CString strMotorType, CString strMotorNumber, CString strMtuOrderNumber, 
					CString strDisplayTelNumber, CString strInhibitTime,
					CString strTelNumberSMS1, CString strTelNumberSMS2, CString strTelNumberSMS3,
					CString strReturnStartPageTime,
					CString strPin, CString strPuk)
{
	CString strMsg;
	int		iLen;
	DWORD	dwBytesMsg;

	strMsg = strCompany + _T(",") +
			 strLocType + _T(",") +
			 strLocNumber + _T(",") +
			 strMotorType + _T(",") +
			 strMotorNumber + _T(",") +
			 strMtuOrderNumber + _T(",") +
			 strDisplayTelNumber + _T(",") +
			 strInhibitTime + _T(",") +
			 strTelNumberSMS1 + _T(",") +
			 strTelNumberSMS2 + _T(",") +
			 strTelNumberSMS3 + _T(",") +
			 strReturnStartPageTime + _T(",") +
			 strPin + _T(",") +
			 strPuk;
	// pin-nummer steht immer an der zweitletzten Stelle !!!
	// puk-nummer steht immer an der letzten Stelle !!!

	iLen = strMsg.GetLength();
	CD_GenerateDataMsg(m_tszSendMsg, &dwBytesMsg, CD_MSG_INIT_CONFIG, strMsg.GetBuffer(1), iLen);
	strMsg.ReleaseBuffer();

	SendSerialData(m_tszSendMsg);
	m_bSendMsg = TRUE;
}

void CMainFrame::ConfigReceive()
{
	DWORD	dwBytesMsg;

	CD_GenerateDataHeader(m_tszSendMsg, &dwBytesMsg, CD_MSG_CONFIG);
	CD_GenerateDataTail(m_tszSendMsg, &dwBytesMsg);

	SendSerialData(m_tszSendMsg);
	m_bSendMsg = TRUE;
}


void CMainFrame::OnSelendokCmLanguage() 
{
	// TODO: Add your control notification handler code here
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CComboBoxEx*	pCBoxEx = (CComboBoxEx*)m_wndDlgBar.GetDlgItem(IDC_COMB_EX_LANG);
	COMBOBOXEXITEM	CombItem;

	if (pCBoxEx)
	{
		int iSel = pCBoxEx->GetCurSel();
			
		CombItem.mask = CBEIF_LPARAM;
		CombItem.iItem = iSel;

		if (pCBoxEx->GetItem(&CombItem))
		{
			TRACE(_T("Language changed to %d \n"), CombItem.lParam);
			pApp->SetLang(CombItem.lParam);

			SendLang(CombItem.lParam);

		}
	}

	SetMenuText(TRUE);

	SetFocus();
}

BOOL CMainFrame::SetMenuText(BOOL bDoMenuChange)
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	BOOL			bRet = FALSE;
	int				iMenuBase = 0;

	// Menü umbenennen
	CMenu* pMenu = GetMenu();
	if (pMenu)
	{
		if (pMenu->GetMenuItemCount() > 6)
		{
			iMenuBase = 1;
		}

		pMenu->GetMenuItemCount();
		CString		str, str0, str1, str2, str3, str4, str5;
		pMenu->GetMenuString(0, str, MF_BYPOSITION);
		if (str == _T("FileX"))
		{
			bDoMenuChange = TRUE;
		}

		if (!bDoMenuChange)
		{
			pMenu->GetMenuString(iMenuBase+0, str0, MF_BYPOSITION);
			pMenu->GetMenuString(iMenuBase+1, str1, MF_BYPOSITION);
			pMenu->GetMenuString(iMenuBase+2, str2, MF_BYPOSITION);
			pMenu->GetMenuString(iMenuBase+3, str3, MF_BYPOSITION);
			pMenu->GetMenuString(iMenuBase+4, str4, MF_BYPOSITION);
			pMenu->GetMenuString(iMenuBase+5, str5, MF_BYPOSITION);
			if (str0 != pApp->m_stMenu.strFile ||
				str1 != pApp->m_stMenu.strConnection ||
				str2 != pApp->m_stMenu.strData ||
				str3 != pApp->m_stMenu.strView ||
				str4 != pApp->m_stMenu.strWindow ||
				str5 != pApp->m_stMenu.strHelp)
			{
				bDoMenuChange = TRUE;
			}
		}

		if (bDoMenuChange)
		{
			for (int i = iMenuBase; i < (int)pMenu->GetMenuItemCount(); i++)
			{
				if (pMenu->GetMenuItemID(i) != 0)
				{
	//				TRACE(_T("Menu Item %3.3d Text %s\n"), i, str);

					switch(i - iMenuBase)
					{
					case 0:
						pMenu->ModifyMenu(i, MF_BYPOSITION, MF_POPUP, pApp->m_stMenu.strFile);
						break;
					case 1:
						pMenu->ModifyMenu(i, MF_BYPOSITION, MF_POPUP, pApp->m_stMenu.strConnection);
						break;
					case 2:
						pMenu->ModifyMenu(i, MF_BYPOSITION, MF_POPUP, pApp->m_stMenu.strData);
						break;
					case 3:
						pMenu->ModifyMenu(i, MF_BYPOSITION, MF_POPUP, pApp->m_stMenu.strView);
						break;
					case 4:
						pMenu->ModifyMenu(i, MF_BYPOSITION, MF_POPUP, pApp->m_stMenu.strWindow);
						break;
					case 5:
						pMenu->ModifyMenu(i, MF_BYPOSITION, MF_POPUP, pApp->m_stMenu.strHelp);
						break;
					}
				}
			}

			// file
			pMenu->ModifyMenu(ID_FILE_OPEN, MF_BYCOMMAND, ID_FILE_OPEN, pApp->m_stMenu.stMnuFile.strOpen);
			pMenu->ModifyMenu(ID_FILE_CLOSE, MF_BYCOMMAND, ID_FILE_CLOSE, pApp->m_stMenu.stMnuFile.strClose);
			pMenu->ModifyMenu(ID_FILE_SAVE, MF_BYCOMMAND, ID_FILE_SAVE, pApp->m_stMenu.stMnuFile.strSave);
			pMenu->ModifyMenu(ID_FILE_SAVE_AS, MF_BYCOMMAND, ID_FILE_SAVE_AS, pApp->m_stMenu.stMnuFile.strSaveAs);
			pMenu->ModifyMenu(ID_FILE_EXPORT, MF_BYCOMMAND, ID_FILE_EXPORT, pApp->m_stMenu.stMnuFile.strExport);
			pMenu->ModifyMenu(ID_FILE_PRINT, MF_BYCOMMAND, ID_FILE_PRINT, pApp->m_stMenu.stMnuFile.strPrint);
			pMenu->ModifyMenu(ID_FILE_PRINT_PREVIEW, MF_BYCOMMAND, ID_FILE_PRINT_PREVIEW, pApp->m_stMenu.stMnuFile.strPrintPreview);
			pMenu->ModifyMenu(ID_FILE_PRINT_SETUP, MF_BYCOMMAND, ID_FILE_PRINT_SETUP, pApp->m_stMenu.stMnuFile.strPrintSettings);
			pMenu->ModifyMenu(ID_FILE_MRU_FILE1, MF_BYCOMMAND, ID_FILE_MRU_FILE1, pApp->m_stMenu.stMnuFile.strLast);
			pMenu->ModifyMenu(ID_APP_EXIT, MF_BYCOMMAND, ID_APP_EXIT, pApp->m_stMenu.stMnuFile.strExit);
			
			// verbindung
			pMenu->ModifyMenu(ID_CONN_TYPE_SERIAL, MF_BYCOMMAND, ID_CONN_TYPE_SERIAL, pApp->m_stMenu.stMnuCon.strConSerial);
			pMenu->ModifyMenu(ID_CONN_TYPE_MODEM, MF_BYCOMMAND, ID_CONN_TYPE_MODEM, pApp->m_stMenu.stMnuCon.strConModem);
			pMenu->ModifyMenu(ID_COM_CONNECT, MF_BYCOMMAND, ID_COM_CONNECT, pApp->m_stMenu.stMnuCon.strConnect);
			pMenu->ModifyMenu(ID_COM_DISCONNECT, MF_BYCOMMAND, ID_COM_DISCONNECT, pApp->m_stMenu.stMnuCon.strDisconnect);
			pMenu->ModifyMenu(ID_COM_SETTINGS, MF_BYCOMMAND, ID_COM_SETTINGS, pApp->m_stMenu.stMnuCon.strSettings);
			pMenu->ModifyMenu(ID_PHONE_BOOK, MF_BYCOMMAND, ID_PHONE_BOOK, pApp->m_stMenu.stMnuCon.strPhoneBook);
			pMenu->ModifyMenu(ID_FLEET_MGR, MF_BYCOMMAND, ID_FLEET_MGR, pApp->m_stMenu.stMnuCon.strConSerial);//DJ
			pMenu->ModifyMenu(ID_FLEET_EDIT, MF_BYCOMMAND, ID_FLEET_EDIT, pApp->m_stMenu.stMnuCon.strConSerial);//DJ

			// daten
			pMenu->ModifyMenu(ID_NEW_BIN_DATA, MF_BYCOMMAND, ID_NEW_BIN_DATA, pApp->m_stMenu.stMnuData.strBinary);
			pMenu->ModifyMenu(ID_NEW_ANA_DATA, MF_BYCOMMAND, ID_NEW_ANA_DATA, pApp->m_stMenu.stMnuData.strAnalog);
			pMenu->ModifyMenu(ID_NEW_ALARM_DATA, MF_BYCOMMAND, ID_NEW_ALARM_DATA, pApp->m_stMenu.stMnuData.strAlarm);
			pMenu->ModifyMenu(ID_NEW_DURATION_DATA, MF_BYCOMMAND, ID_NEW_DURATION_DATA, pApp->m_stMenu.stMnuData.strDuration);
			pMenu->ModifyMenu(ID_NEW_HIST_DATA, MF_BYCOMMAND, ID_NEW_HIST_DATA, pApp->m_stMenu.stMnuData.strHistory);
			pMenu->ModifyMenu(ID_NEW_HIST20_DATA, MF_BYCOMMAND, ID_NEW_HIST20_DATA, pApp->m_stMenu.stMnuData.strHistory20);
			pMenu->ModifyMenu(ID_NEW_HIST100_DATA, MF_BYCOMMAND, ID_NEW_HIST100_DATA, pApp->m_stMenu.stMnuData.strHistory100);
			pMenu->ModifyMenu(ID_NEW_HIST2_DATA, MF_BYCOMMAND, ID_NEW_HIST2_DATA, pApp->m_stMenu.stMnuData.strHistory2);
			pMenu->ModifyMenu(ID_LOC_SETTINGS, MF_BYCOMMAND, ID_LOC_SETTINGS, pApp->m_stMenu.stMnuData.strConfigSettings);
			pMenu->ModifyMenu(ID_LOC_SETTINGS_SEND, MF_BYCOMMAND, ID_LOC_SETTINGS_SEND, pApp->m_stMenu.stMnuData.strConfigSend);
			pMenu->ModifyMenu(ID_RELOAD_DATA, MF_BYCOMMAND, ID_RELOAD_DATA, pApp->m_stMenu.stMnuData.strReload);

			// ansicht
			pMenu->ModifyMenu(ID_VIEW_TOOLBAR, MF_BYCOMMAND, ID_VIEW_TOOLBAR, pApp->m_stMenu.stMnuView.strSymbolBar);
			pMenu->ModifyMenu(ID_VIEW_STATUS_BAR, MF_BYCOMMAND, ID_VIEW_STATUS_BAR, pApp->m_stMenu.stMnuView.strStatusBar);

			// fenster
			pMenu->ModifyMenu(ID_WINDOW_NEW, MF_BYCOMMAND, ID_WINDOW_NEW, pApp->m_stMenu.stMnuWnd.strNew);
			pMenu->ModifyMenu(ID_WINDOW_CASCADE, MF_BYCOMMAND, ID_WINDOW_CASCADE, pApp->m_stMenu.stMnuWnd.strCascade);
			pMenu->ModifyMenu(ID_WINDOW_TILE_HORZ, MF_BYCOMMAND, ID_WINDOW_TILE_HORZ, pApp->m_stMenu.stMnuWnd.strTile);
			pMenu->ModifyMenu(ID_WINDOW_ARRANGE, MF_BYCOMMAND, ID_WINDOW_ARRANGE, pApp->m_stMenu.stMnuWnd.strArrange);

			// hilfe
			pMenu->ModifyMenu(ID_HELP_FINDER, MF_BYCOMMAND, ID_HELP_FINDER, pApp->m_stMenu.stMnuHelp.strFinder);
			pMenu->ModifyMenu(ID_APP_ABOUT, MF_BYCOMMAND, ID_APP_ABOUT, pApp->m_stMenu.stMnuHelp.strAbout);

			DrawMenuBar();
			bRet = TRUE;
		}
	}

	return bRet;
}


void CMainFrame::ReconnectReceive()
{

	DWORD	dwBytesMsg;

	if (m_bSendMsg)
	{
		SendSerialData(m_tszSendMsg);
	}
	else
	{
		CD_GenerateDataHeader(m_tszSendMsg, &dwBytesMsg, CD_MSG_LINE_TEST);
		CD_GenerateDataTail(m_tszSendMsg, &dwBytesMsg);

		SendSerialData(m_tszSendMsg);
		m_bSendMsg = TRUE;
	}
}

void CMainFrame::SetLangImageList()
{
	// Imagelist erzeugen
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CBitmap bitmap, *pBmp;
	HBITMAP hBitmap = NULL;
	CString	strFile;

	int     i, iLangIdx;

	m_imageList.DeleteImageList();
	m_imageList.Create(30, 15, ILC_COLOR, 2, 2);

	for (i = 0; i < pApp->m_uiarrLangOrder.GetSize(); i++)
	{
		iLangIdx = pApp->m_uiarrLangOrder.GetAt(i);

		if (iLangIdx >= 0 && iLangIdx < pApp->m_strarrLangIcon.GetSize())
		{
			strFile = pApp->m_strLangIconPath + _T("\\") + pApp->m_strarrLangIcon.GetAt(iLangIdx);
			hBitmap = (HBITMAP)LoadImage(NULL, strFile, IMAGE_BITMAP, 30, 15, 
				LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
			if (hBitmap != NULL)
			{
				pBmp = bitmap.FromHandle(hBitmap);
				if (pBmp != NULL)
				{
					m_imageList.Add(pBmp, (COLORREF)0xFFFFFF);
					pBmp->DeleteObject();
				}
				DeleteObject(hBitmap);
			}
			else
			{
				CString	strMsg;

				strMsg.Format(_T("Could not open Image:\n%s\n\nPlease edit RailDiag.ini!"), strFile);
				AfxMessageBox(strMsg, MB_OK|MB_ICONSTOP);
			}
		}
		else
		{
			CString	strMsg;

			strMsg.Format(_T("Wrong number of languages\n\nPlease correct RailDiag.ini!"));
			AfxMessageBox(strMsg, MB_OK|MB_ICONSTOP);
		}
	}
}

void CMainFrame::SetLangList(CUIntArray *puiarrLangSel)
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CComboBoxEx*	pCBoxEx = (CComboBoxEx*)m_wndDlgBar.GetDlgItem(IDC_COMB_EX_LANG);
	int				i;

	if (pCBoxEx)
	{
		for (i = 0; i < pApp->m_uiarrLangOrder.GetSize(); i++)
		{
			pCBoxEx->DeleteItem(0);
		}

		pApp->SetLangOrder(puiarrLangSel);

		// creating image list
		SetLangImageList();
		pCBoxEx->SetImageList(&m_imageList); //, LVSIL_SMALL);
//		pCBoxEx->SetCurSel(0);

		COMBOBOXEXITEM	CombItem;
		CString			strName;
		int				iLangIdx;
		int				iLangSel = -1;

		for (i = 0; i < pApp->m_uiarrLangOrder.GetSize(); i++)
		{
			CombItem.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_LPARAM;

			iLangIdx = pApp->m_uiarrLangOrder.GetAt(i);

			CombItem.iItem = i;

			if (iLangIdx < pApp->m_strarrLangName.GetSize())
			{
				strName = pApp->m_strarrLangName.GetAt(iLangIdx);
				CombItem.pszText = strName.GetBuffer(1);
				strName.ReleaseBuffer();
				CombItem.cchTextMax = strName.GetLength();
			}
			else
			{
				CombItem.pszText = _T("???");
				CombItem.cchTextMax = 3;
			}
			CombItem.iImage = i;
			CombItem.iSelectedImage = i;
			CombItem.iOverlay = 0;
			CombItem.iIndent = 0;
			if (iLangIdx < pApp->m_uiarrLangNr.GetSize())
			{
				CombItem.lParam = pApp->m_uiarrLangNr.GetAt(iLangIdx);
			}
			else
			{
				CombItem.lParam = 0;
			}

			if (pApp->m_iLang == CombItem.lParam)
			{
				iLangSel = i;
				SendLang(pApp->m_iLang);
			}

			pCBoxEx->InsertItem(&CombItem);
		}

		if (iLangSel != -1)
		{
			pCBoxEx->SetCurSel(iLangSel);
		}

		SetFocus();

	}
}

void CMainFrame::EnableFleetList(BOOL bEnable)
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CComboBoxEx*	pCBoxEx = (CComboBoxEx*)m_wndDlgBarFleet.GetDlgItem(IDC_COMB_EX_FLEET);

	if (pCBoxEx)
	{
		pCBoxEx->EnableWindow(bEnable);
	}
}

void CMainFrame::SetFleetList(CStringArray &strarrFleetNames)
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CComboBoxEx*	pCBoxEx = (CComboBoxEx*)m_wndDlgBarFleet.GetDlgItem(IDC_COMB_EX_FLEET);
	int				i;
	int				iFleetSel = 0;
	CString			strSel;

	if (pCBoxEx)
	{
		COMBOBOXEXITEM	CombItem;
		CString			strName;

		iFleetSel = pCBoxEx->GetCurSel();
		if (iFleetSel >= 0 && pCBoxEx->GetCount())
		{
			pCBoxEx->GetLBText(iFleetSel, strSel);
		}

		for (i = pCBoxEx->GetCount() - 1; i >= 0 ; i--)
		{
			pCBoxEx->DeleteItem(i);
		}

		// creating image list

		iFleetSel = 0;
		for (i = 0; i < strarrFleetNames.GetSize(); i++)
		{
			CombItem.mask = CBEIF_TEXT;

			CombItem.iItem = i;

			strName = strarrFleetNames.GetAt(i);
			CombItem.pszText = strName.GetBuffer(1);
			strName.ReleaseBuffer();
			CombItem.cchTextMax = strName.GetLength();
			CombItem.iOverlay = 0;
			CombItem.iIndent = 0;

			if (strSel == strName)
			{
				iFleetSel = i;
			}
			pCBoxEx->InsertItem(&CombItem);
		}

		if (iFleetSel != -1)
		{
			pCBoxEx->SetCurSel(iFleetSel);
			pApp->m_iFleetSelected = iFleetSel;
			pApp->m_strFleetName = strarrFleetNames.GetAt(iFleetSel);
		}
		else
		{
			pApp->m_iFleetSelected = -1;
			pApp->m_strFleetName = _T("-Not-Found-");
		}

//		SetFocus();
	}
}

void CMainFrame::SendLang(int iLang)
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	TCHAR			tszSendMsg[256];
	DWORD			dwCharsMsg;

	if (pApp->m_bDataIn)
	{
		CD_GenerateDataHeader(tszSendMsg, &dwCharsMsg, CD_MSG_SET_LANG);
		CD_GenerateDataUINT32(tszSendMsg, &dwCharsMsg, iLang);
		dwCharsMsg = dwCharsMsg - 1; // Komma nach Wert wieder entfernen
		CD_GenerateDataTail(tszSendMsg, &dwCharsMsg);
		tszSendMsg[dwCharsMsg] = '\0';
		SendSerialData(tszSendMsg);
		m_bSendMsg = TRUE;
	}
}

void CMainFrame::ClearConnError()
{
	int	iIdxConnError;

	m_iReconnectStat = 0;
	m_strConnError = _T(" ");
	iIdxConnError = m_wndStatusBar.CommandToIndex(ID_CONN_ERROR);
	m_wndStatusBar.SetPaneText(iIdxConnError, m_strConnError);
}

void CMainFrame::SetConnError(CString strError)
{
	int	iIdxConnError;

	m_iReconnectStat = 0;
	m_strConnError = strError;
	iIdxConnError = m_wndStatusBar.CommandToIndex(ID_CONN_ERROR);
	m_wndStatusBar.SetPaneText(iIdxConnError, strError);
}

BOOL CMainFrame::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_iNoifyState != -1)
	{
		NOTIFYICONDATA	NotifyIconData;

		NotifyIconData.cbSize = sizeof(NotifyIconData);
		NotifyIconData.hWnd = this->m_hWnd;
		NotifyIconData.uID = 0;
		NotifyIconData.uFlags = 0;

		Shell_NotifyIcon(NIM_DELETE, &NotifyIconData);

		m_iNoifyState = -1;
	}
	
	return CMDIFrameWnd::DestroyWindow();
}

void CMainFrame::NotifyConnected()
{
#ifdef NOTYFY_IMPL
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();

	if (m_iNoifyState != -1 && m_iNoifyState != 2)
	{
		NOTIFYICONDATA	NotifyIconData;

		NotifyIconData.cbSize = sizeof(NotifyIconData);
		NotifyIconData.hWnd = this->m_hWnd;
		NotifyIconData.uID = 0;
		NotifyIconData.uFlags = NIF_ICON | NIF_TIP;
		NotifyIconData.hIcon = LoadIcon(pApp->m_hInstance, MAKEINTRESOURCE(IDI_CONNECT));
		_tcscpy(NotifyIconData.szTip, _T("RailDiag connected"));

		Shell_NotifyIcon(NIM_MODIFY, &NotifyIconData);

		m_iNoifyState = 2;
	}
#endif
}

void CMainFrame::NotifyDisconnected()
{
#ifdef NOTYFY_IMPL
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();

	if (m_iNoifyState != -1 && m_iNoifyState != 1)
	{
		NOTIFYICONDATA	NotifyIconData;

		NotifyIconData.cbSize = sizeof(NotifyIconData);
		NotifyIconData.hWnd = this->m_hWnd;
		NotifyIconData.uID = 0;
		NotifyIconData.uFlags = NIF_ICON | NIF_TIP;
		NotifyIconData.hIcon = LoadIcon(pApp->m_hInstance, MAKEINTRESOURCE(IDI_DISCONNECT));
		_tcscpy(NotifyIconData.szTip, _T("RailDiag disonnected"));

		Shell_NotifyIcon(NIM_MODIFY, &NotifyIconData);

		m_iNoifyState = 1;
	}
#endif
}

void CMainFrame::SendStopCmdNoData()
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	DWORD			dwBytesMsg = 0;

	m_iSendType = 0;
	m_InOutList.DeInit();

	TRACE(_T("CMainFrame::SendStopCmd Stop Line \n"));
	CD_GenerateDataHeader(m_tszSendMsg, &dwBytesMsg, CD_MSG_STOP);
	CD_GenerateDataTail(m_tszSendMsg, &dwBytesMsg);

	SendSerialData(m_tszSendMsg);
	m_bSendMsg = TRUE;

	if (!pApp->m_bMessage)
	{
		pApp->Message(pApp->m_stConStatus.strNoData);
	}
}

void CMainFrame::SendStopCmdNoAccess()
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	DWORD			dwBytesMsg = 0;

	m_iSendType = 0;
	m_InOutList.DeInit();

	TRACE(_T("CMainFrame::SendStopCmd Stop Line \n"));
	CD_GenerateDataHeader(m_tszSendMsg, &dwBytesMsg, CD_MSG_STOP);
	CD_GenerateDataTail(m_tszSendMsg, &dwBytesMsg);

	SendSerialData(m_tszSendMsg);
	m_bSendMsg = TRUE;

	if (!pApp->m_bMessage)
	{
		pApp->Message(pApp->m_stConStatus.strNoAccess);
	}
}

void CMainFrame::SendNextList()
{
	BOOL		bNext;
	int			iLine;
	int			iLen;
	int			i;
	CString		strCount;
	DWORD		dwBytesMsg;

	for (bNext = TRUE, i = 0; bNext; i++)
	{
		m_InOutList.GetNext2Send(&iLine, &bNext);

		if (iLine > 0)
		{
			strCount.Format(_T("%d"), iLine);
			TRACE(_T("Get Line %d\n"), iLine);
			iLen = strCount.GetLength();
			CD_GenerateDataMsg(m_tszSendMsg, &dwBytesMsg, m_iSendType, strCount.GetBuffer(1), iLen);
			strCount.ReleaseBuffer();

			SendSerialData(m_tszSendMsg);
			m_bSendMsg = FALSE;
		}
	}
}

void CMainFrame::SetProgressLineCount(int iLineTotal)
{
	CRailDiagDoc	*pDoc = m_pDoc;

	if (m_pDlgProg && pDoc)
	{
		int		iLineCount;

//		OutputDebugString(_T("--> Create Progress-Dialog\n"));

		switch (pDoc->m_eDataType)
		{
		case EN_DATA_TYPE_HIST20:
			iLineCount =  min(20, iLineTotal);
			break;
		case EN_DATA_TYPE_HIST100:
			iLineCount =  min(100, iLineTotal);
			break;
		default:
			iLineCount = iLineTotal;
			break;
		}

		if (iLineCount < 1)
		{
			iLineCount = 1;
		}
		m_pDlgProg->SetRange(0, iLineCount);
	}
}

#undef __FUNC__
#define __FUNC__ _T("CMainFrame::OnNotifyNeedText")
BOOL CMainFrame::OnNotifyNeedText(UINT id, NMHDR *pNMHDR, LRESULT *result)
{
	// Tooltip texte sprachabhängig ausgeben
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
    TOOLTIPTEXT		*pTTT = (TOOLTIPTEXT *)pNMHDR;
	NMTTDISPINFO    *pTTDispInfo = (NMTTDISPINFO *)pNMHDR;

	TRACE(_T("%s : requesting tool tip text\n"), __FUNC__);

    if (pTTT->uFlags & TTN_NEEDTEXTW)
    {
		// tooltip auf mehrzeilig umstellen, zusätzlich maximale länge
		::SendMessage(pNMHDR->hwndFrom, TTM_SETMAXTIPWIDTH, 0 , pApp->m_dwToolTipWidth);

		// tooltip timout einstellen
		::SendMessage(pNMHDR->hwndFrom, TTM_SETDELAYTIME, 
			(WPARAM)(DWORD) TTDT_AUTOPOP, 
			(LPARAM)(INT)pApp->m_dwToolTipOn);
		switch(pNMHDR->idFrom)
		{

		// toolbar
		case ID_FILE_OPEN:
			pTTT->lpszText = pApp->m_stMenu.stMnuFile.strOpen.GetBuffer(1);
			pApp->m_stMenu.stMnuFile.strOpen.ReleaseBuffer();
			break;
		case ID_FILE_SAVE:
			pTTT->lpszText = pApp->m_stMenu.stMnuFile.strSave.GetBuffer(1);
			pApp->m_stMenu.stMnuFile.strSave.ReleaseBuffer();
			break;
		case ID_FILE_EXPORT:
			pTTT->lpszText = pApp->m_stMenu.stMnuFile.strExport.GetBuffer(1);
			pApp->m_stMenu.stMnuFile.strExport.ReleaseBuffer();
			break;
		case ID_FILE_PRINT:
			pTTT->lpszText = pApp->m_stMenu.stMnuFile.strPrint.GetBuffer(1);
			pApp->m_stMenu.stMnuFile.strPrint.ReleaseBuffer();
			break;

		case ID_CONN_TYPE_SERIAL:
			pTTT->lpszText = pApp->m_stMenu.stMnuCon.strConSerial.GetBuffer(1);
			pApp->m_stMenu.stMnuCon.strConSerial.ReleaseBuffer();
			break;
		case ID_COM_DISCONNECT_SER:
			pTTT->lpszText = pApp->m_stMenu.stMnuCon.strDisconnect.GetBuffer(1);
			pApp->m_stMenu.stMnuCon.strDisconnect.ReleaseBuffer();
			break;
		case ID_COM_STATE:
			pTTT->lpszText = pApp->m_stMenu.stMnuCon.strState.GetBuffer(1);
			pApp->m_stMenu.stMnuCon.strState.ReleaseBuffer();
			break;
		case ID_CONN_TYPE_MODEM:
			pTTT->lpszText = pApp->m_stMenu.stMnuCon.strConModem.GetBuffer(1);
			pApp->m_stMenu.stMnuCon.strConModem.ReleaseBuffer();
			break;
		case ID_COM_DISCONNECT_MODEM:
			pTTT->lpszText = pApp->m_stMenu.stMnuCon.strDisconnect.GetBuffer(1);
			pApp->m_stMenu.stMnuCon.strDisconnect.ReleaseBuffer();
			break;

		case ID_NEW_BIN_DATA:
			pTTT->lpszText = pApp->m_stMenu.stMnuData.strBinary.GetBuffer(1);
			pApp->m_stMenu.stMnuData.strBinary.ReleaseBuffer();
			break;
		case ID_NEW_ANA_DATA:
			pTTT->lpszText = pApp->m_stMenu.stMnuData.strAnalog.GetBuffer(1);
			pApp->m_stMenu.stMnuData.strAnalog.ReleaseBuffer();
			break;
		case ID_NEW_ALARM_DATA:
			pTTT->lpszText = pApp->m_stMenu.stMnuData.strAlarm.GetBuffer(1);
			pApp->m_stMenu.stMnuData.strAlarm.ReleaseBuffer();
			break;
		case ID_NEW_DURATION_DATA:
			pTTT->lpszText = pApp->m_stMenu.stMnuData.strDuration.GetBuffer(1);
			pApp->m_stMenu.stMnuData.strDuration.ReleaseBuffer();
			break;
		case ID_NEW_HIST_DATA:
			pTTT->lpszText = pApp->m_stMenu.stMnuData.strHistory.GetBuffer(1);
			pApp->m_stMenu.stMnuData.strHistory.ReleaseBuffer();
			break;
		case ID_NEW_HIST20_DATA:
			pTTT->lpszText = pApp->m_stMenu.stMnuData.strHistory20.GetBuffer(1);
			pApp->m_stMenu.stMnuData.strHistory20.ReleaseBuffer();
			break;
		case ID_NEW_HIST100_DATA:
			pTTT->lpszText = pApp->m_stMenu.stMnuData.strHistory100.GetBuffer(1);
			pApp->m_stMenu.stMnuData.strHistory100.ReleaseBuffer();
			break;
		case ID_NEW_HIST2_DATA:
			pTTT->lpszText = pApp->m_stMenu.stMnuData.strHistory2.GetBuffer(1);
			pApp->m_stMenu.stMnuData.strHistory2.ReleaseBuffer();
			break;
		case ID_RELOAD_DATA:
			pTTT->lpszText = pApp->m_stMenu.stMnuData.strReload.GetBuffer(1);
			pApp->m_stMenu.stMnuData.strReload.ReleaseBuffer();
			break;

		case ID_PHONE_BOOK:
			pTTT->lpszText = pApp->m_stMenu.stMnuCon.strPhoneBook.GetBuffer(1);
			pApp->m_stMenu.stMnuCon.strPhoneBook.ReleaseBuffer();
			break;
			break;

		case ID_COM_SETTINGS:
			pTTT->lpszText = pApp->m_stMenu.stMnuCon.strSettings.GetBuffer(1);
			pApp->m_stMenu.stMnuCon.strSettings.ReleaseBuffer();
			break;
		case ID_LOC_SETTINGS:
			pTTT->lpszText = pApp->m_stMenu.stMnuData.strConfigSettings.GetBuffer(1);
			pApp->m_stMenu.stMnuData.strConfigSettings.ReleaseBuffer();
			break;
		case ID_APP_ABOUT:
			pTTT->lpszText = pApp->m_stMenu.stMnuHelp.strAbout.GetBuffer(1);
			pApp->m_stMenu.stMnuHelp.strAbout.ReleaseBuffer();
			break;

		case ID_FLEET_MGR://DJ
			pTTT->lpszText = pApp->m_stMenu.stMnuFleet.strFleetMgr.GetBuffer(1);
			pApp->m_stMenu.stMnuFleet.strFleetMgr.ReleaseBuffer();
			break;

		case ID_FLEET_EDIT://EDIT
			pTTT->lpszText = pApp->m_stMenu.stMnuFleet.strFleetEdit.GetBuffer(1);
			pApp->m_stMenu.stMnuFleet.strFleetEdit.ReleaseBuffer();
			break;

		// statusbar
		case 0: // ID_CONN_STATE
		case 1: // ID_CONN_ERROR
		case 2: // ID_CONN_LEVEL 
		case 3:
			switch(m_wndStatusBar.GetItemID(pNMHDR->idFrom))
			{
			case ID_CONN_STATE:
				pTTT->lpszText = m_strConnState.GetBuffer(1);
				m_strConnState.ReleaseBuffer();
				break;
			case ID_CONN_ERROR:
				pTTT->lpszText = m_strConnError.GetBuffer(1);
				m_strConnError.ReleaseBuffer();
				break;
			case ID_CONN_LEVEL:
				pTTT->lpszText = m_strConnLevel.GetBuffer(1);
				m_strConnLevel.ReleaseBuffer();
				break;
			}
//			m_wndStatusBar.SetWindowText(_T("hallo"));
			break;
		default:
			break;

		}
        pTTT->hinst = AfxGetResourceHandle();
        return(TRUE);
    }

	return (FALSE);
}



void CMainFrame::SendDataLineCancel()
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CRailDiagDoc	*pDoc = m_pDoc;
	int				iLine;

	TCHAR			tszData[1024] = _T("Transfer stopped,Transfer stopped,");

	iLine = m_iLineCount;
	if (pDoc)
	{
		pDoc->m_bCancel = TRUE;
		_stprintf(tszData, _T("TRANSFER_CANCEL,%s,"), pApp->m_stConStatus.strTransferCancel);
		pDoc->NewErrorHistData(tszData, &iLine);

//		PostMessage(WM_USER_NEW_SERIAL_DATA);
		if (m_pDoc && !m_pDoc->IsPrinting())
		{
			m_pDoc->UpdateAllViews(m_pDoc->GetStaticView());
			
			if (pDoc)
			{
				pDoc->m_bUpdateView = TRUE;
			}
		}
	}
}

void CMainFrame::ResetDataCount()
{
	m_iLineCount = 1;
}

void CMainFrame::OnCbnSelendokCombExFleet()
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CComboBoxEx*	pCBoxEx = (CComboBoxEx*)m_wndDlgBarFleet.GetDlgItem(IDC_COMB_EX_FLEET);
	int				iFleetSel = 0;
	CString			strSel;

	if (pCBoxEx)
	{
		iFleetSel = pCBoxEx->GetCurSel();
		if (iFleetSel >= 0 && pCBoxEx->GetCount())
		{
			pCBoxEx->GetLBText(iFleetSel, strSel);
			pApp->m_strFleetName = strSel;
			pApp->m_iFleetSelected = iFleetSel;
		}
	}
}
