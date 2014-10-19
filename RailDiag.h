// RailDiag.h : main header file for the RAILDIAG application
//

#if !defined(AFX_RAILDIAG_H__E862ABBB_EBA0_44EB_BD97_1EE40CC26785__INCLUDED_)
#define AFX_RAILDIAG_H__E862ABBB_EBA0_44EB_BD97_1EE40CC26785__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// Window messages for LM-xP
#define WM_USER_NEW_SERIAL_DATA					WM_USER+1
#define WM_USER_NEW_SERIAL_STATE				WM_USER+2
#define WM_USER_NEW_PROCESS_DIALOG_START		WM_USER+3
#define WM_USER_NEW_PROCESS_DIALOG_END			WM_USER+4
#define WM_USER_ACTIVATE_LIST_VIEW				WM_USER+5
#define	WM_USER_CONFIG_OK						WM_USER+6
#define WM_USER_LANGUAGES						WM_USER+7
#define WM_USER_RELOAD_DATA						WM_USER+8
#define WM_USER_NEW_PROCESS_DIALOG_FLEET_START	WM_USER+9
#define WM_USER_NEW_PROCESS_DIALOG_FLEET_END	WM_USER+10
#define WM_USER_NEW_PROCESS_DIALOG_FLEET_STEP	WM_USER+11
#define WM_USER_NEW_PROCESS_DIALOG_FLEET_NAME	WM_USER+12

#define _STATE_CONN_DISCONNECTED			0
#define _STATE_CONN_CONNECTING				1
#define _STATE_CONN_DISCONNECTING			2
#define	_STATE_CONN_CONNECTED				3

/////////////////////////////////////////////////////////////////////////////
// CRailDiagApp:
// See RailDiag.cpp for the implementation of this class
//

typedef enum _tag_EN_DATA_TYPE
{
	EN_DATA_TYPE_BIN,
	EN_DATA_TYPE_ANA,
	EN_DATA_TYPE_ALARM,
	EN_DATA_TYPE_DURATION,
	EN_DATA_TYPE_HIST,
	EN_DATA_TYPE_HIST20,
	EN_DATA_TYPE_HIST100,
	EN_DATA_TYPE_HIST2,
	EN_DATA_TYPE_DURATION_FLEET,
} EN_DATA_TYPE;

typedef enum _tag_EN_CONN_STATE
{
	EN_CONN_IDLE,
	EN_CONN_OPENING,
	EN_CONN_ANSWERING,
	EN_CONN_CONNECTED
} EN_CONN_STATE;

typedef enum _tag_EN_FLEET_STATE
{
	EN_FLEET_OFF,
	EN_FLEET_ON,
	EN_FLEET_EDIT,
	EN_FLEET_DISCONNECTED,
	EN_FLEET_CONNECTING,
	EN_FLEET_CONNECTED,
	EN_FLEET_COMMAND_RUNNING,
	EN_FLEET_COMMAND_NEXT,
	EN_FLEET_END
} EN_FLEET_STATE;

typedef struct _tag_LOC_SETTINGS
{
	CString		strCompany;
	CString		strLocType;
	CString		strLocNumber;
	CString		strMotorType;
	CString		strMotorNumber;
	CString		strMtuOrderNumber;
	CString		strDisplayTelNumber;
	CString		strInhibitTime;
	CString		strTelNumberSMS1;
	CString		strTelNumberSMS2;
	CString		strTelNumberSMS3;
	CString		strReturnStartPageTime;
	CString		strPin;
	CString		strPuk;
} LOC_SETTINGS;

typedef struct _tag_STAT_LANG_TXT_LIST
{
	CString				strTitle;
	CStringArray		strarrRow;
} STAT_LANG_TXT_LIST;

typedef struct _tag_STAT_LANG_TXT_DOC
{
	CString				strHeadNameShort;
	CString				strHeadName;
	CString				strHeadCity;
	CString				strCompany;
	CString				strLocType;
	CString				strLocNumber;
	CString				strMotorType;
	CString				strMotorNumber;
	CString				strDurationCount;
	CString				strTerminal;
	CString				strDiag;
	CString				strMode;
	CString				strMode0;
	CString				strMode1;
} STAT_LANG_TXT_DOC;

