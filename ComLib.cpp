// =========================================================================
//                      (C) Kinz Elektronik
//
// Produkt:      QLib
//
// Modulname:    ComLib.cpp
//
// Funktion:     COM-Bibliothek
//
// Autor:
//      Name:    F. Kinz
//      Datum:   2.8.2000
//
// Updates:
//      Version:	1.00
//      Name:		R. Bernecker
//      Datum:		23.01.2002
//      Grund:		Simmulationswerte auf Serielle Schnittstelle ausgeben
//
// Hardware:            PC-1100, PC
// Betriebsystem:       Windows-CE, Windows-NT/2000
// Compiler:            Visual C++
//
// =========================================================================

// -----------------------------------------------------------------------------
// includes

//#include <windows.h>
//#include "SerialExt.h"
#include "stdafx.h"

#include "ComLib.h"

#ifndef _WIN32_WCE

#define	UINT8	BYTE

#endif

// -----------------------------------------------------------------------------
// defines

#define COM_MAX 10

typedef struct
{
	HANDLE  hCom;
	HANDLE  hOpenThread;
	HANDLE  hCloseThread;
	int		iOpenError;
	int     iCloseError;
	BYTE  byMode;
	DWORD dwBaud;
	BYTE  byDataSize;
	BYTE  byStopBits;
	BYTE  byParity;
	DWORD dwBufferSize;
} COM_INFO;


// -----------------------------------------------------------------------------
// statics

static COM_INFO com[COM_MAX] = 
{
	{ INVALID_HANDLE_VALUE, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ INVALID_HANDLE_VALUE, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ INVALID_HANDLE_VALUE, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ INVALID_HANDLE_VALUE, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ INVALID_HANDLE_VALUE, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ INVALID_HANDLE_VALUE, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ INVALID_HANDLE_VALUE, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ INVALID_HANDLE_VALUE, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ INVALID_HANDLE_VALUE, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ INVALID_HANDLE_VALUE, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0 },
};


// --------------------------------------------------------------------------
//  Funktion:       _OpenTread()
//  Beschreibung :  ausführender Thread der Bibliotheksfunktion ComOpen
//  Parameter :     Schnittstellen-Nummer 0..9
//  Returnwert :    0 - Thread finished OK

