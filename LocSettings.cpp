// LocSettings.cpp : implementation file
//

#include "stdafx.h"
#include "raildiag.h"
#include "LocSettings.h"
#include "FileIO.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLocSettings dialog


CLocSettings::CLocSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CLocSettings::IDD, pParent)
	, m_strDisplayTelNumber(_T(""))
	, m_strInhibitTime(_T(""))
	, m_strTelNumberSMS1(_T(""))
	, m_strTelNumberSMS2(_T(""))
	, m_strTelNumberSMS3(_T(""))
	, m_strStDisplayTelNumber(_T(""))
	, m_strStInhibitTime(_T(""))
	, m_strStTelNumberSMS1(_T(""))
	, m_strStTelNumberSMS2(_T(""))
	, m_strStTelNumberSMS3(_T(""))
{
	//{{AFX_DATA_INIT(CLocSettings)
	m_strCompany = _T("");
	m_strLocNumber = _T("");
	m_strLocType = _T("");
	m_strMotorNumber = _T("");
	m_strMotorType = _T("");
	m_strMtuOrderNumber = _T("");
	m_strStCompany = _T("");
	m_strStLocNumber = _T("");
	m_strStLocType = _T("");
	m_strStMotorNumber = _T("");
	m_strStMotorType = _T("");
	m_strStMtuOrderNumber = _T("");
	m_strPin = _T("");
	m_strStPin = _T("");
	m_strStMessage = _T("");
	m_strPuk = _T("");
	m_strStPuk = _T("");
	//}}AFX_DATA_INIT
}


void CLocSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLocSettings)
	DDX_Control(pDX, IDC_SETTINGS_SEND, m_CtrlSend);
	DDX_Control(pDX, IDOK, m_CtrlOk);
	DDX_Control(pDX, IDCANCEL, m_CtrlCancel);
	DDX_Text(pDX, IDC_ED_COMPANY, m_strCompany);
	DDX_Text(pDX, IDC_ED_LOC_NUMBER, m_strLocNumber);
	DDX_Text(pDX, IDC_ED_LOC_TYPE, m_strLocType);
	DDX_Text(pDX, IDC_ED_MOTOR_NUMBER, m_strMotorNumber);
	DDX_Text(pDX, IDC_ED_MOTOR_TYPE, m_strMotorType);
	DDX_Text(pDX, IDC_ED_MTU_ORDER_NUMBER, m_strMtuOrderNumber);
	DDX_Text(pDX, IDC_ST_COMPANY, m_strStCompany);
	DDX_Text(pDX, IDC_ST_LOC_NUMBER, m_strStLocNumber);
	DDX_Text(pDX, IDC_ST_LOC_TYPE, m_strStLocType);
	DDX_Text(pDX, IDC_ST_MOTOR_NUMBER, m_strStMotorNumber);
	DDX_Text(pDX, IDC_ST_MOTOR_TYPE, m_strStMotorType);
	DDX_Text(pDX, IDC_ST_MTU_ORDER_NUMBER, m_strStMtuOrderNumber);
	DDX_Text(pDX, IDC_ED_PIN, m_strPin);
	DDX_Text(pDX, IDC_ST_PIN, m_strStPin);
	DDX_Text(pDX, IDC_ST_MESSAGE, m_strStMessage);
	DDX_Text(pDX, IDC_ED_PUK, m_strPuk);
	DDX_Text(pDX, IDC_ST_PUK, m_strStPuk);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_ED_DISPLAY_TEL_NUM, m_strDisplayTelNumber);
	DDX_Text(pDX, IDC_ED_INHIBIT_TIME, m_strInhibitTime);
	DDV_MaxChars(pDX, m_strInhibitTime, 2);
	DDX_Text(pDX, IDC_ED_TEL_NUM_SMS1, m_strTelNumberSMS1);
	DDX_Text(pDX, IDC_ED_TEL_NUM_SMS2, m_strTelNumberSMS2);
	DDX_Text(pDX, IDC_ED_TEL_NUM_SMS3, m_strTelNumberSMS3);
	DDX_Text(pDX, IDC_ST_DISPLAY_TEL_NUM, m_strStDisplayTelNumber);
	DDX_Text(pDX, IDC_ST_INHIBIT_TIME, m_strStInhibitTime);
	DDX_Text(pDX, IDC_ST_TEL_NUM_SMS1, m_strStTelNumberSMS1);
	DDX_Text(pDX, IDC_ST_TEL_NUM_SMS2, m_strStTelNumberSMS2);
	DDX_Text(pDX, IDC_ST_TEL_NUM_SMS3, m_strStTelNumberSMS3);
	DDX_Control(pDX, IDC_SETTINGS_LOAD, m_CtrlLoad);
	DDX_Control(pDX, IDC_SETTINGS_SAVE, m_CtrlSave);
}