typedef struct _tag_STAT_LANG_TEXT
{
	STAT_LANG_TXT_DOC	stDoc;
	STAT_LANG_TXT_LIST	stBinary;
	STAT_LANG_TXT_LIST	stAnalog;
	STAT_LANG_TXT_LIST	stAlarm;
	STAT_LANG_TXT_LIST	stHistory;
} STAT_LANG_TEXT;

typedef struct _tag_MENU_FILE
{
	CString				strOpen;
	CString				strClose;
	CString				strSave;
	CString				strSaveAs;
	CString				strExport;
	CString				strPrint;
	CString				strPrintPreview;
	CString				strPrintSettings;
	CString				strLast;
	CString				strExit;
} MENU_FILE;

typedef struct _tag_MENU_CON
{
	CString				strConSerial;
	CString				strConModem;
	CString				strConnect;
	CString				strDisconnect;
	CString				strSettings;
	CString				strState;
	CString				strPhoneBook;
} MENU_CON;

typedef struct _tag_MENU_DATA
{
	CString				strBinary;
	CString				strAnalog;
	CString				strAlarm;
	CString				strDuration;
	CString				strHistory;
	CString				strHistory20;
	CString				strHistory100;
	CString				strConfigSettings;
	CString				strConfigSend;
	CString				strHistory2;
	CString				strReload;
} MENU_DATA;

typedef struct _tag_MENU_VIEW
{
	CString				strSymbolBar;
	CString				strStatusBar;
} MENU_VIEW;

typedef struct _tag_MENU_WND
{
	CString				strNew;
	CString				strCascade;
	CString				strTile;
	CString				strArrange;
} MENU_WND;

typedef struct _tag_MENU_HELP
{
	CString				strFinder;
	CString				strAbout;
} MENU_HELP;

typedef struct _tag_MENU_FLEET //Dj
{
	CString				strFleetMgr;
	CString				strFleetEdit;
} MENU_FLEET;

typedef struct _tag_MENU_MAIN
{
	CString				strFile;
	CString				strConnection;
	CString				strData;
	CString				strView;
	CString				strWindow;
	CString				strHelp;
	MENU_FILE			stMnuFile;
	MENU_CON			stMnuCon;
	MENU_DATA			stMnuData;
	MENU_VIEW			stMnuView;
	MENU_WND			stMnuWnd;
	MENU_HELP			stMnuHelp;
	MENU_FLEET			stMnuFleet;
} MENU_MAIN;

typedef struct _tag_DLG_PROG
{
	CString				strTitle;
	CString				strCancel;
} DLG_PROG;

typedef struct _tag_DLG_SER_CON
{
	CString				strTitle;
	CString				strPort;
	CString				strBps;
	CString				strBits;
	CString				strParity;
	CString				strStopBits;
	CString				strControl;
	CString				strOk;
	CString				strCancel;
} DLG_SER_CON;

typedef struct _tag_DLG_MODEM_CON
{
	CString				strTitle;
	CString				strConnectionName;
	CString				strDesc;
	CString				strNumber;
	CString				strPin;
	CString				strSavePin;
	CString				strPuk;
	CString				strConnect;
	CString				strOk;
	CString				strCancel;
	CString				strSettings;
	CString				strHelpDialPrefix;
	CString				strDialPrefix;
} DLG_MODEM_CON;

typedef struct _tag_DLG_PHONE_BOOK
{
	CString				strTitle;
	CString				strDesc;
	CString				strNumber;
	CString				strNewEntry;
	CString				strEditEntry;
	CString				strDeleteEntry;
	CString				strOk;
	CString				strCancel;
	CString				strImport;
	CString				strExport;
} DLG_PHONE_BOOK;

typedef struct _tag_DLG_PHONE_BOOK_ENTRY
{
	CString				strTitle;
	CString				strDesc;
	CString				strNumber;
	CString				strOk;
	CString				strCancel;
	CString				strHelpNumber;
} DLG_PHONE_BOOK_ENTRY;

