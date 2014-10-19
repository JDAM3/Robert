// =========================================================================
//                      (C) Kinz Elektronik
//
// Produkt:      ComDiag
//
// Modulname:    Line.c
//
// Funktion:     Diese Datei enthält alle erforderlichen Funktion um Per Modem
//				 eine Verbindung aufzubauen und Daten zu übertragen
//
// Autor:
//      Name:    R. Bernecker
//      Datum:   16.5.2002
//
// Hardware:            PC-1110, PC
// Betriebsystem:       Windows-CE, Win2000
// Compiler:            Visual C++
//
// =========================================================================

// -------------------------------------------------------------------------
// Includes

//#include <stdio.h>
//#include "stdafx.h"
//#include "resource.h"
#include <stdlib.h> 
#include <malloc.h>
#include <stdio.h>
#include <tchar.h>
#include <tapi.h>
#include <windows.h>
#include <winreg.h>

#ifdef _WIN32_WCE
#include "../../QVis-MTU/RT/Include/ComLib.h"
#else
#include "../../RailDiag-MTU/ComLib.h"
#endif

#include "Line.h"


// -------------------------------------------------------------------------
// Defines

#define _LINE_TAPI_CURRENT_VERSION 0x00020000
#define WRITER_LIST_COUNT	10

//#define _LINE_TRACE



typedef struct
{
	int		nPosSend;		// position of element is to send
	int		nPosWrite;		// position of element is written to port
	UCHAR	*pData[WRITER_LIST_COUNT]; // write data-buffer
	DWORD	dwSize[WRITER_LIST_COUNT]; // write size
} WRITER_LIST;

// -------------------------------------------------------------------------
// Globals

BOOL				g_bGsmRail = FALSE;


// -------------------------------------------------------------------------
// Statics

static BOOL			g_bModemComm = FALSE;
static BOOL			g_bFixDataCall = FALSE;
static BOOL			g_bShutdown = FALSE;
static DWORD		g_dwNumDevs = 0;				// count of found devices
static DWORD		g_dwActDev = 0;					// Actual device for First,Next-Name
static DWORD		g_dwMediaMode = LINEMEDIAMODE_DATAMODEM;
static DWORD		g_dwTAPIMsg = 0;
static DWORD		g_dwLine = 0;
static DWORD		g_dwTimeTick = 0;
static DWORD		g_dwBaud2Rail = 0;				// neue Baudrate für Thread
static DWORD		g_dwBaudClearCom = 0;			// neue Baudrate für Thread ClearCom

static EN_LINE_STATE			g_eState = EN_LINE_STATE_NOINIT;
static EN_LINE_ERROR			g_eError = EN_LINE_OK;

static HLINEAPP		g_hLineApp = NULL;				// line application handle
static HLINE		g_hLine = NULL;					// Line handle
static HCALL		g_hCall = NULL;					// call handle
static HCALL		g_hCallRepl = NULL;				// call handle
static HANDLE		g_hComm = NULL;					// comm handle
static HANDLE		g_hIncommingThread = NULL;		// incomming thread handle
static HANDLE		g_hWriterThread = NULL;			// writer thread for data
static HANDLE		g_hWriterThreadEnd = NULL;		// to stop writer thread
static HANDLE		g_hCallThread = NULL;			// call thread handle
static HANDLE		g_hEvReply = NULL;				// replay handle
static char			g_szPin[20] = "\0";				// Pin falls gesetzt
static char			g_szPuk[20] = "\0";				// Puk falls gesetzt
static char			g_szBearer[20] = "\0";			// Einstellungen für Funkstrecke
static int			g_iStatSetPinThread = 0;		// Status des set pin thread
static int			g_iStatBaud2RailThread = 0;		// Status des change baud thread
static int			g_iStatShutdownThread = 0;		// Status des shutdown thread
static HANDLE		g_hThreadLineSetPin = NULL;     // set pin thread
static HANDLE		g_hThreadLineShutdown = NULL;	// shutdown thread
static HANDLE		g_hThreadLineClose = NULL;		// close thread
static HANDLE		g_hThreadLineBaud2Rail = NULL;	// change baud for rail handle
static HANDLE		g_hThreadClearCom = NULL;		// clear com if dcd is set
static HANDLE		g_hChangeBaudThread = NULL;		// call thread handle
static BOOL			g_bChangeBaud = FALSE;
#ifndef _WIN32_WCE
static OVERLAPPED	g_ovIn = {0, 0, 0, 0, NULL};
static OVERLAPPED	g_ovOut = {0, 0, 0, 0, NULL};
static HANDLE		g_hReadThread = NULL;
static BOOL			g_bReadThread = TRUE;
static int			g_iReading = -1;
#endif
static BOOL			g_bSetPinStop = FALSE;			// stopping SetPin Thread

static WRITER_LIST	g_WriterList;					// Writer List



#ifdef _LINE_TRACE

static TCHAR		g_szMsg[255];
#endif

// -------------------------------------------------------------------------
// Prototypes
int	_Line_ChangeBaud(UCHAR ucCom, DWORD dwBaud2Rail);
int	_Line_SetPin(UCHAR ucCom, BOOL bFixDataCall);
int	_Line_SetPinStop(void);
int	_Line_Shutdown(UCHAR ucCom);
int	_Line_ComOpen(UCHAR ucCom, DWORD dwBaude);
int	_Line_ComClose(UCHAR ucCom);
DWORD WINAPI _Line_ClearCom_Thread(LPVOID lParam);
DWORD WINAPI _Line_ChangeBaud_Thread(LPVOID lParam);
DWORD WINAPI _Line_SetPin_Thread(LPVOID lParam);
DWORD WINAPI _Line_Shutdown_Thread(LPVOID lParam);
DWORD WINAPI _Line_Close_Thread(LPVOID lParam);
DWORD WINAPI ReadThread(LPVOID pvParam);
WCHAR* CharString2UnicodeString(char *szStr);

void	WriterListInit(void);
void	WriterListClear(void);


//////////////////////////////////////////////////////////////////////////
// internal functions
DWORD WINAPI ReadThread(LPVOID pvParam)
{
#ifdef _LINE_TRACE
	_stprintf(g_szMsg, _T("ReadThread: start.\r\n"));
	OutputDebugString(g_szMsg);
#endif

#ifndef _WIN32_WCE
	while (g_bReadThread)
	{
		WaitForSingleObject(g_ovIn.hEvent, INFINITE);
		ResetEvent(g_ovIn.hEvent);
		g_iReading = 0;
	}
#endif

#ifdef _LINE_TRACE
	_stprintf(g_szMsg, _T("ReadThread: end.\r\n"));
	OutputDebugString(g_szMsg);
#endif


#ifndef _WIN32_WCE
	g_hReadThread = NULL;
#endif
	return 0;
}

VOID FAR PASCAL Line_Int_Callback(DWORD hDevice, DWORD dwMsg, DWORD dwCallbackInstance, 
							 DWORD dwParam1, DWORD dwParam2, DWORD dwParam3)
{
	LONG			lRes = 0;
	LINECALLINFO	*lpCallInfo;

#ifdef _LINE_TRACE
	_stprintf(g_szMsg, _T("LineCallback: ....\r\n"));
	OutputDebugString(g_szMsg);
#endif

	switch (dwMsg)
	{
	case LINE_REPLY: // Sent after lineMakeCall or lineDrop
#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("LineCallback: Line Replay dwParam1 %x, dwParam2 %x.\r\n"), dwParam1, dwParam2);
		OutputDebugString(g_szMsg);
#endif
		lRes = (LONG)dwParam2;
		if (lRes < 0)
		{
#ifdef _LINE_TRACE
			_stprintf(g_szMsg, _T("LineCallback: Line Replay Line set Idle. %x\r\n"), lRes);
			OutputDebugString(g_szMsg);
#endif
			g_eState = EN_LINE_STATE_IDLE;
		}
		else
		{
			g_hCallRepl = (HCALL)dwParam1;
		}
		break;

	case LINE_CALLSTATE:  // Sent after change of call state
		{
		switch (dwParam1)
			{
			case LINECALLSTATE_ACCEPTED:
#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("LineCallback: Line Accepted.\r\n"));
				OutputDebugString(g_szMsg);
#endif
				g_dwTAPIMsg = LINECALLSTATE_ACCEPTED;
				SetEvent(g_hEvReply);
				break;

			case LINECALLSTATE_OFFERING:	//Incoming call is offering.
				{
#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("LineCallback: Line Offering.\r\n"));
				OutputDebugString(g_szMsg);
#endif
				//  Get the call handle
				g_hCall = (HCALL)hDevice;
#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("LineCallback: Signaling Event. \r\n"));
				OutputDebugString(g_szMsg);
#endif
				g_dwTAPIMsg = LINECALLSTATE_OFFERING;
				SetEvent(g_hEvReply);
				break;
				}
			case LINECALLSTATE_IDLE:
				{
#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("LineCallback: Line Idle.\r\n"));
				OutputDebugString(g_szMsg);
#endif
				if (!g_bChangeBaud)
				{
					if (g_hCall)
					{
#ifdef _LINE_TRACE
						_stprintf(g_szMsg, _T("LineCallback: Droping call.\r\n"));
						OutputDebugString(g_szMsg);
#endif
						lineDrop(g_hCall, NULL, 0);
						g_hCall = NULL;
						g_bModemComm = FALSE;
					}

					g_eState = EN_LINE_STATE_IDLE;
#ifdef _LINE_TRACE
					_stprintf(g_szMsg, _T("LineCallback: set idle state.\r\n"));
					OutputDebugString(g_szMsg);
#endif
				}
				else
				{
					g_bChangeBaud = FALSE;
#ifdef _LINE_TRACE
					_stprintf(g_szMsg, _T("LineCallback: set end for change baud.\r\n"));
					OutputDebugString(g_szMsg);
#endif
				}
				break;
				}
			case LINECALLSTATE_CONNECTED:
				{
#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("LineCallback: Line Connected.\r\n"));
				OutputDebugString(g_szMsg);
#endif
//				if(lm.dwCallbackInstance == 1)
//				{
					g_dwTAPIMsg = LINECALLSTATE_CONNECTED;
					SetEvent(g_hEvReply);
//				}
				break;
				}
			case LINECALLSTATE_DISCONNECTED:
#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("LineCallback: Line Disonnected. 0x%X, 0x%X\r\n"), dwParam2, dwParam3);
				OutputDebugString(g_szMsg);
#endif
				// We got disconnected, so drop the call
//				lineDrop((HCALL) hDevice, NULL, 0);
				if (!g_bChangeBaud)
				{
					if (g_hCall)
					{
						lineDrop(g_hCall, NULL, 0);
						g_hCall = NULL;
						g_bModemComm = FALSE;
					}
				}
				switch(dwParam2)
				{
				case LINEDISCONNECTMODE_NORMAL:
					if (!g_bChangeBaud)
					{
						g_eError = EN_LINE_ERR_MODEM;
					}
#ifdef _LINE_TRACE
					_stprintf(g_szMsg, _T("LineCallback: Line Disonnected. normal (no modem)\r\n"));
					OutputDebugString(g_szMsg);
#endif
					break;
				case LINEDISCONNECTMODE_NODIALTONE:
					g_eError = EN_LINE_ERR_NODIALTONE;
#ifdef _LINE_TRACE
					_stprintf(g_szMsg, _T("LineCallback: Line Disonnected. no dialtone\r\n"));
					OutputDebugString(g_szMsg);
#endif
					break;
				case LINEDISCONNECTMODE_BUSY:
					g_eError = EN_LINE_ERR_BUSY;
#ifdef _LINE_TRACE
					_stprintf(g_szMsg, _T("LineCallback: Line Disonnected. busy\r\n"));
					OutputDebugString(g_szMsg);
#endif
					break;
				case LINEDISCONNECTMODE_NOANSWER:
					g_eError = EN_LINE_ERR_NOANSWER;
#ifdef _LINE_TRACE
					_stprintf(g_szMsg, _T("LineCallback: Line Disonnected. no answer\r\n"));
					OutputDebugString(g_szMsg);
#endif
					break;
				default:
					break;
				}
				if (!g_bChangeBaud)
				{
					g_eState = EN_LINE_STATE_IDLE;
				}
				break;
			default:
#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("LineCallback: Line Callstate default 0x%X.\r\n"), dwParam1);
				OutputDebugString(g_szMsg);
#endif
				break;
			}
		break;
		}
	case LINE_CALLINFO: //  Call Info is available
		{
			if(dwParam1 == LINECALLINFOSTATE_CALLID)
			{  //Caller ID became available.
				lpCallInfo = (LINECALLINFO *)malloc(sizeof(LINECALLINFO)+1000);
				memset(lpCallInfo, 0, sizeof(LINECALLINFO)+1000);
				lpCallInfo->dwTotalSize = sizeof(LINECALLINFO)+1000;
				while (1)
				{
					lineGetCallInfo(g_hCall, lpCallInfo);
					if (lpCallInfo->dwTotalSize < lpCallInfo->dwNeededSize)
						lpCallInfo = (LINECALLINFO *)realloc(lpCallInfo, lpCallInfo->dwNeededSize);
					else 
						break;
				} 
#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("LineCallback: Caller is %s : %s\r\n"), 
					(LPTSTR)((DWORD)lpCallInfo+(DWORD)lpCallInfo->dwCallerIDOffset), 
					(LPTSTR)((DWORD)lpCallInfo+(DWORD)lpCallInfo->dwCallerIDNameOffset));
				OutputDebugString(g_szMsg);
#endif
				free(lpCallInfo);
				lpCallInfo = NULL;
			}
		}
		break;

	case LINE_CLOSE:
#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("LineCallback: Close \r\n"));
		OutputDebugString(g_szMsg);
#endif
		g_eError = EN_LINE_ERR_CLOSE; // um ausgeschaltetes Modem zu erkennen
		g_eState = EN_LINE_STATE_IDLE;
		break;

	default:
#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("LineCallback: Message 0x%X\r\n"), dwMsg);
		OutputDebugString(g_szMsg);
#endif
		break;
	}
}


// --------------------------------------------------------------------------
//  Funktion:       _Line_ComOpen()
//  Beschreibung :  versucht die Serielle Schnittstelle des "Handy's" zu öffnen
//  Parameter :     Schnittstellen-Nummer (0..9), 
//  Returnwert :    0	- in Bearbeitung
//					-1  - Fehler öffnen Schnittstelle
//					1	- Schnittstelle offen