BEGIN_MESSAGE_MAP(CLocSettings, CDialog)
	//{{AFX_MSG_MAP(CLocSettings)
	ON_BN_CLICKED(IDC_SETTINGS_SEND, OnSettingsSend)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SETTINGS_LOAD, &CLocSettings::OnBnClickedSettingsLoad)
	ON_BN_CLICKED(IDC_SETTINGS_SAVE, &CLocSettings::OnBnClickedSettingsSave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLocSettings message handlers

BOOL CLocSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	
	m_strStCompany = pApp->m_stDlgTermConf.strCompany;
	m_strStLocNumber = pApp->m_stDlgTermConf.strLocNumber;
	m_strStLocType = pApp->m_stDlgTermConf.strLocType;
	m_strStMotorNumber = pApp->m_stDlgTermConf.strMotorNumber;
	m_strStMotorType = pApp->m_stDlgTermConf.strMotorType;
	m_strStMtuOrderNumber = pApp->m_stDlgTermConf.strMtuOrderNumber;
	m_strStDisplayTelNumber = pApp->m_stDlgTermConf.strDisplayTelNumber;
	m_strStInhibitTime = pApp->m_stDlgTermConf.strInhibitTime;
	m_strStTelNumberSMS1 = pApp->m_stDlgTermConf.strTelNumberSMS1;
	m_strStTelNumberSMS2 = pApp->m_stDlgTermConf.strTelNumberSMS2;
	m_strStTelNumberSMS3 = pApp->m_stDlgTermConf.strTelNumberSMS3;

	m_strStPin = pApp->m_stDlgTermConf.strPin;
	m_strStPuk = pApp->m_stDlgTermConf.strPuk;

	SetWindowText(pApp->m_stDlgTermConf.strTitle);
	m_CtrlOk.SetWindowText(pApp->m_stDlgTermConf.strOk);
	m_CtrlCancel.SetWindowText(pApp->m_stDlgTermConf.strCancel);
	m_CtrlSend.SetWindowText(pApp->m_stDlgTermConf.strSend);
	if (pApp->GetConnectionState() != EN_CONN_CONNECTED)
	{
		m_CtrlSend.EnableWindow(FALSE);
	}
	m_CtrlLoad.SetWindowText(pApp->m_stDlgTermConf.strLoad);
	m_CtrlSave.SetWindowText(pApp->m_stDlgTermConf.strSave);

	UpdateData(FALSE);	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLocSettings::OnSettingsSend() 
{
	// TODO: Add your control notification handler code here
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	TRACE(_T("Konfigdaten senden\n"));

	PumpMessages();

	pApp->m_pLocSettings = this;

	UpdateData(TRUE);
	pApp->m_stLocSettings.strCompany			= m_strCompany;
	pApp->m_stLocSettings.strLocType			= m_strLocType;
	pApp->m_stLocSettings.strLocNumber			= m_strLocNumber;
	pApp->m_stLocSettings.strMotorType			= m_strMotorType;
	pApp->m_stLocSettings.strMotorNumber		= m_strMotorNumber;
	pApp->m_stLocSettings.strMtuOrderNumber		= m_strMtuOrderNumber;

	pApp->m_stLocSettings.strDisplayTelNumber	= m_strDisplayTelNumber;
	pApp->m_stLocSettings.strInhibitTime		= m_strInhibitTime;
	pApp->m_stLocSettings.strTelNumberSMS1		= m_strTelNumberSMS1;
	pApp->m_stLocSettings.strTelNumberSMS2		= m_strTelNumberSMS2;
	pApp->m_stLocSettings.strTelNumberSMS3		= m_strTelNumberSMS3;
	pApp->m_stLocSettings.strReturnStartPageTime = _T("300"); // 5 Minits

	pApp->m_stLocSettings.strPin				= m_strPin;
	pApp->m_stLocSettings.strPuk				= m_strPuk;

	m_strStMessage = pApp->m_stDlgTermConf.strMessageSend;
	UpdateData(FALSE);	

	pApp->OnLocSettingsSend();
}

void CLocSettings::SettingsSet()
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();

	PumpMessages();
	m_strStMessage = pApp->m_stDlgTermConf.strMessageOk;
	UpdateData(FALSE);	
	m_CtrlSend.SetFocus();
}