typedef struct _tag_DLG_TERM_CONF
{
	CString				strTitle;
	CString				strCompany;
	CString				strLocType;
	CString				strLocNumber;
	CString				strMotorType;
	CString				strMotorNumber;
	CString				strMtuOrderNumber;
	CString				strDisplayTelNumber;
	CString				strInhibitTime;
	CString				strTelNumberSMS1;
	CString				strTelNumberSMS2;
	CString				strTelNumberSMS3;
	CString				strPin;
	CString				strPuk;
	CString				strMessageOk;
	CString				strMessageSend;
	CString				strMessageError;
	CString				strSend;
	CString				strOk;
	CString				strCancel;
	CString				strLoad;
	CString				strSave;
} DLG_TERM_CONF;

typedef struct _tag_CON_STATUS
{
	CString				strUnknown;
	CString				strDisconnecting;
	CString				strDisconnected;
	CString				strInit;
	CString				strIdle;
	CString				strOpening;
	CString				strAnswering;
	CString				strConnected;
	CString				strClose;
	CString				strNoDialTone;
	CString				strBusy;
	CString				strNoAnswer;
	CString				strWrongPin;
	CString				strWrongPuk;
	CString				strInUse;
	CString				strMessage;
	CString				strModemDriverError;
	CString				strModemOff;
	CString				strSerialOff;
	CString				strReconnect;
	CString				strNoData;
	CString				strNoAccess;
	CString				strTransferCancel;
} CON_STATUS;

typedef struct _tag_DLG_FLEET_EDIT
{
	CString				strTitle;
	CString				strListPhoneBook;
	CString				strListFleet;
	CString				strDesc;
	CString				strRowNumber;
	CString				strTelNumber;
	CString				strFleetName;
	CString				strFleetNew;
	CString				strFleetDel;
	CString				strOk;
	CString				strCancel;
	CString				strImport;
	CString				strExport;
	CString				strMess1;
	CString				strMess2;
} DLG_FLEET_EDIT;

typedef enum _tag_EnFleetState
{
	enFleetStateInit = 0,
	enFleetStateStart,
	enFleetStateSettings,
	enFleetStateLangGet,
	enFleetStateLangSet,
	enFleetStateData,

} EnFleetState;

typedef struct _tag_FLEET_ELEM
{
	CStringArray		strarrPhoneBookDescr;
	CStringArray		strarrPhoneBookNumber;
} FLEET_ELEM;

class CRailDiagApp : public CWinApp
{

protected:
	DWORD			m_dwBaud;
	int				m_iComStateConnect;
	EN_CONN_STATE	m_eConnState;
	CString			m_strComPort;
	CString			m_strConnName;
	CString			m_strConnDesc;
	CString			m_strConnNumber;
	CString			m_strDialPrefix;
	BOOL			m_bDialPrefix;
	CString			m_strConnPin;
	BOOL			m_bConnPinSave;
	CString			m_strConnPuk;
	CString			m_strCBST;
	BOOL			m_bReconnect;
	BOOL			m_bLineInit;
	BOOL			m_bLog;
	CString			m_strMessage;
	BOOL			m_bBlinkState;
	DWORD			m_dwBlinkNextTick;
	DWORD			m_dwBlinkFastOn;
	DWORD			m_dwBlinkFastOff;
	DWORD			m_dwBlinkSlowOn;
	DWORD			m_dwBlinkSlowOff;

public:
	BOOL			m_bGSM_Rail;
	DWORD			m_dwToolTipOn;
	DWORD			m_dwToolTipWidth;
	CString			m_strAppPath;
	BOOL			m_bAuto;
	int				m_iLanguages;
	CString			m_strLangIconPath;
	CStringArray	m_strarrLangName;
	CStringArray	m_strarrLangIcon;
	CUIntArray		m_uiarrLangPos;
	CUIntArray		m_uiarrLangNr;
	CUIntArray		m_uiarrLangOrder;
	int				m_iConnType;
	HANDLE			m_hComThread;
	HANDLE			m_hExitComEvent;
	EN_DATA_TYPE	m_eDataType;
	LOC_SETTINGS	m_stLocSettings;
	LOC_SETTINGS	m_stLocSettingsLoad;
	int				m_iLang;
	int				m_iDefaultLang;
	CUIntArray		m_uiarrPrintPosBinaryList;
	CUIntArray		m_uiarrPrintPosAnalogList;
	CUIntArray		m_uiarrPrintPosAlarmList;
	CUIntArray		m_uiarrPrintPosDurationList;
	CUIntArray		m_uiarrPrintPosDurationFleetList;
	CUIntArray		m_uiarrPrintPosHistoryList;
	CUIntArray		m_uiarrPrintPosHistory2List;
	CStdioFile		m_fLogFile;
	BOOL			m_bLogFile;
	CUIntArray		m_uiarrAutoCommands;
	CStringArray	m_strarrAutoSaveFiles;