int	_Line_ComOpen(UCHAR ucCom, DWORD dwBaude)
{
	int		iRet = 0;

	iRet = ComOpen(ucCom, 
				   1,			// 0..RS232 ohne HS, 1..RS232 mit HS
				   dwBaude,		// 300 - 115200
				   8,			// 4 - 8
				   1,			// 1, 2
				   0,			// 0..no, 1..ungerade, 2..gerade
				   1024		// Puffergrösse Ein/Aus in Bytes
		);

	return iRet;
}

// --------------------------------------------------------------------------
//  Funktion:       _Line_ComClose()
//  Beschreibung :  versucht die Serielle Schnittstelle des "Handy's" zu schliessen
//  Parameter :     Schnittstellen-Nummer (0..9), 
//  Returnwert :    0	- in Bearbeitung
//					-1  - Fehler schliessen Schnittstelle
//					1	- Schnittstelle geschlossen

int	_Line_ComClose(UCHAR ucCom)
{
	int		iRet = 0;

	iRet = ComClose(ucCom);

	return iRet;
}

// --------------------------------------------------------------------------
//  Funktion:       _Line_SetPin()
//  Beschreibung :  Steuerung für Thread der versucht den Pin über die Serielle Schnittstelle am Handy zu setzen
//  Parameter :     Schnittstellen-Nummer (0..9), ob auf Data-Call fix umgeschaltet werden soll
//  Returnwert :    0	- in Bearbeitung
//					-9	- falsche AT+CBST Einstellung
//					-8	- falsche PUK2
//					-7	- falsche PIN2
//					-6	- falsche PH PUK
//					-5	- falsche PH PIN
//					-4	- falsche PUK
//					-3	- falsche PIN
//					-2	- sonstiger Fehler
//					-1	- Problem mit Kommunikation über die serielle Schnittstelle
//					1	- OK

int	_Line_SetPin(UCHAR ucCom, BOOL bFixDataCall)
{
	int		iRet = 0;

	g_bFixDataCall = bFixDataCall;

	if (g_hThreadLineSetPin == NULL)
	{
		DWORD	dwThreadId;
		g_bSetPinStop = FALSE;
		g_hThreadLineSetPin = CreateThread(NULL, 0, _Line_SetPin_Thread, (LPVOID)ucCom, 0, &dwThreadId);
	}
	else
	{
		DWORD	dwRes;

		dwRes = WaitForSingleObject(g_hThreadLineSetPin, 0);
		if (dwRes == WAIT_OBJECT_0)
		{
			// Schliessen beendet
			switch (g_iStatSetPinThread)
			{
			case 0:
				iRet = 1;	// Ok
				break;

			case -1:
			case -3:
			case -4:
			case -5:
			case -6:
			case -7:
			case -8:
			case -9:
				iRet = g_iStatSetPinThread;
				break;

			default:
				iRet = -2; // sonstiger Fehler
			}

			CloseHandle(g_hThreadLineSetPin);
			g_hThreadLineSetPin = NULL;
		}
	}

	return iRet;
}

// --------------------------------------------------------------------------
//  Funktion:       _Line_SetPinStop()
//  Beschreibung :  Beenden des Steuerung-Thread der versucht den Pin über die Serielle Schnittstelle am Handy zu setzen
//  Parameter :     
//  Returnwert :    0	- in Bearbeitung
//					1	- OK

int	_Line_SetPinStop(void)
{
	int		iRet = 1;

	if (g_hThreadLineSetPin)
	{
		DWORD	dwRes;

		iRet = 0;	// working
		g_bSetPinStop = TRUE;

		dwRes = WaitForSingleObject(g_hThreadLineSetPin, 0);
		if (dwRes == WAIT_OBJECT_0)
		{
			// Schliessen beendet
			iRet = 1;	// Ok

			CloseHandle(g_hThreadLineSetPin);
			g_hThreadLineSetPin = NULL;
		}
	}

	return iRet;
}

// --------------------------------------------------------------------------
//  Funktion:       _Line_SetPin()
//  Beschreibung :  versucht den Pin über die Serielle Schnittstelle am Handy zu setzen
//  Parameter :     Schnittstellen-Nummer (0..9), 
//  Returnwert :    0
//	Status in g_iStatSetPinThread:
//					-9	- falsche AT+CBST Eintstellung
//					-8	- falsche PUK2
//					-7	- falsche PIN2
//					-6	- falsche PH PUK
//					-5	- falsche PH PIN
//					-4	- falsche PUK
//					-3	- falsche PIN
//					-1	- Problem mit schreiben auf Com
//					0	- Pin OK

DWORD WINAPI _Line_SetPin_Thread(LPVOID lParam)
{
	UCHAR	ucCom;
	DWORD	dwSendStat;
	DWORD	dwReadStat;
	DWORD	dwReadLen;
	DWORD	dwWriteLen;
	DWORD	dwLen;
	CHAR	szReadBuf[128];
	CHAR	szWriteBuf[128];
	int		iLoop;
	BOOL	bFound;

	ucCom = (UCHAR)lParam;

	if (strlen(g_szPin) > 0 || strlen(g_szBearer) > 0)
	{
		Sleep(2000);

		// echo mode ausschalten
		strcpy(szWriteBuf, "ATE0V1\r\n");
		dwWriteLen = strlen(szWriteBuf);
		dwSendStat = ComSend(ucCom, szWriteBuf, dwWriteLen, &dwLen);
		if (dwSendStat != 0 || dwLen != dwWriteLen)
		{
			g_iStatSetPinThread = -1;
#ifdef _LINE_TRACE
			_stprintf(g_szMsg, _T("_Line_SetPin_Thread: pos1\r\n"));
			OutputDebugString(g_szMsg);
#endif
			return 0;
		}

		{
			CHAR	szComp[] = "OK";
			DWORD	dwCompLen;
			DWORD	dwPos;
			int		iComp;
			BOOL	bReady	= FALSE;

			bFound = FALSE;
			dwReadLen = 0;
			dwPos = 0;
			dwCompLen = strlen(szComp);
			for (iLoop = 0; !g_bSetPinStop && !bFound && iLoop < 100; iLoop++)
			{
				dwReadStat = ComReceive(ucCom, &szReadBuf[dwReadLen], 128-dwReadLen, &dwLen);
				if (dwReadStat != 0xFFFFFFFF && dwLen > 0)
				{
					dwReadLen += dwLen;
					szReadBuf[dwReadLen] = '\0';
					if (!bReady && dwReadLen - dwPos >= dwCompLen)
					{
						iComp = strcspn(szReadBuf, "O");
						if (iComp > 0)
						{
							if (!strncmp(&szReadBuf[iComp], szComp, strlen(szComp)))
							{
								bReady = TRUE;
							}
						}
					}

					iComp = strcspn(szReadBuf, "OE");
					while (iComp > 0 && iComp < (int)dwReadLen && !bFound && iLoop < 100)
					{
						iLoop++;
						if (!strncmp(&szReadBuf[iComp], "OK\r\n", strlen("OK\r\n")) ||
							!strncmp(&szReadBuf[iComp], "ERROR\r\n", strlen("ERROR\r\n"))
						   )
						{
							bFound = TRUE;
						}
						else
						{
							iComp += strcspn(&szReadBuf[iComp+1], "OE");
							iComp++;
						}
					}
				}

				if (!bFound)
				{
					Sleep(10);
				}
			}
		}


		// fix auf data call umschalten
		if (!g_bSetPinStop && g_bFixDataCall)
		{
			CHAR	szComp[] = "OK";
			DWORD	dwCompLen;
			DWORD	dwPos;
			int		iComp;
			BOOL	bReady	= FALSE;

			strcpy(szWriteBuf, "AT+CSNS=4\r\n");
			dwWriteLen = strlen(szWriteBuf);
			dwSendStat = ComSend(ucCom, szWriteBuf, dwWriteLen, &dwLen);
			if (dwSendStat != 0 || dwLen != dwWriteLen)
			{
				g_iStatSetPinThread = -1;
#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("_Line_SetPin_Thread: set data call problem\r\n"));
				OutputDebugString(g_szMsg);
#endif
				return 0;
			}


			bFound = FALSE;
			dwReadLen = 0;
			dwPos = 0;
			dwCompLen = strlen(szComp);
			for (iLoop = 0; !g_bSetPinStop && !bFound && iLoop < 100; iLoop++)
			{
				dwReadStat = ComReceive(ucCom, &szReadBuf[dwReadLen], 128-dwReadLen, &dwLen);
				if (dwReadStat != 0xFFFFFFFF && dwLen > 0)
				{
					dwReadLen += dwLen;
					szReadBuf[dwReadLen] = '\0';
					if (!bReady && dwReadLen - dwPos >= dwCompLen)
					{
						iComp = strcspn(szReadBuf, "O");
						if (iComp > 0)
						{
							if (!strncmp(&szReadBuf[iComp], szComp, strlen(szComp)))
							{
								bReady = TRUE;
#ifdef _LINE_TRACE
								_stprintf(g_szMsg, _T("_Line_SetPin_Thread: set data call ok!!!\r\n"));
								OutputDebugString(g_szMsg);
#endif
							}
						}
					}

					iComp = strcspn(szReadBuf, "OE");
					while (iComp > 0 && iComp < (int)dwReadLen && !bFound && iLoop < 100)
					{
						iLoop++;
						if (!strncmp(&szReadBuf[iComp], "OK\r\n", strlen("OK\r\n")) ||
							!strncmp(&szReadBuf[iComp], "ERROR\r\n", strlen("ERROR\r\n"))
						   )
						{
							bFound = TRUE;
						}
						else
						{
							iComp += strcspn(&szReadBuf[iComp+1], "OE");
							iComp++;
						}
					}
				}

				if (!bFound)
				{
					Sleep(10);
				}
			}
		}


		// gegebenenfalls Funkstrecke umstellen
		if (!g_bSetPinStop && strlen(g_szBearer) > 0)
		{
			CHAR	szComp[] = "OK";
			DWORD	dwCompLen;
			DWORD	dwPos;
			int		iComp;
			BOOL	bReady	= FALSE;

			sprintf(szWriteBuf, "AT+CBST=%s\r\n", g_szBearer);
			dwWriteLen = strlen(szWriteBuf);
			dwSendStat = ComSend(ucCom, szWriteBuf, dwWriteLen, &dwLen);
			if (dwSendStat != 0 || dwLen != dwWriteLen)
			{
				g_iStatSetPinThread = -1;
#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("_Line_SetPin_Thread: select bearer service type problem\r\n"));
				OutputDebugString(g_szMsg);
#endif
				return 0;
			}


			bFound = FALSE;
			dwReadLen = 0;
			dwPos = 0;
			dwCompLen = strlen(szComp);
			for (iLoop = 0; !g_bSetPinStop && !bFound && iLoop < 100; iLoop++)
			{
				dwReadStat = ComReceive(ucCom, &szReadBuf[dwReadLen], 128-dwReadLen, &dwLen);
				if (dwReadStat != 0xFFFFFFFF && dwLen > 0)
				{
					dwReadLen += dwLen;
					szReadBuf[dwReadLen] = '\0';
					if (!bReady && dwReadLen - dwPos >= dwCompLen)
					{
						iComp = strcspn(szReadBuf, "O");
						if (iComp > 0)
						{
							if (!strncmp(&szReadBuf[iComp], szComp, strlen(szComp)))
							{
								bReady = TRUE;
#ifdef _LINE_TRACE
								_stprintf(g_szMsg, _T("_Line_SetPin_Thread: select bearer service type ok!!!\r\n"));
								OutputDebugString(g_szMsg);
#endif
							}
						}
					}

					iComp = strcspn(szReadBuf, "OE");
					while (iComp > 0 && iComp < (int)dwReadLen && !bFound && iLoop < 100)
					{
						iLoop++;
						if (!strncmp(&szReadBuf[iComp], "OK\r\n", strlen("OK\r\n")) ||
							!strncmp(&szReadBuf[iComp], "ERROR\r\n", strlen("ERROR\r\n"))
						   )
						{
							bFound = TRUE;
						}
						else
						{
							iComp += strcspn(&szReadBuf[iComp+1], "OE");
							iComp++;
						}
					}
				}

				if (!bFound)
				{
					Sleep(10);
				}
			}
		}


		// prüfen ob pin ok ist
		strcpy(szWriteBuf, "AT+CPIN?\r\n");
		dwWriteLen = strlen(szWriteBuf);
		dwSendStat = ComSend(ucCom, szWriteBuf, dwWriteLen, &dwLen);
		if (dwSendStat != 0 || dwLen != dwWriteLen)
		{
			g_iStatSetPinThread = -1;
#ifdef _LINE_TRACE
			_stprintf(g_szMsg, _T("_Line_SetPin_Thread: pos2\r\n"));
			OutputDebugString(g_szMsg);
#endif
			return 0;
		}

		{
			CHAR	szComp[7][20] = {	"+CPIN: READY",
										"+CPIN: SIM PIN",
										"+CPIN: SIM PUK",
										"+CPIN: PH_SIM PIN",
										"+CPIN: PH_SIM PUK",
										"+CPIN: SIM PIN2",
										"+CPIN: SIM PUK2",
			};
			int		iState[7] = {		0,
										-3,
										-4,
										-5,
										-6,
										-7,
										-8,
			};
			DWORD	dwCompLen;
			DWORD	dwPos;
			int		iComp;
			int		i;
			BOOL	bReady	= FALSE;

			bFound = FALSE;
			dwReadLen = 0;
			dwPos = 0;
			for (iLoop = 0; !g_bSetPinStop && !bFound && iLoop < 100; iLoop++)
			{
				dwReadStat = ComReceive(ucCom, &szReadBuf[dwReadLen], 128-dwReadLen, &dwLen);
				if (dwReadStat != 0xFFFFFFFF && dwLen > 0)
				{
					dwReadLen += dwLen;
					szReadBuf[dwReadLen] = '\0';

					for (i = 0; i < 7 && !bReady; i++)
					{
						dwCompLen = strlen(szComp[i]);
						if (dwReadLen - dwPos >= dwCompLen)
						{
							iComp = strcspn(szReadBuf, "+");
							if (iComp > 0)
							{
								if (!strncmp(&szReadBuf[iComp], szComp[i], strlen(szComp[i])))
								{
									bReady = TRUE;
									g_iStatSetPinThread = iState[i];
								}
							}
						}
					}

					iComp = strcspn(szReadBuf, "OE");
					while (iComp > 0 && iComp < (int)dwReadLen && !bFound && iLoop < 100)
					{
						iLoop++;
						if (!strncmp(&szReadBuf[iComp], "OK\r\n", strlen("OK\r\n")) ||
							!strncmp(&szReadBuf[iComp], "ERROR\r\n", strlen("ERROR\r\n"))
						   )
						{
							bFound = TRUE;
						}
						else
						{
							iComp += strcspn(&szReadBuf[iComp+1], "OE");
							iComp++;
						}
					}
				}

				if (!bFound)
				{
					Sleep(10);
				}
			}

			if (g_iStatSetPinThread == 0)
			{
				g_iStatSetPinThread = 0;
#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("_Line_SetPin_Thread: pin was ok\r\n"));
				OutputDebugString(g_szMsg);
#endif
				return 0;
			}
		}

		// pin setzen
		{
			CHAR	szComp[] = "OK";
			DWORD	dwCompLen;
			DWORD	dwPos;
			int		iComp;
			int		iLoop;
			BOOL	bReady	= FALSE;

			bFound = FALSE;

			if (g_iStatSetPinThread == -3) // pin
			{ 
				if (strlen(g_szPin) > 0)
				{
					sprintf(szWriteBuf, "AT+CPIN=%s\r\n", g_szPin);
				}
				else
				{
#ifdef _LINE_TRACE
					_stprintf(g_szMsg, _T("_Line_SetPin_Thread: No PIN to set !!!\r\n"));
					OutputDebugString(g_szMsg);
#endif
					return 0;
				}
			}
			if (g_iStatSetPinThread == -4)
			{
				if (strlen(g_szPin) > 0 && strlen(g_szPuk) > 0)
				{
					sprintf(szWriteBuf, "AT+CPIN=\"%s\",\"%s\"\r\n", g_szPuk, g_szPin);
				}
				else
				{
#ifdef _LINE_TRACE
					_stprintf(g_szMsg, _T("_Line_SetPin_Thread: No PIN or no PUK to set !!!\r\n"));
					OutputDebugString(g_szMsg);
#endif
					return 0;
				}
			}
			dwWriteLen = strlen(szWriteBuf);
			dwSendStat = ComSend(ucCom, szWriteBuf, dwWriteLen, &dwLen);
			if (dwSendStat != 0 || dwLen != dwWriteLen)
			{
				g_iStatSetPinThread = -1;
#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("_Line_SetPin_Thread: pos1\r\n"));
				OutputDebugString(g_szMsg);
#endif
				return 0;
			}

			dwReadLen = 0;
			dwPos = 0;
			dwCompLen = strlen(szComp);
			for (iLoop = 0; !g_bSetPinStop && !bFound && iLoop < 1000; iLoop++)
			{
				dwReadStat = ComReceive(ucCom, &szReadBuf[dwReadLen], 128-dwReadLen, &dwLen);
				if (dwReadStat != 0xFFFFFFFF && dwLen > 0)
				{
					dwReadLen += dwLen;
					szReadBuf[dwReadLen] = '\0';
					if (dwReadLen - dwPos >= dwCompLen)
					{
						iComp = strcspn(szReadBuf, "O");
						if (iComp > 0)
						{
							if (!strncmp(&szReadBuf[iComp], szComp, strlen(szComp)))
							{
								bReady = TRUE;
								g_iStatSetPinThread = 0; // pin ok
//										OutputDebugString(_T("Ready\n"));
							}
						}
					}
					
					iLoop = 0;
					iComp = strcspn(szReadBuf, "OE");
					while (iComp > 0 && iComp < (int)dwReadLen && !bFound && iLoop < 1000)
					{
						iLoop++;
						if (!strncmp(&szReadBuf[iComp], "OK\r\n", strlen("OK\r\n")) ||
							!strncmp(&szReadBuf[iComp], "ERROR\r\n", strlen("ERROR\r\n"))
						   )
						{
							bFound = TRUE;
//									OutputDebugString(_T("Found\n"));
						}
						else
						{
							iComp += strcspn(&szReadBuf[iComp+1], "OE");
							iComp++;
						}
					}
				}

				if (!bFound)
				{
					Sleep(100);
//							OutputDebugString(_T("Sleep\n"));
				}
#ifndef _WIN32_WCE
				else
				{
					Sleep(6000); // m20 muss sich noch im Netz anmelden, nur für NT Version
				}
#endif
			}
		}
	}

	return 0;
}