void CLocSettings::ConnectionLost()
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();

	PumpMessages();
	m_strStMessage = pApp->m_stDlgTermConf.strMessageError;
	m_CtrlSend.EnableWindow(FALSE);
	UpdateData(FALSE);	
	m_CtrlOk.SetFocus();
}

void CLocSettings::PumpMessages()
{
    // Must call Create() before using the dialog
    ASSERT(m_hWnd!=NULL);

    MSG msg;
    // Handle dialog messages
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
		if (!IsDialogMessage(&msg))
		{
	        TranslateMessage(&msg);
			DispatchMessage(&msg);  
		}
    }
}

void CLocSettings::OnBnClickedSettingsLoad()
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CString			strFile;

	UpdateData(TRUE);

	strFile.Format(_T("%s_Settings.rcf"), m_strLocNumber);

	CFileDialog	FileDialog(TRUE, _T("rcf"), strFile, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
						   _T("Load Settings (*.rcf)|*.rcf|All Files (*.*)|*.*||"));
	int			iStatus;
	CString		strPath;

	FileDialog.m_ofn.lpstrInitialDir = pApp->m_strInitialDirectory;

	FileDialog.m_ofn.lpstrTitle = _T("Settings Load");
	iStatus = FileDialog.DoModal();
	if (iStatus == IDOK)
	{
//		TRACE(_T("Settings Load \n"));
		strPath = FileDialog.GetPathName();

		BOOL		bRet = TRUE;
		CStdioFile	file;
		CWaitCursor	wait;
		CString		key, param;

		if (file.Open(strPath, CFile::modeRead | CFile::typeText))
		{
			if (FindSection(file, _T("LOC_SETTINGS")))
			{
				while (bRet && TranslateNextLine(file, key, param))
				{
					if (key == _T("COMPANY"))
					{
						m_strCompany = param;
					}
					else if (key == _T("LOC_TYPE"))
					{
						m_strLocType = param;
					}
					else if (key == _T("LOC_NUMBER"))
					{
						m_strLocNumber = param;
					}
					else if (key == _T("ENG_TYPE"))
					{
						m_strMotorType = param;
					}
					else if (key == _T("ENG_NUMBER"))
					{
						m_strMotorNumber = param;
					}
					else if (key == _T("ORDER_NUMBER"))
					{
						m_strMtuOrderNumber = param;
					}

					else if (key == _T("TEL_NUMBER"))
					{
						m_strDisplayTelNumber = param;
					}
					else if (key == _T("INHIBIT_TIME"))
					{
						m_strInhibitTime = param;
					}
					else if (key == _T("TEL_NUMBER_SMS1"))
					{
						m_strTelNumberSMS1 = param;
					}
					else if (key == _T("TEL_NUMBER_SMS2"))
					{
						m_strTelNumberSMS2 = param;
					}
					else if (key == _T("TEL_NUMBER_SMS3"))
					{
						m_strTelNumberSMS3 = param;
					}
					else if (key == _T("PIN"))
					{
						m_strPin = param;
					}
					else if (key == _T("PUK"))
					{
						m_strPuk = param;
					}
				}
			}
			file.Close();
		}
		else
			bRet = FALSE;

		if (!bRet)
		{
			CString	csMsg;

			csMsg.Format(_T("Settings Load failed: %s"), strPath);
			AfxMessageBox(csMsg);
		}
	}

	UpdateData(FALSE);
}

