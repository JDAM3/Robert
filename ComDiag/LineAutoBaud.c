// =========================================================================
//                      (C) Kinz Elektronik
//
// Produkt:      ComDiag
//
// Modulname:    LineAutoBaud.c
//
// Funktion:     Diese Datei enthält alle erforderlichen Funktion um 
//				 die Geschwindigkeit der Seriellen Schnittstelle eines Modems
//				 festzustellen
//
// Autor:
//      Name:    R. Bernecker
//      Datum:   30.9.2002
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
//#include <tapi.h>
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

//#define _LINE_TRACE

// -------------------------------------------------------------------------
// Globals

extern BOOL			g_bGsmRail;
#ifdef _WIN32_WCE
extern BOOL			g_bGsmRailDTRReset;
#endif


// -------------------------------------------------------------------------
// Statics

static int			g_iStatCheckBaudThread = 0;			// Status des check baud thread
static int			g_iStatGsmRailWaitThread = 0;		// Status des GsmRailWait thread
static HANDLE		g_hThreadLineCheckBaud = NULL;		// check baud thread
static HANDLE		g_hThreadGsmRailWait = NULL;		// GsmRailWait thread
static DWORD		g_dwBaud = 0xFFFFFFFF;				// gefundene Baudrate
static DWORD		g_dwBaudSet = 0xFFFFFFFF;			// gefundene Baudrate
static BOOL			g_bBaudFindStop = FALSE;


#ifdef _LINE_TRACE

static TCHAR		g_szMsg[255];
#endif

// -------------------------------------------------------------------------
// Prototypes
int	_Line_CheckBaud(UCHAR ucCom, DWORD *pdwBaud, DWORD dwBaudSet);
int _Line_CheckBaudSingle(UCHAR ucCom, DWORD dwBaud);
DWORD WINAPI _Line_CheckBaud_Thread(LPVOID lParam);
BOOL _Line_SetBaud(UCHAR ucCom, DWORD dwBaudInterface, DWORD dwBaud2Set);
DWORD WINAPI _Line_GsmRailWait_Thread(LPVOID lParam);


//////////////////////////////////////////////////////////////////////////
// internal functions

// --------------------------------------------------------------------------
//  Funktion:       _Line_CheckBaud()
//  Beschreibung :  Steuerung für Thread der versucht die Baudrate der Seriellen Schnittstelle am Handy festzustellen
//  Parameter :     Schnittstellen-Nummer (0..9)
//  Returnwert :    0	- in Bearbeitung
//					-3	- Baudrate konnte nicht gefunden werden
//					-2	- sonstiger Fehler
//					-1	- Problem mit Kommunikation über die serielle Schnittstelle
//					1	- OK

int	_Line_CheckBaud(UCHAR ucCom, DWORD *pdwBaud, DWORD dwBaudSet)
{
	int		iRet = 0;

	g_dwBaudSet = dwBaudSet;

	if (g_hThreadLineCheckBaud == NULL)
	{
		DWORD	dwThreadId;
		g_bBaudFindStop = FALSE;
		g_hThreadLineCheckBaud = CreateThread(NULL, 0, _Line_CheckBaud_Thread, (LPVOID)ucCom, 0, &dwThreadId);
	}
	else
	{
		DWORD	dwRes;

		dwRes = WaitForSingleObject(g_hThreadLineCheckBaud, 0);
		if (dwRes == WAIT_OBJECT_0)
		{
			// Schliessen beendet
			switch (g_iStatCheckBaudThread)
			{
			case 0:
				iRet = 1;	// Ok
				*pdwBaud = g_dwBaud;
				break;

			case -1:
				iRet = g_iStatCheckBaudThread;
				break;

			default:
				iRet = -2; // sonstiger Fehler
			}

			CloseHandle(g_hThreadLineCheckBaud);
			g_hThreadLineCheckBaud = NULL;
		}
	}

	return iRet;
}

