// =============================================================================
//                      (C) Kinz Elektronik
//
// Produkt:      
//
// Modulname:    ComDiag.c
//
// Funktion:     Diverse C-Funktionen
//
// Autor:
//      Name:    R. Bernecker
//      Datum:   2.4.2002
//
// Updates:
//      Version:
//      Name:
//      Datum:
//      Grund:
//
// Hardware:            PC-1100, PC
// Betriebsystem:       Windows-CE, Windows-2000
// Compiler:            Visual C++
//
// =============================================================================

// -----------------------------------------------------------------------------
// includes

#include <windows.h>
//#include <winver.h>

// TGR Linux changed
#ifdef LINUX
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "linux.h"
#include "tchar.h"
#endif

#include "ComDiag.h"

// -----------------------------------------------------------------------------
// defines
#define CDI_OFF_MODE		1
#define CDI_OFF_TYPE		2
#define CDI_OFF_LEN			3
#define CDI_OFF_DATA		6

#define CD_MSG_HEAD_LEN		12
#define CD_MSG_TAIL_LEN		5
#define CD_MIN_MSG_LEN		( CD_MSG_HEAD_LEN + CD_MSG_TAIL_LEN )

#define CD_MSG_HEADER		0xFEFF

#define VALUE_MD				0x7FFFFFFF
#define VALUE_SD				0x80000001

// -----------------------------------------------------------------------------
// externals


// -----------------------------------------------------------------------------
// statics
static TCHAR	g_tchCDEquip = 'T';

// -----------------------------------------------------------------------------
// prototypes


typedef struct
{
	UCHAR	min; 	/* Minutes */
	UCHAR	hour;	/* Hours */
	UCHAR	hund;	/* Hundredths of seconds */
	UCHAR	sec; 	/* Seconds */
} QVIS_TIME;

typedef struct
{
	USHORT	year;	/* Year */
	UCHAR	day; 	/* Day of the month */
	UCHAR	mon; 	/* Month (1 = Jan) */
} QVIS_DATE;

typedef union
{
    BYTE		bit;
    CHAR		int8;
	UCHAR		uint8;
    SHORT		int16;
	USHORT		uint16;
	INT			int32;
	UINT		uint32;
	FLOAT		real;
	LPTSTR		lpString;
    QVIS_TIME	time;
    QVIS_DATE	date;
} VAR_DATA;



// -----------------------------------------------------------------------------
//  Funktion:       CD_Init
//  Beschreibung :  Setz die Art des Gerätes
//  Parameter :     Zeichen für Gerät 'T' für Terminal, 'D' für Diagnosesoftware
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_Init(TCHAR tchEquip)
{
	BOOL	bRet = TRUE;

	if (tchEquip == 'T' || tchEquip == 'D')
	{
		g_tchCDEquip = tchEquip;
	}
	else
	{
		bRet = FALSE;
	}

	return bRet;
}


// -----------------------------------------------------------------------------
//  Funktion:       CD_GetMsgLen
//  Beschreibung :  Liefert Anzahl Zeichen der Meldung
//  Parameter :     Meldung, Länge
//  Returnwert :    TRUE..OK, FALSE..Fehler

BOOL CD_GetMsgLen(LPCTSTR ptszMsg, int *piLenData)
{
	BOOL	bRet = TRUE;
	TCHAR	tszLen[4];
	TCHAR	*tszPos;
	int		iLen;

	tszPos = (TCHAR*)ptszMsg + CDI_OFF_LEN;
	_tcsncpy(tszLen, tszPos, 3);
	tszLen[3] = '\0';
	iLen = (int)_tcstol(tszLen, NULL, 10);

	*piLenData = iLen;

	return bRet;
}

// -----------------------------------------------------------------------------
//  Funktion:       CD_GetMsgCount
//  Beschreibung :  Liefert die angeforderte Zeilenzahl
//  Parameter :     Meldung, Zähler
//  Returnwert :    TRUE..OK, FALSE..Fehler

BOOL CD_GetMsgCount(LPCTSTR ptszMsg, int *piCount)
{
	BOOL	bRet = TRUE;
	TCHAR	tszCount[20];
	TCHAR	*tszPos;
	int		iLen;

	CD_GetMsgLen(ptszMsg, &iLen);
	tszPos = (TCHAR*)ptszMsg + CDI_OFF_LEN + 3;
	_tcsncpy(tszCount, tszPos, iLen);
	tszCount[iLen] = '\0';
	*piCount = (int)_tcstol(tszCount, NULL, 10);

	return bRet;
}