	BOOL			m_bUserConnect;
	UINT			m_uiMaxWait4Data;
	UINT			m_uiMaxWait4Check;
	UINT			m_uiWait4Reconnect;
	UINT			m_uiMaxWait4Active;
	DWORD			m_dwDisconnectTick;
	BOOL			m_bMessage;
	BOOL			m_bDataIn;
	void			*m_pLocSettings;
	BOOL			m_bUserInput;

	CStringArray	m_strarrPhoneBookDesc;
	CStringArray	m_strarrPhoneBookNumber;

	int				m_iInOutMaxWaiting;
	CString			m_strInitialDirectory;

	//FleetMgr
	EN_FLEET_STATE	m_iFleetMgrState;
	HANDLE			m_hFleetThread;
	HANDLE			m_hExitFleetEvent;
	EnFleetState	m_enFleetState;
	CString			m_strFleetName;
	int				m_iFleetSelected;
	CString			m_strFleetDateTime;
	BOOL			m_bFleetCancel;
	BOOL			bFleetDialogEnd;
	BOOL			m_bFleetConnectActive;
	BOOL			m_bFleetCmdActive;
	int				m_iDurationFleetCount;
	CUIntArray		m_uiarrError;

	CStringArray	m_strarrFleetDesc;
	CPtrArray		m_ptrarrFleetElem;

public:
	//statische texte
	STAT_LANG_TXT_DOC		m_stStatText;
	STAT_LANG_TXT_LIST		m_stStatBinary;
	STAT_LANG_TXT_LIST		m_stStatAnalog;
	STAT_LANG_TXT_LIST		m_stStatAlarm;
	STAT_LANG_TXT_LIST		m_stStatDuration;
	STAT_LANG_TXT_LIST		m_stStatDurationFleet;
	STAT_LANG_TXT_LIST		m_stStatHistory;
	STAT_LANG_TXT_LIST		m_stStatHistory2;
	MENU_MAIN				m_stMenu;
	DLG_PROG				m_stDlgProg;
	DLG_SER_CON				m_stDlgSerCon;	
	DLG_MODEM_CON			m_stDlgModemCon;
	DLG_TERM_CONF			m_stDlgTermConf;
	DLG_PHONE_BOOK			m_stDlgPhoneBook;
	DLG_PHONE_BOOK_ENTRY	m_stDlgPhoneBookEntry;
	CON_STATUS				m_stConStatus;
	CString					m_strPrintHeader;
	CString					m_strPrintFooter;
	BOOL					m_bPUK;
	DLG_FLEET_EDIT			m_stDlgFleetEdit;

protected:
	void StartComThread(void);
	void StopComThread(void);
	void StartFleetThread(void);
	void StopFleetThread(void);

public:
	void ComConnect();
	DWORD ComDisconnect();
	static DWORD WINAPI _MsgThread(LPVOID lParam);
	static DWORD WINAPI _ComThread(LPVOID lParam);
	static DWORD WINAPI _ComDisconnectThread(LPVOID lParam);
	DWORD ComThread(void);
	DWORD MsgThread(void);
	static DWORD WINAPI FleetThreadHandler(LPVOID lParam);
	DWORD FleetThread(void);

public:
	void Message(CString cStrMsg);
	BOOL WritePhoneBookIni(CStdioFile &file, CStringArray &strarrDesc, CStringArray &strarrNumber);
	BOOL ReadPhoneBookIni(CStdioFile &file, CStringArray &strarrDesc, CStringArray &strarrNumber);
	BOOL WriteFleetIni(CStdioFile &file, CStringArray &strarrDesc, CPtrArray &ptrarrNumber);
	BOOL ReadFleetIni(CStdioFile &file, CStringArray &strarrDesc, CPtrArray &ptrarrNumber);
	void UpdateDescNumber(CString strDescriptionOld, CString strNumberOld, 
				CString strDescription, CString strNumber);
	void SetLangOrder(CUIntArray *puiarrLangSelected);
	void ClearLocSettingsLoad(void);
	void Write2Log(CString strMsg);
	BOOL GetReconnect(void);
	void SetReconnect(BOOL bReconnect);
	CString MenuReplaceTab(CString strIn);
	CString ReplaceCRLF(CString strIn);
	void SetLang(int iLang);
	BOOL ReadLangIni(CStdioFile &file);
	BOOL ReadRailDiagIni(CStdioFile &file);
	void SetConnectionState(EN_CONN_STATE eConnState);
	EN_CONN_STATE GetConnectionState(void);
	int GetComStateConnect(void);
	BYTE GetComPort(void);
	CString GetDialNumber(void);
	CString GetDialName(void);
	void SaveOpenDocument(void);