// --------------------------------------------------------------------------
//  Funktion:       _Line_CheckBaud_Thread()
//  Beschreibung :  versucht die Baudrate für die Serielle Schnittstelle zu ermitteln
//  Parameter :     Schnittstellen-Nummer (0..9), 
//  Returnwert :    0
//	Status in g_iStatSetPinThread:
//					-1	- Problem mit schreiben auf Com
//					0	- Baud found

DWORD WINAPI _Line_CheckBaud_Thread(LPVOID lParam)
{
	UCHAR	ucCom;
	DWORD	dwBaudCheck[] = {4800, 19200, 9600, 57600, 19200, 300, 600, 1200, 2400, 9600, 14400, 19200, 28800, 57600, 38400, 115200, 19200};
	BOOL	bFound = FALSE;
	int		iLoop;
	BOOL	bBaudOk = FALSE;
	int		iBaudStat;

#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("_Line_CheckBaud_Thread: start!\r\n"));
		OutputDebugString(g_szMsg);
#endif
	ucCom = (UCHAR)lParam;
	g_iStatCheckBaudThread	= -1;

	if(g_bGsmRail)
	{
#ifdef _WIN32_WCE
		g_bGsmRailDTRReset = TRUE;
#endif
		ComActivateDTR(ucCom, FALSE);
		OutputDebugString(_T("."));
		Sleep(5000);
		ComActivateDTR(ucCom, TRUE);
		Sleep(30000);
#ifdef _WIN32_WCE
		g_bGsmRailDTRReset = FALSE;
#endif
	}
	else
	{
#ifdef _WIN32_WCE
		g_bGsmRailDTRReset = TRUE;
#endif
		ComActivateDTR(ucCom, FALSE);
		Sleep(400);
		ComActivateDTR(ucCom, TRUE);
		Sleep(300);
#ifdef _WIN32_WCE
		g_bGsmRailDTRReset = FALSE;
#endif
	}

	iBaudStat = _Line_CheckBaudSingle(ucCom, g_dwBaudSet);
	if (iBaudStat == 1)
	{
		bBaudOk = TRUE;
		g_iStatCheckBaudThread	= 0;
	}
	else if (iBaudStat == -1)
	{
		g_iStatCheckBaudThread	= -2;
		return 0;
	}
	else
	{
		bBaudOk = FALSE;
	}
#ifdef _LINE_TRACE
	_stprintf(g_szMsg, _T("_Line_CheckBaud_Thread: bBaudOk %d\r\n"), bBaudOk);
	OutputDebugString(g_szMsg);
#endif

	if (!g_bBaudFindStop && !bBaudOk)
	{
		iBaudStat = _Line_CheckBaudSingle(ucCom, g_dwBaudSet);
		if (iBaudStat == 1)
		{
			g_iStatCheckBaudThread	= 0;
			bBaudOk = TRUE;
		}
		else if (iBaudStat == -1)
		{
			g_iStatCheckBaudThread	= -2;
			return 0;
		}
		else
		{
			bBaudOk = FALSE;
		}
#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("_Line_CheckBaud_Thread: 2 bBaudOk %d\r\n"), bBaudOk);
		OutputDebugString(g_szMsg);
#endif
	}
	
	for (iLoop = 0; !g_bBaudFindStop && !bBaudOk && !bFound && iLoop < sizeof(dwBaudCheck)/sizeof(DWORD); iLoop++)
	{
#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("_Line_CheckBaud_Thread: check baud %d, loop %d\r\n"), dwBaudCheck[iLoop], iLoop);
		OutputDebugString(g_szMsg);
#endif
		iBaudStat = _Line_CheckBaudSingle(ucCom, dwBaudCheck[iLoop]);
		if (iBaudStat == 1)
		{
			bFound = TRUE;
		}
		else if (iBaudStat == -1)
		{
			g_iStatCheckBaudThread	= -2;
			return 0;
		}
		else
		{
			bFound = FALSE;
		}
		if (bFound)
		{
			g_dwBaud = dwBaudCheck[iLoop];
			g_iStatCheckBaudThread = 0;
#ifdef _LINE_TRACE
			_stprintf(g_szMsg, _T("_Line_CheckBaud_Thread: found serial baud %d\r\n"), g_dwBaud);
			OutputDebugString(g_szMsg);
#endif
		}
	}
	
	if (!g_bBaudFindStop && !bBaudOk && bFound)
	{
		_Line_SetBaud(ucCom, g_dwBaud, g_dwBaudSet);
#ifdef _LINE_TRACE
			_stprintf(g_szMsg, _T("_Line_CheckBaud_Thread: set serial baud %d with baud %d\r\n"), g_dwBaudSet, g_dwBaud);
			OutputDebugString(g_szMsg);
#endif
	}

	g_dwBaud = g_dwBaudSet;
//	g_iStatCheckBaudThread = 0;

#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("_Line_CheckBaud_Thread: end!\r\n"));
		OutputDebugString(g_szMsg);
#endif
	return 0;
}


