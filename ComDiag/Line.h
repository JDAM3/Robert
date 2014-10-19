// =========================================================================
//                      (C) Kinz Elektronik
//
// Produkt:      ComDiag
//
// Modulname:    Line.h
//
// Funktion:     Diese Datei enthält alle erforderlichen Funktion um Per Modem
//				 eine Verbindung aufzubauen und Daten zu übertragen
//
// Autor:
//      Name:    R. Bernecker
//      Datum:   16.5.2002
//
// Hardware:            PC-1100, PC
// Betriebsystem:       Windows-CE, Windows-2000
// Compiler:            Visual C++
//
// =========================================================================

#ifndef _LINE_H
#define _LINE_H

// -------------------------------------------------------------------------
// Defines

//#define _LINE_NOINIT				1
//#define _LINE_INIT				2
//#define _LINE_DEINIT				3
//#define _LINE_NOOPEN				4
//#define _LINE_IDLE				5
//#define _LINE_OPENING				6
//#define _LINE_ANSWERING			7
//#define _LINE_CONNECTED			8
//#define _LINE_CLOSE				9
//#define _LINE_NODIALTONE			10
//#define _LINE_BUSY				11
//#define _LINE_NOANSWER			12
//#define _LINE_DISCONNECTED		13
//#define _LINE_WRONGPIN			14

typedef enum _tag_EN_LINE_STATE
{
	EN_LINE_STATE_NOINIT =		1,	
	EN_LINE_STATE_INIT,	
	EN_LINE_STATE_DEINIT,	
	EN_LINE_STATE_NOOPEN,	
	EN_LINE_STATE_IDLE,	
	EN_LINE_STATE_OPENING,	
	EN_LINE_STATE_ANSWERING,
	EN_LINE_STATE_CHANGEDBAUD,
	EN_LINE_STATE_CONNECTING,
	EN_LINE_STATE_CONNECTED,
} EN_LINE_STATE;

typedef enum _tag_EN_LINE_ERROR
{
	EN_LINE_OK =				1,
	EN_LINE_WORKING =			0,
	EN_LINE_ERR_WRONGPIN =		-1,
	EN_LINE_ERR_CLOSE =			-2,
	EN_LINE_ERR_NODIALTONE =	-3,
	EN_LINE_ERR_BUSY =			-4,
	EN_LINE_ERR_NOANSWER =		-5,
	EN_LINE_ERR_MODEM =			-6,
	EN_LINE_ERR_WRONGPUK =		-7,
	EN_LINE_ERR_WRONGPHPIN =	-8,
	EN_LINE_ERR_WRONGPHPUK =	-9,
	EN_LINE_ERR_WRONGPIN2 =		-10,
	EN_LINE_ERR_WRONGPUK2 =		-11,
	EN_LINE_ERR_WRONGCBST =		-12,
} EN_LINE_ERROR;

#ifdef __cplusplus
extern "C"
{
#endif

// =========================================================================
// Functions

////////////////////////////////////////////////////////////////
//	initialisiert eine Modemverbindung
EN_LINE_ERROR	Line_Init(UCHAR ucCom, DWORD dwBaud, char *szPin, char *szPuk, 
						  char *szCBST, BOOL bFixDataCall, DWORD dwBaud2Rail);

////////////////////////////////////////////////////////////////
//	deinitialisiert eine Modemverbindung
EN_LINE_ERROR	Line_Deinit(UCHAR ucCom, DWORD dwBaud, BOOL bShutdown);

////////////////////////////////////////////////////////////////
//	öffnet eine Modemverbindung für die angegebene Nummer
BOOL	Line_Open(DWORD dwLine);

////////////////////////////////////////////////////////////////
//	beendet eine Modemverbindung
BOOL	Line_Close(void);

////////////////////////////////////////////////////////////////
//	wartet auf eingehenden Anruf
BOOL	Line_Incoming(void);

////////////////////////////////////////////////////////////////
//	Baudrate der seriellen Schnittstelle ändern (für GSM-Rail benötigt)
//BOOL	Line_ChangeBaud(LPTSTR szPhoneNr, int iBaudRate);

////////////////////////////////////////////////////////////////
//	Anruf tätigen
BOOL	Line_Call(LPTSTR szPhoneNr);

////////////////////////////////////////////////////////////////
//	empfängt Daten einer Modemverbindung
BOOL	Line_Receive(void *pvBufIn, DWORD dwMaxLen, DWORD *dwReadIn);

////////////////////////////////////////////////////////////////
//	sendet Daten einer Modemverbindung
BOOL	Line_Send(void *pvBufOut, DWORD dwLenOut, DWORD *dwWrittenOut);

BOOL	Line_GetFirstName(LPTSTR szName);
BOOL	Line_GetNextName(LPTSTR szName);
BOOL	Line_GetLine4Name(LPTSTR szName, DWORD *pdwLine, BYTE *byPort, DWORD *pdwBaud);

EN_LINE_ERROR	Line_GsmRailWait(BYTE byPort);

EN_LINE_ERROR	Line_FindBaud(BYTE byPort, DWORD *pdwBaud, DWORD dwBaudSet);
EN_LINE_ERROR	Line_FindBaudStop(void);

////////////////////////////////////////////////////////////////
//	tickcount der Verbindungsherstellung oder letzte empfangene daten
DWORD	Line_GetLastTickCount(void);

////////////////////////////////////////////////////////////////
//	Status der Verbindung
EN_LINE_STATE	Line_GetState(EN_LINE_ERROR *peLineError);

////////////////////////////////////////////////////////////////
//	Status der Verbindung
WORD	Line_GetSignalLevel(void);

////////////////////////////////////////////////////////////////
//  Serielle Schnittstelle zurücksetzten besonders wenn DCD noch aktiv ist

int	Line_ClearCom(UCHAR ucCom, DWORD dwBaud);
BOOL	Line_ClearComState(void);

#ifdef __cplusplus
}
#endif

#endif // _LINE_H