// --------------------------------------------------------------------------
//  Funktion:       _Line_Shutdown()
//  Beschreibung :  Steuerung für Thread der versucht das Handy auszuschalten
//  Parameter :     Schnittstellen-Nummer (0..9), 
//  Returnwert :    0	- in Bearbeitung
//					-1	- Problem mit Kommunikation über die serielle Schnittstelle
//					1	- OK

int	_Line_Shutdown(UCHAR ucCom)
{
	int		iRet = 0;

	if (g_hThreadLineShutdown == NULL)
	{
		DWORD	dwThreadId;
		g_hThreadLineShutdown = CreateThread(NULL, 0, _Line_Shutdown_Thread, (LPVOID)ucCom, 0, &dwThreadId);
	}
	else
	{
		DWORD	dwRes;

		dwRes = WaitForSingleObject(g_hThreadLineShutdown, 0);
		if (dwRes == WAIT_OBJECT_0)
		{
			// Schliessen beendet
			if (g_iStatShutdownThread == 0)
				iRet = 1;	// Ok
			else
 				iRet = -1;	// Fehler

			CloseHandle(g_hThreadLineShutdown);
			g_hThreadLineShutdown = NULL;
		}
	}

	return iRet;
}

// --------------------------------------------------------------------------
//  Funktion:       _Line_Shutdown_Thread()
//  Beschreibung :  versucht das Handy auszuschalten
//  Parameter :     Schnittstellen-Nummer (0..9), 
//  Returnwert :    0
//	Status in g_iStatShutdownThread:
//				-1	- Problem mit schreiben auf Com
//				0	- OK

DWORD WINAPI _Line_Shutdown_Thread(LPVOID lParam)
{
	UCHAR	ucCom;
	DWORD	dwRet = 0;
	DWORD	dwSendStat;
	DWORD	dwReadStat;
	DWORD	dwReadLen;
	DWORD	dwWriteLen;
	DWORD	dwLen;
	CHAR	szReadBuf[128];
	CHAR	szWriteBuf[128];
	int		iLoop;
	BOOL	bFound;

	ucCom = (UCHAR)lParam;

	if (g_bShutdown)
	{
		strcpy(szWriteBuf, "AT^SMSO\r\n");
		dwWriteLen = strlen(szWriteBuf);
		dwSendStat = ComSend(ucCom, szWriteBuf, dwWriteLen, &dwLen);
		if (dwSendStat != 0 || dwLen != dwWriteLen)
		{
			g_iStatShutdownThread = -1;
			return 0;
		}

		{
			CHAR	szComp[] = "^SMSO: MS OFF";
			DWORD	dwCompLen;
			DWORD	dwPos;
			int		iComp;
			BOOL	bReady	= FALSE;

			bFound = FALSE;
			dwReadLen = 0;
			dwPos = 0;
			dwCompLen = strlen(szComp);
			for (iLoop = 0; !bFound && iLoop < 100; iLoop++)
			{
				dwReadStat = ComReceive(ucCom, &szReadBuf[dwReadLen], 128-dwReadLen, &dwLen);
				if (dwReadStat != 0xFFFFFFFF && dwLen > 0)
				{
					dwReadLen += dwLen;
					szReadBuf[dwReadLen] = '\0';
					if (!bReady && dwReadLen - dwPos >= dwCompLen)
					{
						iComp = strcspn(szReadBuf, "^");
						if (iComp > 0)
						{
							if (!strncmp(&szReadBuf[iComp], szComp, strlen(szComp)))
							{
								bReady = TRUE;
							}
						}
					}

					iComp = strcspn(szReadBuf, "OE");
					while (iComp > 0 && iComp < (int)dwReadLen && !bFound && iLoop < 10)
					{
						iLoop++;
						if (!strncmp(&szReadBuf[iComp], "OK\r\n", strlen("OK\r\n")) ||
							!strncmp(&szReadBuf[iComp], "ERROR\r\n", strlen("ERROR\r\n"))
						   )
						{
							bFound = TRUE;
						}
						else
						{
							iComp += strcspn(&szReadBuf[iComp+1], "OE");
							iComp++;
						}
					}
				}

				if (!bFound)
				{
					Sleep(10);
				}
			}

		}
	}

	g_iStatShutdownThread = 0;
	return 0;
}

// --------------------------------------------------------------------------
//  Funktion:       Line_Init()
//  Beschreibung :  Initialisiert TAPI und setzt die Pin
//  Parameter :     Schnittstellen-Nummer (0..9), Pinnummer 
//  Returnwert :    -1	- Fehler
//				    0	- in Bearbeitung
//					1	- OK

EN_LINE_ERROR	Line_Init(UCHAR ucCom, DWORD dwBaud, char *szPin, char *szPuk, 
						  char *szCBST, BOOL bFixDataCall, DWORD dwBaud2Rail)
{
	BOOL					bRet = FALSE;
	LINEINITIALIZEEXPARAMS	lineInitializeExParams;
	LONG					lRes;
	DWORD					dwTapiVer;
	int						iStat = 0;
	static int				iInitState = 0;		// (0 = check baud and set, 1 = open serial, 2 = set pin, 3 = close serial)
#ifndef _WIN32_WCE
	DWORD					dwId;
#endif

	if (g_eState != EN_LINE_STATE_NOINIT &&
		g_eState != EN_LINE_STATE_INIT ||
		g_hLineApp != NULL)
	{
		Line_Deinit(ucCom, dwBaud, FALSE);
		return EN_LINE_WORKING;
	}
	
	if (dwBaud2Rail > 0)
	{
		g_bGsmRail = TRUE;
		if (dwBaud2Rail < 10)
		{
			dwBaud2Rail = 0;
		}
	}
	else
	{
		g_bGsmRail = FALSE;
	}

	if (g_eState == EN_LINE_STATE_NOINIT)
	{
		iInitState = 0;
		if (szPin && strlen(szPin) > 0 && strlen(szPin) < sizeof(g_szPin))
		{
			strcpy(g_szPin, szPin);
		}
		else
		{
			strcpy(g_szPin, "\0");
		}
		if (szPuk && strlen(szPuk) > 0 && strlen(szPuk) < sizeof(g_szPuk))
		{
			strcpy(g_szPuk, szPuk);
		}
		else
		{
			strcpy(g_szPuk, "\0");
		}
		if (szCBST && strlen(szCBST) > 0 && strlen(szCBST) < sizeof(g_szBearer))
		{
			strcpy(g_szBearer, szCBST);
		}
		else
		{
			strcpy(g_szBearer, "\0");
		}

		// Modem einschalten
		ComActivateDTR(ucCom, FALSE);
		ComActivateDTR(ucCom, TRUE);
	}

	g_eState = EN_LINE_STATE_INIT;
	if (iInitState == 0)
	{
		g_eError = EN_LINE_OK;
		WriterListInit();
	}

	if (ucCom > 0)
	{
		if (iInitState == 0) // Serielle Schnittstelle Baude prüfen und setzen
		{
#ifndef _WIN32_WCE
			if (g_bGsmRail)
			{
				EN_LINE_ERROR	eStat;

				eStat = Line_GsmRailWait(ucCom);
				switch(eStat)
				{
				case EN_LINE_OK:
					iInitState = 1;
					break;
				case EN_LINE_WORKING:
					return EN_LINE_WORKING;
					break;
				case -1:
					g_eState = EN_LINE_STATE_NOINIT;
					g_eError = EN_LINE_ERR_MODEM;
					return g_eError;
					break;
				default:
					return eStat;
				}
			}
			else
#endif
			{
				EN_LINE_ERROR	eStat;
				DWORD			dwBaudOld;

				eStat = Line_FindBaud(ucCom, &dwBaudOld, dwBaud);
				switch(eStat)
				{
				case EN_LINE_OK:
					iInitState = 1;
					break;
				case EN_LINE_WORKING:
					return EN_LINE_WORKING;
					break;
				default:
					return eStat;
				}
			}
		}
	}

	if (ucCom > 0 && strlen(g_szPin) > 0 || strlen(g_szBearer) > 0 || dwBaud2Rail)
	{
#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("Line_Init: iInitState %d\r\n"), iInitState);
		OutputDebugString(g_szMsg);
#endif

		// Serielle Schnittstelle öffnen
		if (iInitState == 1)
		{
#ifdef _LINE_TRACE
			_stprintf(g_szMsg, _T("Line_Init: Open serial for pin or bearer\r\n"));
			OutputDebugString(g_szMsg);
#endif
			iStat = _Line_ComOpen(ucCom, dwBaud);
			switch (iStat)
			{
			case 1:
				iInitState = 2;
				g_eError = EN_LINE_OK;
				break;
			case -1:
				iInitState = 0;
				g_eState = EN_LINE_STATE_NOINIT;
				g_eError = EN_LINE_ERR_CLOSE;
#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("Line_Init: Open serial code %d\r\n"), iStat);
				OutputDebugString(g_szMsg);
#endif
				return g_eError;
				break;
			default:
				return EN_LINE_WORKING;
				break;
			}
		}

		// Pin setzten
		if (iInitState == 2)
		{
			if (!strcmp(g_szPin, "_"))
			{
				iInitState = 3;
#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("Line_Init: Wrong PIN during retry\r\n"));
				OutputDebugString(g_szMsg);
#endif
				g_eError = EN_LINE_ERR_WRONGPIN;
			}
			else
			{
				iStat = _Line_SetPin(ucCom, bFixDataCall);
				switch (iStat)
				{
				case 1:
					iInitState = 3;
					g_eError = EN_LINE_OK;
#ifdef _LINE_TRACE
					_stprintf(g_szMsg, _T("Line_Init: Set Pin ok!!\r\n"), iStat);
					OutputDebugString(g_szMsg);
#endif
					break;
				case -1: // kommunikationsfehler
				case -2: // sonstiger fehler
				default:
					iInitState = 3;
					g_eError = EN_LINE_ERR_MODEM;
#ifdef _LINE_TRACE
					_stprintf(g_szMsg, _T("Line_Init: Set Pin code %d\r\n"), iStat);
					OutputDebugString(g_szMsg);
#endif
					break;
				case -3: // falscher PIN
					iInitState = 3;
#ifdef _LINE_TRACE
					_stprintf(g_szMsg, _T("Line_Init: Wrong PIN code %d\r\n"), iStat);
					OutputDebugString(g_szMsg);
#endif
					g_eError = EN_LINE_ERR_WRONGPIN;
					break;
				case -4: // falsche PUK
					iInitState = 3;
#ifdef _LINE_TRACE
					_stprintf(g_szMsg, _T("Line_Init: Wrong PUK code %d\r\n"), iStat);
					OutputDebugString(g_szMsg);
#endif
					g_eError = EN_LINE_ERR_WRONGPUK;
					break;
				case -5: // falsche PH PIN
					iInitState = 3;
#ifdef _LINE_TRACE
					_stprintf(g_szMsg, _T("Line_Init: Wrong PH PIN code %d\r\n"), iStat);
					OutputDebugString(g_szMsg);
#endif
					g_eError = EN_LINE_ERR_WRONGPHPIN;
					break;
				case -6: // falsche PH PUK
					iInitState = 3;
#ifdef _LINE_TRACE
					_stprintf(g_szMsg, _T("Line_Init: Wrong PH PUK code %d\r\n"), iStat);
					OutputDebugString(g_szMsg);
#endif
					g_eError = EN_LINE_ERR_WRONGPHPUK;
					break;
				case -7: // falsche PIN2
					iInitState = 3;
#ifdef _LINE_TRACE
					_stprintf(g_szMsg, _T("Line_Init: Wrong PIN2 code %d\r\n"), iStat);
					OutputDebugString(g_szMsg);
#endif
					g_eError = EN_LINE_ERR_WRONGPIN2;
					break;
				case -8: // falsche PUK2
					iInitState = 3;
#ifdef _LINE_TRACE
					_stprintf(g_szMsg, _T("Line_Init: Wrong PUK2 code %d\r\n"), iStat);
					OutputDebugString(g_szMsg);
#endif
					g_eError = EN_LINE_ERR_WRONGPUK2;
					break;
				case -9: // falsche CBST-Befehl
					iInitState = 3;
#ifdef _LINE_TRACE
					_stprintf(g_szMsg, _T("Line_Init: Wrong AT+CBST code %d\r\n"), iStat);
					OutputDebugString(g_szMsg);
#endif
					g_eError = EN_LINE_ERR_WRONGCBST;
					break;
				case 0:
					return EN_LINE_WORKING;
					break;
				} 
			}
		}

		// Baudrate umschalten
		if (iInitState == 3)
		{
#ifdef _WIN32_WCE
			if (dwBaud2Rail == 0)
			{
				iInitState = 4;
			}
			else
			{
				iStat = _Line_ChangeBaud(ucCom, dwBaud2Rail);
				switch (iStat)
				{
				case 1:
					iInitState = 4;
					g_eError = EN_LINE_OK;
#ifdef _LINE_TRACE
					_stprintf(g_szMsg, _T("Line_Init: Baud changed ok!!, baud %d\r\n"), iStat, dwBaud2Rail);
					OutputDebugString(g_szMsg);
#endif
					break;
				case -1: // kommunikationsfehler
				case -2: // sonstiger fehler
				default:
					iInitState = 4;
					g_eError = EN_LINE_ERR_MODEM;
#ifdef _LINE_TRACE
					_stprintf(g_szMsg, _T("Line_Init: Baud change code %d, baud %d\r\n"), iStat, dwBaud2Rail);
					OutputDebugString(g_szMsg);
#endif
					break;
				case 0:
					return EN_LINE_WORKING;
					break;
				}
			}
#else
			iInitState = 4;
#endif
		}

		// Seriele Schnittstelle schliessen
		if (iInitState == 4)
		{
			iStat = _Line_ComClose(ucCom);
			switch (iStat)
			{
			case 1:
				iInitState = 0;
				break;
			case -1:
				iInitState = 0;
				g_eState = EN_LINE_STATE_NOINIT;
				g_eError = EN_LINE_ERR_CLOSE;
#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("Line_Init: Close serial code %d\r\n"), iStat);
				OutputDebugString(g_szMsg);
#endif
				return g_eError;
				break;
			default:
				return EN_LINE_WORKING;
				break;
			}
		}
	}

	if (g_eError != EN_LINE_OK)
	{
		g_eState = EN_LINE_STATE_NOINIT;

		return g_eError;
	}

	g_hEvReply = CreateEvent(NULL, FALSE, FALSE, NULL);

	// using callback
	// Clear the structure before using...
	memset(&lineInitializeExParams, 0, sizeof(LINEINITIALIZEEXPARAMS));

	// Populate the options...
	lineInitializeExParams.dwTotalSize = sizeof(LINEINITIALIZEEXPARAMS);
	lineInitializeExParams.dwOptions = LINEINITIALIZEEXOPTION_USEHIDDENWINDOW;

	// Initialize TAPI.
	dwTapiVer = _LINE_TAPI_CURRENT_VERSION;
	lRes = lineInitializeEx(&g_hLineApp, NULL, Line_Int_Callback, 
		_T("CallLine_Init"), &g_dwNumDevs, &dwTapiVer, &lineInitializeExParams);

	if (lRes == 0)
	{
		bRet = TRUE;
		g_eState = EN_LINE_STATE_NOOPEN;
	}
	else
	{
#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("Line_Init: lineInitializeEx error %X \r\n"), lRes);
		OutputDebugString(g_szMsg);
#endif
		bRet = FALSE;
	}

#ifndef _WIN32_WCE
	g_ovIn.Offset = 0;
    g_ovIn.OffsetHigh = 0;
    g_ovIn.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	g_ovOut.Offset = 0;
    g_ovOut.OffsetHigh = 0;
    g_ovOut.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	g_bReadThread = TRUE;
	g_iReading = -1;
	g_hReadThread = CreateThread(NULL, 0, ReadThread, NULL, 0, &dwId);
//	TRACE(_T("Com- Thread created: h = 0x%08X, ID = 0x%08X\n"), m_hComThread, dwId);

#endif

	return EN_LINE_OK;
}