int _Line_CheckBaudSingle(UCHAR ucCom, DWORD dwBaud)
{
	DWORD	dwSendStat;
	DWORD	dwReadStat;
	DWORD	dwReadLen;
	DWORD	dwWriteLen;
	DWORD	dwLen;
	CHAR	szReadBuf[128];
	CHAR	szWriteBuf[128];
	int		iLoop;
	BOOL	bFound;
	CHAR	szComp[] = "OK";
	DWORD	dwCompLen;
	DWORD	dwPos;
	int		iComp;
	BOOL	bReady	= FALSE;
	int		iOpen = 0;
	int		iClose = 0;
	DWORD	dwOutCount, dwInCount;


//	ComActivateDTR(ucCom, FALSE);
//	Sleep(400);
//	ComActivateDTR(ucCom, TRUE);
//	Sleep(2000);

	// serielle Schnittstelle öffnen
	iOpen = 0;
	for (iLoop = 0; iLoop < 1000 && iOpen == 0; iLoop++)
	{
		iOpen = ComOpen(ucCom, 
					   1,			// 0..RS232 ohne HS, 1..RS232 mit HS
					   dwBaud,		// 300 - 115200
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
		_stprintf(g_szMsg, _T("_Line_CheckBaudSingle: open error!\r\n"));
		OutputDebugString(g_szMsg);
#endif
		return -1;
	}

	ComClear(ucCom);
	
	ComGetStatus(ucCom, &dwOutCount, &dwInCount);

//	Sleep(5200);

	strcpy(szWriteBuf, "AT\r\n");
	dwWriteLen = strlen(szWriteBuf);
	dwSendStat = ComSend(ucCom, szWriteBuf, dwWriteLen, &dwLen);
	if (dwSendStat != 0 || dwLen != dwWriteLen)
	{
#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("_Line_CheckBaudSingle: pos1\r\n"));
		OutputDebugString(g_szMsg);
#endif
		return 0;
	}

//	Sleep(5000);

	bFound = FALSE;
	dwReadLen = 0;
	dwPos = 0;
	dwCompLen = strlen(szComp);
	for (iLoop = 0; iOpen && !bFound && iLoop < 200; iLoop++) // 1000ms warten
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
			while (iComp > 0 && iComp < (int)dwReadLen && !bFound && iLoop < 200)
			{
				iLoop++;
				if (!strncmp(&szReadBuf[iComp], "OK", strlen("OK")) ||
					!strncmp(&szReadBuf[iComp], "ERROR", strlen("ERROR"))
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
			Sleep(20);
		}
	}

	if (bFound && g_bGsmRail)
	{
		BOOL bFoundSet;

		strcpy(szWriteBuf, "ATS0=0\r\n");
		dwWriteLen = strlen(szWriteBuf);
		dwSendStat = ComSend(ucCom, szWriteBuf, dwWriteLen, &dwLen);
		if (dwSendStat != 0 || dwLen != dwWriteLen)
		{
#ifdef _LINE_TRACE
			_stprintf(g_szMsg, _T("_Line_CheckBaudSingle: set-error ATS0=0\r\n"));
			OutputDebugString(g_szMsg);
#endif
			return 0;
		}

		bFoundSet = FALSE;
		dwReadLen = 0;
		dwPos = 0;
		dwCompLen = strlen(szComp);
		for (iLoop = 0; iOpen && !bFoundSet && iLoop < 200; iLoop++) // 1000ms warten
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
				while (iComp > 0 && iComp < (int)dwReadLen && !bFoundSet && iLoop < 200)
				{
					iLoop++;
					if (!strncmp(&szReadBuf[iComp], "OK", strlen("OK")) ||
						!strncmp(&szReadBuf[iComp], "ERROR", strlen("ERROR"))
					   )
					{
						bFoundSet = TRUE;
#ifdef _LINE_TRACE
						_stprintf(g_szMsg, _T("_Line_CheckBaudSingle: set ok ATS0=0\r\n"));
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

			if (!bFoundSet)
			{
				Sleep(20);
			}
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
			_stprintf(g_szMsg, _T("_Line_CheckBaudSingle: close error!\r\n"));
			OutputDebugString(g_szMsg);
	#endif
			return 0;
		}
	}

	if (bFound)
		return 1;
	else
		return 0;
}

BOOL _Line_SetBaud(UCHAR ucCom, DWORD dwBaudInterface, DWORD dwBaud2Set)
{
	DWORD	dwSendStat;
	DWORD	dwReadStat;
	DWORD	dwReadLen;
	DWORD	dwWriteLen;
	DWORD	dwLen;
	CHAR	szReadBuf[128];
	CHAR	szWriteBuf[128];
	int		iLoop;
	BOOL	bFound;
	CHAR	szComp[] = "OK";
	DWORD	dwCompLen;
	DWORD	dwPos;
	int		iComp;
	BOOL	bReady	= FALSE;
	int		iOpen = 0;
	int		iClose = 0;


	// serielle Schnittstelle öffnen
	iOpen = 0;
	for (iLoop = 0; iLoop < 1000 && iOpen == 0; iLoop++)
	{
		iOpen = ComOpen(ucCom, 
					   1,			// 0..RS232 ohne HS, 1..RS232 mit HS
					   dwBaudInterface,		// 300 - 115200
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
		_stprintf(g_szMsg, _T("_Line_SetBaud: open error!\r\n"));
		OutputDebugString(g_szMsg);
#endif
		return FALSE;
	}

	sprintf(szWriteBuf, "AT+IPR=%d\r\n", dwBaud2Set);
	dwWriteLen = strlen(szWriteBuf);
	dwSendStat = ComSend(ucCom, szWriteBuf, dwWriteLen, &dwLen);
	if (dwSendStat != 0 || dwLen != dwWriteLen)
	{
#ifdef _LINE_TRACE
		_stprintf(g_szMsg, _T("_Line_SetBaud: pos1\r\n"));
		OutputDebugString(g_szMsg);
#endif
		return FALSE;
	}


	bFound = FALSE;
	dwReadLen = 0;
	dwPos = 0;
	dwCompLen = strlen(szComp);
	for (iLoop = 0; iOpen && !bFound && iLoop < 200; iLoop++) // 500ms warten
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
			while (iComp > 0 && iComp < (int)dwReadLen && !bFound && iLoop < 200)
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

	if (iOpen)
	{
		// serielle Schnittstelle öffnen
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
			_stprintf(g_szMsg, _T("_Line_SetBaud: close error!\r\n"));
			OutputDebugString(g_szMsg);
	#endif
			return FALSE;
		}
	}

	return bFound;
}

EN_LINE_ERROR	Line_FindBaud(BYTE byPort, DWORD *pdwBaud, DWORD dwBaudSet)
{
	EN_LINE_ERROR	eRet;
	int				iState;

	iState = _Line_CheckBaud(byPort, pdwBaud, dwBaudSet);
	switch(iState)
	{
	case 0: // running
		eRet = EN_LINE_WORKING;
		break;
	case 1:
		eRet = EN_LINE_OK;
#ifdef _LINE_TRACE
		OutputDebugString(_T("Baud found \n"));
#endif
		break;
	case -1:
		eRet = EN_LINE_ERR_MODEM;
		break;
	default:
		eRet = EN_LINE_ERR_CLOSE;
		break;
	}

	return eRet;
}

EN_LINE_ERROR	Line_FindBaudStop(void)
{
	EN_LINE_ERROR	enStatus = EN_LINE_OK;
	DWORD			dwRes;

	if (g_hThreadLineCheckBaud)
	{

		enStatus = EN_LINE_WORKING;
		g_bBaudFindStop = TRUE;

		dwRes = WaitForSingleObject(g_hThreadLineCheckBaud, 0);
		if (dwRes == WAIT_OBJECT_0)
		{
			// Schliessen beendet
			CloseHandle(g_hThreadLineCheckBaud);
			g_hThreadLineCheckBaud = NULL;

			enStatus = EN_LINE_OK;
		}
	}

	return enStatus;
}

// --------------------------------------------------------------------------
//  Funktion:       _Line_GsmRailWait()
//  Beschreibung :  Steuerung für Thread der versucht die Baudrate der Seriellen Schnittstelle am Handy festzustellen
//  Parameter :     Schnittstellen-Nummer (0..9)
//  Returnwert :    0	- in Bearbeitung
//					-3	- Baudrate konnte nicht gefunden werden
//					-2	- sonstiger Fehler
//					-1	- Problem mit Kommunikation über die serielle Schnittstelle
//					1	- OK

int	_Line_GsmRailWait(UCHAR ucCom)
{
	int		iRet = 0;

	if (g_hThreadGsmRailWait == NULL)
	{
		DWORD	dwThreadId;
		g_hThreadGsmRailWait = CreateThread(NULL, 0, _Line_GsmRailWait_Thread, (LPVOID)ucCom, 0, &dwThreadId);
	}
	else
	{
		DWORD	dwRes;

		dwRes = WaitForSingleObject(g_hThreadGsmRailWait, 0);
		if (dwRes == WAIT_OBJECT_0)
		{
			// Schliessen beendet
			switch (g_iStatGsmRailWaitThread)
			{
			case 0:
				iRet = 1;	// Ok
				break;

			case -1:
				iRet = g_iStatGsmRailWaitThread;
				break;

			default:
				iRet = -2; // sonstiger Fehler
			}

			CloseHandle(g_hThreadGsmRailWait);
			g_hThreadGsmRailWait = NULL;
		}
	}

	return iRet;
}

EN_LINE_ERROR	Line_GsmRailWait(BYTE byPort)
{
	EN_LINE_ERROR	eRet;
	int				iState;

	iState = _Line_GsmRailWait(byPort);
	switch(iState)
	{
	case 0: // running
		eRet = EN_LINE_WORKING;
		break;
	case 1:
		eRet = EN_LINE_OK;
#ifdef _LINE_TRACE
		OutputDebugString(_T("Gsm active \n"));
#endif
		break;
	case -1:
		eRet = -1;
		break;
	default:
		eRet = EN_LINE_ERR_CLOSE;
		break;
	}

	return eRet;
}

// --------------------------------------------------------------------------
//  Funktion:       _Line_GsmRailWait_Thread()
//  Beschreibung :  öffnet die serielle Schnittstelle und wartet 30 Sekunden
//  Parameter :     Schnittstellen-Nummer (0..9), 
//  Returnwert :    0
//	Status in g_iStatSetPinThread:
//					-1	- Problem mit schreiben auf Com
//					0	- Baud found

DWORD WINAPI _Line_GsmRailWait_Thread(LPVOID lParam)
{
	UCHAR	ucCom;
	int		iOpen, iClose, iLoop;
	DWORD	dwModemStatus = 0;
	BOOL	bModemFound = FALSE;

#ifdef _LINE_TRACE
	_stprintf(g_szMsg, _T("_Line_GsmRailWait_Thread: begin!\r\n"));
	OutputDebugString(g_szMsg);
#endif

	ucCom = (UCHAR) lParam;
	g_iStatGsmRailWaitThread = -1;

	// serielle Schnittstelle öffnen
	iOpen = 0;
	for (iLoop = 0; iLoop < 1000 && iOpen == 0; iLoop++)
	{
		iOpen = ComOpen(ucCom, 
					   1,			// 0..RS232 ohne HS, 1..RS232 mit HS
					   4800,		// 300 - 115200
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
		_stprintf(g_szMsg, _T("_Line_GsmRailWait_Thread: open error!\r\n"));
		OutputDebugString(g_szMsg);
#endif
		return -1;
	}

	if (iOpen)
	{
		Sleep(30000);

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
			_stprintf(g_szMsg, _T("_Line_GsmRailWait_Thread: close error!\r\n"));
			OutputDebugString(g_szMsg);
#endif
			g_iStatGsmRailWaitThread = -1;
			return 0;
		}
	}

	// serielle Schnittstelle öffnen um modem zu initialisieren
	iOpen = 0;
	for (iLoop = 0; iLoop < 1000 && iOpen == 0; iLoop++)
	{
		iOpen = ComOpen(ucCom, 
					   1,			// 0..RS232 ohne HS, 1..RS232 mit HS
					   4800,		// 300 - 115200
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
		_stprintf(g_szMsg, _T("_Line_GsmRailWait_Thread: open error!\r\n"));
		OutputDebugString(g_szMsg);
#endif
		g_iStatGsmRailWaitThread = -1;
		return 0;
	}

	if (iOpen)
	{
		DWORD	dwSendStat;
		DWORD	dwReadStat;
		DWORD	dwReadLen;
		DWORD	dwWriteLen;
		DWORD	dwLen;
		CHAR	szReadBuf[128];
		CHAR	szWriteBuf[128];
		int		iLoop;
		BOOL	bFound;
		CHAR	szComp[] = "OK";
		DWORD	dwCompLen;
		DWORD	dwPos;
		int		iComp;
		BOOL	bReady = FALSE;
		BOOL	bWrite = TRUE;

//		if (CommGetModemStatus(ucCom, &dwModemStatus))
//		{
//			if (dwModemStatus & MS_DSR_ON)
//			{
//				bModemDsr = TRUE;
//			}
//		}
//		else
//		{
//			OutputDebugString(_T("_Line_GsmRailWait_Thread: GetModemStatus error!\n"));
//		}

		// Modem Initialisierung
		sprintf(szWriteBuf, "AT&D2\r\n");
		dwWriteLen = strlen(szWriteBuf);
		dwSendStat = ComSend(ucCom, szWriteBuf, dwWriteLen, &dwLen);
		if (dwSendStat != 0 || dwLen != dwWriteLen)
		{
#ifdef _LINE_TRACE
			_stprintf(g_szMsg, _T("_Line_GsmRailWait_Thread: pos1\r\n"));
			OutputDebugString(g_szMsg);
#endif
			bWrite = FALSE;
		}


		bFound = FALSE;
		dwReadLen = 0;
		dwPos = 0;
		dwCompLen = strlen(szComp);
		for (iLoop = 0; iOpen && bWrite && !bFound && iLoop < 200; iLoop++) // 500ms warten
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
				while (iComp > 0 && iComp < (int)dwReadLen && !bFound && iLoop < 200)
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

		if (bFound)
			bModemFound = TRUE;

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
			_stprintf(g_szMsg, _T("_Line_GsmRailWait_Thread: close error!\r\n"));
			OutputDebugString(g_szMsg);
#endif
			g_iStatGsmRailWaitThread = -1;
			return 0;
		}
	}

#ifdef _LINE_TRACE
	_stprintf(g_szMsg, _T("_Line_GsmRailWait_Thread: end!\r\n"));
	OutputDebugString(g_szMsg);
#endif

	if (!bModemFound)
	{
//		TCHAR	szMsg[256];

//		_stprintf(szMsg, _T("_Line_GsmRailWait_Thread: DSR not set! %d\r\n"), dwModemStatus);
//		OutputDebugString(szMsg);
		g_iStatGsmRailWaitThread = -1;
		return 0;
	}

	g_iStatGsmRailWaitThread = 0;
	return 0;
}