	CRailDiagApp();
	~CRailDiagApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRailDiagApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CRailDiagApp)
	afx_msg void OnAppAbout();
	afx_msg void OnComConnect();
	afx_msg void OnUpdateComConnect(CCmdUI* pCmdUI);
	afx_msg void OnComDisconnect();
	afx_msg void OnUpdateComDisconnect(CCmdUI* pCmdUI);
	afx_msg void OnComSettings();
	afx_msg void OnUpdateComSettings(CCmdUI* pCmdUI);
	afx_msg void OnConnTypeSerial();
	afx_msg void OnUpdateConnTypeSerial(CCmdUI* pCmdUI);
	afx_msg void OnConnTypeModem();
	afx_msg void OnUpdateConnTypeModem(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileNew(CCmdUI* pCmdUI);
	afx_msg void OnFileNew();
	afx_msg void OnUpdateNewBinData(CCmdUI* pCmdUI);
	afx_msg void OnNewBinData();
	afx_msg void OnUpdateNewAnaData(CCmdUI* pCmdUI);
	afx_msg void OnNewAnaData();
	afx_msg void OnUpdateNewAlarmData(CCmdUI* pCmdUI);
	afx_msg void OnNewAlarmData();
	afx_msg void OnUpdateNewHistData(CCmdUI* pCmdUI);
	afx_msg void OnNewHistData();
	afx_msg void OnUpdateLocSettings(CCmdUI* pCmdUI);
	afx_msg void OnLocSettings();
	afx_msg void OnUpdateLocSettingsSend(CCmdUI* pCmdUI);
	afx_msg void OnLocSettingsSend();
	afx_msg void OnUpdateNewHist20Data(CCmdUI* pCmdUI);
	afx_msg void OnNewHist20Data();
	afx_msg void OnUpdateNewHist100Data(CCmdUI* pCmdUI);
	afx_msg void OnNewHist100Data();
	afx_msg void OnPhoneBook();
	afx_msg void OnUpdatePhoneBook(CCmdUI* pCmdUI);
	afx_msg void OnComDisconnectModem();
	afx_msg void OnUpdateComDisconnectModem(CCmdUI* pCmdUI);
	afx_msg void OnComDisconnectSer();
	afx_msg void OnUpdateComDisconnectSer(CCmdUI* pCmdUI);
	afx_msg void OnUpdateConnState(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNewDurationData(CCmdUI* pCmdUI);
	afx_msg void OnNewDurationData();
	afx_msg void OnUpdateNewHist2Data(CCmdUI* pCmdUI);
	afx_msg void OnNewHist2Data();
	afx_msg void OnFleetMgr();
	afx_msg void OnUpdateFleetMgr(CCmdUI* pCmdUI);
	afx_msg void OnFleetEdit();
	afx_msg void OnUpdateFleetEdit(CCmdUI* pCmdUI);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RAILDIAG_H__E862ABBB_EBA0_44EB_BD97_1EE40CC26785__INCLUDED_)