///////////////////////////////////////////////////////////////////////////////////
void	Line_Int_CloseHandle(void)
{
#ifndef _WIN32_WCE

	g_bReadThread = FALSE;
	Sleep(100);

	if (g_hReadThread)
	{
		TerminateThread(g_hReadThread, 0);
		g_hReadThread = NULL;
	}

	if (g_ovIn.hEvent != NULL)
	{
		CloseHandle(g_ovIn.hEvent);
		g_ovIn.hEvent = NULL;
	}

	if (g_ovOut.hEvent != NULL)
	{
		CloseHandle(g_ovOut.hEvent);
		g_ovOut.hEvent = NULL;
	}
#endif

	if (g_hWriterThreadEnd)
	{
		SetEvent(g_hWriterThreadEnd);
		WaitForSingleObject(g_hWriterThread, 5000);

		CloseHandle(g_hWriterThread);
		g_hWriterThread = NULL;

		ResetEvent(g_hWriterThreadEnd);
	}
	CloseHandle(g_hWriterThreadEnd);
	g_hWriterThreadEnd = NULL;

	if (g_hIncommingThread != NULL)
	{
		CloseHandle(g_hIncommingThread);
		g_hIncommingThread = NULL;
	}

	if (g_hChangeBaudThread != NULL)
	{
		CloseHandle(g_hChangeBaudThread);
		g_hChangeBaudThread = NULL;
	}

	if (g_hThreadLineBaud2Rail != NULL)
	{
		CloseHandle(g_hThreadLineBaud2Rail);
		g_hThreadLineBaud2Rail = NULL;
	}

	if (g_hCallThread != NULL)
	{
		CloseHandle(g_hCallThread);
		g_hCallThread = NULL;
	}

	if (g_hEvReply)
	{
		CloseHandle(g_hEvReply);
		g_hEvReply = NULL;
	}

	if (g_hComm != NULL && g_hComm != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_hComm);
		g_hComm = NULL;
	}

	g_dwTimeTick = 0;
	g_bModemComm = FALSE;
}

// --------------------------------------------------------------------------
//  Funktion:       Line_Deinit()
//  Beschreibung :  Deinitialisiert TAPI und schaltet Handy aus
//  Parameter :     Schnittstellen-Nummer (0..9), Pinnummer 
//  Returnwert :    -1	- Fehler
//				    0	- in Bearbeitung
//					1	- OK

EN_LINE_ERROR	Line_Deinit(UCHAR ucCom, DWORD dwBaud, BOOL bShutdown)
{
	BOOL			bRet = FALSE;
	LINECALLSTATUS	CallStatus;
	LONG			lRes;
	static			iDeinitState = 0;
	EN_LINE_ERROR	enBaudFindState;
	int				iSetPinState;

	if (g_eState == EN_LINE_STATE_NOINIT)
	{
		iDeinitState = 0;
		return EN_LINE_OK;
	}

	g_bShutdown = bShutdown;
	
	// thread für setpin stoppen
	iSetPinState = _Line_SetPinStop();
	if (iSetPinState == 0)
	{
		return EN_LINE_WORKING;
	}

	// thread für autobaud stoppen
	enBaudFindState = Line_FindBaudStop();
	if (enBaudFindState == EN_LINE_WORKING)
	{
		return EN_LINE_WORKING;
	}

	if (iDeinitState == 0)
	{
		Line_Int_CloseHandle();

		if(g_hCall != NULL)  // Call might be in progress...
		{
			memset(&CallStatus, 0, sizeof(LINECALLSTATUS));

			lRes = lineGetCallStatus(g_hCall, &CallStatus); 
				// Technically, lineGetCallStatus returns more info than 
				// there is in the structure.  Since we don't care about it,
				// We just go on our merry way...
			if (lRes == 0) // If it didn't fail, there's at least a call that needs
			{
				// to be droped.
	#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("Line_Deinit: dropping Line !!!! please drop yourself \r\n"));
				OutputDebugString(g_szMsg);
	#endif
				lineDrop(g_hCall, NULL, 0);
				g_hCall = NULL;
			}
		}

		if(g_hLine)
		{
			lRes = lineClose(g_hLine);
			if (lRes != 0)
			{
	#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("Line_Deinit: lineClose error %X\r\n"), lRes);
				OutputDebugString(g_szMsg);
	#endif
			}
			g_hLine = NULL;
		}


	//	g_bTerminate = TRUE;  // Tell the event thread that it's time to go...
	//	WaitForSingleObject(hEventThread, INFINITE); // Wait for it to comit suicide..
	//	CloseHandle(hEventThread);
	//	printf("Shutting down TAPI.\r\n");
		lRes = lineShutdown(g_hLineApp);
		if(lRes == 0)
		{
			bRet = FALSE;
		}
		else
		{
	#ifdef _LINE_TRACE
			_stprintf(g_szMsg, _T("Line_Deinit: lineShutdown error %X \r\n"), lRes);
			OutputDebugString(g_szMsg);
	#endif
			bRet = TRUE;
		}
		g_hLineApp = NULL;
		g_eState = EN_LINE_STATE_DEINIT;
		iDeinitState = 1;
	}

	if (ucCom > 0 && g_bShutdown)
	{
		int iStat;

		// serielle Schnittstelle öffnen
		if (iDeinitState == 1)
		{
			iStat = _Line_ComOpen(ucCom, dwBaud);
			switch (iStat)
			{
			case 1:
				iDeinitState = 2;
				g_eError = EN_LINE_OK;
				break;
			case -1:
				iDeinitState = 0;
				g_eState = EN_LINE_STATE_NOINIT;
				return EN_LINE_ERR_CLOSE;
				break;
			default:
				return EN_LINE_WORKING;
				break;
			}
		}

		// Handy wieder ausschalten
		if (iDeinitState == 2)
		{
			iStat = _Line_Shutdown(ucCom);
			switch (iStat)
			{
			case 1:
				iDeinitState = 3;
				g_eError = EN_LINE_OK;
				break;
			case -1:
				iDeinitState = 0;
				return EN_LINE_ERR_CLOSE;
				break;
			default:
				return EN_LINE_WORKING;
				break;
			}
		}

		// serielle Schnittstelle schliessen
		if (iDeinitState == 3)
		{
			iStat = _Line_ComClose(ucCom);
			switch (iStat)
			{
			case 1:
				iDeinitState = 0;
				g_eState = EN_LINE_STATE_NOINIT;
				break;
			case -1:
				iDeinitState = 0;
				return EN_LINE_ERR_CLOSE;
				break;
			default:
				return EN_LINE_WORKING;
				break;
			}
		}
	}

	WriterListClear();

	iDeinitState = 0;
	g_eError = EN_LINE_OK;
	g_eState = EN_LINE_STATE_NOINIT;
	return EN_LINE_OK;
}


///////////////////////////////////////////////////////////////////////////////////
BOOL	Line_Open(DWORD dwLine)
{
	BOOL	bRet = FALSE;
	LONG	lRes;
#ifdef _LINE_TRACE
	DWORD	dwLineStates;
	DWORD	dwAddressStates;
#endif

		// Open the line...
	lRes = lineOpen(g_hLineApp, dwLine, &g_hLine, _LINE_TAPI_CURRENT_VERSION, 0x00000000, 1, 
			LINECALLPRIVILEGE_OWNER, g_dwMediaMode, NULL);
	if(lRes != 0)
	{	
#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("Line_Open: lineOpen error %X\r\n"), lRes);
		OutputDebugString(g_szMsg);
#endif
		bRet = FALSE;
		return bRet;
	}

	// We want to be notified for everything LINEDEVSTATE_REINIT 
	//                                                0x00000001
	lRes = lineSetStatusMessages(g_hLine, 0x01ffffff, 0x000001ff);
#ifdef _WIN32_WCE
	if(lRes != 0)
#else
	if(lRes != 0 && lRes != LINEERR_OPERATIONUNAVAIL) // fehler von eigenem TSP ACHTUNG!!!
#endif
	{	
#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("Line_Open: lineSetStatusMessages error %X\r\n"), lRes);
		OutputDebugString(g_szMsg);
#endif
		bRet = FALSE;
	}
	else
	{
		bRet = TRUE;
		g_dwLine = dwLine;
		g_eState = EN_LINE_STATE_OPENING;
	}

#ifdef _LINE_TRACE
	lRes = lineGetStatusMessages(g_hLine, &dwLineStates, &dwAddressStates);
	if (lRes >= 0)
	{
		_stprintf(g_szMsg, _T("Line_Open: lineGetStatusMessages state %X, address %X.\r\n"), dwLineStates, dwAddressStates);
		OutputDebugString(g_szMsg);
	}
#endif

	return bRet;
}


