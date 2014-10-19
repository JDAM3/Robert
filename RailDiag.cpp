// RailDiag.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "RailDiag.h"
#include "DlgProgress.h"

#include "ComLib.h"
#include "ComDiag.h"
#include "Line.h"
#include "Util.h"
#include "FileIo.h"
#include "InOutList.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "RailDiagDoc.h"
#include "RailDiagView.h"
#include "StateView.h"
#include "splitter.h"
#include "ComSettings.h"
#include "ModemSettings.h"
#include "LocSettings.h"
#include "PhoneBook.h"

#include "DlgFleetEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BYTE			g_bySerReadBuf[1024];
DWORD			g_dwSerReadBufPos;
static BYTE		g_byPort = 0;
static DWORD	g_dwLine = 0;

/////////////////////////////////////////////////////////////////////////////
// CRailDiagApp

BEGIN_MESSAGE_MAP(CRailDiagApp, CWinApp)
	//{{AFX_MSG_MAP(CRailDiagApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_COM_CONNECT, OnComConnect)
	ON_UPDATE_COMMAND_UI(ID_COM_CONNECT, OnUpdateComConnect)
	ON_COMMAND(ID_COM_DISCONNECT, OnComDisconnect)
	ON_UPDATE_COMMAND_UI(ID_COM_DISCONNECT, OnUpdateComDisconnect)
	ON_COMMAND(ID_COM_SETTINGS, OnComSettings)
	ON_UPDATE_COMMAND_UI(ID_COM_SETTINGS, OnUpdateComSettings)
	ON_COMMAND(ID_CONN_TYPE_SERIAL, OnConnTypeSerial)
	ON_UPDATE_COMMAND_UI(ID_CONN_TYPE_SERIAL, OnUpdateConnTypeSerial)
	ON_COMMAND(ID_CONN_TYPE_MODEM, OnConnTypeModem)
	ON_UPDATE_COMMAND_UI(ID_CONN_TYPE_MODEM, OnUpdateConnTypeModem)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileNew)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_UPDATE_COMMAND_UI(ID_NEW_BIN_DATA, OnUpdateNewBinData)
	ON_COMMAND(ID_NEW_BIN_DATA, OnNewBinData)
	ON_UPDATE_COMMAND_UI(ID_NEW_ANA_DATA, OnUpdateNewAnaData)
	ON_COMMAND(ID_NEW_ANA_DATA, OnNewAnaData)
	ON_UPDATE_COMMAND_UI(ID_NEW_ALARM_DATA, OnUpdateNewAlarmData)
	ON_COMMAND(ID_NEW_ALARM_DATA, OnNewAlarmData)
	ON_UPDATE_COMMAND_UI(ID_NEW_HIST_DATA, OnUpdateNewHistData)
	ON_COMMAND(ID_NEW_HIST_DATA, OnNewHistData)
	ON_UPDATE_COMMAND_UI(ID_LOC_SETTINGS, OnUpdateLocSettings)
	ON_COMMAND(ID_LOC_SETTINGS, OnLocSettings)
	ON_UPDATE_COMMAND_UI(ID_LOC_SETTINGS_SEND, OnUpdateLocSettingsSend)
	ON_COMMAND(ID_LOC_SETTINGS_SEND, OnLocSettingsSend)
	ON_UPDATE_COMMAND_UI(ID_NEW_HIST20_DATA, OnUpdateNewHist20Data)
	ON_COMMAND(ID_NEW_HIST20_DATA, OnNewHist20Data)
	ON_UPDATE_COMMAND_UI(ID_NEW_HIST100_DATA, OnUpdateNewHist100Data)
	ON_COMMAND(ID_NEW_HIST100_DATA, OnNewHist100Data)
	ON_COMMAND(ID_PHONE_BOOK, OnPhoneBook)
	ON_UPDATE_COMMAND_UI(ID_PHONE_BOOK, OnUpdatePhoneBook)
	ON_COMMAND(ID_COM_DISCONNECT_MODEM, OnComDisconnectModem)
	ON_UPDATE_COMMAND_UI(ID_COM_DISCONNECT_MODEM, OnUpdateComDisconnectModem)
	ON_COMMAND(ID_COM_DISCONNECT_SER, OnComDisconnectSer)
	ON_UPDATE_COMMAND_UI(ID_COM_DISCONNECT_SER, OnUpdateComDisconnectSer)
	ON_UPDATE_COMMAND_UI(ID_COM_STATE, OnUpdateConnState)
	ON_UPDATE_COMMAND_UI(ID_NEW_DURATION_DATA, OnUpdateNewDurationData)
	ON_COMMAND(ID_NEW_DURATION_DATA, OnNewDurationData)
	ON_UPDATE_COMMAND_UI(ID_NEW_HIST2_DATA, OnUpdateNewHist2Data)
	ON_COMMAND(ID_NEW_HIST2_DATA, OnNewHist2Data)
	// Fleetmgr
	ON_COMMAND(ID_FLEET_MGR, OnFleetMgr)
	ON_UPDATE_COMMAND_UI(ID_FLEET_MGR, OnUpdateFleetMgr)
	ON_COMMAND(ID_FLEET_EDIT, OnFleetEdit)
	ON_UPDATE_COMMAND_UI(ID_FLEET_EDIT, OnUpdateFleetEdit)

	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRailDiagApp construction

CRailDiagApp::CRailDiagApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	m_dwBaud = 19200;
	m_uiMaxWait4Data = 2000;
	m_uiWait4Reconnect = 10000;
	m_iComStateConnect = _STATE_CONN_DISCONNECTED;
	m_uiMaxWait4Active = 15 * 60 * 1000; // 15 Minuten
	m_eConnState = EN_CONN_IDLE;
	m_strComPort = _T("COM3");
	m_strConnName = _T("");
	m_strConnDesc	= _T("");
	m_strConnNumber = _T("");
	m_strConnPin = _T("");
	m_strConnPuk = _T("");
	m_strCBST = _T("");
	m_bConnPinSave = FALSE;
	m_hComThread = NULL;
	m_hExitComEvent = NULL;
	m_iConnType = 0;
	m_iLanguages = 1;
	m_iLang = 1;
	m_iDefaultLang = 1;
	m_bAuto = FALSE;
	m_bLogFile = FALSE;
	m_bUserConnect = FALSE;
	m_dwDisconnectTick = 0;
	m_bReconnect = FALSE;
	m_bLineInit = FALSE;
	m_bMessage = FALSE;
	m_bDataIn = FALSE;
	m_pLocSettings = NULL;
	m_bUserInput = FALSE;
	m_bBlinkState = FALSE;
	m_dwBlinkNextTick = 0;
	m_dwBlinkFastOn = 700;
	m_dwBlinkFastOff = 700;
	m_dwBlinkSlowOn = 4000;
	m_dwBlinkSlowOff = 500;
	m_bGSM_Rail = FALSE;

	m_hFleetThread = NULL;
	m_hExitFleetEvent = NULL;
	m_enFleetState = enFleetStateInit;
	m_iFleetSelected = -1;

	// statische texte
	m_stStatText.strHeadNameShort	= _T("MTU");
	m_stStatText.strHeadName		= _T("Motor und TurbinenUnion");
	m_stStatText.strHeadCity		= _T("Friedrichshafen");
	m_stStatText.strCompany			= _T("Eisenbahngesellschaft");
	m_stStatText.strLocType			= _T("Loktyp");
	m_stStatText.strLocNumber		= _T("Loknummer");
	m_stStatText.strMotorType		= _T("Motortyp");
	m_stStatText.strMotorNumber		= _T("MotorNummer");
	m_stStatText.strDurationCount	= _T("Betriebsstunden");
	m_stStatText.strTerminal		= _T("Display");
	m_stStatText.strDiag			= _T("PC");
	m_stStatText.strMode			= _T("Betriebsart");

	m_stStatBinary.strTitle	   = _T("Binärmessstellen");
	m_stStatBinary.strarrRow.Add(_T("Zeile"));
	m_stStatBinary.strarrRow.Add(_T("Beschreibung"));
	m_stStatBinary.strarrRow.Add(_T("Status"));

	m_stStatAnalog.strTitle    = _T("Analogmessstellen");
	m_stStatAnalog.strarrRow.Add(_T("Zeile"));
	m_stStatAnalog.strarrRow.Add(_T("Beschreibung"));
	m_stStatAnalog.strarrRow.Add(_T("Betriebswert"));
	m_stStatAnalog.strarrRow.Add(_T("Einheit"));

	m_stStatAlarm.strTitle    = _T("Alarme");
	m_stStatAlarm.strarrRow.Add(_T("Zeile"));
	m_stStatAlarm.strarrRow.Add(_T("Code"));
	m_stStatAlarm.strarrRow.Add(_T("Typ"));
	m_stStatAlarm.strarrRow.Add(_T("Kategorie"));

	m_stStatDuration.strTitle    = _T("Betriebsstunden");
	m_stStatDuration.strarrRow.Add(_T("Zeile"));
	m_stStatDuration.strarrRow.Add(_T("Beschreibung"));
	m_stStatDuration.strarrRow.Add(_T("Betriebswert"));
	m_stStatDuration.strarrRow.Add(_T("Einheit"));

	m_stStatDurationFleet.strTitle    = _T("Betriebsstunden");
	m_stStatDurationFleet.strarrRow.Add(_T("Zeile"));
	m_stStatDurationFleet.strarrRow.Add(_T("Lok Nr."));
	m_stStatDurationFleet.strarrRow.Add(_T("Motor Nr."));
	m_stStatDurationFleet.strarrRow.Add(_T("Beschreibung"));
	m_stStatDurationFleet.strarrRow.Add(_T("Betriebswert"));
	m_stStatDurationFleet.strarrRow.Add(_T("Einheit"));

	m_stStatHistory.strTitle  = _T("Fehlerringspeicher");
	m_stStatHistory.strarrRow.Add(_T("Zeile"));
	m_stStatHistory.strarrRow.Add(_T("Code"));
	m_stStatHistory.strarrRow.Add(_T("Datum"));
	m_stStatHistory.strarrRow.Add(_T("Uhrzeit"));
	m_stStatHistory.strarrRow.Add(_T("Betriebs-H."));
	m_stStatHistory.strarrRow.Add(_T("Typ"));
	m_stStatHistory.strarrRow.Add(_T("Kategorie"));
	m_stStatHistory.strarrRow.Add(_T("Status"));

	m_iInOutMaxWaiting = 2;
	m_bPUK = FALSE;
}

CRailDiagApp::~CRailDiagApp()
{
	StopComThread();

	if (m_bLineInit)
	{
//		Line_Deinit(0);
	}

	if (m_bLogFile)
	{
		m_fLogFile.Close();
	}

}
/////////////////////////////////////////////////////////////////////////////
// The one and only CRailDiagApp object

CRailDiagApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRailDiagApp initialization