// -----------------------------------------------------------------------------
//  Funktion:       CD_GetMsgCheckSum
//  Beschreibung :  Liefert die in der Meldung gespeicherte Prüfsumme
//  Parameter :     Meldung, Länge, Prüfsumme
//  Returnwert :    TRUE..OK, FALSE..Fehler

BOOL CD_GetMsgCheckSum(LPCTSTR ptszMsg, int iLenData, BYTE *pbyCheckSum)
{
	BOOL	bRet = TRUE;
	TCHAR	tszLen[3];
	TCHAR	*tszPos;
	int		iCheckSum;

	tszPos = (TCHAR*)ptszMsg + CDI_OFF_LEN + 3 + iLenData;
	_tcsncpy(tszLen, tszPos, 2);
	tszLen[2] = '\0';
	iCheckSum = (int)_tcstol(tszLen, NULL, 16);

	*pbyCheckSum = (BYTE)iCheckSum;

	return bRet;
}

// -----------------------------------------------------------------------------
//  Funktion:       CD_CalcCheckSumTotalMsg
//  Beschreibung :  berechnet die Prüfsumme einer Meldung
//  Parameter :     Meldung, Anzahl Zeichen der Meldung samt Meldungskopf, Prüfsumme
//  Returnwert :    TRUE..OK, FALSE..Fehler

BOOL CD_CalcCheckSumTotalMsg(LPCTSTR ptszMsg, int iLenMsg, BYTE *pbyCheckSum)
{
	BOOL			bRet = TRUE;
	int				i;
	BYTE			*pbyBuf;
	BYTE			bySumme;

	pbyBuf = (BYTE*)ptszMsg;

	bySumme = 0;

	for (i = 0; i < iLenMsg*2; i++)
	{
		bySumme += *pbyBuf;
		pbyBuf++;
	}

	*pbyCheckSum = bySumme;

	return bRet;
}

// -----------------------------------------------------------------------------
//  Funktion:       CD_CalcCheckSum
//  Beschreibung :  berechnet die Prüfsumme einer Meldung
//  Parameter :     Meldung, Anzahl Zeichen der Meldung ohne Meldungskopf, Prüfsumme
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_CalcCheckSum(LPCTSTR ptszMsg, int iLenData, BYTE *pbyCheckSum)
{
	return CD_CalcCheckSumTotalMsg(ptszMsg, 6 + iLenData, pbyCheckSum);
}


// -----------------------------------------------------------------------------
//  Funktion:       CD_SetMsgLen
//  Beschreibung :  setzt die Länge Meldung an der entsprechenden Position
//  Parameter :     Meldung, Länge
//  Returnwert :    TRUE..OK, FALSE..Fehler

BOOL CD_SetMsgLen(LPTSTR ptszMsg, int iLenData)
{
	BOOL	bRet = TRUE;
	TCHAR	tszLen[4];
	TCHAR	*tszPos;

	_stprintf(tszLen, _T("%3.3d"), 	iLenData);
	tszLen[3] = '\0';

	tszPos = (TCHAR*)ptszMsg + CDI_OFF_LEN;
	_tcsncpy(tszPos, tszLen, 3);

	return bRet;
}

// -----------------------------------------------------------------------------
//  Funktion:       CD_SetCheckSum
//  Beschreibung :  setzt die Prüfsumme einer Meldung an der entsprechenden Position
//  Parameter :     Meldung, Länge der Meldung ohne Meldungskopf, Prüfsumme
//  Returnwert :    TRUE..OK, FALSE..Fehler

BOOL CD_SetCheckSum(LPTSTR ptszMsg, int iLenData, BYTE byCheckSum)
{
	BOOL	bRet = TRUE;
	TCHAR	tszCheckSum[3];
	TCHAR	*tszPos;

	_stprintf(tszCheckSum, _T("%2.2X"), byCheckSum);
	tszCheckSum[2] = '\0';

	tszPos = ptszMsg + CDI_OFF_LEN + 3 + iLenData;
	_tcsncpy(tszPos, tszCheckSum, 2);

	return bRet;
}