// --------------------------------------------------------------------------
//  Funktion:       _Line_Close_Thread()
//  Beschreibung :  Trennt die Modemverbindung
//  Parameter :      
//  Returnwert :    0
DWORD WINAPI _Line_Close_Thread(LPVOID lParam)
{
	LONG	lRes;

	if (g_hComm != NULL && g_hComm != INVALID_HANDLE_VALUE)
	{
		BOOL	bStat;
		DWORD	dwErr;

#ifndef _WIN32_WCE
		bStat = PurgeComm(g_hComm, PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_RXABORT);
		if (!bStat)
		{
			dwErr = GetLastError();
		}
#else
		bStat = PurgeComm(g_hComm, PURGE_TXCLEAR | PURGE_RXCLEAR);
		if (!bStat)
		{
			dwErr = GetLastError();
		}
#endif
	}

	Line_Int_CloseHandle();

	if(g_hCall != NULL)  // Call might be in progress...
	{
#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("_Line_Close_Thread: dropping Line\r\n"));
		OutputDebugString(g_szMsg);
#endif
		lineDrop(g_hCall, NULL, 0);
		g_hCall = NULL;
	}

	if (g_hLine)
	{
		lRes = lineClose(g_hLine);
		if (lRes != 0)
		{
#ifdef _LINE_TRACE
			_stprintf(g_szMsg, _T("_Line_Close_Thread: lineClose error %X\r\n"), lRes);
			OutputDebugString(g_szMsg);
#endif
		}
		g_hLine = NULL;
	}

#ifdef _LINE_TRACE
	_stprintf(g_szMsg, _T("_Line_Close_Thread: end\r\n"));
	OutputDebugString(g_szMsg);
#endif

	return 0;
}
///////////////////////////////////////////////////////////////////////////////////
BOOL	Line_Close(void)
{
	BOOL bRet = FALSE;

	if (g_eState > EN_LINE_STATE_NOOPEN)
	{
		if (g_hThreadLineClose == NULL)
		{
			DWORD	dwThreadId;
			g_hThreadLineClose = CreateThread(NULL, 0, _Line_Close_Thread, NULL, 0, &dwThreadId);
#ifdef _LINE_TRACE
			_stprintf(g_szMsg, _T("Line_Close: start thread\r\n"));
			OutputDebugString(g_szMsg);
#endif
		}
		else
		{
			DWORD	dwRes;

			dwRes = WaitForSingleObject(g_hThreadLineClose, 0);
			if (dwRes == WAIT_OBJECT_0)
			{
				// Schliessen beendet
				CloseHandle(g_hThreadLineClose);
				g_hThreadLineClose = NULL;
				g_eState = EN_LINE_STATE_NOOPEN;
#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("Line_Close: completed\r\n"));
				OutputDebugString(g_szMsg);
#endif
			}
		}
	}

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD WINAPI IncommingThread (LPVOID lpThreadParam)
{
	BOOL	bLoop = TRUE;
	LONG	lRes;

#ifdef _LINE_TRACE
	_stprintf(g_szMsg, _T("IncommingThread: start.\r\n"));
	OutputDebugString(g_szMsg);
#endif

	while (bLoop)
	{
		// Now we wait for notification...
		WaitForSingleObject(g_hEvReply, INFINITE);
		ResetEvent(g_hEvReply);
		if(g_dwTAPIMsg == LINECALLSTATE_OFFERING)
		{
#ifdef _LINE_TRACE
			_stprintf(g_szMsg, _T("Incoming thread: Answering.\r\n"));
			OutputDebugString(g_szMsg);
#endif

			lRes = lineAnswer(g_hCall, NULL, 0);
			if(lRes < 0)
			{	//  Doh!
#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("Incoming thread: lineAnswer error: %X\n"), lRes);
				OutputDebugString(g_szMsg);
#endif
				bLoop = FALSE;
			}
			else
			{
#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("Incoming thread: lineAnswer %X.\n"), lRes);
				OutputDebugString(g_szMsg);
#endif
				g_eState = EN_LINE_STATE_ANSWERING;
			}
		}
		else if(g_dwTAPIMsg == LINECALLSTATE_CONNECTED)
		{
			VARSTRING	*pVarString;

#ifdef _LINE_TRACE
			_stprintf(g_szMsg, _T("Incoming thread: opening data communication.\n"));
			OutputDebugString(g_szMsg);
#endif

//			g_lState = _LINE_CONNECTED;
			pVarString = (VARSTRING*)malloc(sizeof(VARSTRING)+100);
			memset(pVarString, 0, sizeof(VARSTRING)+100);
			// Populate the options...
			pVarString->dwTotalSize = sizeof(VARSTRING)+100;
			pVarString->dwStringFormat = STRINGFORMAT_ASCII;

			lRes = lineGetID(g_hLine, g_dwLine, g_hCall, LINECALLSELECT_CALL, pVarString, _T("comm/datamodem"));
			if (lRes < 0)
			{	//  Doh!
#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("Incoming thread: lineGetID error: %X\n"), lRes);
				OutputDebugString(g_szMsg);
#endif
			}
			else
			{
				BOOL	bStat;
				DWORD	dwErr;

				g_hComm = *((LPHANDLE) ((LPBYTE)pVarString+pVarString->dwStringOffset));

				if (g_hComm != NULL && g_hComm != INVALID_HANDLE_VALUE)
				{
					// Now we wait for notification...
					COMMTIMEOUTS	ctmoCommPort;
					DCB				dcbCommPort;

//					bStat = GetCommTimeouts(g_hComm, &ctmoCommPort);
//					ctmoCommPort.ReadIntervalTimeout = 1;
//					ctmoCommPort.ReadTotalTimeoutConstant = 1;
//					ctmoCommPort.ReadTotalTimeoutMultiplier = 1;
					ctmoCommPort.ReadIntervalTimeout = MAXDWORD;
					ctmoCommPort.ReadTotalTimeoutConstant = 0;
					ctmoCommPort.ReadTotalTimeoutMultiplier = 0;
					ctmoCommPort.WriteTotalTimeoutConstant = 0;
					ctmoCommPort.WriteTotalTimeoutMultiplier = 0;

					bStat = SetCommTimeouts(g_hComm, &ctmoCommPort);
					if (!bStat)
					{
						dwErr = GetLastError();
					}

					dcbCommPort.DCBlength = sizeof(DCB);

//					bStat = GetCommState(g_hComm, &dcbCommPort);
//					if (!bStat)
//					{
//						dwErr = GetLastError();
//					}
//					bStat = SetCommState(g_hComm, &dcbCommPort);
//					if (!bStat)
//					{
//						dwErr = GetLastError();
//					}

					bStat = SetCommMask(g_hComm, EV_RXCHAR);
					if (!bStat)
					{
						dwErr = GetLastError();
					}

					g_bModemComm = TRUE;
					g_dwTimeTick = GetTickCount();
					g_eState = EN_LINE_STATE_CONNECTED;
				}
			}

			free(pVarString);
			bLoop = FALSE;
		}
	}

	return 0;
}

// -----------------------------------------------------------------------------
// initialisiert die Writer liste
// 
void	WriterListInit(void)
{
	int		i;

	g_WriterList.nPosSend = -1;
	g_WriterList.nPosWrite = -1;

	for (i = 0; i < WRITER_LIST_COUNT; i++)
	{
		g_WriterList.dwSize[i] = 0;
		g_WriterList.pData[i] = NULL;
	}
}

// -----------------------------------------------------------------------------
// löscht die Writer liste
// 
void	WriterListClear(void)
{
	int		i;

	g_WriterList.nPosSend = -1;
	g_WriterList.nPosWrite = -1;

	for (i = 0; i < WRITER_LIST_COUNT; i++)
	{
		g_WriterList.dwSize[i] = 0;
		if (g_WriterList.pData[i] != NULL)
		{
			free(g_WriterList.pData[i]);
			g_WriterList.pData[i] = NULL;
		}
	}
}

// -----------------------------------------------------------------------------
// sendet einen Datensatz an die schnittstelle und liefert TRUE zurück wenn ein
// datensatz geschrieben wurde
BOOL WriterThreadWrite(void)
{
	BOOL	bSend = FALSE;
	int		nPosStart;
	int		nPosEnd;
	int		nPos;
	DWORD	dwWritten;

	nPosStart = g_WriterList.nPosWrite;
	nPosEnd = g_WriterList.nPosSend;

	if (nPosEnd >= 0)
	{
		if (nPosStart < 0)
		{
			nPosStart = 0;
		}
		else
		{
			if (nPosStart > nPosEnd)
			{
				nPosEnd += WRITER_LIST_COUNT;
			}
			nPosStart++;
		}
	}

	if (nPosStart >= 0 && nPosStart <= nPosEnd)
	{
		// daten schreiben
		nPos = nPosStart % WRITER_LIST_COUNT;
#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("WriterListWrite start nPos %d, dwSize %d\n"), nPos, g_WriterList.dwSize[nPos]);
		OutputDebugString(g_szMsg);
#endif
		WriteFile(g_hComm, g_WriterList.pData[nPos], g_WriterList.dwSize[nPos], &dwWritten, NULL);
		if (dwWritten < g_WriterList.dwSize[nPos])
		{
			DWORD	dwErrorFlags;
			COMSTAT	comStat;

			ClearCommError(g_hComm, &dwErrorFlags, &comStat);

			OutputDebugString(_T("WriterListWrite data not written !\n"));
		}
		bSend = TRUE;

		free(g_WriterList.pData[nPos]);
		g_WriterList.pData[nPos] = NULL;

		g_WriterList.nPosWrite = nPos;
	}

	return bSend;
}

///////////////////////////////////////////////////////////////////////
// fügt neuen Statz zur Writerliste hinzu
BOOL WriterListAdd(void *pvBufOut, DWORD dwLenOut, DWORD *pdwWrittenOut)
{
	BOOL	bRet = FALSE;
	int		nPosStart;
	int		nPosEnd;

	nPosStart = g_WriterList.nPosWrite;
	nPosEnd = g_WriterList.nPosSend;

	if (nPosEnd < 0)
	{
		nPosEnd = 0;
		bRet = TRUE;
	}
	else
	{
		nPosEnd = ( nPosEnd + 1 ) % WRITER_LIST_COUNT;
		if (nPosStart < 0 && nPosEnd != 0 || nPosStart >= 0)
		{
			if (nPosEnd != nPosStart)
			{
				bRet = TRUE;
			}
		}

	}

	// daten einfügen
	if (bRet)
	{
		if (g_WriterList.pData[nPosEnd])
		{
			bRet = FALSE;
			OutputDebugString(_T("WriterListAdd error insert !!!! memory not free\n"));
		}
		else
		{
			g_WriterList.pData[nPosEnd] = malloc(dwLenOut);
			if (g_WriterList.pData[nPosEnd] != NULL)
			{
				memcpy(g_WriterList.pData[nPosEnd], pvBufOut, dwLenOut);
				g_WriterList.dwSize[nPosEnd] = dwLenOut;
				*pdwWrittenOut = dwLenOut;	

#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("WriterListAdd insert Pos %d, size %d\n"), nPosEnd, g_WriterList.dwSize[nPosEnd]);
				OutputDebugString(g_szMsg);
#endif
			}
			g_WriterList.nPosSend = nPosEnd;
		}
	}
	return bRet;
}

///////////////////////////////////////////////////////////////////////////////////
#undef __FUNC__
#define __FUNC__ TEXT("WriterThread")
DWORD WINAPI WriterThread (LPVOID lpThreadParam)
{
	BOOL	bLoop = TRUE;
//	LONG	lRes;
	DWORD	dwRet;
	BOOL	bWritten;

#ifdef _LINE_TRACE
	_stprintf(g_szMsg, _T("%s: start.\r\n"), __FUNC__);
	OutputDebugString(g_szMsg);
#endif

	while (bLoop)
	{
		// Now we wait for notification...
//		_stprintf(g_szMsg, _T("%s: loop.\r\n"), __FUNC__);
//		OutputDebugString(g_szMsg);

		dwRet = WaitForSingleObject(g_hWriterThreadEnd, 100);
		switch(dwRet)
		{
		case WAIT_OBJECT_0:
			bLoop = FALSE;
			break;

		case WAIT_TIMEOUT:
			bWritten = TRUE;
			for (bWritten = TRUE; bWritten; )
			{
				bWritten = WriterThreadWrite();
				if(bWritten)
				{
					Sleep(30);
				}
			}
			// prüfen ob daten für output vorhanden sind
			break;
		}
	}

#ifdef _LINE_TRACE
	_stprintf(g_szMsg, _T("%s: end.\r\n"), __FUNC__);
	OutputDebugString(g_szMsg);
#endif

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL	Line_Incoming(void)
{
	BOOL	bRet = FALSE;

	g_hIncommingThread = CreateThread(NULL, 0, IncommingThread, NULL, 0, 0);
	if (g_hIncommingThread != NULL)
	{
		bRet = TRUE;
	}

	g_hWriterThreadEnd = CreateEvent(NULL, TRUE, FALSE, NULL);

	g_hWriterThread = CreateThread(NULL, 0, WriterThread, NULL, 0, 0);
	if (g_hIncommingThread != NULL)
	{
		bRet = bRet && TRUE;
	}

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////////
#undef __FUNC__
#define __FUNC__ TEXT("_Line_RailIni")
BOOL _Line_RailIni(UCHAR ucCom, int iBaudRate)
{
	DWORD	dwRet = 0;
	DWORD	dwReadLen;
	DWORD	dwWriteLen;
	DWORD	dwLen;
	CHAR	szReadBuf[128];
	CHAR	szWriteBuf[128];
	int		iLoop;
	BOOL	bFound;
//	BOOL	bStat;
	BOOL	bRet = FALSE;
	DWORD	dwSendStat = 0;

	sprintf(szWriteBuf, "AT+IPR=%d\r\n", iBaudRate);
	dwWriteLen = strlen(szWriteBuf);

	dwSendStat = ComSend(ucCom, szWriteBuf, dwWriteLen, &dwLen);
	if (dwSendStat != 0 || dwLen != dwWriteLen)
	{
		return FALSE;
	}

	{
//		DWORD	dwPos;
		DWORD	dwReadStat;
		int		iComp;
		BOOL	bReady	= FALSE;

		bFound = FALSE;
		dwReadLen = 0;
//		dwPos = 0;
		for (iLoop = 0; !bFound && iLoop < 100; iLoop++)
		{
			dwReadStat = ComReceive(ucCom, &szReadBuf[dwReadLen], 128-dwReadLen, &dwLen);
			if (dwReadStat != 0xFFFFFFFF && dwLen > 0)
			{
				dwReadLen += dwLen;
				szReadBuf[dwReadLen] = '\0';
				iComp = strcspn(szReadBuf, "OE");
				while (iComp > 0 && iComp < (int)dwReadLen && !bFound && iLoop < 10)
				{
					iLoop++;
					if (!strncmp(&szReadBuf[iComp], "OK\r\n", strlen("OK\r\n")))
					{
						bFound = TRUE;
						bRet = TRUE;
					}
					else if (!strncmp(&szReadBuf[iComp], "ERROR\r\n", strlen("ERROR\r\n")))
					{
						bFound = TRUE;
					}
					else
					{
						iComp += strcspn(&szReadBuf[iComp+1], "OE");
						iComp++;
					}
				}
			}

			if (!bFound)
			{
				Sleep(20);
			}
		}

#ifdef _LINE_TRACE
	if (!bRet)
	{
		_stprintf(g_szMsg, _T("%s: not found response was '%s'\r\n"), __FUNC__, szReadBuf);
		OutputDebugString(g_szMsg);
	}
#endif


	}

	return bRet;
}
/*
///////////////////////////////////////////////////////////////////////////////////
#undef __FUNC__
#define __FUNC__ TEXT("ChangeBaudThread")
DWORD WINAPI ChangeBaudThread (LPVOID lpThreadParam)
{
	int		iBaudRate;
	BOOL	bLoop = TRUE;
	LONG	lRes;


#ifdef _LINE_TRACE
	_stprintf(g_szMsg, _T("%s: start.\r\n"), __FUNC__);
	OutputDebugString(g_szMsg);
#endif

	iBaudRate = (int)lpThreadParam;

	while (bLoop)
	{
		// Now we wait for notification...
		WaitForSingleObject(g_hEvReply, INFINITE);
		ResetEvent(g_hEvReply);
		if(g_dwTAPIMsg == LINECALLSTATE_CONNECTED)
		{
			VARSTRING	*pVarString = NULL;

#ifdef _LINE_TRACE
			_stprintf(g_szMsg, _T("%s: opening data communication.\n"), __FUNC__);
			OutputDebugString(g_szMsg);
#endif

//			g_lState = _LINE_CONNECTED;
			pVarString = (VARSTRING*)malloc(sizeof(VARSTRING)+100);
			memset(pVarString, 0, sizeof(VARSTRING)+100);
			// Populate the options...
			pVarString->dwTotalSize = sizeof(VARSTRING)+100;
			pVarString->dwStringFormat = STRINGFORMAT_ASCII;

			lRes = lineGetID(g_hLine, g_dwLine, g_hCall, LINECALLSELECT_CALL, pVarString, _T("comm/datamodem"));
//			lRes = lineGetID(g_hLine, 0L, NULL, LINECALLSELECT_CALL, pVarString, _T("comm/datamodem"));
			if (lRes < 0)
			{	//  Doh!
#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("%s: lineGetID error: %X\n"), __FUNC__, lRes);
				OutputDebugString(g_szMsg);
#endif
			}
			else
			{
				LINEDIALPARAMS	lineDialParams;
				BOOL			bStat;
				DWORD			dwErr;
				VARSTRING		*pVarString2 = NULL;
				DWORD			dwSize;
				int				iLoop;

#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("%s: lineGetID lRes, %X\n"), __FUNC__, lRes);
				OutputDebugString(g_szMsg);
#endif
				g_hComm = *((LPHANDLE) ((LPBYTE)pVarString+pVarString->dwStringOffset));

				if (g_hComm != NULL && g_hComm != INVALID_HANDLE_VALUE)
				{
					// Now we wait for notification...
					COMMTIMEOUTS	ctmoCommPort;
					DCB				dcbCommPort;
#ifndef _WIN32_WCE
//					COMMCONFIG		commConfig;
//					DWORD			dwCommConfigSize;
#endif

					ctmoCommPort.ReadIntervalTimeout = 1;
					ctmoCommPort.ReadTotalTimeoutConstant = 1;
					ctmoCommPort.ReadTotalTimeoutMultiplier = 1;
					ctmoCommPort.WriteTotalTimeoutConstant = 0;
					ctmoCommPort.WriteTotalTimeoutMultiplier = 0;

					bStat = SetCommTimeouts(g_hComm, &ctmoCommPort);
					if (!bStat)
					{
						dwErr = GetLastError();
					}

					dcbCommPort.DCBlength = sizeof(DCB);

					// Modembaudrate umstellen mit Befehl an Modem
					bStat = _Line_RailIni(g_hComm, iBaudRate);

					if (bStat)
					{
						bStat = GetCommState(g_hComm, &dcbCommPort);
						if (!bStat)
						{
							dwErr = GetLastError();
						}
						else
						{
							dcbCommPort.BaudRate = iBaudRate;
#ifdef _LINE_TRACE
							_stprintf(g_szMsg, _T("%s: SetCommState baud: %d\n"), __FUNC__, iBaudRate);
							OutputDebugString(g_szMsg);
#endif
							bStat = SetCommState(g_hComm, &dcbCommPort);
							if (!bStat)
							{
								dwErr = GetLastError();
#ifdef _LINE_TRACE
								_stprintf(g_szMsg, _T("%s: SetCommState error: %u\n"), __FUNC__, dwErr);
								OutputDebugString(g_szMsg);
#endif
							}
						}
					}

					bStat = SetCommMask(g_hComm, EV_RXCHAR);
					if (!bStat)
					{
						dwErr = GetLastError();
					}

					g_bModemComm = TRUE;
				}

				pVarString2 = (VARSTRING*)malloc(sizeof(VARSTRING)+200);
				memset(pVarString2, 0, sizeof(VARSTRING)+200);
				// Populate the options...
				pVarString2->dwTotalSize = sizeof(VARSTRING)+200;
				pVarString2->dwStringFormat = STRINGFORMAT_ASCII;


//				lRes = lineGetDevConfig(g_dwLine, pVarString2, _T("comm/datamodem"));
				if (lRes < 0)
				{
#ifdef _LINE_TRACE
					_stprintf(g_szMsg, _T("%s: lineGetDevConfig error: %X\n"), __FUNC__, lRes);
					OutputDebugString(g_szMsg);
#endif
				}

				CloseHandle(g_hComm);
				g_hComm = NULL;

				lineDialParams.dwDialSpeed = 0;
				lineDialParams.dwDigitDuration = 0;
				lineDialParams.dwDialPause = 0;
				lineDialParams.dwWaitForDialtone = 0;

////				lRes = lineSetMediaMode(g_hCall, LINEMEDIAMODE_DATAMODEM);
				lRes = lineSetCallParams(g_hCall, LINEBEARERMODE_VOICE, 2400, 9600, &lineDialParams);
				lRes = lineDial(g_hCall, _T("05522713405"), 0);

				Sleep(30000);

				g_bChangeBaud = TRUE;

				lRes = lineDrop(g_hCall, NULL, 0);

				// warten bis call beendet wurde
				for (iLoop = 0 ; g_bChangeBaud && iLoop < 200; iLoop++) // max 20 Sekunden warten
				{
					Sleep(100);
				}

				lRes = lineDeallocateCall(g_hCall);
				g_hCall = NULL;

				dwSize = pVarString2->dwStringSize;
				lRes = lineSetDevConfig(g_dwLine, ((LPBYTE)pVarString2+pVarString2->dwStringOffset), dwSize, _T("comm/datamodem"));
				if (lRes < 0)
				{
#ifdef _LINE_TRACE
					_stprintf(g_szMsg, _T("%s: lineSetDevConfig error: %X\n"), __FUNC__, lRes);
					OutputDebugString(g_szMsg);
#endif
				}

				if (pVarString2)
				{
					free(pVarString2);
					pVarString2 = NULL;
				}
			}

			if (pVarString)
			{
				free(pVarString);
				pVarString = NULL;
			}

			bLoop = FALSE;
			g_dwTimeTick = GetTickCount();
			g_eState = EN_LINE_STATE_CHANGEDBAUD;
		}
	}

	return 0;
}
*/

/*
///////////////////////////////////////////////////////////////////////////////////
BOOL	Line_ChangeBaud(LPTSTR szPhoneNr, int iBaudRate)
{
	BOOL			bRet = FALSE;
	LONG			lRes;
    LINECALLPARAMS	lineCallParams;

#ifdef _LINE_TRACE
	_stprintf(g_szMsg, _T("Line_ChangeBaud: lineMakeCall\n"));
	OutputDebugString(g_szMsg);
#endif

    memset(&lineCallParams, 0, sizeof(LINECALLPARAMS));
    lineCallParams.dwTotalSize = sizeof(LINECALLPARAMS);
    lineCallParams.dwMinRate = 2400;
    lineCallParams.dwMaxRate = 9600;
    lineCallParams.dwMediaMode = LINEMEDIAMODE_DATAMODEM;

	// allow direct access to the modem in command mode
	lineCallParams.dwBearerMode = LINEBEARERMODE_PASSTHROUGH;

	// this specifies that we want to use only IDLE calls and 
	// don't want to cut into a call taht might not be IDLE (ie, in use)
	lineCallParams.dwCallParamFlags = LINECALLPARAMFLAGS_IDLE;

	// we will assume there is only one address and use it (normal mode)
//	lineCallParams.dwAddressMode = LINEADDRESSMODE_ADDRESSID;
//	lineCallParams.dwAddressID = 0;

	// unimodem ignores these values
	lineCallParams.DialParams.dwDialSpeed = 0;
	lineCallParams.DialParams.dwDigitDuration = 0;
	lineCallParams.DialParams.dwDialPause = 0;
	lineCallParams.DialParams.dwWaitForDialtone = 0;

	// we're using lineMakeCall to open a "call" but it doesn't actually
	// dial when dwBearerMode = LINEBEARERMODE_PASSTHROUGH. The
	// "passthrough" call is used to set modem parameters such as baud rate,
	// parity, data bits, etc.

	lRes = lineMakeCall(g_hLine, &g_hCall, szPhoneNr, 0, &lineCallParams);
	if(lRes < 0)
	{	//  Doh!
#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("Line_Call: lineMakeCall error: %X\n"), lRes);
		OutputDebugString(g_szMsg);
#endif
		bRet = FALSE;
	}
	else
	{
#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("Line_Call: lineMakeCall g_call %X, lRes %x\n"), g_hCall, lRes);
		OutputDebugString(g_szMsg);
#endif
		g_hChangeBaudThread = CreateThread(NULL, 0, ChangeBaudThread, (LPVOID)iBaudRate, 0, NULL);
		if (g_hChangeBaudThread != NULL)
		{
			bRet = TRUE;
		}
	}
	return bRet;
}
*/


///////////////////////////////////////////////////////////////////////////////////
DWORD WINAPI CallThread (LPVOID lpThreadParam)
{
	BOOL	bLoop = TRUE;
	LONG	lRes;


#ifdef _LINE_TRACE
	_stprintf(g_szMsg, _T("Call thread: start.\r\n"));
	OutputDebugString(g_szMsg);
#endif

	while (bLoop)
	{
		// Now we wait for notification...
		WaitForSingleObject(g_hEvReply, INFINITE);
		ResetEvent(g_hEvReply);
		if(g_dwTAPIMsg == LINECALLSTATE_CONNECTED)
		{
			VARSTRING	*pVarString;

#ifdef _LINE_TRACE
			_stprintf(g_szMsg, _T("Call thread: opening data communication.\n"));
			OutputDebugString(g_szMsg);
#endif

//			g_lState = _LINE_CONNECTED;
			pVarString = (VARSTRING*)malloc(sizeof(VARSTRING)+100);
			memset(pVarString, 0, sizeof(VARSTRING)+100);
			// Populate the options...
			pVarString->dwTotalSize = sizeof(VARSTRING)+100;
			pVarString->dwStringFormat = STRINGFORMAT_ASCII;

			lRes = lineGetID(g_hLine, g_dwLine, g_hCall, LINECALLSELECT_CALL, pVarString, _T("comm/datamodem"));
			if (lRes < 0)
			{	//  Doh!
#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("Call thread: lineGetID error: %X\n"), lRes);
				OutputDebugString(g_szMsg);
#endif
			}
			else
			{
				BOOL	bStat;
				DWORD	dwErr;

				g_hComm = *((LPHANDLE) ((LPBYTE)pVarString+pVarString->dwStringOffset));

				if (g_hComm != NULL && g_hComm != INVALID_HANDLE_VALUE)
				{
					// Now we wait for notification...
					COMMTIMEOUTS	ctmoCommPort;
					DCB				dcbCommPort;

					ctmoCommPort.ReadIntervalTimeout = 1;
					ctmoCommPort.ReadTotalTimeoutConstant = 1;
					ctmoCommPort.ReadTotalTimeoutMultiplier = 1;
					ctmoCommPort.WriteTotalTimeoutConstant = 0;
					ctmoCommPort.WriteTotalTimeoutMultiplier = 0;

					bStat = SetCommTimeouts(g_hComm, &ctmoCommPort);
					if (!bStat)
					{
						dwErr = GetLastError();
					}
/*
					bStat = GetCommTimeouts(g_hComm, &ctmoCommPort);
					if (bStat)
					{
						TCHAR	szMsg[255];

						_stprintf(szMsg, _T("CommSettings ReadIntervalTimeout %d, ReadTotalTimeoutConstant %d, ReadTotalTimeoutMultiplier %d\n"), 
							ctmoCommPort.ReadIntervalTimeout, ctmoCommPort.ReadTotalTimeoutConstant,
							ctmoCommPort.ReadTotalTimeoutMultiplier);
						OutputDebugString(szMsg);
					}
					else
						OutputDebugString(_T("Error getting timeouts for Modem-Interface\n"));
*/
					dcbCommPort.DCBlength = sizeof(DCB);

					bStat = GetCommState(g_hComm, &dcbCommPort);
					if (!bStat)
					{
						dwErr = GetLastError();
					}
					bStat = SetCommState(g_hComm, &dcbCommPort);
					if (!bStat)
					{
						dwErr = GetLastError();
					}

					bStat = SetCommMask(g_hComm, EV_RXCHAR);
					if (!bStat)
					{
						dwErr = GetLastError();
					}

#ifndef _WIN32_WCE
					bStat = PurgeComm(g_hComm, PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_RXABORT);
					if (!bStat)
					{
						dwErr = GetLastError();
					}
#else
					bStat = PurgeComm(g_hComm, PURGE_TXCLEAR | PURGE_RXCLEAR);
					if (!bStat)
					{
						dwErr = GetLastError();
					}
#endif

					g_bModemComm = TRUE;
					g_dwTimeTick = GetTickCount();
					g_eState = EN_LINE_STATE_CONNECTED;
				}
			}

			free(pVarString);
			bLoop = FALSE;
		}
	}

	return 0;
}


///////////////////////////////////////////////////////////////////////////////////
BOOL	Line_Call(LPTSTR szPhoneNr)
{
	BOOL			bRet = FALSE;
	LONG			lRes;
    LINECALLPARAMS	lineCallParams;

#ifdef _LINE_TRACE
	_stprintf(g_szMsg, _T("Line_Call: lineMakeCall\n"));
	OutputDebugString(g_szMsg);
#endif

    memset(&lineCallParams, 0, sizeof(LINECALLPARAMS));
    lineCallParams.dwTotalSize = sizeof(LINECALLPARAMS);
    lineCallParams.dwMinRate = 2400;
    lineCallParams.dwMaxRate = 9600;
    lineCallParams.dwMediaMode = LINEMEDIAMODE_DATAMODEM;

	// test 
	lineCallParams.dwBearerMode = LINEBEARERMODE_VOICE; //LINEBEARERMODE_DATA;
	lineCallParams.dwAddressMode = LINEADDRESSMODE_DIALABLEADDR;

	g_bChangeBaud = FALSE;
	lRes = lineMakeCall(g_hLine, &g_hCall, szPhoneNr, 0, &lineCallParams);
	if(lRes < 0)
	{	//  Doh!
#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("Line_Call: lineMakeCall error: %X\n"), lRes);
		OutputDebugString(g_szMsg);
#endif
		bRet = FALSE;
	}
	else
	{
#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("Line_Call: lineMakeCall g_hCall %X, lRes %X\n"), g_hCall, lRes);
		OutputDebugString(g_szMsg);
#endif
		g_hCallThread = CreateThread(NULL, 0, CallThread, NULL, 0, NULL);
		if (g_hCallThread != NULL)
		{
			bRet = TRUE;
			g_eState = EN_LINE_STATE_CONNECTING;
		}
	}
	return bRet;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL	Line_Receive(void *pvBufIn, DWORD dwMaxLen, DWORD *dwReadIn)
{
	BOOL	bRet = FALSE;
	BOOL	bStat = FALSE;
	DWORD	dwRead = 0;
#ifndef _WIN32_WCE
	DWORD	dwCount;
#endif

#ifdef _WIN32_WCE

	bStat = ReadFile(g_hComm, pvBufIn, dwMaxLen, &dwRead, NULL);
	if (bStat && dwRead > 0)
	{
		bRet = TRUE;
		*dwReadIn = dwRead;
		g_dwTimeTick = GetTickCount();
	}
	else
	{
		*dwReadIn = 0;
	}

#else

	if (g_iReading == 1)
	{
		return FALSE; // waiting for event
	}

	if (g_iReading == 0)
	{
		if (GetOverlappedResult(g_hComm, &g_ovIn, &dwCount, FALSE))
		{
			*dwReadIn = dwCount;
			g_iReading = -1;
			return TRUE;
		}
//		*dwReadIn = g_ovIn.Offset;
	}

	bStat = ReadFile(g_hComm, pvBufIn, dwMaxLen, &dwRead, &g_ovIn);
	if (bStat && dwRead > 0)
	{
		bRet = TRUE;
		*dwReadIn = dwRead;
		g_dwTimeTick = GetTickCount();
	}
	if (!bStat)
	{
		if (ERROR_IO_PENDING == GetLastError())
		{
			g_iReading = 1;
		}
		bRet = FALSE;
#ifdef _LINE_TRACE
//		_stprintf(g_szMsg, _T("Line_Read: error %d\n"), GetLastError());
//		OutputDebugString(g_szMsg);
#endif
	}

#endif

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL	Line_Send(void *pvBufOut, DWORD dwLenOut, DWORD *pdwWrittenOut)
{
	BOOL	bRet = FALSE;
#ifndef _WIN32_WCE
	DWORD	dwWritten;
#endif
	BOOL	bStat = FALSE;

#ifdef _WIN32_WCE
/*
	WriteFile(g_hComm, pvBufOut, dwLenOut, &dwWritten, NULL);
	if (dwLenOut == dwWritten)
	{
		bRet = TRUE;
	}
	*dwWrittenOut = dwWritten;
*/
	bRet = WriterListAdd(pvBufOut, dwLenOut, pdwWrittenOut);

#else

	bStat = WriteFile(g_hComm, pvBufOut, dwLenOut, &dwWritten, &g_ovOut);
	if (dwLenOut == dwWritten)
	{
		bRet = TRUE;
	}
	
	if (!bStat)
	{
#ifdef _LINE_TRACE
//		_stprintf(g_szMsg, _T("Line_Write: error %d\n"), GetLastError());
//		OutputDebugString(g_szMsg);
#endif
	}
	*pdwWrittenOut = dwWritten;

#endif

	return bRet;
}
///////////////////////////////////////////////////////////////////////////////////
BOOL Line_Int_GetLine(DWORD dwDev, LPTSTR szName, char* szcRegPath)
{
	DWORD			dwAPIVersion;
	DWORD			dwResult = 0xFFFFFFFF;
	LINEEXTENSIONID ExtentionId;
	LINEDEVCAPS		*lpDevCaps;
	LONG			lRes;
	BOOL			bDone;
	BOOL			bRet = FALSE;


	lRes = lineNegotiateAPIVersion(g_hLineApp, dwDev, _LINE_TAPI_CURRENT_VERSION, _LINE_TAPI_CURRENT_VERSION, 
									&dwAPIVersion, &ExtentionId);
	if(lRes != 0)  //Oops!
	{
#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("Line_Int_GetLine: lineNegotiateAPIVersion error %X\n"), lRes);
		OutputDebugString(g_szMsg);
#endif
		bRet = FALSE;
	}
	else
		{
		lpDevCaps = (LINEDEVCAPS *)malloc(sizeof(LINEDEVCAPS)+1000);	// Allocate a little extra memory...
		if(lpDevCaps == NULL) return dwResult;	// No luck allocating memory...

		memset(lpDevCaps, 0, sizeof(LINEDEVCAPS)+1000);
		lpDevCaps->dwTotalSize = sizeof(LINEDEVCAPS)+1000;
		bDone = FALSE;
		do // Go around and keep trying until there's enough memory
		{
			lRes = lineGetDevCaps(g_hLineApp, dwDev, dwAPIVersion, 0, lpDevCaps);
			if(lRes != 0)  //Oops!
			{
#ifdef _LINE_TRACE
				_stprintf(g_szMsg, _T("Line_Int_GetLine: lineGetDevCaps error %X\n"), lRes);
				OutputDebugString(g_szMsg);
#endif
				free(lpDevCaps);
				break;
			}
			if(lpDevCaps->dwNeededSize > lpDevCaps->dwTotalSize)
			{
				// Reallocate for dwNeededSize
				lpDevCaps = (LINEDEVCAPS *)realloc(lpDevCaps, lpDevCaps->dwNeededSize);
			}
			else 
			{
				bDone = TRUE;
			}
		}
		while (!bDone);

#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("Line_Int_GetLine: Line %d is %s.\r\n"), dwDev, (LPBYTE)((DWORD)lpDevCaps+(DWORD)lpDevCaps->dwLineNameOffset));
		OutputDebugString(g_szMsg);
#endif
		if(lpDevCaps->dwMediaModes & g_dwMediaMode)
		{
			DWORD	dwBytesPerChar;
			DWORD	dwOffset;
#ifdef _LINE_TRACE
			_stprintf(g_szMsg, _T("Line_Int_GetLine: Line device %d selected.\r\n"), dwDev);
			OutputDebugString(g_szMsg);
#endif
			_tcscpy(szName, (TCHAR*)((DWORD)lpDevCaps+(DWORD)lpDevCaps->dwLineNameOffset));

			dwBytesPerChar = *((DWORD*)((DWORD)lpDevCaps+(DWORD)lpDevCaps->dwDevSpecificOffset));
			dwOffset = *((DWORD*)((DWORD)lpDevCaps+(DWORD)lpDevCaps->dwDevSpecificOffset + sizeof(DWORD)));

			strcpy(szcRegPath, (char*)((DWORD)lpDevCaps+(DWORD)lpDevCaps->dwDevSpecificOffset+dwOffset));

			bRet = TRUE;
		}
		else 
		{
#ifdef _LINE_TRACE
			_stprintf(g_szMsg, _T("Line_Int_GetLine: Line device %d rejected.\r\n"), dwDev);
			OutputDebugString(g_szMsg);
#endif
			bRet = FALSE;
		}

		free(lpDevCaps);
	}

	return bRet;
}


///////////////////////////////////////////////////////////////////////////////////
BOOL	Line_GetFirstName(LPTSTR szName)
{
	BOOL	bRet = FALSE;

	DWORD	dwDev;
	BOOL	bFound = FALSE;
	char	szRegPath[256];

	g_dwActDev = 0;
	for (dwDev = g_dwActDev; !bFound && dwDev < g_dwNumDevs; dwDev++)
	{
		if(Line_Int_GetLine(dwDev, szName, szRegPath))
		{
			bFound = TRUE;
			bRet = TRUE;
			g_dwActDev = dwDev;
		}
	}
	if (!bRet)
	{
		szName[0] = '\0';
	}

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL	Line_GetNextName(LPTSTR szName)
{
	BOOL	bRet = FALSE;
	BOOL	bFound = FALSE;
	DWORD	dwDev;
	char	szRegPath[256];

	g_dwActDev++;
	for (dwDev = g_dwActDev; !bFound && dwDev < g_dwNumDevs; dwDev++)
	{
		if(Line_Int_GetLine(dwDev, szName, szRegPath))
		{
			bFound = TRUE;
			bRet = TRUE;
			g_dwActDev = dwDev;
		}
	}
	if (!bRet)
	{
		szName[0] = '\0';
	}

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL	Line_GetLine4Name(LPTSTR szName, DWORD *pdwLine, BYTE *pbyPort, DWORD *pdwBaud)
{
	BOOL	bRet = FALSE;
	DWORD	dwDev;
	TCHAR	szBuf[256];
	BOOL	bFound = FALSE;
	char	szRegPath[256];

	for (dwDev = 0; !bFound && dwDev < g_dwNumDevs; dwDev++)
	{
		if(Line_Int_GetLine(dwDev, szBuf, szRegPath))
		{
			if (!_tcscmp(szName, szBuf))
			{
				bFound = TRUE;
				bRet = TRUE;
				*pdwLine = dwDev;
			}
		}
	}

	if (bFound)
	{
		REGSAM	RegSam;
		HKEY	hKey;
		WCHAR	*pszRegPath;
		TCHAR	szCom[25];
		DWORD	dwConf[7];
		long	lCom;
		DWORD	dwType;
		DWORD	dwBaud;

		*pbyPort = 0;


#ifdef _WIN32_WCE
		RegSam = KEY_READ;
#else
		RegSam = KEY_READ;
#endif
		pszRegPath = CharString2UnicodeString(szRegPath);
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, pszRegPath, 0, RegSam, &hKey) == ERROR_SUCCESS)
		{
			long	lStat;
			lCom = 20;

			lStat = RegQueryValueEx(hKey, _T("AttachedTo"), NULL, NULL, (unsigned char*)szCom, &lCom);
			if (lStat == ERROR_SUCCESS)
			{
				*pbyPort = (BYTE)_tcstol(&szCom[3], NULL, 10);
			}
			lCom = sizeof(dwConf);
			lStat = RegQueryValueEx(hKey, _T("DCB"), NULL, &dwType, (LPBYTE)dwConf, &lCom);
			if (lStat == ERROR_SUCCESS && dwType == REG_BINARY)
			{
				*pdwBaud = dwConf[1];
			}
			else
			{
				lStat = RegQueryValueEx(hKey, _T("PrimaryBaud"), NULL, &dwType, (LPBYTE)&dwBaud, &lCom);
				if (lStat == ERROR_SUCCESS && dwType == REG_DWORD)
				{
					*pdwBaud = dwBaud;
				}
			}
		}
		free(pszRegPath);
	}

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD	Line_GetLastTickCount(void)
{
	return g_dwTimeTick;
}

///////////////////////////////////////////////////////////////////////////////////
EN_LINE_STATE	Line_GetState(EN_LINE_ERROR *peLineError)
{
	if (peLineError)
	{
		*peLineError = g_eError;
	}

	return g_eState;
}

////////////////////////////////////////////////////////////////
//	Status der Verbindung
WORD	Line_GetSignalLevel(void)
{
	WORD			wLevel;
	LINEDEVSTATUS	LineDevState;
	LONG			lRes;

	if (g_hCall)
	{
		LineDevState.dwTotalSize = sizeof(LineDevState);
		lRes = lineGetLineDevStatus(g_hLine, &LineDevState);
		if(lRes != 0)
		{	
	#ifdef _LINE_TRACE
			_stprintf(g_szMsg, _T("Line_Open: lineGetLineDevStatus error %X\r\n"), lRes);
			OutputDebugString(g_szMsg);
	#endif
			wLevel = 0;
		}
		else
		{
	#ifdef _LINE_TRACE
			_stprintf(g_szMsg, _T("Line_Open: dwNumOpens %d, dwNumActiveCalls %d, dwNumOnHoldCalls %d, dwNumOnHoldPendCalls %d\n"), 
				LineDevState.dwNumOpens, LineDevState.dwNumActiveCalls, LineDevState.dwNumOnHoldCalls, LineDevState.dwNumOnHoldPendCalls);
	//		OutputDebugString(g_szMsg);
			_stprintf(g_szMsg, _T("Line_Open: dwRingMode %d, dwSignalLevel %d, dwRoamMode %d, dwDevStatusFlags %d\n"), 
				LineDevState.dwRingMode, LineDevState.dwSignalLevel, LineDevState.dwRoamMode, LineDevState.dwDevStatusFlags);
	//		OutputDebugString(g_szMsg);
	#endif
			wLevel = (WORD)LineDevState.dwSignalLevel;

		}
	}
	else
	{
		wLevel = 0;
	}

	return wLevel;
}

WCHAR* CharString2UnicodeString(char *szStr)
{
	WCHAR	*szuBuf;
	int		iConvStatus;
	int		iLen, i;

	iLen = 0;
	for (i = 0; szStr[i] != 0x00; i++)
		iLen++;

	iLen = strlen(szStr);
	szuBuf = malloc(sizeof(WCHAR) * (iLen + 1));

	iConvStatus = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1, szuBuf, iLen+1);
	if (iConvStatus != iLen+1)
	{
		szuBuf[0] = '\0';
	}

	return szuBuf;
}


// --------------------------------------------------------------------------
//  Funktion:       _Line_ChangeBaud()
//  Beschreibung :  Steuerung für Thread der versucht die Baudrate der Serielle Schnittstelle zu ändern
//  Parameter :     Schnittstellen-Nummer (0..9), neue Baudrate für Schnittstelle
//  Returnwert :    0	- in Bearbeitung
//					-2	- sonstiger Fehler
//					-1	- Problem mit Kommunikation über die serielle Schnittstelle
//					1	- OK

int	_Line_ChangeBaud(UCHAR ucCom, DWORD dwBaud2Rail)
{
	int		iRet = 0;

	g_dwBaud2Rail = dwBaud2Rail;

	if (g_hThreadLineBaud2Rail == NULL)
	{
		DWORD	dwThreadId;
		g_hThreadLineBaud2Rail = CreateThread(NULL, 0, _Line_ChangeBaud_Thread, (LPVOID)ucCom, 0, &dwThreadId);
	}
	else
	{
		DWORD	dwRes;

		dwRes = WaitForSingleObject(g_hThreadLineBaud2Rail, 0);
		if (dwRes == WAIT_OBJECT_0)
		{
			// Schliessen beendet
			switch (g_iStatBaud2RailThread)
			{
			case 0:
				iRet = 1;	// Ok
				break;

			case -1:
			case -3:
			case -4:
			case -5:
			case -6:
			case -7:
			case -8:
			case -9:
				iRet = g_iStatBaud2RailThread;
				break;

			default:
				iRet = -2; // sonstiger Fehler
			}

			CloseHandle(g_hThreadLineBaud2Rail);
			g_hThreadLineBaud2Rail = NULL;
		}
	}

	return iRet;
}

///////////////////////////////////////////////////////////////////////////////////
#undef __FUNC__
#define __FUNC__ TEXT("_Line_ChangeBaud_Thread")
DWORD WINAPI _Line_ChangeBaud_Thread (LPVOID lpThreadParam)
{
	int		iBaudRate;
	UCHAR	ucCom;
	BOOL	bStat;
	DWORD	dwOutCount, dwInCount;


#ifdef _LINE_TRACE
	_stprintf(g_szMsg, _T("%s: start. for baud %d\r\n"), __FUNC__, g_dwBaud2Rail);
	OutputDebugString(g_szMsg);
#endif
	g_iStatBaud2RailThread = -1;

	ucCom = (int)lpThreadParam;
	iBaudRate = g_dwBaud2Rail;

	ComClear(ucCom);
	ComGetStatus(ucCom, &dwOutCount, &dwInCount);

	// Modembaudrate umstellen mit Befehl an Modem
	bStat = _Line_RailIni(ucCom, iBaudRate);

	if (!bStat) // mehrmals versuchen, da es nicht immer gleich funktioniert
	{
		ComClear(ucCom);
		ComGetStatus(ucCom, &dwOutCount, &dwInCount);

		bStat = _Line_RailIni(ucCom, iBaudRate);
	}

	if (bStat)
	{

		bStat = ComChangeBaud(ucCom, iBaudRate);

		g_iStatBaud2RailThread = 0;
	}
//	bStat = SetCommMask(g_hComm, EV_RXCHAR);


#ifdef _LINE_TRACE
	_stprintf(g_szMsg, _T("%s: end.\r\n"), __FUNC__);
	OutputDebugString(g_szMsg);
#endif

	return 0;
}


// --------------------------------------------------------------------------
//  Funktion:       Line_ClearCom()
//  Beschreibung :  Serielle Schnittstelle zurücksetzten besonders wenn DCD noch aktiv ist
//  Parameter :     Schnittstellen-Nummer (0..9), neue Baudrate für Schnittstelle
//  Returnwert :    0	- in Bearbeitung
//					-1	- Problem mit Kommunikation über die serielle Schnittstelle
//					1	- OK

int	Line_ClearCom(UCHAR ucCom, DWORD dwBaud)
{
	int		iRet = 0;

	if (g_hThreadClearCom == NULL)
	{
		DWORD	dwThreadId;
		g_dwBaudClearCom = dwBaud;
		g_hThreadClearCom = CreateThread(NULL, 0, _Line_ClearCom_Thread, (LPVOID)ucCom, 0, &dwThreadId);
	}
	else
	{
		DWORD	dwRes;

		dwRes = WaitForSingleObject(g_hThreadClearCom, 0);
		if (dwRes == WAIT_OBJECT_0)
		{
			iRet = 1;	// Ok

			CloseHandle(g_hThreadLineBaud2Rail);
			g_hThreadClearCom = NULL;
		}
	}

	return iRet;
}

BOOL	Line_ClearComState(void)
{
	if (g_hThreadClearCom == NULL)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

///////////////////////////////////////////////////////////////////////////////////
#undef __FUNC__
#define __FUNC__ TEXT("_Line_ClearCom_Thread")
DWORD WINAPI _Line_ClearCom_Thread(LPVOID lpThreadParam)
{
	UCHAR	ucCom;
	DWORD	dwOutCount, dwInCount;
	int		iOpen, iClose, iLoop;
	DWORD	dwWriteLen, dwReadLen;
	CHAR	szWriteBuf[512];
	CHAR	szReadBuf[1024];
	DWORD	dwSendStat;
	DWORD	dwLen;
	BOOL	bFound;
	DWORD	dwReadStat;
	CHAR	szComp[] = "OK";
	DWORD	dwCompLen;
	DWORD	dwPos;
	int		iComp;
	BOOL	bReady	= FALSE;

#ifdef _LINE_TRACE
	_stprintf(g_szMsg, _T("%s: start\r\n"), __FUNC__);
	OutputDebugString(g_szMsg);
#endif
	ucCom = (int)lpThreadParam;

	ComActivateDTR(ucCom, TRUE);
	Sleep(400);
	ComActivateDTR(ucCom, FALSE);
	Sleep(400);
	ComActivateDTR(ucCom, TRUE);
	Sleep(400);

	// serielle Schnittstelle öffnen
	iOpen = 0;
	for (iLoop = 0; iLoop < 1000 && iOpen == 0; iLoop++)
	{
		iOpen = ComOpen(ucCom, 
					   1,			// 0..RS232 ohne HS, 1..RS232 mit HS
					   g_dwBaudClearCom,		// 300 - 115200
					   8,			// 4 - 8
					   1,			// 1, 2
					   0,			// 0..no, 1..ungerade, 2..gerade
					   1024		// Puffergrösse Ein/Aus in Bytes
			);
		if (iOpen != 1)
		{
			Sleep(10);
		}
	}
	if (iOpen != 1)
	{
#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("%s: open com error!\r\n"), __FUNC__);
		OutputDebugString(g_szMsg);
#endif
		ComActivateDTR(ucCom, FALSE);
		return 1;
	}


	// datenmodus unterbrechen
	ComClear(ucCom);
	ComGetStatus(ucCom, &dwOutCount, &dwInCount);

	sprintf(szWriteBuf, "+++");
	dwWriteLen = strlen(szWriteBuf);
	dwSendStat = ComSend(ucCom, szWriteBuf, dwWriteLen, &dwLen);
	if (dwSendStat != 0 || dwLen != dwWriteLen)
	{
#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("%s: pos1\r\n"), __FUNC__);
		OutputDebugString(g_szMsg);
#endif
		ComActivateDTR(ucCom, FALSE);
		return 1;
	}

	bFound = FALSE;
	dwReadLen = 0;
	dwPos = 0;
	dwCompLen = strlen(szComp);
	for (iLoop = 0; iOpen && !bFound && iLoop < 200; iLoop++) // 500ms warten
	{
		dwReadStat = ComReceive(ucCom, &szReadBuf[dwReadLen], 1024-dwReadLen, &dwLen);
		if (dwReadStat != 0xFFFFFFFF && dwLen > 0)
		{
			dwReadLen += dwLen;
			szReadBuf[dwReadLen] = '\0';
			if (!bReady && dwReadLen - dwPos >= dwCompLen)
			{
				iComp = strcspn(szReadBuf, "O");
				if (iComp > 0)
				{
					if (!strncmp(&szReadBuf[iComp], szComp, strlen(szComp)))
					{
						bReady = TRUE;
					}
				}
			}

			iComp = strcspn(szReadBuf, "OE");
			while (iComp > 0 && iComp < (int)dwReadLen && !bFound && iLoop < 200)
			{
				iLoop++;
				if (!strncmp(&szReadBuf[iComp], "OK\r\n", strlen("OK\r\n")) ||
					!strncmp(&szReadBuf[iComp], "ERROR\r\n", strlen("ERROR\r\n"))
				   )
				{
					bFound = TRUE;
#ifdef _LINE_TRACE
					_stprintf(g_szMsg, _T("%s: +++ OK\r\n"), __FUNC__);
					OutputDebugString(g_szMsg);
#endif
				}
				else
				{
					iComp += strcspn(&szReadBuf[iComp+1], "OE");
					iComp++;
				}
			}
		}

		if (!bFound)
		{
			Sleep(10);
		}
	}

	Sleep(1100);

	ComClear(ucCom);
	ComGetStatus(ucCom, &dwOutCount, &dwInCount);

	sprintf(szWriteBuf, "+++\r\n");
	dwWriteLen = strlen(szWriteBuf);
	dwSendStat = ComSend(ucCom, szWriteBuf, dwWriteLen, &dwLen);
	if (dwSendStat != 0 || dwLen != dwWriteLen)
	{
#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("%s: pos1\r\n"), __FUNC__);
		OutputDebugString(g_szMsg);
#endif
		ComActivateDTR(ucCom, FALSE);
		return 1;
	}

	bFound = FALSE;
	dwReadLen = 0;
	dwPos = 0;
	dwCompLen = strlen(szComp);
	for (iLoop = 0; iOpen && !bFound && iLoop < 200; iLoop++) // 500ms warten
	{
		dwReadStat = ComReceive(ucCom, &szReadBuf[dwReadLen], 1024-dwReadLen, &dwLen);
		if (dwReadStat != 0xFFFFFFFF && dwLen > 0)
		{
			dwReadLen += dwLen;
			szReadBuf[dwReadLen] = '\0';
			if (!bReady && dwReadLen - dwPos >= dwCompLen)
			{
				iComp = strcspn(szReadBuf, "O");
				if (iComp > 0)
				{
					if (!strncmp(&szReadBuf[iComp], szComp, strlen(szComp)))
					{
						bReady = TRUE;
					}
				}
			}

			iComp = strcspn(szReadBuf, "OE");
			while (iComp > 0 && iComp < (int)dwReadLen && !bFound && iLoop < 200)
			{
				iLoop++;
				if (!strncmp(&szReadBuf[iComp], "OK\r\n", strlen("OK\r\n")) ||
					!strncmp(&szReadBuf[iComp], "ERROR\r\n", strlen("ERROR\r\n"))
				   )
				{
					bFound = TRUE;
#ifdef _LINE_TRACE
					_stprintf(g_szMsg, _T("%s: ATH OK\r\n"), __FUNC__);
					OutputDebugString(g_szMsg);
#endif
				}
				else
				{
					iComp += strcspn(&szReadBuf[iComp+1], "OE");
					iComp++;
				}
			}
		}

		if (!bFound)
		{
			Sleep(10);
		}
	}

	ComClear(ucCom);
	ComGetStatus(ucCom, &dwOutCount, &dwInCount);

	sprintf(szWriteBuf, "AT&F0\r\n");
	dwWriteLen = strlen(szWriteBuf);
	dwSendStat = ComSend(ucCom, szWriteBuf, dwWriteLen, &dwLen);
	if (dwSendStat != 0 || dwLen != dwWriteLen)
	{
#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("%s: pos2\r\n"), __FUNC__);
		OutputDebugString(g_szMsg);
#endif
		ComActivateDTR(ucCom, FALSE);
		return 1;
	}

	bFound = FALSE;
	dwReadLen = 0;
	dwPos = 0;
	dwCompLen = strlen(szComp);
	for (iLoop = 0; iOpen && !bFound && iLoop < 200; iLoop++) // 500ms warten
	{
		dwReadStat = ComReceive(ucCom, &szReadBuf[dwReadLen], 1024-dwReadLen, &dwLen);
		if (dwReadStat != 0xFFFFFFFF && dwLen > 0)
		{
			dwReadLen += dwLen;
			szReadBuf[dwReadLen] = '\0';
			if (!bReady && dwReadLen - dwPos >= dwCompLen)
			{
				iComp = strcspn(szReadBuf, "O");
				if (iComp > 0)
				{
					if (!strncmp(&szReadBuf[iComp], szComp, strlen(szComp)))
					{
						bReady = TRUE;
					}
				}
			}

			iComp = strcspn(szReadBuf, "OE");
			while (iComp > 0 && iComp < (int)dwReadLen && !bFound && iLoop < 200)
			{
				iLoop++;
				if (!strncmp(&szReadBuf[iComp], "OK\r\n", strlen("OK\r\n")) ||
					!strncmp(&szReadBuf[iComp], "ERROR\r\n", strlen("ERROR\r\n"))
				   )
				{
					bFound = TRUE;
#ifdef _LINE_TRACE
					_stprintf(g_szMsg, _T("%s: AT&F OK\r\n"), __FUNC__);
					OutputDebugString(g_szMsg);
#endif
				}
				else
				{
					iComp += strcspn(&szReadBuf[iComp+1], "OE");
					iComp++;
				}
			}
		}

		if (!bFound)
		{
			Sleep(10);
		}
	}

	if (iOpen)
	{
		// serielle Schnittstelle schliessen
		iClose = 0;
		for (iLoop = 0; iLoop < 1000 && iClose == 0; iLoop++)
		{
			iClose = ComClose(ucCom);
			if (iClose != 1)
			{
				Sleep(10);
			}
		}
		if (iClose != 1)
		{
	#ifdef _LINE_TRACE
			_stprintf(g_szMsg, _T("%s: close com error!\r\n"), __FUNC__);
			OutputDebugString(g_szMsg);
	#endif
			ComActivateDTR(ucCom, FALSE);
			return 1;
		}
	}

	ComActivateDTR(ucCom, FALSE);
	Sleep(300);

#ifdef _LINE_TRACE
	_stprintf(g_szMsg, _T("%s: end.\r\n"), __FUNC__);
	OutputDebugString(g_szMsg);
#endif

	return 0;
}