void CLocSettings::OnBnClickedSettingsSave()
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CString			strFile;

	UpdateData(TRUE);

	strFile.Format(_T("%s_Settings.rcf"), m_strLocNumber);

	CFileDialog	FileDialog(FALSE, _T("rcf"), strFile, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
						   _T("Save Settings (*.rcf)|*.rcf|All Files (*.*)|*.*||"));
	int			iStatus;
	CString		strPath;

	FileDialog.m_ofn.lpstrInitialDir = pApp->m_strInitialDirectory;

	FileDialog.m_ofn.lpstrTitle = _T("Settings Save");
	iStatus = FileDialog.DoModal();
	if (iStatus == IDOK)
	{
//		TRACE(_T("Exporting file \n"));
		strPath = FileDialog.GetPathName();

		BOOL		bRet = TRUE;
		CStdioFile	file;
		CWaitCursor	wait;
		CString		buf;

		if (file.Open(strPath, CFile::modeWrite | CFile::modeCreate | CFile::typeText))
		{
			file.WriteString(_T("[LOC_SETTINGS]\r\n"));

			buf.Format(_T("%s = %s\r\n"), _T("COMPANY"), m_strCompany);
			file.WriteString(buf);
			buf.Format(_T("%s = %s\r\n"), _T("LOC_TYPE"), m_strLocType);
			file.WriteString(buf);
			buf.Format(_T("%s = %s\r\n"), _T("LOC_NUMBER"), m_strLocNumber);
			file.WriteString(buf);
			buf.Format(_T("%s = %s\r\n"), _T("ENG_TYPE"), m_strMotorType);
			file.WriteString(buf);
			buf.Format(_T("%s = %s\r\n"), _T("ENG_NUMBER"), m_strMotorNumber);
			file.WriteString(buf);
			buf.Format(_T("%s = %s\r\n"), _T("ORDER_NUMBER"), m_strMtuOrderNumber);
			file.WriteString(buf);

			buf.Format(_T("%s = %s\r\n"), _T("TEL_NUMBER"), m_strDisplayTelNumber);
			file.WriteString(buf);
			buf.Format(_T("%s = %s\r\n"), _T("INHIBIT_TIME"), m_strInhibitTime);
			file.WriteString(buf);
			buf.Format(_T("%s = %s\r\n"), _T("TEL_NUMBER_SMS1"), m_strTelNumberSMS1);
			file.WriteString(buf);
			buf.Format(_T("%s = %s\r\n"), _T("TEL_NUMBER_SMS2"), m_strTelNumberSMS2);
			file.WriteString(buf);
			buf.Format(_T("%s = %s\r\n"), _T("TEL_NUMBER_SMS3"), m_strTelNumberSMS3);
			file.WriteString(buf);
			//buf.Format(_T("%s = %s\r\n"), _T("START_PAGE_TIME"), );
			//file.WriteString(buf);

			buf.Format(_T("%s = %s\r\n"), _T("PIN"), m_strPin);
			file.WriteString(buf);
			buf.Format(_T("%s = %s\r\n"), _T("PUK"), m_strPuk);
			file.WriteString(buf);

			file.Close();
		}
		else
			bRet = FALSE;

		if (!bRet)
		{
			CString	csMsg;

			csMsg.Format(_T("Settings save failed: %s"), strPath);
			AfxMessageBox(csMsg);
		}
	}
}