// -----------------------------------------------------------------------------
//  Funktion:       CD_GenerateConfirmMsg
//  Beschreibung :  erzeugt eine Bestätigungsmeldung
//  Parameter :     Meldung, Bestätigungsmeldung, Länge Bestätigungsmeldung in Unicodezeichen, Anzahl Zeilen
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_GenerateConfirmMsg(LPCTSTR ptszMsg, LPTSTR ptszMsgConf, DWORD *pdwMsgConfChars, DWORD dwCount)
{
	BOOL	bRet = TRUE;
	TCHAR	*tszPosMsg;
	TCHAR	*tszPosMsgConf;
	TCHAR	tszLen[20];
	int		iLen;
	int		iLenMsg;
	BYTE	byCheckSumMsg;
	BYTE	byCheckSumMsgTest;
	BYTE	byCheckSumMsgConf;

	tszPosMsgConf = ptszMsgConf;
	*tszPosMsgConf = CD_MSG_HEADER;

	tszPosMsg = (TCHAR*)ptszMsg + CDI_OFF_MODE;
	tszPosMsgConf++;
	if (*tszPosMsg != g_tchCDEquip)
	{
		bRet = FALSE;
	}

	if (*tszPosMsg == 'T')
	{
		*tszPosMsgConf = 'D';
	}
	else
	{
		*tszPosMsgConf = 'T';
	}

	tszPosMsg++;
	tszPosMsgConf++;
	*tszPosMsgConf = *tszPosMsg;

	tszPosMsgConf++;
	*tszPosMsgConf = '0';
	tszPosMsgConf++;
	*tszPosMsgConf = '0';
	tszPosMsgConf++;
	*tszPosMsgConf = '3';

	tszPosMsgConf++;
	*tszPosMsgConf = *tszPosMsg;

	CD_GetMsgLen(ptszMsg, &iLenMsg);
	CD_GetMsgCheckSum(ptszMsg, iLenMsg, &byCheckSumMsgTest);
	CD_CalcCheckSumTotalMsg(ptszMsg, iLenMsg+6, &byCheckSumMsg);

	CD_SetCheckSum(ptszMsgConf, 1, byCheckSumMsg);

	iLen = 0;
	if (dwCount > 0)
	{
		if (dwCount == 0xFFFFFFFF)
		{
			_stprintf(tszLen, _T("-1"), dwCount);
		}
		else
		{
			_stprintf(tszLen, _T("%d"), dwCount);
		}
		tszPosMsgConf = ptszMsgConf + CDI_OFF_DATA + 3;
		iLen = _tcslen(tszLen);
		memcpy(tszPosMsgConf, tszLen, iLen*2);
		CD_SetMsgLen(ptszMsgConf, iLen+3);
	}

	CD_CalcCheckSumTotalMsg(ptszMsgConf, 6 + 3 + iLen, &byCheckSumMsgConf);
	CD_SetCheckSum(ptszMsgConf, 3 + iLen, byCheckSumMsgConf);

	*pdwMsgConfChars = CD_MIN_MSG_LEN / 2 + 3 + iLen;

	return bRet;
}

// -----------------------------------------------------------------------------
//  Funktion:       CD_CheckConfirmMsg
//  Beschreibung :  prüft eine Bestätigungsmeldung
//  Parameter :     Meldung, Typ-Orginalmeldung, Checksumme-Orginalmeldung, Anzahl Zeilen
//  Returnwert :    TRUE..OK, FALSE..Fehler

BOOL CD_CheckConfirmMsg(LPCTSTR ptszMsg, TCHAR tchType, BYTE byCheckSum, int *piCount)
{
	BOOL	bRet = TRUE;
	TCHAR	*tszPosMsg;
	int		iLenMsg;
	TCHAR	tszLen[20];
	BYTE	byCheckSumMsg;
	BYTE	byCheckSumMsgTest;
	int		iLen;


	tszPosMsg = (TCHAR*)ptszMsg + CDI_OFF_MODE;
	if (*tszPosMsg != g_tchCDEquip)
	{
		bRet = FALSE;
	}

	tszPosMsg++;
//	if (*tszPosMsg != 'B')
//	{
//		bRet = FALSE;
//	}

	CD_GetMsgLen(ptszMsg, &iLenMsg);
	if (iLenMsg < 3)
	{
		bRet = FALSE;
	}

	tszPosMsg = (TCHAR*)ptszMsg + CDI_OFF_LEN + 3;
	if (*tszPosMsg != tchType)
	{
		bRet = FALSE;
	}

	CD_GetMsgCheckSum(ptszMsg, 1, &byCheckSumMsgTest);
	if (byCheckSumMsgTest != byCheckSum)
	{
		bRet = FALSE;
	}

	iLen = iLenMsg - 3;
	*piCount = 0;
	if (iLen > 0)
	{
		tszPosMsg = (TCHAR*)ptszMsg + CDI_OFF_DATA + 3;
		memcpy(tszLen, tszPosMsg, iLen*2);
		tszLen[iLen] = '\0';
		*piCount = (int)_tcstol(tszLen, NULL, 10);
	}

	CD_GetMsgCheckSum(ptszMsg, iLenMsg, &byCheckSumMsgTest);
	CD_CalcCheckSumTotalMsg(ptszMsg, CDI_OFF_LEN + 3 + iLenMsg, &byCheckSumMsg);
	if (byCheckSumMsgTest != byCheckSumMsg)
	{
		bRet = FALSE;
	}

	return bRet;
}