DWORD WINAPI _xxxOpenThread(LPVOID lParam)
{
	UINT32			k;
	DCB				dcb;
	COMMTIMEOUTS	timeouts;
	TCHAR			comName[10];
	BYTE byComNr = (BYTE)lParam;

//	Sleep(1000);
	OutputDebugString(TEXT("b"));
//	OutputDebugString(TEXT("_xxxOpenThread begin\n"));


	if (com[byComNr].hCom != INVALID_HANDLE_VALUE) // schließen wenn geöffnet
	{
//		OutputDebugString(TEXT("_xxxOpenThread exit 1\n"));
		CloseHandle(com[byComNr].hCom);
		com[byComNr].hCom = INVALID_HANDLE_VALUE;
	}

	wsprintf(comName, TEXT("COM%d:"), byComNr);

	com[byComNr].hCom = CreateFile(
		comName,
		GENERIC_READ | GENERIC_WRITE,
		0,		// exklusiver Zugriff
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

//	OutputDebugString(TEXT("_xxxOpenThread pos 1\n"));

	if (com[byComNr].hCom == INVALID_HANDLE_VALUE)
	{
		com[byComNr].iOpenError = -1;
//		OutputDebugString(TEXT("_xxxOpenThread exit 2\n"));
		return 0;
	}

	// Initialise the DCB

	if (!GetCommState(com[byComNr].hCom, &dcb))
	{
		memset(&dcb, 0, sizeof(dcb)); // ensure any new fields are set to default
	}
	dcb.DCBlength			= sizeof(dcb);
	dcb.BaudRate			= com[byComNr].dwBaud; 
	dcb.fBinary				= TRUE;
	dcb.fParity				= (com[byComNr].byParity > 0) ? TRUE : FALSE;
	dcb.fOutxCtsFlow		= (com[byComNr].byMode == 1) ? TRUE : FALSE; // RS232 mit CTS Control
	dcb.fOutxDsrFlow		= FALSE;
	dcb.fDtrControl			= DTR_CONTROL_ENABLE;
	dcb.fDsrSensitivity		= FALSE;
	dcb.fTXContinueOnXoff	= TRUE;
	dcb.fOutX				= FALSE;
	dcb.fInX				= FALSE;
	dcb.fErrorChar			= FALSE;
	dcb.fNull				= FALSE;

	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	switch(com[byComNr].byMode) 
	{
		case 2: // RS422
			dcb.fRtsControl      = RTS_CONTROL_DISABLE;   // RTS = 1 -> Transmitter ein oder
			break;                                        // enspr. Lötbrücke gesetzt
		case 3: // RS485
			dcb.fRtsControl      = RTS_CONTROL_ENABLE;    // RTS = 0 -> RTS vom FIQ-Teil 
			break;                                        // verwendet
		default: // RS232
			dcb.fRtsControl      = RTS_CONTROL_HANDSHAKE; // alle Möglichkeiten
			break;
	}

	dcb.fAbortOnError		= TRUE;
	dcb.XonLim				= 0; 
	dcb.XoffLim				= 0; 
	dcb.ByteSize			= com[byComNr].byDataSize; 

	switch(com[byComNr].byParity)
	{
	default:
	case 0: // keine
		dcb.Parity = NOPARITY;
		break;
	case 1: // ungerade
		dcb.Parity = ODDPARITY;
		break;
	case 2: // gerade
		dcb.Parity = EVENPARITY;
		break;

	}
//	dcb.Parity				= com[byComNr].byParity; 

	switch(com[byComNr].byStopBits)
	{
	case 1:
	default:
		dcb.StopBits = ONESTOPBIT;
		break;
	case 2:
		dcb.StopBits = TWOSTOPBITS;
		break;
	}
//	dcb.StopBits			= com[byComNr].byStopBits;
	dcb.XonChar				= 0; 
	dcb.XoffChar			= 0; 
	dcb.ErrorChar			= 0;
	dcb.EofChar				= 0; 
	dcb.EvtChar				= 0; 

	if (!SetCommState(com[byComNr].hCom, &dcb))
	{
		CloseHandle(com[byComNr].hCom);
		com[byComNr].iOpenError = -1;
		DWORD dwErr = GetLastError();

//		OutputDebugString(TEXT("_xxxOpenThread exit 3\n"));
		return 0;
	}

//	OutputDebugString(TEXT("_xxxOpenThread pos 2\n"));

	// Grösse der Puffer setzen
	if (!SetupComm(com[byComNr].hCom, com[byComNr].dwBufferSize, com[byComNr].dwBufferSize))
	{
		CloseHandle(com[byComNr].hCom);
		com[byComNr].iOpenError = -1;
//		OutputDebugString(TEXT("_xxxOpenThread exit 4\n"));
		return 0;
	}

//	OutputDebugString(TEXT("_xxxOpenThread pos 3\n"));

	// Timeout einstellen
	k = 1;
	if (com[byComNr].dwBaud > 0)
	{
		k = 1 + 10000/com[byComNr].dwBaud;
	}
	timeouts.ReadIntervalTimeout = MAXDWORD;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = k;  // x = k * Anzahl Zeichen
	timeouts.WriteTotalTimeoutConstant = 1000; // Timeout = x  + 1000ms

	if (!SetCommTimeouts(com[byComNr].hCom, &timeouts))
	{
		CloseHandle(com[byComNr].hCom);
		com[byComNr].iOpenError = -1;
//		OutputDebugString(TEXT("_xxxOpenThread exit 5\n"));
		return 0;
	}

//	OutputDebugString(TEXT("_xxxOpenThread pos 4\n"));
#ifndef _WIN32_WCE
#else
	/*
	switch(com[byComNr].byMode) 
	{
   		case 3: // RS485
			if(!SerSetMode(com[byComNr].hCom, CTRL_MODE_RS485))
			{
				CloseHandle(com[byComNr].hCom);
				com[byComNr].iOpenError = -1;
				return 0;
			}
			break;
		default: // RS422 und RS232
			if(!SerSetMode(com[byComNr].hCom,CTRL_MODE_RS232))
			{
				CloseHandle(com[byComNr].hCom);
				com[byComNr].iOpenError = -1;
				return 0;
			}
			break;
	}
	*/
#endif
	PurgeComm(com[byComNr].hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

	OutputDebugString(TEXT("e\n"));
	return 1;
}


// --------------------------------------------------------------------------
//  Funktion:       ComOpen()
//  Beschreibung :  Schnittstelle öffnen. 
//  Parameter:      byComNr : Schnittstellen-Nummer (0..9)
//                  Schnittstellenparameter
//  Returnwert :     0  Funktion in Bearbeitung
//					 1  Funktion erfolgreich abgeschlossen
//					-1	Fehler

int ComOpen(BYTE  byComNr,		// 0 - 9 (COM0..COM9)
			BYTE  byMode,		// 0..RS232 ohne HS, 1..RS232 mit HS,
								// 2..RS422, 3..RS485
			DWORD dwBaud,		// 300 - 115200
			BYTE  byDataSize,	// 4 - 8
			BYTE  byStopBits,	// 1, 2
			BYTE  byParity,		// 0..no, 1..ungerade, 2..gerade
			DWORD dwBufferSize	// Puffergrösse Ein/Aus in Bytes
		   )
{
#ifndef _WIN32_WCE
#else
	DWORD	dwId;
#endif

	int		iRet = 0;

	if ((byComNr > (COM_MAX - 1)) || (byComNr < 0))
	{
		return -1; // Fehler
	}

	if (com[byComNr].hCloseThread != NULL)
	{
		CloseHandle(com[byComNr].hCloseThread);
		com[byComNr].hCloseThread = NULL;
	}

	com[byComNr].byMode = byMode;
	com[byComNr].dwBaud = dwBaud;
	com[byComNr].byDataSize = byDataSize;
	com[byComNr].byStopBits = byStopBits;
	com[byComNr].byParity = byParity;
	com[byComNr].dwBufferSize = dwBufferSize;

#ifndef _WIN32_WCE
	iRet = _xxxOpenThread((LPVOID)byComNr);
#else
	iRet = _xxxOpenThread((LPVOID)byComNr);
	/*
	if (com[byComNr].hOpenThread == NULL)
	{

		com[byComNr].iOpenError = 0;
		com[byComNr].hOpenThread = CreateThread(NULL, 0, _xxxOpenThread, (LPVOID)byComNr, 0, &dwId);
	}
	else
	{
		DWORD	dwRes;

		dwRes = WaitForSingleObject(com[byComNr].hOpenThread, 0);
		if (dwRes == WAIT_OBJECT_0)
		{
			// Öffnen beendet
			if (com[byComNr].iOpenError == 0)
				iRet = 1;	// Ok
			else
				iRet = -1;	// Fehler

			CloseHandle(com[byComNr].hOpenThread);
			com[byComNr].hOpenThread = NULL;
		}
	}
	*/
#endif
	if (iRet != 1)
		return -1;

	return iRet;
}


// --------------------------------------------------------------------------
//  Funktion:       _CloseTread()
//  Beschreibung :  ausführender Thread der Bibliotheksfunktion ComClose
//  Parameter :     Schnittstellen-Nummer (0..9)
//  Returnwert :    0 - Thread finished OK

DWORD WINAPI _CloseThread(LPVOID lParam)
{
	BYTE byComNr = (BYTE)lParam;

	if (com[byComNr].hCom == INVALID_HANDLE_VALUE)
	{
		com[byComNr].iCloseError = -1;
		return 0;
	}

	CloseHandle(com[byComNr].hCom);

	return 0;
}


// --------------------------------------------------------------------------
//  Funktion:       ComClose()
//  Beschreibung :  Schnittstelle schliessen
//  Parameter:      byComNr : Schnittstellen-Nummer (0..9)
//  Returnwert :     0  Funktion in Bearbeitung
//					 1  Funktion erfolgreich abgeschlossen
//					-1	Fehler

int ComClose(BYTE byComNr)
{
	int	iRet = 0;

	if ((byComNr > (COM_MAX - 1)) || (byComNr < 0))
		return(-1);

    if (com[byComNr].hCom == INVALID_HANDLE_VALUE)
		return(-1);

	if (com[byComNr].hOpenThread != NULL)
	{
		CloseHandle(com[byComNr].hOpenThread);
		com[byComNr].hOpenThread = NULL;
	}

	if (com[byComNr].hCloseThread == NULL)
	{
		DWORD	dwId;

		com[byComNr].iCloseError = 0;
		com[byComNr].hCloseThread = CreateThread(NULL, 0, _CloseThread, (LPVOID)byComNr, 0, &dwId);
	}
	else
	{
		DWORD	dwRes;

		dwRes = WaitForSingleObject(com[byComNr].hCloseThread, 0);
		if (dwRes == WAIT_OBJECT_0)
		{
			// Schliessen beendet
			if (com[byComNr].iCloseError == 0)
				iRet = 1;	// Ok
			else
 				iRet = -1;	// Fehler

			CloseHandle(com[byComNr].hCloseThread);
			com[byComNr].hCloseThread = NULL;
			com[byComNr].hCom = INVALID_HANDLE_VALUE;
		}
	}

	return iRet;
}


// --------------------------------------------------------------------------
//  Funktion:       ComSend()
//  Beschreibung :  Daten versenden
//  Parameter :     byComNr   : Schnittstellen-Nummer (0..9)				
//                  lpvBuffer : Zeiger auf den Sende-Puffer (Adresse)
//                  dwLength  : Anzahl Bytes im Sende-Puffer
//                  lpdwBytes   : Zeiger auf ein Doppelwort, welches die
//                                Anzahl der übergebenen Bytes enthält.
//  Returnwert :    0           Datenausgabe konnte gestartet werden
//					0xFFFFFFFF  Datenausgabe konnte nicht gestartet werden
//					Fehlerflags	

DWORD ComSend(BYTE byComNr, LPVOID lpvBuffer, DWORD dwLength, DWORD *lpdwBytesWritten)
{
	DWORD	dwErrorFlags = 0;
	BOOL	bWriteStatus;
	COMSTAT	comStat;

	if ((byComNr > (COM_MAX - 1)) || (byComNr < 0))
		return(0xFFFFFFFF);

    if (com[byComNr].hCom == INVALID_HANDLE_VALUE)
		return(0xFFFFFFFF);

	if (lpvBuffer == NULL)
		return(0xFFFFFFFF);

	bWriteStatus = WriteFile(com[byComNr].hCom, lpvBuffer, dwLength, lpdwBytesWritten, NULL);

	if (!bWriteStatus)
	{
		// Fehler beim Schreiben
		ClearCommError(com[byComNr].hCom, &dwErrorFlags, &comStat);
	}

	return dwErrorFlags;
}


// --------------------------------------------------------------------------
//  Funktion:       ComGetStatus()
//  Beschreibung :  Schnittstellen-Status lesen
//  Parameter:      byComNr : Schnittstellen-Nummer (0..9)
//					pdwOutCount : Zeiger auf ein Doppelwort, welches die 
//                                Anzahl der Zeichen im Ausgangs-Puffer enthält.
//                  lpdwInCount : Zeiger auf ein Doppelwort, welches die 
//                                Anzahl der Zeichen im Eingangs-Puffer enthält.
//  Returnwert :    0xFFFFFFFF  Schnittstelle nicht geöffnet
//					Fehlerflags	

DWORD ComGetStatus(BYTE byComNr, DWORD *lpdwOutCount, DWORD *lpdwInCount)
{
	DWORD	dwErrorFlags;
	COMSTAT	comStat;

	if ((byComNr > (COM_MAX - 1)) || (byComNr < 0))
		return 0xFFFFFFFF;

    if (com[byComNr].hCom == INVALID_HANDLE_VALUE)
		return(0xFFFFFFFF);

	ClearCommError(com[byComNr].hCom, &dwErrorFlags, &comStat);

	*lpdwOutCount = comStat.cbOutQue;
	*lpdwInCount = comStat.cbInQue;

	return dwErrorFlags;
}


// --------------------------------------------------------------------------
//  Funktion:       ComReceive()
//  Beschreibung :  Daten empfangen
//  Parameter :     byComNr       : Schnittstellen-Nummer (0..9)				
//                  lpvBuffer     : Zeiger auf den Empfangs-Puffer (Adresse)
//                  dwMaxLength   : Max. Größe des Empfangs-Puffers in Bytes
//                  lpdwBytesRead : Zeiger auf ein Doppelwort, welches die
//                                  Anzahl der empfangenen Bytes enthält.
//  Returnwert :    0xFFFFFFFF  Schnittstelle nicht geöffnet
//					Fehlerflags	

DWORD ComReceive(BYTE byComNr, LPVOID lpvBuffer, DWORD dwMaxLength, DWORD *lpdwBytesRead)
{
	BOOL	bReadStatus;
	DWORD	dwErrorFlags, dwLength;
	COMSTAT	comStat;

	if ((byComNr > (COM_MAX - 1)) || (byComNr < 0))
		return(0xFFFFFFFF);

    if (com[byComNr].hCom == INVALID_HANDLE_VALUE)
		return(0xFFFFFFFF);

	// only try to read number of bytes in queue
	ClearCommError(com[byComNr].hCom, &dwErrorFlags, &comStat);
	dwLength = min(dwMaxLength, comStat.cbInQue);

	if (dwLength > 0)
	{
		bReadStatus = ReadFile(com[byComNr].hCom, lpvBuffer, dwLength, &dwLength, NULL);

		if (!bReadStatus)
		{
			// Fehler beim Lesen
			dwLength = 0;
			ClearCommError(com[byComNr].hCom, &dwErrorFlags, &comStat);
		}
	}
	
	*lpdwBytesRead = dwLength;
	return dwErrorFlags;
}


// --------------------------------------------------------------------------
//  Funktion:       ComGet485Error()
//  Beschreibung :  RS485 Sende-Fehler detailliert lesen
//  Parameter:      byComNr : Schnittstellen-Nummer (0..9)
//  Returnwert :    RS485 Sende-Fehler
//                  0 = kein Fehler	
//                  1 = Treiber Handlingsfehler
//                  2 = Treiber Sendefehler, Kurzschluß oder Unterbruch
//                  3 = Fehler bei Datenvergleich, Kollision
//                  4 = Zeichen-Fehler, Kollision
//                  5 = Timeout, Kurzschluß oder Unterbruch

DWORD ComGet485Error(BYTE byComNr)
{
	UINT32 err = 1;

//	SerReadError(com[byComNr].hCom, &err);

	return(err);	
}


// --------------------------------------------------------------------------
//  Funktion:       ComClear()
//  Beschreibung :  Der Inhalt des Eingabe- und Ausgabepuffers wird gelöscht
//  Parameter:      byComNr : Schnittstellen-Nummer (0..9)
//	Returnwert:		 1  Funktion erfolgreich abgeschlossen
//					-1	Fehler

int ComClear(BYTE byComNr)
{
	PurgeComm(com[byComNr].hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);
	return(1);
}


// --------------------------------------------------------------------------
//  Funktion:       ComActivateDTR()
//  Beschreibung :  Setzen und löschen des DTR-Signales
//  Parameter:      byComNr : Schnittstellen-Nummer (0..9)
//					bActivate : Signal setzen (TRUE) oder löschen (FALSE)
//	Returnwert:		 1  Funktion erfolgreich abgeschlossen
//					-1	Fehler

int ComActivateDTR(BYTE byComNr, BOOL bActivate)
{
	int		iRet;
	BOOL	bStat;

	if (bActivate)
	{
		bStat = EscapeCommFunction(com[byComNr].hCom, SETDTR);
	}
	else
	{
		bStat = EscapeCommFunction(com[byComNr].hCom, CLRDTR);
	}

	if (bStat)
	{
		iRet = 1;
	}
	else
	{
		iRet = -1;
	}

	return iRet;
}


// --------------------------------------------------------------------------
//  Funktion:       ComChangeBaud()
//  Beschreibung :  Ändern der Baudrate einer seriellen Schnittstelle
//  Parameter:      byComNr : Schnittstellen-Nummer (0..9)
//					iBaudRate : neue Baudrate
//	Returnwert:		 1  Funktion erfolgreich abgeschlossen
//					-1	Fehler

int ComChangeBaud(BYTE byComNr, int iBaudRate)
{
	int		iRet;
	BOOL	bStat = FALSE;
	DCB		dcbCommPort;

	bStat = GetCommState(com[byComNr].hCom, &dcbCommPort);
	if (bStat)
	{
		dcbCommPort.BaudRate = iBaudRate;
		bStat = SetCommState(com[byComNr].hCom, &dcbCommPort);
	}

	if (bStat)
	{
		iRet = 1;
	}
	else
	{
		iRet = -1;
	}

	return iRet;
}


// --------------------------------------------------------------------------
//  Funktion:       CommGetModemStatus()
//  Beschreibung :  Ändern der Baudrate einer seriellen Schnittstelle
//  Parameter:      byComNr : Schnittstellen-Nummer (0..9)
//					pdwModemStatus : ModemStatus wie (GetCommModemStatus)
//	Returnwert:		 1  Funktion erfolgreich abgeschlossen
//					-1	Fehler

int CommGetModemStatus(BYTE byComNr, DWORD *pdwModemStatus)
{
	int		iRet = -1;
	BOOL	bState;

	bState = GetCommModemStatus(com[byComNr].hCom, pdwModemStatus);

	if (bState)
	{
		iRet = 1;
	}

	return iRet;
}


// -----------------------------------------------------------------------------
//  Funktion:       ComLibIinit()
//  Beschreibung :  Init-Funktion
//  Parameter :     kein
//  Returnwert :    kein

void ComLibInit(void)
{
	UINT8 i;

	for (i = 0; i < COM_MAX; i++)
	{
		com[i].hCom = INVALID_HANDLE_VALUE;
		com[i].hOpenThread = NULL;
		com[i].hCloseThread = NULL;
	}
}


// -----------------------------------------------------------------------------
//  Funktion:       ComLibDeinit()
//  Beschreibung :  Deinit-Funktion
//  Parameter :     kein
//  Returnwert :    kein

void ComLibDeinit(void)
{
	UINT8 i;

	for (i = 0; i < COM_MAX; i++)
	{	
		CloseHandle(com[i].hCom);
		com[i].hCom = INVALID_HANDLE_VALUE;
	}
}