BOOL CRailDiagApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	//rb Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("MTU"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)
	CString		strBuf;
	CString		strDownload;

	strDownload = GetProfileString(_T("Settings"), _T("DownloadPath"), _T("Download"));
	m_strComPort = GetProfileString(_T("Settings"), _T("ComPort"), _T("COM1"));
	m_strConnName = GetProfileString(_T("Settings"), _T("ConnName"), _T(""));
	m_strConnDesc = GetProfileString(_T("Settings"), _T("ConnDesc"), _T(""));
	m_strConnNumber = GetProfileString(_T("Settings"), _T("ConnNumber"), _T(""));
	m_strDialPrefix = GetProfileString(_T("Settings"), _T("DialPrefix"), _T(""));
	strBuf = GetProfileString(_T("Settings"), _T("DialPrefixUsage"), _T(""));
	if (_tcstol(strBuf, NULL, 10) > 0)
	{
		m_bDialPrefix = TRUE;
	}
	else
	{
		m_bDialPrefix = FALSE;
	}
	m_strConnPin = GetProfileString(_T("Settings"), _T("ConnPin"), _T(""));
	strBuf = GetProfileString(_T("Settings"), _T("ConnPinSave"), _T(""));
	if (_tcstol(strBuf, NULL, 10) > 0)
	{
		m_bConnPinSave = TRUE;
	}
	else
	{
		m_bConnPinSave = FALSE;
	}
	strBuf = GetProfileString(_T("Settings"), _T("Language"), _T("1"));
	m_iLang = _tcstol(strBuf, NULL, 10);

	strBuf = GetProfileString(_T("Settings"), _T("ConnType"), _T("0"));
	m_iConnType = _tcstol(strBuf, NULL, 10);

	strBuf = GetProfileString(_T("Settings"), _T("TimeoutOnlineData"), _T("3000"));
	m_uiMaxWait4Data = _tcstol(strBuf, NULL, 10);

	strBuf = GetProfileString(_T("Settings"), _T("TimeoutOnlineCheck"), _T("7000"));
	m_uiMaxWait4Check = _tcstol(strBuf, NULL, 10);

	strBuf = GetProfileString(_T("Settings"), _T("TimeoutAutoReconnect"), _T("3000"));
	m_uiWait4Reconnect = _tcstol(strBuf, NULL, 10);

	strBuf = GetProfileString(_T("Settings"), _T("TimeoutNewCommand"), _T("15")); // in Minuten
	m_uiMaxWait4Active = _tcstol(strBuf, NULL, 10) * 60 *1000;

	strBuf = GetProfileString(_T("Settings"), _T("ConcurentCommands"), _T("5")); // Anzahl gleichzeitige Sätze
	m_iInOutMaxWaiting = _tcstol(strBuf, NULL, 10);

	strBuf = GetProfileString(_T("Settings"), _T("BlinkTimeFastOn"), _T("250")); 
	m_dwBlinkFastOn = _tcstol(strBuf, NULL, 10);
	strBuf = GetProfileString(_T("Settings"), _T("BlinkTimeFastOff"), _T("250")); 
	m_dwBlinkFastOff = _tcstol(strBuf, NULL, 10);
	strBuf = GetProfileString(_T("Settings"), _T("BlinkTimeSlowOn"), _T("2000")); 
	m_dwBlinkSlowOn = _tcstol(strBuf, NULL, 10);
	strBuf = GetProfileString(_T("Settings"), _T("BlinkTimeSlowOff"), _T("250")); 
	m_dwBlinkSlowOff = _tcstol(strBuf, NULL, 10);

	// Dauer wie lange ein Tooltip angezeigt werden soll
	strBuf = GetProfileString(_T("Settings"), _T("ToolTipOn"), _T("32000")); 
	m_dwToolTipOn = _tcstol(strBuf, NULL, 10);
	// Breite des Tooltip Fensters
	strBuf = GetProfileString(_T("Settings"), _T("ToolTipOn"), _T("700")); 
	m_dwToolTipWidth = _tcstol(strBuf, NULL, 10);

	strBuf = GetProfileString(_T("Settings"), _T("FileLog"), _T("0"));
	if (_tcstol(strBuf, NULL, 10) > 0)
	{
		m_bLog = TRUE;
	}
	else
	{
		m_bLog = FALSE;
	}
	m_strPrintHeader = GetProfileString(_T("Settings"), _T("PrintHeader"), _T("MTU Motoren- und Turbinen-Union"));
	m_strPrintFooter = GetProfileString(_T("Settings"), _T("PrintFooter"), _T(" "));

	if(TRUE)
	{
		CString			strList;
		CStringArray	strarrList;
		CUtil			Util;
		int				iVal;
		int				i;

		// binärmessstellen
		strList = GetProfileString(_T("Settings"), _T("PrintPosBinaryList"), _T("0, 10, 50"));
		strarrList.RemoveAll();
		Util.SeparateComma(strList, strarrList);
		for(i = 0; i < strarrList.GetSize(); i++)
		{
			iVal = _tcstol(strarrList.GetAt(i), NULL, 10);
			m_uiarrPrintPosBinaryList.Add(iVal);
		}

		// analogmessstellen
		strList = GetProfileString(_T("Settings"), _T("PrintPosAnalogList"), _T("0, 10, 60, 80"));
		strarrList.RemoveAll();
		Util.SeparateComma(strList, strarrList);
		for(i = 0; i < strarrList.GetSize(); i++)
		{
			iVal = _tcstol(strarrList.GetAt(i), NULL, 10);
			m_uiarrPrintPosAnalogList.Add(iVal);
		}

		// alarmseite
		strList = GetProfileString(_T("Settings"), _T("PrintPosAlarmList"), _T("0, 10, 70, 80"));
		strarrList.RemoveAll();
		Util.SeparateComma(strList, strarrList);
		for(i = 0; i < strarrList.GetSize(); i++)
		{
			iVal = _tcstol(strarrList.GetAt(i), NULL, 10);
			m_uiarrPrintPosAlarmList.Add(iVal);
		}

		// betriebsstundenzähler fleet
		strList = GetProfileString(_T("Settings"), _T("PrintPosDurationFleetList"), _T("0, 7, 24, 41, 82, 94"));
		strarrList.RemoveAll();
		Util.SeparateComma(strList, strarrList);
		for(i = 0; i < strarrList.GetSize(); i++)
		{
			iVal = _tcstol(strarrList.GetAt(i), NULL, 10);
			m_uiarrPrintPosDurationFleetList.Add(iVal);
		}

		// fehlerringspeicher
		strList = GetProfileString(_T("Settings"), _T("PrintPosHistoryList"), _T("0, 7, 45, 55, 65, 75, 82, 92"));
		strarrList.RemoveAll();
		Util.SeparateComma(strList, strarrList);
		for(i = 0; i < strarrList.GetSize(); i++)
		{
			iVal = _tcstol(strarrList.GetAt(i), NULL, 10);
			m_uiarrPrintPosHistoryList.Add(iVal);
		}

		// fehlerstatistik
		strList = GetProfileString(_T("Settings"), _T("PrintPosHistory2List"), _T("0, 7, 35, 45, 55, 65, 75, 85, 95"));
		strarrList.RemoveAll();
		Util.SeparateComma(strList, strarrList);
		for(i = 0; i < strarrList.GetSize(); i++)
		{
			iVal = _tcstol(strarrList.GetAt(i), NULL, 10);
			m_uiarrPrintPosHistory2List.Add(iVal);
		}
	}

	bool bExcelTest = true;

	// Konfiguration von RailDiag.ini einlesen
	CStdioFile	file;
	CString		strFile;
	CString		strTmp;
	TCHAR		tszCurrentPath[_MAX_PATH];

	if (GetCurrentDirectory(_MAX_PATH, tszCurrentPath))
	{
		m_strAppPath = tszCurrentPath;
	}

	int	iPos;
	iPos = strDownload.Find(_T("\\"), 0); // Prüfen ob Downloadverzeichnis auch Pfadangaben enthält
	if (iPos >= 0)
	{
		m_strInitialDirectory = strDownload; // Pfad übernehmen
	}
	else
	{
		m_strInitialDirectory = m_strAppPath + _T("\\") + strDownload;
	}
	CreateDirectory(m_strInitialDirectory, NULL);

	strFile = m_strAppPath + _T("\\RailDiag.ini");
    if (file.Open(strFile, CFile::modeRead | CFile::typeBinary))
	{
		ReadRailDiagIni(file);

		if (m_strLangIconPath.IsEmpty())
		{
			m_strLangIconPath = m_strAppPath;
		}
		file.Close();
	}
	else
	{
		CString		strMsg;
		strMsg.Format(_T("Could not open Ini-file:\n%s\n\nPlease check the File and resart the Application!"), 
			strFile);
		AfxMessageBox(strMsg, MB_OK|MB_ICONSTOP);
		return FALSE;
	}
	
	strTmp.Format(_T("\\Lang_%2.2d.ini"), m_iLang);
	strFile = m_strAppPath + strTmp;
    if (file.Open(strFile, CFile::modeRead | CFile::typeBinary))
	{
		ReadLangIni(file);
		file.Close();
	}
	else
	{
		// open default lang
		strTmp.Format(_T("\\Lang_%2.2d.ini"), m_iDefaultLang);
		strFile = m_strAppPath + strTmp;
		if (file.Open(strFile, CFile::modeRead | CFile::typeBinary))
		{
			ReadLangIni(file);
			file.Close();
		}
	}

	// Telefonbuch aus PhoneBook.ini einlesen
	strFile = m_strAppPath + _T("\\PhoneBook.ini");
    if (file.Open(strFile, CFile::modeRead | CFile::typeBinary))
	{
		ReadPhoneBookIni(file, m_strarrPhoneBookDesc, m_strarrPhoneBookNumber);

		file.Close();
	}
	else
	{
		CString		strMsg;
		strMsg.Format(_T("Could not open Ini-file:\n%s\n\nPlease check the File and resart the Application!"), 
			strFile);
		AfxMessageBox(strMsg, MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	// Flottenliste aus Datei einlesen
	strFile = m_strAppPath + _T("\\Fleets.ini");
    if (file.Open(strFile, CFile::modeRead | CFile::typeBinary))
	{
		ReadFleetIni(file, m_strarrFleetDesc, m_ptrarrFleetElem);

		file.Close();
	}
	else
	{
		CString		strMsg;
		strMsg.Format(_T("Could not open Ini-file:\n%s\n\nPlease check the File and resart the Application!"), 
			strFile);
		AfxMessageBox(strMsg, MB_OK|MB_ICONSTOP);
		return FALSE;
	}



	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
/*
	pDocTemplate = new CMultiDocTemplate(
		IDR_RAILDITYPE,
		RUNTIME_CLASS(CRailDiagDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CRailDiagView));
//		RUNTIME_CLASS(CStateView));
*/
	pDocTemplate = new CMultiDocTemplate(
//		IDR_RAILSPLIT,
		IDR_RAILDITYPE,
		RUNTIME_CLASS(CRailDiagDoc),
		RUNTIME_CLASS(CSplitterFrame), // custom MDI child frame
		RUNTIME_CLASS(CRailDiagView));
//		RUNTIME_CLASS(CStateView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Enable drag/drop open
//	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
//	EnableShellOpen();
//	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
//	if (!ProcessShellCommand(cmdInfo))
//		return FALSE;

	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
	{
		// Optionsdatei interpretieren
		BOOL		bQuitAfterAuto = FALSE;
		CStdioFile	file;
		CString		optionFileName, csBuf;
		CString		prjFileName, importFileName, replaceFileName, pageFileName, dirFileName;

		optionFileName = cmdInfo.m_strFileName;
		if (file.Open(optionFileName, CFile::modeRead | CFile::typeText))
		{
			if (FindSection(file, _T("GLOBAL")))
			{
				CString	key, param;

				while (TranslateNextLine(file, key, param))
				{
					if (key == _T("COM_PORT"))
						m_strComPort = param;
					else if (key == _T("CONN_NAME"))
						m_strConnName = param;
					else if (key == _T("CONN_NUMBER"))
						m_strConnNumber = param;
					else if (key == _T("CONN_PIN"))
						m_strConnPin = param;
					else if (key == _T("CONN_TYPE"))
						m_iConnType = _tcstol(param, NULL, 10);
					else if (key == _T("BINARY_READ"))
					{
						if (param.CompareNoCase(_T("TRUE")) == 0)
						{
							m_uiarrAutoCommands.Add(EN_DATA_TYPE_BIN);
						}
					}
					else if (key == _T("BINARY_SAVE"))
					{
						if (m_uiarrAutoCommands.GetSize() > m_strarrAutoSaveFiles.GetSize())
						{
							m_strarrAutoSaveFiles.Add(param);
						}
					}
					else if (key == _T("ANALOG_READ"))
					{
						if (param.CompareNoCase(_T("TRUE")) == 0)
						{
							m_uiarrAutoCommands.Add(EN_DATA_TYPE_ANA);
						}
					}
					else if (key == _T("ANALOG_SAVE"))
					{
						if (m_uiarrAutoCommands.GetSize() > m_strarrAutoSaveFiles.GetSize())
						{
							m_strarrAutoSaveFiles.Add(param);
						}
					}
					else if (key == _T("ALARM_READ"))
					{
						if (param.CompareNoCase(_T("TRUE")) == 0)
						{
							m_uiarrAutoCommands.Add(EN_DATA_TYPE_ALARM);
						}
					}
					else if (key == _T("ALARM_SAVE"))
					{
						if (m_uiarrAutoCommands.GetSize() > m_strarrAutoSaveFiles.GetSize())
						{
							m_strarrAutoSaveFiles.Add(param);
						}
					}
					else if (key == _T("DURATION_READ"))
					{
						if (param.CompareNoCase(_T("TRUE")) == 0)
						{
							m_uiarrAutoCommands.Add(EN_DATA_TYPE_DURATION);
						}
					}
					else if (key == _T("DURATION_SAVE"))
					{
						if (m_uiarrAutoCommands.GetSize() > m_strarrAutoSaveFiles.GetSize())
						{
							m_strarrAutoSaveFiles.Add(param);
						}
					}
					else if (key == _T("HISTORY_READ"))
					{
						if (param.CompareNoCase(_T("TRUE")) == 0)
						{
							m_uiarrAutoCommands.Add(EN_DATA_TYPE_HIST);
						}
					}
					else if (key == _T("HISTORY_SAVE"))
					{
						if (m_uiarrAutoCommands.GetSize() > m_strarrAutoSaveFiles.GetSize())
						{
							m_strarrAutoSaveFiles.Add(param);
						}
					}
					else if (key == _T("HISTORY20_READ"))
					{
						if (param.CompareNoCase(_T("TRUE")) == 0)
						{
							m_uiarrAutoCommands.Add(EN_DATA_TYPE_HIST20);
						}
					}
					else if (key == _T("HISTORY20_SAVE"))
					{
						if (m_uiarrAutoCommands.GetSize() > m_strarrAutoSaveFiles.GetSize())
						{
							m_strarrAutoSaveFiles.Add(param);
						}
					}
					else if (key == _T("HISTORY100_READ"))
					{
						if (param.CompareNoCase(_T("TRUE")) == 0)
						{
							m_uiarrAutoCommands.Add(EN_DATA_TYPE_HIST100);
						}
					}
					else if (key == _T("HISTORY100_SAVE"))
					{
						if (m_uiarrAutoCommands.GetSize() > m_strarrAutoSaveFiles.GetSize())
						{
							m_strarrAutoSaveFiles.Add(param);
						}
					}
					else if (key == _T("HIST_STAT_READ"))
					{
						if (param.CompareNoCase(_T("TRUE")) == 0)
						{
							m_uiarrAutoCommands.Add(EN_DATA_TYPE_HIST2);
						}
					}
					else if (key == _T("HIST_STAT_SAVE"))
					{
						if (m_uiarrAutoCommands.GetSize() > m_strarrAutoSaveFiles.GetSize())
						{
							m_strarrAutoSaveFiles.Add(param);
						}
					}
				}
			}

			file.Close();

			// Logdatei erzeugen
			CString		logFileName;

			logFileName = _T("auto.log");
			if (m_fLogFile.Open(logFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
			{
				m_bLogFile = TRUE;
			}
			
			if (m_bLogFile)
			{
				time_t t;
				time(&t);

				csBuf.Format(_T("RailDiag-Logfile erstellt am: %s\n"), _wctime(&t));
				m_fLogFile.WriteString(csBuf);
			}

		}
		else
		{
			// keine gültige Datei als Kommandozeilenparameter
			return FALSE;
		}

		// Projekt öffnen
		m_bAuto = TRUE;
	}
	else
	{
		if (m_bLog)
		{
			// logfile für normalen betrieb
			CString		logFileName;

			logFileName = _T("raildiag.log");
			if (m_fLogFile.Open(logFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
			{
				m_bLogFile = TRUE;
				Write2Log(_T("RailDiag-Logfile erstellt"));
			}
		}
	}

#ifndef _DEBUG
	if (!m_bAuto)
#endif
	{
		// The main window has been initialized, so show and update it.
//		m_nCmdShow = SW_SHOWMAXIMIZED;  // to maximize

		pMainFrame->ShowWindow(m_nCmdShow);
		pMainFrame->UpdateWindow();
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_strCopyright;
	CString	m_strVersion;
	CString	m_strDescription;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_strCopyright = _T("");
	m_strVersion = _T("");
	m_strDescription = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_COPYRIGHT, m_strCopyright);
	DDX_Text(pDX, IDC_VERSION, m_strVersion);
	DDX_Text(pDX, IDC_DESCRIPTION, m_strDescription);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CRailDiagApp::OnAppAbout()
{
	m_bUserInput = TRUE;

	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CRailDiagApp message handlers


void CRailDiagApp::OnComConnect() 
{
	// TODO: Add your command handler code here
	m_bUserInput = TRUE;
	m_bUserConnect = TRUE;
	m_bMessage = FALSE;

//	if (m_eDataType != EN_DATA_TYPE_DURATION_FLEET)
	{
		SaveOpenDocument();
	}

	ClearLocSettingsLoad();

	if (m_iFleetMgrState == EN_FLEET_ON)
	{
		m_bMessage = TRUE; // keine Messageboxen öffnen
	}

	if (m_iConnType == 1 && m_iFleetMgrState == EN_FLEET_OFF)
	{
		// dialog für Verbindung einblenden
		CModemSettings	ModemSettings;

		ModemSettings.m_ModemName = m_strConnName;
		ModemSettings.m_ModemDesc = m_strConnDesc;
		ModemSettings.m_ModemNumber = m_strConnNumber;
		ModemSettings.m_bDialPrefix = m_bDialPrefix;
		ModemSettings.m_DialPrefix = m_strDialPrefix;
		ModemSettings.m_ModemPin = m_strConnPin;
		ModemSettings.m_bSavePin = m_bConnPinSave;
		ModemSettings.m_ModemPuk = m_strConnPuk;

		if (ModemSettings.DoModal() == IDOK)
		{
			// set name, number, pin
			m_strConnName = ModemSettings.m_ModemName;
			WriteProfileString(_T("Settings"), _T("ConnName"), m_strConnName);
			m_strConnDesc = ModemSettings.m_ModemDesc;
			WriteProfileString(_T("Settings"), _T("ConnDesc"), m_strConnDesc);
			m_strConnNumber = ModemSettings.m_ModemNumber;
			WriteProfileString(_T("Settings"), _T("ConnNumber"), m_strConnNumber);
			m_strDialPrefix = ModemSettings.m_DialPrefix;
			WriteProfileString(_T("Settings"), _T("DialPrefix"), m_strDialPrefix);
			m_bDialPrefix = ModemSettings.m_bDialPrefix;
			if (m_bDialPrefix)
			{
				WriteProfileString(_T("Settings"), _T("DialPrefixUsage"), _T("1"));
			}
			else
			{
				WriteProfileString(_T("Settings"), _T("DialPrefixUsage"), _T("0"));
			}
			m_strConnPin = ModemSettings.m_ModemPin;
			m_bConnPinSave = ModemSettings.m_bSavePin;
			m_strConnPuk = ModemSettings.m_ModemPuk;
			if (ModemSettings.m_bSavePin)
			{
				WriteProfileString(_T("Settings"), _T("ConnPin"), m_strConnPin);
				WriteProfileString(_T("Settings"), _T("ConnPinSave"), _T("1"));
			}
			else
			{
				WriteProfileString(_T("Settings"), _T("ConnPin"), _T(""));
				WriteProfileString(_T("Settings"), _T("ConnPinSave"), _T("0"));
			}

			ComConnect();
		}
		else
		{
			m_bUserConnect = FALSE;
		}
		m_bPUK = FALSE;
	}
	else
	{
		ComConnect();
	}

}

CString CRailDiagApp::GetDialNumber(void)
{
	CString dialNumber;

	if (m_bDialPrefix)
	{
		dialNumber = m_strDialPrefix + m_strConnNumber;
	}
	else
	{
		dialNumber = m_strConnNumber;
	}

	return dialNumber;
}

CString CRailDiagApp::GetDialName(void)
{
	return m_strConnDesc;
}

void CRailDiagApp::ComConnect()
{
	CString		strMsg;
	BYTE		byPort;
	CMainFrame	*pMainFrame = NULL;

	TRACE(_T("CRailDiagApp::OnComConnect\n"));

	pMainFrame = (CMainFrame*)GetMainWnd();

	if (m_iConnType == 0) // serielles Kabel
	{
		Write2Log(_T("Start connect serial"));
		ComLibInit();

		byPort = (BYTE)_tcstol(m_strComPort.Right(1), NULL, 10);
		if (ComOpen(
				byPort,			// COM3
				0,				// Typ: 0..RS232 ohne HS, 1..RS232 mit HS
				9600,			// Baudrate: 300 - 115200
				8,				// Datenlänge: 4 - 8 Bit
				1,				// Stoppbits: 1, 2
				0,				// Parität: 0..no, 1..ungerade, 2..gerade
				1024				// Puffergrösse Ein/Aus in Bytes
				) == 1)
		{
			m_iComStateConnect = _STATE_CONN_CONNECTED;
			StartComThread();
			CD_Init('D');
		}

		if (m_iComStateConnect != _STATE_CONN_CONNECTED)
		{
			strMsg.Format(m_stConStatus.strInUse, byPort);
			AfxMessageBox ( strMsg );
		}
	}

	if (m_iConnType == 1 && !m_strConnName.IsEmpty() && !m_strConnNumber.IsEmpty()) // Festnetzmodem
	{
//		DWORD	dwLine;
		BOOL	bStat = TRUE;
		CHAR	*pszPin = NULL;
		CHAR	*pszPuk = NULL;
		CHAR	*pszCBST = NULL;

		Write2Log(_T("Start connect modem"));

		EN_LINE_ERROR	eLineError;
		EN_LINE_STATE	eLineState;

		eLineState = Line_GetState(&eLineError);

		if (eLineState == EN_LINE_STATE_IDLE)
		{
			if (eLineError < 0)
			{
				OnComDisconnect();
			}
		}

		TRACE(_T("CRailDiagApp::ComConnect state %d, error %d\n"), eLineState, eLineError);

		if (eLineState == EN_LINE_STATE_NOINIT)
		{
			Line_Init(0, 0, NULL, NULL, NULL, FALSE, 0);
			g_dwLine = 0;
			g_byPort = 0;
			bStat = Line_GetLine4Name(m_strConnName.GetBuffer(1), &g_dwLine, &g_byPort, &m_dwBaud);
			m_strConnName.ReleaseBuffer();
			Line_Deinit(0, 0, FALSE);

			if (!bStat)
			{
				m_iComStateConnect = _STATE_CONN_DISCONNECTED;
				if (!m_bMessage)
				{
					m_bMessage = TRUE;
					AfxMessageBox(m_stConStatus.strModemDriverError);
				}
			}
		}

		if (bStat &&
			(eLineState == EN_LINE_STATE_NOINIT  ||
			 eLineState == EN_LINE_STATE_INIT)
		   )
		{
			DWORD	dwBaud4Rail = 0;

			m_iComStateConnect = _STATE_CONN_CONNECTING;

			if (m_strConnPin.GetLength() < 2)
			{
				pszPin = NULL;
			}
			else
			{
				pszPin = UnicodeString2CharString(m_strConnPin);
			}
			if (m_strConnPuk.GetLength() < 2)
			{
				pszPuk = NULL;
			}
			else
			{
				pszPuk = UnicodeString2CharString(m_strConnPuk);
			}
			if (m_strCBST.GetLength() < 2)
			{
				pszCBST = NULL;
			}
			else
			{
				pszCBST = UnicodeString2CharString(m_strCBST);
			}

			if (!m_strConnName.CompareNoCase(_T("Gsm-Rail")))
			{
				dwBaud4Rail = 1;
			}

			eLineError = Line_Init(g_byPort, m_dwBaud, pszPin, pszPuk, pszCBST, TRUE, dwBaud4Rail); 
								// port, baude, pin, puk, cbst, fix auf data call stellen

			TRACE(_T("CRailDiagApp::ComConnect after Line_Init port %d, error %d\n"), g_byPort, eLineError);

			switch(eLineError)
			{
			case EN_LINE_ERR_CLOSE:
				if (!m_bMessage)
				{
					m_bMessage = TRUE;
					AfxMessageBox(m_stConStatus.strInUse);
				}
				m_iComStateConnect = _STATE_CONN_DISCONNECTED;
//				pMainFrame->SetConnError(m_stConStatus.strInUse);
				pMainFrame->ClearConnError();
				m_bReconnect = FALSE;
				ComDisconnect();
				break;
			case EN_LINE_ERR_MODEM:
				if (!m_bMessage)
				{
					m_bMessage = TRUE;
					AfxMessageBox(m_stConStatus.strModemOff);
				}
				m_iComStateConnect = _STATE_CONN_DISCONNECTED;
//				pMainFrame->SetConnError(m_stConStatus.strModemOff);
				pMainFrame->ClearConnError();
				m_bReconnect = FALSE;
				ComDisconnect();
				break;
			case EN_LINE_ERR_WRONGPIN:
				if (!m_bMessage)
				{
					m_bMessage = TRUE;
					AfxMessageBox(m_stConStatus.strWrongPin);
				}
				m_iComStateConnect = _STATE_CONN_DISCONNECTED;
//				pMainFrame->SetConnError(m_stConStatus.strWrongPin);
				pMainFrame->ClearConnError();
				m_bReconnect = FALSE;
				ComDisconnect();
				break;
			case EN_LINE_ERR_WRONGPUK:
				if (!m_bMessage)
				{
					m_bMessage = TRUE;
					AfxMessageBox(m_stConStatus.strWrongPuk);
				}
				m_iComStateConnect = _STATE_CONN_DISCONNECTED;
				m_bPUK = TRUE;
//				pMainFrame->SetConnError(m_stConStatus.strWrongPuk);
				pMainFrame->ClearConnError();
				m_bReconnect = FALSE;
				ComDisconnect();
				break;
			default:
				break;
			}
		}
		else if (eLineState == EN_LINE_STATE_NOOPEN)
		{
			if (g_byPort != 0)
			{
				if(Line_Open(g_dwLine))
				{
					TRACE(_T("CRailDiagApp::ComConnect open\n"));
					CString	number = GetDialNumber();
					Line_Call(number.GetBuffer(1));
					TRACE(_T("CRailDiagApp::ComConnect call\n"));
					number.ReleaseBuffer();
					m_iComStateConnect = _STATE_CONN_CONNECTED;
					StartComThread();
					CD_Init('D');
				}
				else
				{
					if (!m_bMessage)
					{
						m_bMessage = TRUE;
						AfxMessageBox(m_stConStatus.strInUse);
					}
					OnComDisconnect();
				}
			}
			else
			{
				if (!m_bMessage)
				{
					m_bMessage = TRUE;
					AfxMessageBox(m_stConStatus.strInUse);
				}
				OnComDisconnect();
			}
		}
		else if (eLineState == EN_LINE_STATE_CHANGEDBAUD)
		{
			Sleep(2000);
			CString	number = GetDialNumber();
			TRACE(_T("CRailDiagApp::ComConnect call after change baud\n"));
			Line_Call(number.GetBuffer(1));
			number.ReleaseBuffer();
			m_iComStateConnect = _STATE_CONN_CONNECTED;
			StartComThread();
			CD_Init('D');
		}

		if (pszPin)
		{
			delete pszPin;
			pszPin = NULL;
		}
		if (pszPuk)
		{
			delete pszPuk;
			pszPuk = NULL;
		}
	}

	m_bFleetConnectActive = FALSE;
}

void CRailDiagApp::OnUpdateComConnect(CCmdUI* pCmdUI) 
{
//	TRACE(_T("CRailDiagApp::OnUpdateComConnect\n"));

	if (m_iComStateConnect == _STATE_CONN_DISCONNECTED)
	{
		if (GetTickCount() - m_dwDisconnectTick > m_uiWait4Reconnect)
		{
			pCmdUI->Enable(TRUE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CRailDiagApp::SaveOpenDocument()
{
	CMainFrame		*pMainFrame;
	pMainFrame = (CMainFrame*)GetMainWnd();

	if (m_iFleetMgrState == EN_FLEET_ON && pMainFrame && pMainFrame->m_pDoc)
	{
		CString fileName = m_strInitialDirectory + _T("\\") + pMainFrame->m_pDoc->GetTitle();
		pMainFrame->m_pDoc->OnSaveDocument(fileName);
		//pMainFrame->SendMessage(WM_COMMAND, ID_FILE_CLOSE, 0);
	}
}

void CRailDiagApp::OnComDisconnect() 
{
	// TODO: Add your command handler code here
	CMainFrame		*pMainFrame;

	m_bUserInput = TRUE;

	m_bUserConnect = FALSE;
	m_bReconnect = FALSE;
	Write2Log(_T("Disconnecting by user"));
	ComDisconnect();

	pMainFrame = (CMainFrame*)GetMainWnd();
	pMainFrame->ClearConnError();

//	if (m_eDataType != EN_DATA_TYPE_DURATION_FLEET)
	{
		SaveOpenDocument();
	}
}

DWORD WINAPI CRailDiagApp::_ComDisconnectThread(LPVOID lParam)
{
	return ((CRailDiagApp *)lParam)->ComDisconnect();
}

DWORD CRailDiagApp::ComDisconnect() 
{
	CMainFrame		*pMainFrame;
	BYTE			byPort;

	m_bDataIn = FALSE;

	BeginWaitCursor();

	if (m_iConnType == 0) // serielles Kabel
	{
		Write2Log(_T("Stop connect serial"));
		if (m_iComStateConnect == _STATE_CONN_CONNECTED)
		{

			byPort = (BYTE)_tcstol(m_strComPort.Right(1), NULL, 10);

			StopComThread();
		}

		ComLibDeinit();
		m_dwDisconnectTick = 0;

		m_iComStateConnect = _STATE_CONN_DISCONNECTED;
		SetConnectionState(EN_CONN_IDLE);
		pMainFrame = (CMainFrame*)GetMainWnd();
		if (pMainFrame)
		{
			CLocSettings *pLocSettings;

			pLocSettings = (CLocSettings*)m_pLocSettings;
			if (pLocSettings)
			{
				pLocSettings->ConnectionLost();
			}
		}

	}

	if (m_iConnType == 1) // Festnetzmodem
	{
		Write2Log(_T("Stop connect modem"));

		StopComThread();

		m_iComStateConnect = _STATE_CONN_DISCONNECTING;

		EN_LINE_STATE	eLineState;
		EN_LINE_ERROR	eLineError;

		eLineState = Line_GetState(&eLineError);
		if (eLineState != EN_LINE_STATE_NOINIT)
		{
			if (eLineState != EN_LINE_STATE_INIT &&
				eLineState != EN_LINE_STATE_DEINIT &&
				eLineState != EN_LINE_STATE_NOOPEN)
			{
				Line_Close();
			}
			else
			{
				byPort = (BYTE)_tcstol(m_strComPort.Right(1), NULL, 10);
				Line_Deinit(g_byPort, m_dwBaud, FALSE);
			}
		}

//		eLineState = Line_GetState(NULL);
		if (eLineState == EN_LINE_STATE_NOINIT)
		{
			m_iComStateConnect = _STATE_CONN_DISCONNECTED;
			SetConnectionState(EN_CONN_IDLE);
			m_dwDisconnectTick = GetTickCount();
		}
	}

	EndWaitCursor();

	return 0;
}

void CRailDiagApp::OnUpdateComDisconnect(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if (m_iConnType == 0 && 
		(m_iComStateConnect == _STATE_CONN_CONNECTED || 
		 m_iComStateConnect == _STATE_CONN_CONNECTING || 
		GetReconnect()))
	{
		pCmdUI->Enable(TRUE);
	}
	else if (m_iConnType == 1 && 
		(m_iComStateConnect == _STATE_CONN_CONNECTED || 
		 m_iComStateConnect == _STATE_CONN_CONNECTING || 
		GetReconnect()))
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CRailDiagApp::OnComSettings() 
{
	m_bUserInput = TRUE;
	CComSettings	ComSettings;

	ComSettings.m_ComPort = m_strComPort;
	if (ComSettings.DoModal() == IDOK)
	{
		// set port
		m_strComPort = ComSettings.m_ComPort;
		WriteProfileString(_T("Settings"), _T("ComPort"), m_strComPort);
	}
}

void CRailDiagApp::OnUpdateComSettings(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if (m_iComStateConnect == _STATE_CONN_DISCONNECTED)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

DWORD WINAPI CRailDiagApp::_ComThread(LPVOID lParam)
{
	return ((CRailDiagApp *)lParam)->ComThread();

}

DWORD CRailDiagApp::ComThread(void)
{
	BOOL			bExit = FALSE;
	CString			strMsg;
	BYTE			*lpvBuffer;
	DWORD			dwLength;
	DWORD			dwBytes;
	DWORD			dwReadStat;
	BYTE			byPort;
	CMainFrame		*pMainFrame;
	BOOL			bStat;
	int				iCount = 0;

	pMainFrame = (CMainFrame*)GetMainWnd();

	byPort = (BYTE)_tcstol(m_strComPort.Right(1), NULL, 10);
	while (!bExit)
	{
		if (WaitForSingleObject(this->m_hExitComEvent, 0) == WAIT_OBJECT_0)
			bExit = TRUE;
		else
		{
//			TRACE(_T("CRailDiagApp::ComThread loop\n"));

			lpvBuffer = (BYTE*)pMainFrame->m_tszRead + pMainFrame->m_dwPos;
			dwLength = sizeof(pMainFrame->m_tszRead) - pMainFrame->m_dwPos;

			dwLength = 256;
			lpvBuffer = g_bySerReadBuf + g_dwSerReadBufPos;

			iCount = max(iCount+1, 200);
			if (m_iConnType == 0)
			{
				dwReadStat = ComReceive(byPort, lpvBuffer, dwLength, &dwBytes);
				
				if (dwReadStat != 0xFFFFFFFF && dwBytes > 0)
				{
//					TRACE(_T("CRailDiagApp::ComThread Com Read length %d\n"), dwBytes);
					// Meldung an MainFrame weitergeben
					g_dwSerReadBufPos += dwBytes;
					pMainFrame->NewSerialData();
					iCount = 0;
				}
			}

			if (m_iConnType == 1)
			{
				bStat = Line_Receive(lpvBuffer, dwLength, &dwBytes);
				
				if (bStat && dwBytes > 0)
				{
//					TRACE(_T("CRailDiagApp::ComThread Com Read length %d\n"), dwBytes);
					// Meldung an MainFrame weitergeben
					g_dwSerReadBufPos += dwBytes;
					pMainFrame->NewSerialData();
					iCount = 0;
				}
			}

			if (iCount/100 > 0)
			{
				Sleep(1);
			}
		}
	}

	return 0;
}

void CRailDiagApp::StartComThread(void)
{
	DWORD dwId;

	if (m_hExitComEvent == NULL)
	{
		m_hExitComEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

		m_hComThread = ::CreateThread(NULL, 0, CRailDiagApp::_ComThread, (LPVOID)this, 0, &dwId);
		TRACE(_T("Com- Thread created: h = 0x%08X, ID = 0x%08X\n"), m_hComThread, dwId);
	}
}

void CRailDiagApp::StopComThread(void)
{
	if (m_hComThread)
	{
		SetEvent(m_hExitComEvent);
		WaitForSingleObject(m_hComThread, 5000);
	
		ResetEvent(m_hExitComEvent);
		CloseHandle(m_hExitComEvent);
		m_hExitComEvent = NULL;

		CloseHandle(m_hComThread);
		m_hComThread = NULL;
	}

}

BYTE CRailDiagApp::GetComPort()
{
	return (BYTE)_tcstol(m_strComPort.Right(1), NULL, 10);
}

void CRailDiagApp::OnConnTypeSerial() 
{
	m_bUserInput = TRUE;
	// TODO: Add your command handler code here
	m_iConnType = 0;
	//m_iFleetMgrState = EN_FLEET_OFF;

	CString	strBuf;
	strBuf.Format(_T("%d "), m_iConnType);
	WriteProfileString(_T("Settings"), _T("ConnType"), strBuf);
	m_bPUK = FALSE;

	OnComConnect();
}

void CRailDiagApp::OnUpdateConnTypeSerial(CCmdUI* pCmdUI) 
{
	// FleetManagement open
	if (m_iFleetMgrState == EN_FLEET_ON)
		pCmdUI->Enable(TRUE);
	else
	{
		if (m_iConnType == 2)
			pCmdUI->Enable(FALSE);
		else
		{
			if (m_iComStateConnect == _STATE_CONN_DISCONNECTED && !GetReconnect())
			{
				pCmdUI->Enable(TRUE);
			}
			else
			{
				pCmdUI->Enable(FALSE);
			}
		}
	}
}

void CRailDiagApp::OnConnTypeModem() 
{
	m_bUserInput = TRUE;
	// TODO: Add your command handler code here
	m_iConnType = 1;
	//m_iFleetMgrState = EN_FLEET_OFF;

	CString	strBuf;
	strBuf.Format(_T("%d "), m_iConnType);
	WriteProfileString(_T("Settings"), _T("ConnType"), strBuf);

	OnComConnect();
}

void CRailDiagApp::OnUpdateConnTypeModem(CCmdUI* pCmdUI) 
{
	// FleetManagement open
	if (m_iFleetMgrState == EN_FLEET_ON)
	{
		if (m_bFleetCmdActive)
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
	else
	{
		if (m_iComStateConnect == _STATE_CONN_DISCONNECTED)
		{
			if (GetTickCount() - m_dwDisconnectTick > m_uiWait4Reconnect)
			{
				pCmdUI->Enable(TRUE);
			}
			else
			{
				pCmdUI->Enable(FALSE);
			}
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}

}

int CRailDiagApp::GetComStateConnect()
{
	return m_iComStateConnect;
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	DWORD	dwDummy = 1;
	LPVOID	lpData;
	int		iLen;

	iLen = GetFileVersionInfoSize(_T("RailDiag.exe"), &dwDummy);
	lpData = new BYTE[iLen];

		CString	csBuf;

	if (GetFileVersionInfo(_T("RailDiag.exe"), NULL, iLen, lpData))
	{
		LPTSTR lpInfoString;
		UINT	uiBytes;

		if (VerQueryValue(lpData, _T("\\StringFileInfo\\000004b0\\FileDescription"), (LPVOID *)&lpInfoString, &uiBytes))
			m_strDescription = lpInfoString;
		if (VerQueryValue(lpData, _T("\\StringFileInfo\\000004b0\\FileVersion"), (LPVOID *)&lpInfoString, &uiBytes))
			m_strVersion = lpInfoString;
		if (VerQueryValue(lpData, _T("\\StringFileInfo\\000004b0\\LegalCopyright"), (LPVOID *)&lpInfoString, &uiBytes))
			m_strCopyright = lpInfoString;
	}

	delete lpData;

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRailDiagApp::OnUpdateFileNew(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(FALSE);

/*
	if (m_bComStateConnected)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
*/	
}

void CRailDiagApp::OnFileNew() 
{
	m_bUserInput = TRUE;
	// TODO: Add your command handler code here
	CWinApp::OnFileNew();	
}

void CRailDiagApp::OnUpdateNewBinData(CCmdUI* pCmdUI) 
{
	if (m_iFleetMgrState == EN_FLEET_ON)
		pCmdUI->Enable(FALSE);
	else
	{
		if (GetConnectionState() == EN_CONN_CONNECTED)
		{
			pCmdUI->Enable(TRUE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
}

void CRailDiagApp::OnNewBinData() 
{
	m_bUserInput = TRUE;
	m_eDataType = EN_DATA_TYPE_BIN;
	CWinApp::OnFileNew();	
}

void CRailDiagApp::OnUpdateNewAnaData(CCmdUI* pCmdUI) 
{
	if (m_iFleetMgrState == EN_FLEET_ON)
		pCmdUI->Enable(FALSE);
	else
	{
		if (GetConnectionState() == EN_CONN_CONNECTED)
		{
			pCmdUI->Enable(TRUE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
}

void CRailDiagApp::OnNewAnaData() 
{
	m_eDataType = EN_DATA_TYPE_ANA;
	CWinApp::OnFileNew();	
}

void CRailDiagApp::OnUpdateNewAlarmData(CCmdUI* pCmdUI) 
{
	if (m_iFleetMgrState == EN_FLEET_ON)
		pCmdUI->Enable(FALSE);
	else
	{
		if (GetConnectionState() == EN_CONN_CONNECTED)
		{
			pCmdUI->Enable(TRUE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
}

void CRailDiagApp::OnNewAlarmData() 
{
	m_bUserInput = TRUE;
	m_bUserInput = TRUE;
	m_eDataType = EN_DATA_TYPE_ALARM;
	CWinApp::OnFileNew();	
}

void CRailDiagApp::OnUpdateNewHistData(CCmdUI* pCmdUI) 
{
	// FleetManagement open
	if (m_iFleetMgrState == EN_FLEET_ON)
		pCmdUI->Enable(TRUE);
	else
	{
		if (GetConnectionState() == EN_CONN_CONNECTED)
		{
			pCmdUI->Enable(TRUE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
}

void CRailDiagApp::OnNewHistData() 
{
	m_bUserInput = TRUE;
	m_eDataType = EN_DATA_TYPE_HIST;
	if (m_iFleetMgrState == EN_FLEET_OFF || enFleetStateLangSet == m_enFleetState)
		CWinApp::OnFileNew();	
	else
	{
		// fleet management
		StopFleetThread();
		StartFleetThread();
	}
}

void CRailDiagApp::OnUpdateNewHist2Data(CCmdUI* pCmdUI) 
{
	if (m_iFleetMgrState == EN_FLEET_ON)
		pCmdUI->Enable(FALSE);
	else
	{
		if (GetConnectionState() == EN_CONN_CONNECTED)
		{
			pCmdUI->Enable(TRUE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
}

void CRailDiagApp::OnNewHist2Data() 
{
	m_bUserInput = TRUE;
	m_eDataType = EN_DATA_TYPE_HIST2;
	CWinApp::OnFileNew();	
}

void CRailDiagApp::OnUpdateLocSettings(CCmdUI* pCmdUI) 
{
	if (m_iConnType == 0 && m_iComStateConnect == _STATE_CONN_CONNECTED)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CRailDiagApp::OnLocSettings() 
{
	m_bUserInput = TRUE;
	// TODO: Add your command handler code here
	CLocSettings	DlgLocSettings;
	int				nDlgStat;

	DlgLocSettings.m_strCompany			= m_stLocSettings.strCompany;
	DlgLocSettings.m_strLocType			= m_stLocSettings.strLocType;
	DlgLocSettings.m_strLocNumber		= m_stLocSettings.strLocNumber;
	DlgLocSettings.m_strMotorType		= m_stLocSettings.strMotorType;
	DlgLocSettings.m_strMotorNumber		= m_stLocSettings.strMotorNumber;
	DlgLocSettings.m_strMtuOrderNumber	= m_stLocSettings.strMtuOrderNumber;
	DlgLocSettings.m_strDisplayTelNumber= m_stLocSettings.strDisplayTelNumber;
	DlgLocSettings.m_strInhibitTime		= m_stLocSettings.strInhibitTime;
	DlgLocSettings.m_strTelNumberSMS1	= m_stLocSettings.strTelNumberSMS1;
	DlgLocSettings.m_strTelNumberSMS2	= m_stLocSettings.strTelNumberSMS2;
	DlgLocSettings.m_strTelNumberSMS3	= m_stLocSettings.strTelNumberSMS3;

	DlgLocSettings.m_strPin				= m_stLocSettings.strPin;
	DlgLocSettings.m_strPuk				= m_stLocSettings.strPuk;

	if (DlgLocSettings.m_strCompany.IsEmpty() &&
		DlgLocSettings.m_strLocType.IsEmpty() &&			
		DlgLocSettings.m_strLocNumber.IsEmpty() &&		
		DlgLocSettings.m_strMotorType.IsEmpty() &&		
		DlgLocSettings.m_strMotorNumber.IsEmpty() &&		
		DlgLocSettings.m_strMtuOrderNumber.IsEmpty() &&	
		DlgLocSettings.m_strDisplayTelNumber.IsEmpty() &&
		DlgLocSettings.m_strInhibitTime.IsEmpty() &&		
		DlgLocSettings.m_strTelNumberSMS1.IsEmpty() &&	
		DlgLocSettings.m_strTelNumberSMS2.IsEmpty() &&	
		DlgLocSettings.m_strTelNumberSMS3.IsEmpty())
	{
		DlgLocSettings.m_strCompany			= m_stLocSettingsLoad.strCompany;
		DlgLocSettings.m_strLocType			= m_stLocSettingsLoad.strLocType;
		DlgLocSettings.m_strLocNumber		= m_stLocSettingsLoad.strLocNumber;
		DlgLocSettings.m_strMotorType		= m_stLocSettingsLoad.strMotorType;
		DlgLocSettings.m_strMotorNumber		= m_stLocSettingsLoad.strMotorNumber;
		DlgLocSettings.m_strMtuOrderNumber	= m_stLocSettingsLoad.strMtuOrderNumber;
		DlgLocSettings.m_strDisplayTelNumber= m_stLocSettingsLoad.strDisplayTelNumber;
		DlgLocSettings.m_strInhibitTime		= m_stLocSettingsLoad.strInhibitTime;
		DlgLocSettings.m_strTelNumberSMS1	= m_stLocSettingsLoad.strTelNumberSMS1;
		DlgLocSettings.m_strTelNumberSMS2	= m_stLocSettingsLoad.strTelNumberSMS2;
		DlgLocSettings.m_strTelNumberSMS3	= m_stLocSettingsLoad.strTelNumberSMS3;
		DlgLocSettings.m_strPin				= m_stLocSettingsLoad.strPin;
		DlgLocSettings.m_strPuk				= m_stLocSettingsLoad.strPuk;
	}


	nDlgStat = DlgLocSettings.DoModal();
	switch(nDlgStat)
	{
	case IDOK:
		m_stLocSettings.strCompany			= DlgLocSettings.m_strCompany;
		m_stLocSettings.strLocType			= DlgLocSettings.m_strLocType;
		m_stLocSettings.strLocNumber		= DlgLocSettings.m_strLocNumber;
		m_stLocSettings.strMotorType		= DlgLocSettings.m_strMotorType;
		m_stLocSettings.strMotorNumber		= DlgLocSettings.m_strMotorNumber;
		m_stLocSettings.strMtuOrderNumber	= DlgLocSettings.m_strMtuOrderNumber;
		m_stLocSettings.strDisplayTelNumber	= DlgLocSettings.m_strDisplayTelNumber;
		m_stLocSettings.strInhibitTime		= DlgLocSettings.m_strInhibitTime;
		m_stLocSettings.strTelNumberSMS1	= DlgLocSettings.m_strTelNumberSMS1;
		m_stLocSettings.strTelNumberSMS2	= DlgLocSettings.m_strTelNumberSMS2;
		m_stLocSettings.strTelNumberSMS3	= DlgLocSettings.m_strTelNumberSMS3;
		m_stLocSettings.strPin				= DlgLocSettings.m_strPin;
		m_stLocSettings.strPuk				= DlgLocSettings.m_strPuk;
		break;
	default:
		break;
	}

	m_pLocSettings = NULL;
}

void CRailDiagApp::OnUpdateLocSettingsSend(CCmdUI* pCmdUI) 
{
	if (m_iComStateConnect == _STATE_CONN_CONNECTED)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CRailDiagApp::OnLocSettingsSend() 
{
	m_bUserInput = TRUE;
	CMainFrame		*pMainFrame;

	pMainFrame = (CMainFrame*)GetMainWnd();

	pMainFrame->ConfigInitSend(	m_stLocSettings.strCompany, 
								m_stLocSettings.strLocType,
								m_stLocSettings.strLocNumber,
								m_stLocSettings.strMotorType,
								m_stLocSettings.strMotorNumber,
								m_stLocSettings.strMtuOrderNumber,
								m_stLocSettings.strDisplayTelNumber,
								m_stLocSettings.strInhibitTime,
								m_stLocSettings.strTelNumberSMS1,
								m_stLocSettings.strTelNumberSMS2,
								m_stLocSettings.strTelNumberSMS3,
								m_stLocSettings.strReturnStartPageTime,
								m_stLocSettings.strPin,
								m_stLocSettings.strPuk);

	m_stLocSettingsLoad.strCompany			= m_stLocSettings.strCompany;
	m_stLocSettingsLoad.strLocType			= m_stLocSettings.strLocType;
	m_stLocSettingsLoad.strLocNumber		= m_stLocSettings.strLocNumber;
	m_stLocSettingsLoad.strMotorType		= m_stLocSettings.strMotorType;
	m_stLocSettingsLoad.strMotorNumber		= m_stLocSettings.strMotorNumber;
	m_stLocSettingsLoad.strMtuOrderNumber	= m_stLocSettings.strMtuOrderNumber;
	m_stLocSettingsLoad.strDisplayTelNumber	= m_stLocSettings.strDisplayTelNumber;
	m_stLocSettingsLoad.strInhibitTime		= m_stLocSettings.strInhibitTime;
	m_stLocSettingsLoad.strTelNumberSMS1	= m_stLocSettings.strTelNumberSMS1;
	m_stLocSettingsLoad.strTelNumberSMS2	= m_stLocSettings.strTelNumberSMS2;
	m_stLocSettingsLoad.strTelNumberSMS3	= m_stLocSettings.strTelNumberSMS3;
	m_stLocSettingsLoad.strReturnStartPageTime = m_stLocSettings.strReturnStartPageTime;

	m_stLocSettingsLoad.strPin				= m_stLocSettings.strPin;
	m_stLocSettingsLoad.strPuk				= m_stLocSettings.strPuk;
}

EN_CONN_STATE CRailDiagApp::GetConnectionState()
{
	return m_eConnState;
}

void CRailDiagApp::SetConnectionState(EN_CONN_STATE eConnState)
{
	m_eConnState = eConnState;
}

void CRailDiagApp::OnUpdateNewHist20Data(CCmdUI* pCmdUI) 
{
	// FleetManagement open
	if (m_iFleetMgrState == EN_FLEET_ON)
		pCmdUI->Enable(TRUE);
	else
	{
		if (GetConnectionState() == EN_CONN_CONNECTED)
		{
			pCmdUI->Enable(TRUE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
}

void CRailDiagApp::OnNewHist20Data() 
{
	m_bUserInput = TRUE;
	m_eDataType = EN_DATA_TYPE_HIST20;
	if (m_iFleetMgrState == EN_FLEET_OFF || enFleetStateLangSet == m_enFleetState)
		CWinApp::OnFileNew();	
	else
	{
		// fleet management
		StopFleetThread();
		StartFleetThread();
	}
}

void CRailDiagApp::OnUpdateNewHist100Data(CCmdUI* pCmdUI) 
{
	// FleetManagement open
	if (m_iFleetMgrState == EN_FLEET_ON)
		pCmdUI->Enable(TRUE);
	else
	{
		if (GetConnectionState() == EN_CONN_CONNECTED)
		{
			pCmdUI->Enable(TRUE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
}

void CRailDiagApp::OnNewHist100Data() 
{
	m_bUserInput = TRUE;
	m_eDataType = EN_DATA_TYPE_HIST100;
	if (m_iFleetMgrState == EN_FLEET_OFF || enFleetStateLangSet == m_enFleetState)
		CWinApp::OnFileNew();	
	else
	{
		// fleet management
		StopFleetThread();
		StartFleetThread();
	}
}

BOOL CRailDiagApp::ReadRailDiagIni(CStdioFile &file)
{
	BOOL			bRet = TRUE;
	CString			key, param;
	CStringArray	strarrParam;
	CUtil			Util;
	int				iLang;

    if (FindSection(file, _T("GLOBAL")))
    {

		while (bRet && TranslateNextLine(file, key, param))
        {
			if (key == _T("LANGUAGES"))
			{
				m_iLanguages = _tcstol(param, NULL, 10);
			}
			else if (key == _T("LANG_ICON_PATH"))
			{
				m_strLangIconPath = param;
			}
			else if (key == _T("DEFAULT_LANGUAGE"))
			{
				m_iDefaultLang = _tcstol(param, NULL, 10);
			}
			else if (key == _T("MODEM_CBST"))
			{
				m_strCBST = param;
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet && FindSection(file, _T("LANG_SETTINGS")))
    {
		iLang = 1;
		while (bRet && TranslateNextLine(file, key, param))
        {
			if (key == _T("LANG_NR_NAME_ICON"))
			{
				strarrParam.RemoveAll();
				Util.SeparateComma(param, strarrParam);
				if (strarrParam.GetSize() >= 4)
				{
					m_uiarrLangPos.Add(_tcstol(strarrParam.GetAt(0), NULL, 10));
					m_strarrLangName.Add(strarrParam.GetAt(1));
					m_strarrLangIcon.Add(strarrParam.GetAt(2));
					m_uiarrLangNr.Add(_tcstol(strarrParam.GetAt(3), NULL, 10));
				}
				else if (strarrParam.GetSize() >= 3)
				{
					m_uiarrLangPos.Add(_tcstol(strarrParam.GetAt(0), NULL, 10));
					m_strarrLangName.Add(strarrParam.GetAt(1));
					m_strarrLangIcon.Add(strarrParam.GetAt(2));
					m_uiarrLangNr.Add(iLang++);
				}
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	return bRet;
}

BOOL CRailDiagApp::ReadLangIni(CStdioFile &file)
{
	BOOL			bRet = TRUE;
	CString			key, param;
	CStringArray	strarrParam;
	CUtil			Util;

    if (FindSection(file, _T("DESCRIPTION")))
    {

		while (bRet && TranslateNextLine(file, key, param))
        {
			if (key == _T("HEAD_NAME_SHORT"))
			{
				m_stStatText.strHeadNameShort = param;
			}
			else if (key == _T("HEAD_NAME_LONG"))
			{
				m_stStatText.strHeadName = param;
			}
			else if (key == _T("HEAD_CITY"))
			{
				m_stStatText.strHeadCity = param;
			}
			else if (key == _T("COMPANY"))
			{
				m_stStatText.strCompany = param;
			}
			else if (key == _T("LOC_TYPE"))
			{
				m_stStatText.strLocType = param;
			}
			else if (key == _T("LOC_NUMBER"))
			{
				m_stStatText.strLocNumber = param;
			}
			else if (key == _T("MOTOR_TYPE"))
			{
				m_stStatText.strMotorType = param;
			}
			else if (key == _T("MOTOR_NUMBER"))
			{
				m_stStatText.strMotorNumber = param;
			}
			else if (key == _T("DURATION_COUNT"))
			{
				m_stStatText.strDurationCount = param;
			}
			else if (key == _T("TERMINAL"))
			{
				m_stStatText.strTerminal = param;
			}
			else if (key == _T("LAPTOP"))
			{
				m_stStatText.strDiag = param;
			}
			else if (key == _T("MODE_DESC"))
			{
				m_stStatText.strMode = param;
			}
			else if (key == _T("MODE_0"))
			{
				m_stStatText.strMode0 = param;
			}
			else if (key == _T("MODE_1"))
			{
				m_stStatText.strMode1 = param;
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet && FindSection(file, _T("LIST_BINARY")))
    {
		m_stStatBinary.strarrRow.RemoveAll();
		while (bRet && TranslateNextLine(file, key, param))
        {
			if (key == _T("TITLE"))
			{
				m_stStatBinary.strTitle = param;
			}
			else if (key == _T("COLUMN_TITLE"))
			{
				m_stStatBinary.strarrRow.Add(param);
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet && FindSection(file, _T("LIST_ANALOG")))
    {
		m_stStatAnalog.strarrRow.RemoveAll();
		while (bRet && TranslateNextLine(file, key, param))
        {
			if (key == _T("TITLE"))
			{
				m_stStatAnalog.strTitle = param;
			}
			else if (key == _T("COLUMN_TITLE"))
			{
				m_stStatAnalog.strarrRow.Add(param);
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet && FindSection(file, _T("LIST_ALARM")))
    {
		m_stStatAlarm.strarrRow.RemoveAll();
		while (bRet && TranslateNextLine(file, key, param))
        {
			if (key == _T("TITLE"))
			{
				m_stStatAlarm.strTitle = param;
			}
			else if (key == _T("COLUMN_TITLE"))
			{
				m_stStatAlarm.strarrRow.Add(param);
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet && FindSection(file, _T("LIST_DURATION")))
    {
		m_stStatDuration.strarrRow.RemoveAll();
		while (bRet && TranslateNextLine(file, key, param))
        {
			if (key == _T("TITLE"))
			{
				m_stStatDuration.strTitle = param;
			}
			else if (key == _T("COLUMN_TITLE"))
			{
				m_stStatDuration.strarrRow.Add(param);
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet && FindSection(file, _T("LIST_DURATION_FLEET")))
    {
		m_stStatDurationFleet.strarrRow.RemoveAll();
		while (bRet && TranslateNextLine(file, key, param))
        {
			if (key == _T("TITLE"))
			{
				m_stStatDurationFleet.strTitle = param;
			}
			else if (key == _T("COLUMN_TITLE"))
			{
				m_stStatDurationFleet.strarrRow.Add(param);
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet && FindSection(file, _T("LIST_HISTORY")))
    {
		m_stStatHistory.strarrRow.RemoveAll();
		while (bRet && TranslateNextLine(file, key, param))
        {
			if (key == _T("TITLE"))
			{
				m_stStatHistory.strTitle = param;
			}
			else if (key == _T("COLUMN_TITLE"))
			{
				m_stStatHistory.strarrRow.Add(param);
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet && FindSection(file, _T("LIST_HISTORY_STATISTIC")))
    {
		m_stStatHistory2.strarrRow.RemoveAll();
		while (bRet && TranslateNextLine(file, key, param))
        {
			if (key == _T("TITLE"))
			{
				m_stStatHistory2.strTitle = param;
			}
			else if (key == _T("COLUMN_TITLE"))
			{
				m_stStatHistory2.strarrRow.Add(param);
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet && FindSection(file, _T("MENU_MAIN")))
    {
		while (bRet && TranslateNextLine(file, key, param))
        {
			if (key == _T("FILE"))
			{
				m_stMenu.strFile = param;
			}
			else if (key == _T("CONNECTION"))
			{
				m_stMenu.strConnection = param;
			}
			else if (key == _T("DATA"))
			{
				m_stMenu.strData = param;
			}
			else if (key == _T("VIEW"))
			{
				m_stMenu.strView = param;
			}
			else if (key == _T("WINDOW"))
			{
				m_stMenu.strWindow = param;
			}
			else if (key == _T("HELP"))
			{
				m_stMenu.strHelp = param;
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet && FindSection(file, _T("MENU_FILE")))
    {
		while (bRet && TranslateNextLine(file, key, param))
        {
			param = MenuReplaceTab(param);
			param = ReplaceCRLF(param);

			if (key == _T("OPEN"))
			{
				m_stMenu.stMnuFile.strOpen = param;
			}
			else if (key == _T("CLOSE"))
			{
				m_stMenu.stMnuFile.strClose = param;
			}
			else if (key == _T("SAVE"))
			{
				m_stMenu.stMnuFile.strSave = param;
			}
			else if (key == _T("SAV_AS"))
			{
				m_stMenu.stMnuFile.strSaveAs = param;
			}
			else if (key == _T("EXPORT"))
			{
				m_stMenu.stMnuFile.strExport = param;
			}
			else if (key == _T("PRINT"))
			{
				m_stMenu.stMnuFile.strPrint = param;
			}
			else if (key == _T("PR_PREVIEW"))
			{
				m_stMenu.stMnuFile.strPrintPreview = param;
			}
			else if (key == _T("PR_SETTINGS"))
			{
				m_stMenu.stMnuFile.strPrintSettings = param;
			}
			else if (key == _T("LAST"))
			{
				m_stMenu.stMnuFile.strLast = param;
			}
			else if (key == _T("EXIT"))
			{
				m_stMenu.stMnuFile.strExit = param;
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet && FindSection(file, _T("MENU_CONNECTION")))
    {
		while (bRet && TranslateNextLine(file, key, param))
        {
			param = MenuReplaceTab(param);
			param = ReplaceCRLF(param);

			if (key == _T("CON_SERIAL"))
			{
				m_stMenu.stMnuCon.strConSerial = param;
			}
			else if (key == _T("CON_MODEM"))
			{
				m_stMenu.stMnuCon.strConModem = param;
			}
			else if (key == _T("CONNECT"))
			{
				m_stMenu.stMnuCon.strConnect = param;
			}
			else if (key == _T("DISCONNECT"))
			{
				m_stMenu.stMnuCon.strDisconnect = param;
			}
			else if (key == _T("SETTINGS"))
			{
				m_stMenu.stMnuCon.strSettings = param;
			}
			else if (key == _T("STATE"))
			{
				m_stMenu.stMnuCon.strState = param;
			}
			else if (key == _T("PHONE_BOOK"))
			{
				m_stMenu.stMnuCon.strPhoneBook = param;
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet && FindSection(file, _T("MENU_DATA")))
    {
		while (bRet && TranslateNextLine(file, key, param))
        {
			param = MenuReplaceTab(param);
			param = ReplaceCRLF(param);

			if (key == _T("BINARY"))
			{
				m_stMenu.stMnuData.strBinary = param;
			}
			else if (key == _T("ANALOG"))
			{
				m_stMenu.stMnuData.strAnalog = param;
			}
			else if (key == _T("ALARM"))
			{
				m_stMenu.stMnuData.strAlarm = param;
			}
			else if (key == _T("DURATION"))
			{
				m_stMenu.stMnuData.strDuration = param;
			}
			else if (key == _T("HISTORY"))
			{
				m_stMenu.stMnuData.strHistory = param;
			}
			else if (key == _T("HIST20"))
			{
				m_stMenu.stMnuData.strHistory20 = param;
			}
			else if (key == _T("HIST100"))
			{
				m_stMenu.stMnuData.strHistory100 = param;
			}
			else if (key == _T("HIST_STAT"))
			{
				m_stMenu.stMnuData.strHistory2 = param;
			}
			else if (key == _T("CONFIG_SETTINGS"))
			{
				m_stMenu.stMnuData.strConfigSettings = param;
			}
			else if (key == _T("CONFIG_SEND"))
			{
				m_stMenu.stMnuData.strConfigSend = param;
			}
			else if (key == _T("RELOAD"))
			{
				m_stMenu.stMnuData.strReload = param;
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet && FindSection(file, _T("MENU_VIEW")))
    {
		while (bRet && TranslateNextLine(file, key, param))
        {
			param = MenuReplaceTab(param);
			param = ReplaceCRLF(param);

			if (key == _T("SYMBOL_BAR"))
			{
				m_stMenu.stMnuView.strSymbolBar = param;
			}
			else if (key == _T("STATUS_BAR"))
			{
				m_stMenu.stMnuView.strStatusBar = param;
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet && FindSection(file, _T("MENU_WINDOW")))
    {
		while (bRet && TranslateNextLine(file, key, param))
        {
			param = MenuReplaceTab(param);
			param = ReplaceCRLF(param);

			if (key == _T("NEW"))
			{
				m_stMenu.stMnuWnd.strNew = param;
			}
			else if (key == _T("CASCADE"))
			{
				m_stMenu.stMnuWnd.strCascade = param;
			}
			else if (key == _T("TILE"))
			{
				m_stMenu.stMnuWnd.strTile = param;
			}
			else if (key == _T("ARRANGE"))
			{
				m_stMenu.stMnuWnd.strArrange = param;
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet && FindSection(file, _T("MENU_HELP")))
    {
		while (bRet && TranslateNextLine(file, key, param))
        {
			param = MenuReplaceTab(param);
			param = ReplaceCRLF(param);

			if (key == _T("FINDER"))
			{
				m_stMenu.stMnuHelp.strFinder = param;
			}
			else if (key == _T("ABOUT"))
			{
				m_stMenu.stMnuHelp.strAbout = param;
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet && FindSection(file, _T("DIALOG_SERIAL")))
    {
		while (bRet && TranslateNextLine(file, key, param))
        {
			if (key == _T("TITLE"))
			{
				m_stDlgSerCon.strTitle = param;
			}
			else if (key == _T("PORT"))
			{
				m_stDlgSerCon.strPort = param;
			}
			else if (key == _T("BPS"))
			{
				m_stDlgSerCon.strBps = param;
			}
			else if (key == _T("BITS"))
			{
				m_stDlgSerCon.strBits = param;
			}
			else if (key == _T("PARITY"))
			{
				m_stDlgSerCon.strParity = param;
			}
			else if (key == _T("STOP_BITS"))
			{
				m_stDlgSerCon.strStopBits = param;
			}
			else if (key == _T("CONTROL"))
			{
				m_stDlgSerCon.strControl = param;
			}
			else if (key == _T("CANCEL"))
			{
				m_stDlgSerCon.strCancel = param;
			}
			else if (key == _T("OK"))
			{
				m_stDlgSerCon.strOk = param;
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet && FindSection(file, _T("DIALOG_MODEM")))
    {
		while (bRet && TranslateNextLine(file, key, param))
        {
			if (key == _T("TITLE"))
			{
				m_stDlgModemCon.strTitle = param;
			}
			else if (key == _T("CONNECTION_NAME"))
			{
				m_stDlgModemCon.strConnectionName = param;
			}
			else if (key == _T("DESCRIPTION"))
			{
				m_stDlgModemCon.strDesc = param;
			}
			else if (key == _T("NUMBER"))
			{
				m_stDlgModemCon.strNumber = param;
			}
			else if (key == _T("PIN"))
			{
				m_stDlgModemCon.strPin = param;
			}
			else if (key == _T("PUK"))
			{
				m_stDlgModemCon.strPuk = param;
			}
			else if (key == _T("SAVE_PIN"))
			{
				m_stDlgModemCon.strSavePin = param;
			}
			else if (key == _T("CONNECT"))
			{
				m_stDlgModemCon.strConnect = param;
			}
			else if (key == _T("SETTINGS"))
			{
				m_stDlgModemCon.strSettings = param;
			}
			else if (key == _T("DIAL_PREFIX"))
			{
				m_stDlgModemCon.strDialPrefix = param;
			}
			else if (key == _T("HELP_DIAL_PREFIX"))
			{
				m_stDlgModemCon.strHelpDialPrefix = param;
			}
			else if (key == _T("CANCEL"))
			{
				m_stDlgModemCon.strCancel = param;
			}
			else if (key == _T("OK"))
			{
				m_stDlgModemCon.strOk = param;
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet && FindSection(file, _T("DIALOG_TERMINAL_CONFIG")))
    {
		while (bRet && TranslateNextLine(file, key, param))
        {
			if (key == _T("TITLE"))
			{
				m_stDlgTermConf.strTitle = param;
			}
			else if (key == _T("COMPANY"))
			{
				m_stDlgTermConf.strCompany = param;
			}
			else if (key == _T("LOC_TYPE"))
			{
				m_stDlgTermConf.strLocType = param;
			}
			else if (key == _T("LOC_NUMBER"))
			{
				m_stDlgTermConf.strLocNumber = param;
			}
			else if (key == _T("MOTOR_TYPE"))
			{
				m_stDlgTermConf.strMotorType = param;
			}
			else if (key == _T("MOTOR_NUMBER"))
			{
				m_stDlgTermConf.strMotorNumber = param;
			}
			else if (key == _T("MTU_ORDER_NUMBER"))
			{
				m_stDlgTermConf.strMtuOrderNumber = param;
			}
			else if (key == _T("DISPLAY_TELEPHONE_NUMBER"))
			{
				m_stDlgTermConf.strDisplayTelNumber = param;
			}
			else if (key == _T("INHIBIT_TIME_SMS"))
			{
				m_stDlgTermConf.strInhibitTime = param;
			}
			else if (key == _T("TELEPHONE_NUMBER_1_SMS"))
			{
				m_stDlgTermConf.strTelNumberSMS1 = param;
			}
			else if (key == _T("TELEPHONE_NUMBER_2_SMS"))
			{
				m_stDlgTermConf.strTelNumberSMS2 = param;
			}
			else if (key == _T("TELEPHONE_NUMBER_3_SMS"))
			{
				m_stDlgTermConf.strTelNumberSMS3 = param;
			}
			else if (key == _T("PIN"))
			{
				m_stDlgTermConf.strPin = param;
			}
			else if (key == _T("PUK"))
			{
				m_stDlgTermConf.strPuk = param;
			}
			else if (key == _T("MESSAGE_SEND"))
			{
				m_stDlgTermConf.strMessageSend = param;
			}
			else if (key == _T("MESSAGE_OK"))
			{
				m_stDlgTermConf.strMessageOk = param;
			}
			else if (key == _T("MESSAGE_ERR"))
			{
				m_stDlgTermConf.strMessageError = param;
			}
			else if (key == _T("SEND"))
			{
				m_stDlgTermConf.strSend = param;
			}
			else if (key == _T("CANCEL"))
			{
				m_stDlgTermConf.strCancel = param;
			}
			else if (key == _T("OK"))
			{
				m_stDlgTermConf.strOk = param;
			}
			else if (key == _T("LOAD"))
			{
				m_stDlgTermConf.strLoad = param;
			}
			else if (key == _T("SAVE"))
			{
				m_stDlgTermConf.strSave = param;
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet && FindSection(file, _T("DIALOG_PHONE_BOOK")))
    {
		while (bRet && TranslateNextLine(file, key, param))
        {
			if (key == _T("TITLE"))
			{
				m_stDlgPhoneBook.strTitle = param;
			}
			else if (key == _T("DESCRIPTION"))
			{
				m_stDlgPhoneBook.strDesc = param;
			}
			else if (key == _T("NUMBER"))
			{
				m_stDlgPhoneBook.strNumber = param;
			}
			else if (key == _T("NEW_ENTRY"))
			{
				m_stDlgPhoneBook.strNewEntry = param;
			}
			else if (key == _T("EDIT_ENTRY"))
			{
				m_stDlgPhoneBook.strEditEntry = param;
			}
			else if (key == _T("DELETE_ENTRY"))
			{
				m_stDlgPhoneBook.strDeleteEntry = param;
			}
			else if (key == _T("IMPORT"))
			{
				m_stDlgPhoneBook.strImport = param;
			}
			else if (key == _T("EXPORT"))
			{
				m_stDlgPhoneBook.strExport = param;
			}
			else if (key == _T("CANCEL"))
			{
				m_stDlgPhoneBook.strCancel = param;
			}
			else if (key == _T("OK"))
			{
				m_stDlgPhoneBook.strOk = param;
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet && FindSection(file, _T("DIALOG_ENTRY_PHONE_BOOK")))
    {
		while (bRet && TranslateNextLine(file, key, param))
        {
			if (key == _T("TITLE"))
			{
				m_stDlgPhoneBookEntry.strTitle = param;
			}
			else if (key == _T("DESCRIPTION"))
			{
				m_stDlgPhoneBookEntry.strDesc = param;
			}
			else if (key == _T("NUMBER"))
			{
				m_stDlgPhoneBookEntry.strNumber = param;
			}
			else if (key == _T("HELP_NUMBER"))
			{
				m_stDlgPhoneBookEntry.strHelpNumber = param;
			}
			else if (key == _T("CANCEL"))
			{
				m_stDlgPhoneBookEntry.strCancel = param;
			}
			else if (key == _T("OK"))
			{
				m_stDlgPhoneBookEntry.strOk = param;
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet && FindSection(file, _T("DIALOG_PROGRESS")))
    {
		while (bRet && TranslateNextLine(file, key, param))
        {
			if (key == _T("TITLE"))
			{
				m_stDlgProg.strTitle = param;
			}
			else if (key == _T("CANCEL"))
			{
				m_stDlgProg.strCancel = param;
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	if (bRet && FindSection(file, _T("CONNECTION_STATUS")))
    {
		while (bRet && TranslateNextLine(file, key, param))
        {
			param = ReplaceCRLF(param);

			if (key == _T("UNKNOWN"))
			{
				m_stConStatus.strUnknown = param;
			}
			else if (key == _T("DISCONNECTING"))
			{
				m_stConStatus.strDisconnecting = param;
			}
			else if (key == _T("DISCONNECTED"))
			{
				m_stConStatus.strDisconnected = param;
			}
			else if (key == _T("INIT"))
			{
				m_stConStatus.strInit = param;
			}
			else if (key == _T("IDLE"))
			{
				m_stConStatus.strIdle = param;
			}
			else if (key == _T("OPENING"))
			{
				m_stConStatus.strOpening = param;
			}
			else if (key == _T("ANSWERING"))
			{
				m_stConStatus.strAnswering = param;
			}
			else if (key == _T("CONNECTED"))
			{
				m_stConStatus.strConnected = param;
			}
			else if (key == _T("CLOSE"))
			{
				m_stConStatus.strClose = param;
			}
			else if (key == _T("NODIALTONE"))
			{
				m_stConStatus.strNoDialTone = param;
			}
			else if (key == _T("BUSY"))
			{
				m_stConStatus.strBusy = param;
			}
			else if (key == _T("NOANSWER"))
			{
				m_stConStatus.strNoAnswer = param;
			}
			else if (key == _T("WRONGPIN"))
			{
				m_stConStatus.strWrongPin = param;
			}
			else if (key == _T("WRONGPUK"))
			{
				m_stConStatus.strWrongPuk = param;
			}
			else if (key == _T("INUSE"))
			{
				m_stConStatus.strInUse = param;
			}
			else if (key == _T("MESSAGE"))
			{
				m_stConStatus.strMessage = param;
			}
			else if (key == _T("MODEMDRIVERERROR"))
			{
				m_stConStatus.strModemDriverError = param;
			}
			else if (key == _T("MODEMOFF"))
			{
				m_stConStatus.strModemOff = param;
			}
			else if (key == _T("RECONNECT"))
			{
				m_stConStatus.strReconnect = param;
			}
			else if (key == _T("NODATA"))
			{
				m_stConStatus.strNoData = param;
			}
			else if (key == _T("NOACCESS"))
			{
				m_stConStatus.strNoAccess = param;
			}
			else if (key == _T("SERIALOFF"))
			{
				m_stConStatus.strSerialOff = param;
			}
			else if (key == _T("TRANSFER_CANCEL"))
			{
				m_stConStatus.strTransferCancel = param;
			}
		}
	}
	else
	{
		bRet = FALSE;
	}
//Dj
	if (bRet && FindSection(file, _T("MENU_FLEET")))
    {
		while (bRet && TranslateNextLine(file, key, param))
        {
			param = ReplaceCRLF(param);

			if (key == _T("MGR"))
			{
				m_stMenu.stMnuFleet.strFleetMgr = param;
			}
			else if (key == _T("EDIT"))
			{
				m_stMenu.stMnuFleet.strFleetEdit = param;
			}
		}
	}
	else
	{
		bRet = FALSE;
	}


		if (bRet && FindSection(file, _T("DIALOG_FLEET_EDIT")))
    {
		while (bRet && TranslateNextLine(file, key, param))
        {
			if (key == _T("TITLE"))
			{
				m_stDlgFleetEdit.strTitle = param;
			}
			else if (key == _T("LIST_PHONE_BOOK"))
			{
				m_stDlgFleetEdit.strListPhoneBook = param;
			}
			else if (key == _T("LIST_FLEET"))
			{
				m_stDlgFleetEdit.strListFleet = param;
			}
			else if (key == _T("ROW_NUMBER"))
			{
				m_stDlgFleetEdit.strRowNumber = param;
			}
			else if (key == _T("DESCRIPTION"))
			{
				m_stDlgFleetEdit.strDesc = param;
			}
			else if (key == _T("NUMBER"))
			{
				m_stDlgFleetEdit.strTelNumber = param;
			}
			else if (key == _T("FLEET_NAME"))
			{
				m_stDlgFleetEdit.strFleetName = param;
			}
			else if (key == _T("FLEET_NEW"))
			{
				m_stDlgFleetEdit.strFleetNew = param;
			}
			else if (key == _T("FLEET_DELETE"))
			{
				m_stDlgFleetEdit.strFleetDel = param;
			}
			else if (key == _T("CANCEL"))
			{
				m_stDlgFleetEdit.strCancel = param;
			}
			else if (key == _T("OK"))
			{
				m_stDlgFleetEdit.strOk = param;
			}
			else if (key == _T("IMPORT"))
			{
				m_stDlgFleetEdit.strImport = param;
			}
			else if (key == _T("EXPORT"))
			{
				m_stDlgFleetEdit.strExport = param;
			}
			else if (key == _T("MESSAGE1"))
			{
				m_stDlgFleetEdit.strMess1 = param;
			}
			else if (key == _T("MESSAGE2"))
			{
				m_stDlgFleetEdit.strMess2 = param;
			}
		}
	}
	else
	{
		bRet = FALSE;
	}
//Dj
	
		
	return bRet;
}

void CRailDiagApp::SetLang(int iLang)
{
	CString	strBuf;
	
	m_iLang = iLang;
	strBuf.Format(_T(" %d "), m_iLang);
	WriteProfileString(_T("Settings"), _T("Language"), strBuf);

	CStdioFile	file;
	CString		strFile;
	CString		strTmp;

	strTmp.Format(_T("\\Lang_%2.2d.ini"), m_iLang);
	strFile = m_strAppPath + strTmp;
    if (file.Open(strFile, CFile::modeRead | CFile::typeBinary))
	{
		ReadLangIni(file);
		file.Close();
	}
	else
	{
		// open default lang
		strTmp.Format(_T("\\Lang_%2.2d.ini"), m_iDefaultLang);
		strFile = m_strAppPath + strTmp;
		if (file.Open(strFile, CFile::modeRead | CFile::typeBinary))
		{
			ReadLangIni(file);
			file.Close();
		}
	}
}

CString CRailDiagApp::MenuReplaceTab(CString strIn)
{
	int		iPos;

	iPos = strIn.Find(_T("\\t"));
	if (iPos != -1)
	{
		strIn.Delete(iPos);
		strIn.SetAt(iPos, '\t');
	}

	return strIn;
}


CString CRailDiagApp::ReplaceCRLF(CString strIn)
{
	int		iPos;

	for(iPos = 0; iPos != -1; )
	{
		iPos = strIn.Find(_T("\\r"));
		if (iPos != -1)
		{
			strIn.Delete(iPos);
			strIn.SetAt(iPos, '\r');
		}
	}
	for(iPos = 0; iPos != -1; )
	{
		iPos = strIn.Find(_T("\\n"));
		if (iPos != -1)
		{
			strIn.Delete(iPos);
			strIn.SetAt(iPos, '\n');
		}
	}

	return strIn;
}


void CRailDiagApp::SetReconnect(BOOL bReconnect)
{
	m_bReconnect = bReconnect;
}

BOOL CRailDiagApp::GetReconnect()
{
	return m_bReconnect;
}

void CRailDiagApp::Write2Log(CString strMsg)
{
	CString		csBuf;

	if (m_bLogFile)
	{
		time_t t;
		time(&t);

		csBuf.Format(_T("%s, am: %s\n"), strMsg, _wctime(&t));
		TRACE(csBuf);
		m_fLogFile.WriteString(csBuf);
	}
}

void CRailDiagApp::ClearLocSettingsLoad()
{
	m_stLocSettingsLoad.strCompany = _T("");
	m_stLocSettingsLoad.strLocNumber = _T("");
	m_stLocSettingsLoad.strLocType = _T("");
	m_stLocSettingsLoad.strMotorNumber = _T("");
	m_stLocSettingsLoad.strMotorType = _T("");
	m_stLocSettingsLoad.strMtuOrderNumber = _T("");
}


void CRailDiagApp::SetLangOrder(CUIntArray *puiarrLangSelected)
{
	m_uiarrLangOrder.RemoveAll();

	int		iLang;
	int		iLangIdx;
	int		i, j;
	BOOL	bDone;

	for (i = 0; i < puiarrLangSelected->GetSize(); i++)
	{
		iLang = puiarrLangSelected->GetAt(i);

		iLangIdx = -1;
		for (j = 0; iLangIdx == -1 && j < m_uiarrLangNr.GetSize(); j++)
		{
			if (m_uiarrLangNr.GetAt(j) == (UINT)iLang)
			{
				// sprache gefunden index suchen und entsprechend in LangOrder übernehmen
				iLangIdx = j;
			}
		}

		if (iLangIdx != -1)
		{
			// position in OrderListe suchen
			bDone = FALSE;
			for (j = 0; !bDone && j < m_uiarrLangOrder.GetSize(); j++)
			{
				if (m_uiarrLangPos.GetAt(m_uiarrLangOrder.GetAt(j)) >= m_uiarrLangPos.GetAt(iLangIdx))
				{
					m_uiarrLangOrder.InsertAt(j, iLangIdx);
					// InsertPosition found
					bDone = TRUE;
				}
			}

			if (!bDone)
			{
				m_uiarrLangOrder.Add(iLangIdx);
			}
		}

	}
}

void CRailDiagApp::OnPhoneBook() 
{
	// TODO: Add your command handler code here
	CPhoneBook	PhoneBook;
	int			i;

	for (i = 0; i < m_strarrPhoneBookDesc.GetSize(); i++)
	{
		PhoneBook.m_strarrDescription.Add(m_strarrPhoneBookDesc.GetAt(i));
		PhoneBook.m_strarrNumber.Add(m_strarrPhoneBookNumber.GetAt(i));
	}

	if (PhoneBook.DoModal() == IDOK)
	{
		m_strarrPhoneBookDesc.RemoveAll();
		m_strarrPhoneBookNumber.RemoveAll();

		for (i = 0; i < PhoneBook.m_strarrDescription.GetSize(); i++)
		{
			m_strarrPhoneBookDesc.Add(PhoneBook.m_strarrDescription.GetAt(i));
			m_strarrPhoneBookNumber.Add(PhoneBook.m_strarrNumber.GetAt(i));
		}

		if (PhoneBook.m_bModified)
		{
			CString			strFile;
			CStdioFile		file;

			// Telefonbuch in PhoneBook.ini speichern
			strFile = m_strAppPath + _T("\\PhoneBook.ini");
			if (file.Open(strFile, CFile::modeWrite | CFile::modeCreate | CFile::typeBinary))
			{
				WritePhoneBookIni(file, m_strarrPhoneBookDesc, m_strarrPhoneBookNumber);

				file.Close();
			}
			else
			{
				CString		strMsg;
				strMsg.Format(_T("Could not open Ini-file:\n%s\n\nPlease check the File and resart the Application!"), 
					strFile);
				AfxMessageBox(strMsg, MB_OK|MB_ICONSTOP);
			}
		}

	}
}

void CRailDiagApp::OnUpdatePhoneBook(CCmdUI* pCmdUI) 
{
	if (m_iComStateConnect == _STATE_CONN_DISCONNECTED)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

BOOL CRailDiagApp::ReadPhoneBookIni(CStdioFile &file, CStringArray &strarrDesc, CStringArray &strarrNumber)
{
	BOOL			bRet = TRUE;
	CString			key, param;

	strarrDesc.RemoveAll();
	strarrNumber.RemoveAll();

    if (FindSection(file, _T("PHONE_BOOK")))
    {
		while (bRet && TranslateNextLine(file, key, param))
        {
			strarrDesc.Add(key);
			strarrNumber.Add(param);
		}
	}
	else
	{
		bRet = FALSE;
	}

	return bRet;
}

BOOL CRailDiagApp::WritePhoneBookIni(CStdioFile &file, CStringArray &strarrDesc, CStringArray &strarrNumber)
{
	BOOL	bRet = TRUE;
	CString	buf;

	file.WriteString(_T("[PHONE_BOOK]\r\n"));
	for (int i = 0; i < strarrDesc.GetSize(); i++)
	{
		buf.Format(_T("%s = %s\r\n"), strarrDesc.GetAt(i), strarrNumber.GetAt(i));
		file.WriteString(buf);
	}

	return bRet;
}

BOOL CRailDiagApp::ReadFleetIni(CStdioFile &file, CStringArray &strarrDesc, CPtrArray &ptrarrFleetElem)
{
	BOOL			bRet = FALSE;
	CString			section;
	CString			key, param;
	FLEET_ELEM		*pFleetElem;
	int				i;

	strarrDesc.RemoveAll();
	for (i = 0; i < ptrarrFleetElem.GetCount(); i++)
	{
		if (ptrarrFleetElem[i] != NULL)
		{
			delete ptrarrFleetElem[i];
		}
	}
	ptrarrFleetElem.RemoveAll();

    while (TranslateSection(file, section))
    {
		strarrDesc.Add(section);

		pFleetElem = new FLEET_ELEM;
		ptrarrFleetElem.Add(pFleetElem);

		bRet = TRUE;
		while (bRet && TranslateNextLine(file, key, param))
        {
			pFleetElem->strarrPhoneBookDescr.Add(key);
			pFleetElem->strarrPhoneBookNumber.Add(param);
		}
	}

	return bRet;
}

BOOL CRailDiagApp::WriteFleetIni(CStdioFile &file, CStringArray &strarrDesc, CPtrArray &ptrarrFleetElem)
{
	BOOL			bRet = TRUE;
	CString			buf;
	FLEET_ELEM		*pFleetElem;
	int				i, j;

	for (i = 0; i < strarrDesc.GetSize(); i++)
	{
		buf.Format(_T("\r\n[%s]\r\n"), strarrDesc.GetAt(i));
		file.WriteString(buf);
		pFleetElem = (FLEET_ELEM*)ptrarrFleetElem.GetAt(i);
		if (pFleetElem)
		{
			for (j = 0; j < pFleetElem->strarrPhoneBookDescr.GetSize(); j++)
			{
				buf.Format(_T("%s = %s\r\n"), 
					pFleetElem->strarrPhoneBookDescr.GetAt(j), 
					pFleetElem->strarrPhoneBookNumber.GetAt(j));
				file.WriteString(buf);
			}
		}
	}

	return bRet;
}

void CRailDiagApp::UpdateDescNumber(CString strDescriptionOld, CString strNumberOld, 
				CString strDescription, CString strNumber)
{
	int i, j;

	for (i = 0; i < m_ptrarrFleetElem.GetCount(); i++)
	{
		FLEET_ELEM *pFleetElem;
		pFleetElem = (FLEET_ELEM*)m_ptrarrFleetElem.GetAt(i);
		for (j = 0; j < pFleetElem->strarrPhoneBookDescr.GetCount(); j++)
		{
			if (pFleetElem->strarrPhoneBookDescr.GetAt(j) == strDescriptionOld)
			{
				pFleetElem->strarrPhoneBookDescr[j] = strDescription;
				pFleetElem->strarrPhoneBookNumber[j] = strNumber;
			}
		}
	}
}

DWORD WINAPI CRailDiagApp::_MsgThread(LPVOID lParam)
{
	return ((CRailDiagApp *)lParam)->MsgThread();

}

DWORD CRailDiagApp::MsgThread(void)
{
	CString strMsg;

	m_bMessage = TRUE;
	AfxMessageBox(m_strMessage);
	m_bMessage = FALSE;

	return 0;
}

void CRailDiagApp::Message(CString cStrMsg)
{
	DWORD dwId;
	HANDLE	hMsgThread;

	m_strMessage = cStrMsg;

	hMsgThread = ::CreateThread(NULL, 0, CRailDiagApp::_MsgThread, (LPVOID)this, 0, &dwId);
	TRACE(_T("Msg- Thread created: h = 0x%08X, ID = 0x%08X\n"), hMsgThread, dwId);

}

void CRailDiagApp::OnComDisconnectModem() 
{
	OnComDisconnect();
}

void CRailDiagApp::OnUpdateComDisconnectModem(CCmdUI* pCmdUI) 
{
	if (m_iConnType == 1 && 
		(m_iComStateConnect == _STATE_CONN_CONNECTED || 
		 m_iComStateConnect == _STATE_CONN_CONNECTING || 
		GetReconnect()))
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CRailDiagApp::OnComDisconnectSer() 
{
	OnComDisconnect();
}

void CRailDiagApp::OnUpdateComDisconnectSer(CCmdUI* pCmdUI) 
{
	if (m_iConnType == 0 && 
		(m_iComStateConnect == _STATE_CONN_CONNECTED ||
		 GetReconnect())
	   )
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CRailDiagApp::OnUpdateConnState(CCmdUI* pCmdUI) 
{
//	if (GetConnectionState() == EN_CONN_CONNECTED)
//	{
//		pCmdUI->Enable(TRUE);
//	}
//	else
//	{
		if (GetConnectionState() == EN_CONN_OPENING ||
			GetConnectionState() == EN_CONN_ANSWERING)
		{
			pCmdUI->Enable(m_bBlinkState);
			if (GetTickCount() > m_dwBlinkNextTick)
			{
				m_bBlinkState = !m_bBlinkState;
				if (m_bBlinkState)
				{
					m_dwBlinkNextTick = GetTickCount() + m_dwBlinkFastOn;
				}
				else
				{
					m_dwBlinkNextTick = GetTickCount() + m_dwBlinkFastOff;
				}
			}
		}
		else if (GetConnectionState() == EN_CONN_CONNECTED)
		{
//			TRACE(_T("CRailDiagApp::OnUpdateConnState Connected tick %u\n"), GetTickCount());
			pCmdUI->Enable(m_bBlinkState);
			if (GetTickCount() > m_dwBlinkNextTick)
			{
				m_bBlinkState = !m_bBlinkState;
//				TRACE(_T("CRailDiagApp::OnUpdateConnState enable %d\n"), m_bBlinkState);
				if (m_bBlinkState)
				{
					m_dwBlinkNextTick = GetTickCount() + m_dwBlinkSlowOn;
				}
				else
				{
					m_dwBlinkNextTick = GetTickCount() + m_dwBlinkSlowOff;
				}
			}
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
//	}
}

void CRailDiagApp::OnUpdateNewDurationData(CCmdUI* pCmdUI) 
{
	// FleetManagement open
	if (m_iFleetMgrState == EN_FLEET_ON)
		pCmdUI->Enable(TRUE);
	else
	{
		if (GetConnectionState() == EN_CONN_CONNECTED)
		{
			pCmdUI->Enable(TRUE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
}

void CRailDiagApp::OnNewDurationData() 
{
	m_bUserInput = TRUE;
	if (m_iFleetMgrState == EN_FLEET_ON)
	{
		m_eDataType = EN_DATA_TYPE_DURATION_FLEET;
	}
	else
	{
		m_eDataType = EN_DATA_TYPE_DURATION;
	}

	if (m_iFleetMgrState == EN_FLEET_OFF || enFleetStateLangSet == m_enFleetState)
	{
		if (m_iFleetMgrState == EN_FLEET_ON)
		{
			if (m_iDurationFleetCount >= 1)
			{ 
				CMainFrame		*pMainFrame;

				pMainFrame = (CMainFrame*)GetMainWnd();
				if (pMainFrame && pMainFrame->m_pDoc)
				{
					pMainFrame->m_pDoc->NewData4Document(); // daten neu anfordern!!
				}
				return;
			}
			m_iDurationFleetCount++;
		}
		CWinApp::OnFileNew();	
	}
	else
	{
		// fleet management
		StopFleetThread();
		StartFleetThread();
	}
}

//Fleetmgr
void CRailDiagApp::OnFleetMgr() //DJ
{
	CMainFrame		*pMainFrame;

	pMainFrame = (CMainFrame*)GetMainWnd();

	m_bFleetCmdActive = FALSE;
	m_iDurationFleetCount = 0;

	if (m_iFleetMgrState == EN_FLEET_OFF)
	{
		m_iFleetMgrState = EN_FLEET_ON;
		if (pMainFrame)
		{
			pMainFrame->EnableFleetList(TRUE);
		}
	}
	else
	{	
		m_iFleetMgrState = EN_FLEET_OFF;
		if (pMainFrame)
		{
			pMainFrame->EnableFleetList(FALSE);
		}
		StopFleetThread();
	}

	CString	strBuf;
	strBuf.Format(_T("%d "), m_iConnType);
	WriteProfileString(_T("Settings"), _T("ConnType"), strBuf);
	m_bPUK = FALSE;

	//OnComConnect();
}

void CRailDiagApp::OnUpdateFleetMgr(CCmdUI* pCmdUI) //DJ
{
		if (m_iComStateConnect == _STATE_CONN_DISCONNECTED)
		{
			pCmdUI->Enable(TRUE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
}

void CRailDiagApp::OnFleetEdit() //DJ
{
	CDlgFleetEdit	FleetEdit;
	int				i, j;

	// Telefonbucheinträge übernehmen
	for (i = 0; i < m_strarrPhoneBookDesc.GetSize(); i++)
	{
		FleetEdit.m_strarrDescription.Add(m_strarrPhoneBookDesc.GetAt(i));
		FleetEdit.m_strarrNumber.Add(m_strarrPhoneBookNumber.GetAt(i));
	}

	// Flottenliste übernehmen
	FleetEdit.m_strarrFleetDesc.RemoveAll();
	for (i = 0; i < m_strarrFleetDesc.GetCount(); i++)
	{
		FleetEdit.m_strarrFleetDesc.Add(m_strarrFleetDesc.GetAt(i));
	}
	for (i = 0; i < FleetEdit.m_ptrarrFleetElem.GetCount(); i++) // Elemente sauber aufräumen
	{
		FLEET_ELEM *pFleetElem;
		pFleetElem = (FLEET_ELEM*)FleetEdit.m_ptrarrFleetElem.GetAt(i);
		pFleetElem->strarrPhoneBookDescr.RemoveAll();
		pFleetElem->strarrPhoneBookNumber.RemoveAll();
		delete pFleetElem;
	}
	FleetEdit.m_ptrarrFleetElem.RemoveAll();

	for (i = 0; i < m_ptrarrFleetElem.GetCount(); i++)
	{
		FLEET_ELEM *pFleetElem, *pFleetElemInsert;
		pFleetElem = (FLEET_ELEM*)m_ptrarrFleetElem.GetAt(i);
		pFleetElemInsert = new FLEET_ELEM;
		for (j = 0; j < pFleetElem->strarrPhoneBookDescr.GetCount(); j++)
		{
			pFleetElemInsert->strarrPhoneBookDescr.Add(pFleetElem->strarrPhoneBookDescr.GetAt(j));
			pFleetElemInsert->strarrPhoneBookNumber.Add(pFleetElem->strarrPhoneBookNumber.GetAt(j));
		}
		FleetEdit.m_ptrarrFleetElem.Add(pFleetElemInsert);
	}

	m_iFleetMgrState = EN_FLEET_EDIT;
	if (FleetEdit.DoModal() == IDOK)
	{
		// geänderte Werte wieder zurück übernehmen
		m_strarrFleetDesc.RemoveAll();
		for (i = 0; i < FleetEdit.m_strarrFleetDesc.GetCount(); i++)
		{
			m_strarrFleetDesc.Add(FleetEdit.m_strarrFleetDesc.GetAt(i));
		}
		for (i = 0; i < m_ptrarrFleetElem.GetCount(); i++) // Elemente sauber aufräumen
		{
			FLEET_ELEM *pFleetElem;
			pFleetElem = (FLEET_ELEM*)m_ptrarrFleetElem.GetAt(i);
			if (pFleetElem->strarrPhoneBookDescr.GetCount() > 0)
				pFleetElem->strarrPhoneBookDescr.RemoveAll();
			if (pFleetElem->strarrPhoneBookNumber.GetCount() > 0)
				pFleetElem->strarrPhoneBookNumber.RemoveAll();
			delete pFleetElem;
		}
		m_ptrarrFleetElem.RemoveAll();

		for (i = 0; i < FleetEdit.m_ptrarrFleetElem.GetCount(); i++)
		{
			FLEET_ELEM *pFleetElem, *pFleetElemInsert;
			pFleetElem = (FLEET_ELEM*)FleetEdit.m_ptrarrFleetElem.GetAt(i);
			pFleetElemInsert = new FLEET_ELEM;
			for (j = 0; j < pFleetElem->strarrPhoneBookDescr.GetCount(); j++)
			{
				pFleetElemInsert->strarrPhoneBookDescr.Add(pFleetElem->strarrPhoneBookDescr.GetAt(j));
				pFleetElemInsert->strarrPhoneBookNumber.Add(pFleetElem->strarrPhoneBookNumber.GetAt(j));
			}
			m_ptrarrFleetElem.Add(pFleetElemInsert);
		}

		// änderungen speichern
		{
			CString			strFile;
			CStdioFile		file;

			// Telefonbuch in PhoneBook.ini speichern
			strFile = m_strAppPath + _T("\\Fleets.ini");
			if (file.Open(strFile, CFile::modeWrite | CFile::modeCreate | CFile::typeBinary))
			{
				WriteFleetIni(file, m_strarrFleetDesc, m_ptrarrFleetElem);

				file.Close();
			}
			else
			{
				CString		strMsg;
				strMsg.Format(_T("Could not open Ini-file:\n%s\n\nPlease check the File and resart the Application!"), 
					strFile);
				AfxMessageBox(strMsg, MB_OK|MB_ICONSTOP);
			}
		}

		CMainFrame		*pMainFrame;

		pMainFrame = (CMainFrame*)GetMainWnd();
		if (pMainFrame)
		{
			pMainFrame->SetFleetList(m_strarrFleetDesc);
		}

	}

	// aufräumen der FleetEdit Liste
	for (i = 0; i < FleetEdit.m_ptrarrFleetElem.GetCount(); i++)
	{
		FLEET_ELEM *pFleetElem;
		pFleetElem = (FLEET_ELEM*)FleetEdit.m_ptrarrFleetElem.GetAt(i);
		pFleetElem->strarrPhoneBookDescr.RemoveAll();
		pFleetElem->strarrPhoneBookNumber.RemoveAll();
		delete pFleetElem;
	}
	FleetEdit.m_ptrarrFleetElem.RemoveAll();

	m_iFleetMgrState = EN_FLEET_ON;
}

void CRailDiagApp::OnUpdateFleetEdit(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	//if (m_iComStateConnect == _STATE_CONN_DISCONNECTED && !GetReconnect())
	if (m_iFleetMgrState == EN_FLEET_ON)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CRailDiagApp::StartFleetThread(void)
{
	DWORD dwId;

	if (m_hExitFleetEvent == NULL)
	{
		CTime time = CTime::GetCurrentTime();

		m_bFleetCancel = FALSE;
		m_strFleetDateTime.Format(_T("%02d_%02d_%02d_%02d_%02d_%02d"), 
			time.GetYear()%100, time.GetMonth(), time.GetDay(),
			time.GetHour(), time.GetMinute(), time.GetSecond());
		
		m_hExitFleetEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

		m_hFleetThread = ::CreateThread(NULL, 0, CRailDiagApp::FleetThreadHandler, (LPVOID)this, 0, &dwId);
		TRACE(_T("Fleet-Thread created: h = 0x%08X, ID = 0x%08X\n"), m_hFleetThread, dwId);
	}
}

void CRailDiagApp::StopFleetThread(void)
{
	if (m_hFleetThread)
	{
		SetEvent(m_hExitFleetEvent);
		WaitForSingleObject(m_hFleetThread, 5000);
	
		ResetEvent(m_hExitFleetEvent);
		CloseHandle(m_hExitFleetEvent);
		m_hExitFleetEvent = NULL;

		CloseHandle(m_hFleetThread);
		m_hFleetThread = NULL;
	}
}

DWORD WINAPI CRailDiagApp::FleetThreadHandler(LPVOID lParam)
{
	return ((CRailDiagApp *)lParam)->FleetThread();
}

DWORD CRailDiagApp::FleetThread(void)
{
	BOOL			bExit = FALSE;
	BOOL			bDo = TRUE;
	BOOL			bConnected = FALSE;
	CString			strMsg;
	CMainFrame		*pMainFrame;
	int				iIndex = 0;
	BOOL			bWait4Data = FALSE;
	FLEET_ELEM		*pFleetElem = NULL;
	int				iRetryCount;
	BOOL			bStep = FALSE;
	CUIntArray		uiarrFirst;
	CUIntArray		uiarrSecond;
	CUIntArray		uiarrError;
	int				i;

	pMainFrame = (CMainFrame*)GetMainWnd();
	m_uiarrError.RemoveAll();

	if (m_iFleetSelected >= 0 && m_iFleetSelected < m_ptrarrFleetElem.GetCount())
	{
		pFleetElem = (FLEET_ELEM*)m_ptrarrFleetElem.GetAt(m_iFleetSelected);
	}

	if (NULL == pFleetElem)
	{
		return -1;
	}
	else
	{
		uiarrSecond.RemoveAll();
		uiarrFirst.RemoveAll();
		uiarrError.RemoveAll();
		for (i = 0; i < pFleetElem->strarrPhoneBookDescr.GetCount(); i++)
		{
			uiarrFirst.Add(i);
		}
		iIndex = 0;
	}

	if (0 == uiarrFirst.GetCount()) // keine auswahl
	{
		return 0;
	}

	m_iDurationFleetCount = 0;

	pMainFrame->PostMessage(WM_USER_NEW_PROCESS_DIALOG_FLEET_START);
	Sleep(333);
//	byPort = (BYTE)_tcstol(m_strComPort.Right(1), NULL, 10);
	while (!bExit)
	{
		bDo = TRUE;

		if (iIndex >= pFleetElem->strarrPhoneBookDescr.GetCount())
		{
			bExit = TRUE;
		}
		else
		{
			m_strConnDesc = pFleetElem->strarrPhoneBookDescr.GetAt(iIndex);
			m_strConnNumber = pFleetElem->strarrPhoneBookNumber.GetAt(iIndex);
		}

		m_bFleetCmdActive = TRUE;
		pMainFrame->PostMessage(WM_USER_NEW_PROCESS_DIALOG_FLEET_NAME);
//		Sleep(333);
		if (bStep)
		{
			bStep = FALSE;
			pMainFrame->PostMessage(WM_USER_NEW_PROCESS_DIALOG_FLEET_STEP);
		}
		Sleep(333);

		if (WaitForSingleObject(this->m_hExitFleetEvent, 0) == WAIT_OBJECT_0)
			bExit = TRUE;
		else
		{
			iRetryCount = 3;
			m_bFleetConnectActive = TRUE;
			
			TRACE(_T("CRailDiagApp::FleetThread wait for correct state\n"));
			while (bDo && !bExit && m_iComStateConnect != _STATE_CONN_DISCONNECTED)
			{
				if (WaitForSingleObject(this->m_hExitFleetEvent, 33) == WAIT_OBJECT_0)
				{
					bExit = TRUE;
				}
			}

			while(bDo && !bExit && iRetryCount >= 0 && m_iComStateConnect != _STATE_CONN_CONNECTED)
			{
				if (!bExit)
				{
					m_enFleetState = enFleetStateStart;
					pMainFrame->PostMessage(WM_COMMAND, ID_CONN_TYPE_SERIAL);
					//pMainFrame->PostMessage(WM_COMMAND, ID_CONN_TYPE_MODEM);
				}

				TRACE(_T("CRailDiagApp::FleetThread wait for connect\n"));
				while (bDo && !bExit && m_iComStateConnect != _STATE_CONN_CONNECTED)
				{
					if (WaitForSingleObject(this->m_hExitFleetEvent, 33) == WAIT_OBJECT_0)
					{
						bExit = TRUE;
					}
					//if (!m_bFleetConnectActive && m_iComStateConnect == _STATE_CONN_DISCONNECTED)
					//	bDo = FALSE;
				}
				iRetryCount--;
			}

			bConnected = TRUE;
			if (m_iComStateConnect != _STATE_CONN_CONNECTED)
			{
				bDo = FALSE;
				bConnected = FALSE;
			}

			TRACE(_T("CRailDiagApp::FleetThread wait for settings\n"));
			while (bDo && !bExit && m_enFleetState != enFleetStateSettings)
			{
				if (WaitForSingleObject(this->m_hExitFleetEvent, 33) == WAIT_OBJECT_0)
					bExit = TRUE;
				if (m_iComStateConnect != _STATE_CONN_CONNECTED)
					bDo = FALSE;
			}

			TRACE(_T("CRailDiagApp::FleetThread wait for lang get\n"));
			while (bDo && !bExit && m_enFleetState != enFleetStateLangGet)
			{
				if (WaitForSingleObject(this->m_hExitFleetEvent, 33) == WAIT_OBJECT_0)
					bExit = TRUE;
				if (m_iComStateConnect != _STATE_CONN_CONNECTED)
					bDo = FALSE;
			}

			TRACE(_T("CRailDiagApp::FleetThread wait for lang set\n"));
			while (bDo && !bExit && m_enFleetState != enFleetStateLangSet)
			{
				if (WaitForSingleObject(this->m_hExitFleetEvent, 33) == WAIT_OBJECT_0)
					bExit = TRUE;
				if (m_iComStateConnect != _STATE_CONN_CONNECTED)
					bDo = FALSE;
			}

			if (bDo && !bExit)
			{
				// if connected call duration data
				TRACE(_T("CRailDiagApp::FleetThread get duration data\n"));
				if (EN_DATA_TYPE_DURATION == m_eDataType)
				{
					pMainFrame->PostMessage(WM_COMMAND, ID_NEW_DURATION_DATA); 
					bWait4Data = TRUE;
				}
				else if (EN_DATA_TYPE_DURATION_FLEET == m_eDataType)
				{
					pMainFrame->PostMessage(WM_COMMAND, ID_NEW_DURATION_DATA); 
					bWait4Data = TRUE;
				}
				else if (EN_DATA_TYPE_HIST20 == m_eDataType)
				{
					pMainFrame->PostMessage(WM_COMMAND, ID_NEW_HIST20_DATA); 
					bWait4Data = TRUE;
				}
				else if (EN_DATA_TYPE_HIST100 == m_eDataType)
				{
					pMainFrame->PostMessage(WM_COMMAND, ID_NEW_HIST100_DATA); 
					bWait4Data = TRUE;
				}
				else if (EN_DATA_TYPE_HIST == m_eDataType)
				{
					pMainFrame->PostMessage(WM_COMMAND, ID_NEW_HIST_DATA); 
					bWait4Data = TRUE;
				}
				Sleep(133);
			}

			TRACE(_T("CRailDiagApp::FleetThread wait for data\n"));
			while (bDo && !bExit && bWait4Data && m_enFleetState != enFleetStateData)
			{
				if (WaitForSingleObject(this->m_hExitFleetEvent, 33) == WAIT_OBJECT_0)
					bExit = TRUE;
				if (m_iComStateConnect != _STATE_CONN_CONNECTED)
					bDo = FALSE;
			}

			if (bConnected && !bExit)
			{
				TRACE(_T("CRailDiagApp::FleetThread disconnect\n"));
				pMainFrame->PostMessage(WM_COMMAND, ID_COM_DISCONNECT_SER); 
				//pMainFrame->PostMessage(WM_COMMAND, ID_COM_DISCONNECT_MODEM); 
			}
			
			TRACE(_T("CRailDiagApp::FleetThread wait for disconnect\n"));
			while (bConnected && !bExit && m_iComStateConnect != _STATE_CONN_DISCONNECTED)
			{
				if (WaitForSingleObject(this->m_hExitFleetEvent, 33) == WAIT_OBJECT_0)
					bExit = TRUE;
				//if (m_iComStateConnect != _STATE_CONN_CONNECTED)
				//	bDo = FALSE;
			}

			TRACE(_T("\n====> uiarrFirst %d, uiarrSecond %d \n"), uiarrFirst.GetCount(), uiarrSecond.GetCount());
			if (bDo && !bExit) // erfolgreiche Übertragung
			{ 
				bStep = TRUE;

				if (0 < uiarrFirst.GetCount()) // noch Elemente für ersten Durchgang
				{
					uiarrFirst.RemoveAt(0);
					if (0 < uiarrFirst.GetCount())
					{
						iIndex = uiarrFirst.GetAt(0);
					}
					else
					{
						if (0 < uiarrSecond.GetCount())
						{
							iIndex = uiarrSecond.GetAt(0);
						}
						else
						{
							bExit = TRUE;
						}
					}
				}
				else if (0 < uiarrSecond.GetCount()) // zweiter Versuch
				{
					uiarrSecond.RemoveAt(0);
					if (0 < uiarrSecond.GetCount())
					{
						iIndex = uiarrSecond.GetAt(0);
					}
					else
					{
						bExit = TRUE;
					}
				}
				else // alle bereits versucht
				{
					bExit = TRUE;
				}
			}
			else // keine erfolgreiche Übertragung
			{
				if (0 < uiarrFirst.GetCount()) // noch Elemente für ersten Durchgang
				{
					uiarrSecond.Add(uiarrFirst.GetAt(0));
					uiarrFirst.RemoveAt(0);
					if (0 < uiarrFirst.GetCount())
					{
						iIndex = uiarrFirst.GetAt(0);
					}
					else
					{
						if (0 < uiarrSecond.GetCount())
						{
							iIndex = uiarrSecond.GetAt(0);
						}
						else
						{
							bExit = TRUE;
						}
					}
				}
				else if (0 < uiarrSecond.GetCount()) // keine Elemente aus erste Übertragung
				{
					bStep = TRUE;

					uiarrError.Add(uiarrSecond.GetAt(0));
					uiarrSecond.RemoveAt(0);
					if (0 < uiarrSecond.GetCount())
					{
						iIndex = uiarrSecond.GetAt(0);
					}
					else
					{
						bExit = TRUE;
					}
				}
				else // alle Elemente versucht
				{
					bExit = TRUE;
				}
			}

			//if (bDo)
			//{
			//	pMainFrame->PostMessage(WM_USER_NEW_PROCESS_DIALOG_FLEET_STEP);
			//}
			//Sleep(333);

			m_enFleetState = enFleetStateInit;

			if (m_bFleetCancel)
			{
				bExit = TRUE;
				Sleep(1);
			}

		}
	}

	for (i = 0; i < uiarrError.GetCount(); i++)
	{
		m_uiarrError.Add(uiarrError.GetAt(i));
	}

	bFleetDialogEnd = TRUE;
	m_bFleetCmdActive = FALSE;

	i = 0;
	pMainFrame->PostMessage(WM_USER_NEW_PROCESS_DIALOG_FLEET_END);
	while (bFleetDialogEnd)
	{
		Sleep(33);
		i++;
		if (i == 30)
		{
			pMainFrame->PostMessage(WM_USER_NEW_PROCESS_DIALOG_FLEET_END);
			i = 0;
		}
	}

	return 0;
}