// -----------------------------------------------------------------------------
//  Funktion:       CD_CheckConfirmMsg2Msg
//  Beschreibung :  prüft eine Bestätigungsmeldung gegen Orginalmeldung
//  Parameter :     Bestätigungsmeldung, Orginalmeldung, Anzahl Zeilen
//  Returnwert :    TRUE..OK, FALSE..Fehler

BOOL CD_CheckConfirmMsg2Msg(LPCTSTR ptszMsgConf, LPCTSTR ptszMsgOrig, int *piCount)
{
	BOOL		bRet = FALSE;
	TCHAR		tchType;
	BYTE		byCheckSum;
	int			iLen;

	CD_GetMsgLen(ptszMsgOrig, &iLen);
	CD_GetMsgCheckSum(ptszMsgOrig, iLen, &byCheckSum);

	tchType = *((TCHAR*)ptszMsgOrig + CDI_OFF_TYPE);

	bRet = CD_CheckConfirmMsg(ptszMsgConf, tchType, byCheckSum, piCount);

	return bRet;
}


// -----------------------------------------------------------------------------
//  Funktion:       CD_CheckMsg
//  Beschreibung :  prüft eine Meldung auf gültige Checksumme
//  Parameter :     Meldung
//  Returnwert :    TRUE..OK, FALSE..Fehler

BOOL CD_CheckMsg(LPCTSTR ptszMsg)
{
	BOOL		bRet = FALSE;
	int			iLen;
	BYTE		byCheckSumMsg;
	BYTE		byCheckSumCalc;

	CD_GetMsgLen(ptszMsg, &iLen);
	CD_GetMsgCheckSum(ptszMsg, iLen, &byCheckSumMsg);

	CD_CalcCheckSum(ptszMsg, iLen, &byCheckSumCalc);

	if (byCheckSumMsg == byCheckSumCalc)
	{
		bRet = TRUE;
	}

	return bRet;
}


// -----------------------------------------------------------------------------
//  Funktion:       CD_ParseMsg
//  Beschreibung :  prüft den Input-Buffer ob eine Meldung vorliegt
//  Parameter :     Lesebuffer, Position Lesebuffer für Anfügen neuer Daten, 
//					Neue Meldung, Typ der neuen Meldung
//  Returnwert :    TRUE..Meldung gefunden, FALSE..keine neue Meldung
BOOL CD_ParseMsg(BYTE *pbyBufIn, DWORD *dwPosIn, LPTSTR ptszMsgNew, int *piMsgType)
{
	BOOL		bRet = FALSE;
	DWORD		j, dwPosMsg;
	BYTE		*pbyPos;
	BOOL		bLoop;
	TCHAR		tchMsg[CD_MIN_MSG_LEN];
	int			iMsgLen;

	pbyPos = pbyBufIn;
	bLoop  = TRUE;
	dwPosMsg = 0;
	for (j = 0; bLoop && !bRet && j < *dwPosIn; j++, pbyPos++)
	{
		// Meldungskopf prüfen
		if (*pbyPos == 0xFF)
		{
			dwPosMsg = j;
			// auf kleinsmögliche Meldungslänge prüfen
			if (dwPosMsg + CD_MIN_MSG_LEN - 1 < *dwPosIn) 
			{
				memcpy(tchMsg, pbyPos, CD_MIN_MSG_LEN);
				// gültiger Kopf und Kennung
				if (tchMsg[0] == CD_MSG_HEADER && tchMsg[1] == g_tchCDEquip)
				{
					CD_GetMsgLen(tchMsg, &iMsgLen);
					if (iMsgLen < CD_MAX_MSG_LEN)
					{
						// gesamte Meldung im Buffer
						if (dwPosMsg + CD_MIN_MSG_LEN + iMsgLen*2 - 1 < *dwPosIn)
						{
							memcpy(ptszMsgNew, pbyPos, CD_MIN_MSG_LEN + iMsgLen*2);
							*(ptszMsgNew+CD_MIN_MSG_LEN/2 + iMsgLen) = '\0';
							bRet = TRUE;
							dwPosMsg = dwPosMsg + CD_MIN_MSG_LEN + iMsgLen*2;
						}
						else
						{
							// auf weitere Daten Warten
							bLoop = FALSE;
						}

					}
				}
			}
			else
			{
				// auf weitere Daten Warten
				bLoop = FALSE;
			}
		}
		else
		{
			dwPosMsg = j + 1;
		}
	}

	*piMsgType = CD_MSG_NOT_COMPLETE;

	// art der Meldung bestimmen
	if (bRet)
	{
		TCHAR	*tszPos;

		tszPos = ptszMsgNew + CDI_OFF_TYPE;

		switch(*tszPos)
		{
		case 'B': // Meldungsbestätigung
			*piMsgType = CD_MSG_CONFIRM;
			break;
		case 'K': // Konfigurationsdaten
			*piMsgType = CD_MSG_CONFIG;
			break;
		case 'I': // Initialisierung Konfigurationsdaten
			*piMsgType = CD_MSG_INIT_CONFIG;
			break;
		case 'J': // Sprachen des Terminal abfragen
			*piMsgType = CD_MSG_GET_LANGS;
			break;
		case 'L': // Sprache am Terminal setzen
			*piMsgType = CD_MSG_SET_LANG;
			break;
		case 'Z': // Zustandsinfo
			*piMsgType = CD_MSG_STATE;
			break;
		case 'M': // Binärmessstelle-Start
			*piMsgType = CD_MSG_BINARY_START;
			break;
		case 'N': // Binärmessstelle-Daten
			*piMsgType = CD_MSG_BINARY_DATA;
			break;
		case 'A': // Analogmessstelle-Start
			*piMsgType = CD_MSG_ANALOG_START;
			break;
		case 'C': // Analogmessstelle-Daten
			*piMsgType = CD_MSG_ANALOG_DATA;
			break;
		case 'F': // Alarmmeldung-Start
			*piMsgType = CD_MSG_ALARM_START;
			break;
		case 'G': // Alarmmeldung-Daten
			*piMsgType = CD_MSG_ALARM_DATA;
			break;
		case 'H': // Betriebsstunden-Start
			*piMsgType = CD_MSG_DURATION_START;
			break;
		case 'O': // Betriebsstunden-Daten
			*piMsgType = CD_MSG_DURATION_DATA;
			break;
		case 'R': // Fehlerringspeicher-Start
			*piMsgType = CD_MSG_HISTORY_START;
			break;
		case 'S': // Fehlerringspeicher-Daten
			*piMsgType = CD_MSG_HISTORY_DATA;
			break;
		case 'U': // Überwachungsmeldung
			*piMsgType = CD_MSG_LINE_TEST;
			break;
		case 'T': // Datenzeilen-Stop
			*piMsgType = CD_MSG_STOP;
			break;
		case 'V': // Fehlerstatistik-Start
			*piMsgType = CD_MSG_HISTORY2_START;
			break;
		case 'W': // Fehlerstatistik-Daten
			*piMsgType = CD_MSG_HISTORY2_DATA;
			break;
		case 'X': // Fehlerstatistik-Löschen
			*piMsgType = CD_MSG_HISTORY2_DELETE;
			break;
		default:
			*piMsgType = CD_MSG_UNKNOWN;
			break;
		}
	}

	// unvollständige Daten im Lesebuffer wieder an den Anfang schieben
	if (dwPosMsg > 0)
	{
		DWORD		dwCount;
		BYTE		*pbyPosTmp;

		dwCount = *dwPosIn - dwPosMsg;

		pbyPosTmp = pbyBufIn + dwPosMsg;
		pbyPos = pbyBufIn;
		for(j = 0; j < dwCount; j++)
		{
			*pbyPos++ = *pbyPosTmp++;
		}

		*dwPosIn = dwCount;
	}

	return bRet;
}

// -----------------------------------------------------------------------------
//  Funktion:       CD_GenerateDataMsg
//  Beschreibung :  erzeugt eine Daten Meldung
//  Parameter :     Meldung, Länge Meldung in Bytes, Typ der neuen Meldung, Daten in Unicode, Anzahl Unicode Zeichen
//  Returnwert :    TRUE..OK, FALSE..Fehler

BOOL CD_GenerateDataMsg(LPTSTR ptszMsg, DWORD *pdwMsgBytes, int iMsgType, LPTSTR ptszData, int iDataLen)
{
	BOOL	bRet = TRUE;
	TCHAR	*ptszPos;
	BYTE	byCheckSum;

	ptszPos = ptszMsg;
	*ptszPos = CD_MSG_HEADER;

	ptszPos++;
	if (g_tchCDEquip == 'T')
	{
		*ptszPos = 'D';
	}
	else
	{
		*ptszPos = 'T';
	}

	ptszPos++;
	switch (iMsgType)
	{
	case CD_MSG_CONFIRM:	
		*ptszPos = 'B';
		break;
	case CD_MSG_CONFIG:		
		*ptszPos = 'K';
		break;
	case CD_MSG_INIT_CONFIG:
		*ptszPos = 'I';
		break;
	case CD_MSG_GET_LANGS:	
		*ptszPos = 'J';
		break;
	case CD_MSG_SET_LANG:		
		*ptszPos = 'L';
		break;
	case CD_MSG_STATE:		
		*ptszPos = 'Z';
		break;
	case CD_MSG_BINARY_START:	
		*ptszPos = 'M';
		break;
	case CD_MSG_BINARY_DATA:	
		*ptszPos = 'N';
		break;
	case CD_MSG_ANALOG_START:	
		*ptszPos = 'A';
		break;
	case CD_MSG_ANALOG_DATA:	
		*ptszPos = 'C';
		break;
	case CD_MSG_ALARM_START:	
		*ptszPos = 'F';
		break;
	case CD_MSG_ALARM_DATA:	
		*ptszPos = 'G';
		break;
	case CD_MSG_DURATION_START:	
		*ptszPos = 'H';
		break;
	case CD_MSG_DURATION_DATA:	
		*ptszPos = 'O';
		break;
	case CD_MSG_HISTORY_START:	
		*ptszPos = 'R';
		break;
	case CD_MSG_HISTORY_DATA:	
		*ptszPos = 'S';
		break;
	case CD_MSG_LINE_TEST:	
		*ptszPos = 'U';
		break;
	case CD_MSG_STOP:	
		*ptszPos = 'T';
		break;
	case CD_MSG_HISTORY2_START:	
		*ptszPos = 'V';
		break;
	case CD_MSG_HISTORY2_DATA:	
		*ptszPos = 'W';
		break;
	case CD_MSG_HISTORY2_DELETE:	
		*ptszPos = 'X';
		break;
	default:
		*ptszPos = ' ';
		bRet = FALSE;
		break;
	}


	CD_SetMsgLen(ptszMsg, iDataLen);

	if (iDataLen > 0)
	{
		ptszPos = ptszMsg + CDI_OFF_LEN + 3;
		memcpy(ptszPos, ptszData, iDataLen*2);
	}

	CD_CalcCheckSum(ptszMsg, iDataLen, &byCheckSum);
	CD_SetCheckSum(ptszMsg, iDataLen, byCheckSum);

	*pdwMsgBytes = CD_MIN_MSG_LEN + iDataLen * 2; 

	ptszPos = ptszMsg + *pdwMsgBytes / 2;
	*ptszPos = '\0';


	return bRet;
}

// -----------------------------------------------------------------------------
//  Funktion:       CD_GenerateDataHeader
//  Beschreibung :  erzeugt den Kopf einer Daten Meldung
//  Parameter :     Meldung, gesamte Länge Meldung in Unicodezeichen
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_GenerateDataHeader(LPTSTR ptszMsg, DWORD *pdwMsgChars, int iMsgType)
{
	BOOL		bRet = FALSE;
	TCHAR		*ptszPos;

	ptszPos = ptszMsg;
	*ptszPos = CD_MSG_HEADER;

	ptszPos++;
	if (g_tchCDEquip == 'T')
	{
		*ptszPos = 'D';
	}
	else
	{
		*ptszPos = 'T';
	}

	ptszPos++;
	switch (iMsgType)
	{
	case CD_MSG_CONFIRM:	
		*ptszPos = 'B';
		break;
	case CD_MSG_CONFIG:		
		*ptszPos = 'K';
		break;
	case CD_MSG_INIT_CONFIG:
		*ptszPos = 'I';
		break;
	case CD_MSG_GET_LANGS:	
		*ptszPos = 'J';
		break;
	case CD_MSG_SET_LANG:		
		*ptszPos = 'L';
		break;
	case CD_MSG_STATE:		
		*ptszPos = 'Z';
		break;
	case CD_MSG_BINARY_START:	
		*ptszPos = 'M';
		break;
	case CD_MSG_BINARY_DATA:	
		*ptszPos = 'N';
		break;
	case CD_MSG_ANALOG_START:	
		*ptszPos = 'A';
		break;
	case CD_MSG_ANALOG_DATA:	
		*ptszPos = 'C';
		break;
	case CD_MSG_ALARM_START:	
		*ptszPos = 'F';
		break;
	case CD_MSG_ALARM_DATA:	
		*ptszPos = 'G';
		break;
	case CD_MSG_DURATION_START:	
		*ptszPos = 'H';
		break;
	case CD_MSG_DURATION_DATA:	
		*ptszPos = 'O';
		break;
	case CD_MSG_HISTORY_START:	
		*ptszPos = 'R';
		break;
	case CD_MSG_HISTORY_DATA:	
		*ptszPos = 'S';
		break;
	case CD_MSG_LINE_TEST:	
		*ptszPos = 'U';
		break;
	case CD_MSG_STOP:	
		*ptszPos = 'T';
		break;
	case CD_MSG_HISTORY2_START:	
		*ptszPos = 'V';
		break;
	case CD_MSG_HISTORY2_DATA:	
		*ptszPos = 'W';
		break;
	case CD_MSG_HISTORY2_DELETE:	
		*ptszPos = 'X';
		break;
	default:
		*ptszPos = ' ';
		bRet = FALSE;
		break;
	}

	CD_SetMsgLen(ptszMsg, 0);

	*pdwMsgChars = CD_MSG_HEAD_LEN / 2;

	return bRet;
}

// -----------------------------------------------------------------------------
//  Funktion:       CD_GenerateDataTail
//  Beschreibung :  erzeugt das Ende der Daten Meldung (zusätzlich Prüfsumme hinzufügen)
//  Parameter :     Meldung, gesamte Länge Meldung in Unicodezeichen
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_GenerateDataTail(LPTSTR ptszMsg, DWORD *pdwMsgChars)
{
	BOOL		bRet = FALSE;
	int			iDataLen;
	BYTE		byCheckSum;
	TCHAR		*ptszPos;

	iDataLen = *pdwMsgChars - CD_MSG_HEAD_LEN / 2;

	if (iDataLen >= 0)
	{
		CD_SetMsgLen(ptszMsg, iDataLen);

		CD_CalcCheckSum(ptszMsg, iDataLen, &byCheckSum);
		CD_SetCheckSum(ptszMsg, iDataLen, byCheckSum);

		*pdwMsgChars = *pdwMsgChars + CD_MSG_TAIL_LEN / 2; 

		ptszPos = ptszMsg + *pdwMsgChars;
		*ptszPos = '\0';
	}
	else
	{
		bRet = FALSE;
	}

	return bRet;
}

// -----------------------------------------------------------------------------
//  Funktion:       CD_GetMsgData
//  Beschreibung :  Liefert die Daten einer Meldung
//  Parameter :     Meldung, Daten der Meldung
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_GetMsgData(LPCTSTR ptszMsg, LPTSTR ptszData)
{
	BOOL		bRet = TRUE;
	int			iDataLen;
	TCHAR		*tszPos;

	CD_GetMsgLen(ptszMsg, &iDataLen);

	if (iDataLen > 0)
	{
		tszPos = (TCHAR*)ptszMsg + CDI_OFF_LEN + 3;
		memcpy(ptszData, tszPos, iDataLen*2);
		ptszData[iDataLen] = '\0';
	}
	else
	{
		ptszData[0] = '\0';
		bRet = FALSE;
	}

	return bRet;
}

// -----------------------------------------------------------------------------
//  Funktion:       CD_GenerateDataDate
//  Beschreibung :  Fügt das Datum als String an die aktuelle Position der Meldung an und setzt die 
//					aktuelle Position nach dem Datum, nach dem Datum wird zusätzlich ein Komma als
//					Trennzeichen eingefügt
//  Parameter :     Meldung, gesamte Länge Meldung in Unicodezeichen, Datum
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_GenerateDataDate(LPTSTR ptszMsg, DWORD *pdwMsgChars, UINT32 uiDate)
{
	BOOL		bRet = TRUE;
	TCHAR		*ptszPos;
	VAR_DATA	VarData;

	ptszPos = ptszMsg + *pdwMsgChars;

	VarData.uint32 = uiDate;
	_stprintf(ptszPos, _T("%02d.%02d.%02d,"),
	VarData.date.day, VarData.date.mon, VarData.date.year % 100);

	*pdwMsgChars += _tcslen(ptszPos);

	return bRet;
}

// -----------------------------------------------------------------------------
//  Funktion:       CD_GenerateDataTime
//  Beschreibung :  Fügt die Uhrzeit als String an die aktuelle Position der Meldung an und setzt die 
//					aktuelle Position nach der Uhrzeit, nach der Uhrzeit wird zusätzlich ein Komma als
//					Trennzeichen eingefügt
//  Parameter :     Meldung, gesamte Länge Meldung in Unicodezeichen, Datum
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_GenerateDataTime(LPTSTR ptszMsg, DWORD *pdwMsgChars, UINT32 uiTime)
{
	BOOL		bRet = TRUE;
	TCHAR		*ptszPos;
	VAR_DATA	VarData;

	ptszPos = ptszMsg + *pdwMsgChars;

	VarData.uint32 = uiTime;
	_stprintf(ptszPos, _T("%02d:%02d:%02d,"),
	VarData.time.hour, VarData.time.min, VarData.time.sec);

	*pdwMsgChars += _tcslen(ptszPos);

	return bRet;
}


// -----------------------------------------------------------------------------
//  Funktion:       CD_GenerateDataUINT32
//  Beschreibung :  Fügt die Betriebsstunden als String an die aktuelle Position der Meldung an und 
//					setzt die aktuelle Position nach den Betriebsstunden, nach den Betriebsstunden 
//					wird zusätzlich ein Komma als Trennzeichen eingefügt
//  Parameter :     Meldung, gesamte Länge Meldung in Unicodezeichen, UINT32-Wert
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_GenerateDataUINT32(LPTSTR ptszMsg, DWORD *pdwMsgChars, UINT32 uiVal)
{
	BOOL		bRet = TRUE;
	TCHAR		*ptszPos;

	ptszPos = ptszMsg + *pdwMsgChars;

	if (uiVal == VALUE_MD)
		_tcscpy(ptszPos, _T("MD,"));
	else if (uiVal == VALUE_SD)
		_tcscpy(ptszPos, _T("SD,"));
	else
		_stprintf(ptszPos, _T("%d,"), uiVal);

	*pdwMsgChars += _tcslen(ptszPos);

	return bRet;
}

// -----------------------------------------------------------------------------
//  Funktion:       CD_GenerateDataINT32
//  Beschreibung :  Fügt die Drehzahl als String an die aktuelle Position der Meldung an und setzt die 
//					aktuelle Position nach der Drehzahl, nach der Drehzahl wird zusätzlich ein Komma als
//					Trennzeichen eingefügt
//  Parameter :     Meldung, gesamte Länge Meldung in Unicodezeichen, INT32-Wert
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_GenerateDataINT32(LPTSTR ptszMsg, DWORD *pdwMsgChars, INT32 iVal)
{
	BOOL		bRet = TRUE;
	TCHAR		*ptszPos;

	ptszPos = ptszMsg + *pdwMsgChars;

	if (iVal == VALUE_MD)
		_tcscpy(ptszPos, _T("MD,"));
	else if (iVal == VALUE_SD)
		_tcscpy(ptszPos, _T("SD,"));
	else
		_stprintf(ptszPos, _T("%d,"), iVal);

	*pdwMsgChars += _tcslen(ptszPos);

	return bRet;
}

// -----------------------------------------------------------------------------
//  Funktion:       CD_GenerateDataINT32Norm
//  Beschreibung :  Fügt den Wert mit Normierung als String an die aktuelle Position der Meldung an und setzt die 
//					aktuelle Position nach dem Wert, nach dem Wert wird zusätzlich ein Komma als
//					Trennzeichen eingefügt (Beispiel: Drehzahl)
//  Parameter :     Meldung, gesamte Länge Meldung in Unicodezeichen, INT32-Wert
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_GenerateDataINT32Norm(LPTSTR ptszMsg, DWORD *pdwMsgChars, INT32 iVal, INT32 iNorm, INT32 iPreComma, INT32 iPostComma)
{
	BOOL		bRet = TRUE;
	TCHAR		*ptszPos;

	ptszPos = ptszMsg + *pdwMsgChars;

	if (iVal == VALUE_MD)
		_tcscpy(ptszPos, _T("MD,"));
	else if (iVal == VALUE_SD)
		_tcscpy(ptszPos, _T("SD,"));
	else
	{
		double	dBuf;

		if (iNorm == 0)
		{
			iNorm = 1;
		}

		dBuf = (double)iVal / (double)iNorm;
		if (iPostComma > 0)
		{
			_stprintf(ptszPos, _T("%.*f,"), iPostComma, dBuf);
		}
		else
		{
			_stprintf(ptszPos, _T("%d,"), (int)dBuf);
		}
	}

	*pdwMsgChars += _tcslen(ptszPos);

	return bRet;
}


