
// =============================================================================
//                      (C) Kinz Elektronik
//
// Produkt:      
//
// Modulname:    ComDiag.h
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
// Hardware:            
// Betriebsystem:       Windows-NT, Windows 2000, Windows-CE
// Compiler:            Visual C++, eMbedded Visual C++
//
// =============================================================================

#ifndef _COMDIAG_H
#define _COMDIAG_H

#ifndef _WIN32_WCE
#include <tchar.h>
#endif

// -----------------------------------------------------------------------------
// defines

#define CD_MAX_MSG_LEN	1024

#define CD_MSG_CONFIRM			1		// Best�tigungsmeldung
#define CD_MSG_CONFIG			2		// Konfigurationsdaten anfordern
#define CD_MSG_INIT_CONFIG		3		// Konfigurationsdaten initialisieren
#define CD_MSG_GET_LANGS		4		// Modemnummer initialisieren
#define CD_MSG_SET_LANG			5		// PIN-Nummer initialisieren
#define CD_MSG_STATE			6		// Zustandsinfo
#define CD_MSG_BINARY_START		7		// Bin�rmessstellen
#define CD_MSG_BINARY_DATA		8		// Bin�rmessstellen
#define	CD_MSG_ANALOG_START		9		// Analogmessstellen
#define	CD_MSG_ANALOG_DATA		10		// Analogmessstellen
#define CD_MSG_ALARM_START		11		// Alarmmeldung
#define CD_MSG_ALARM_DATA		12		// Alarmmeldung
#define CD_MSG_HISTORY_START	13		// Fehlerringspeicher
#define CD_MSG_HISTORY_DATA		14		// Fehlerringspeicher
#define CD_MSG_LINE_TEST		15		// �berwachungsmeldung
#define	CD_MSG_STOP				16		// Ende der Datenzeilen
#define CD_MSG_DURATION_START	17		// Betriebsstundenz�hler
#define CD_MSG_DURATION_DATA	18		// Betriebsstundenz�hler
#define CD_MSG_HISTORY2_START	19		// Fehlerstatistik
#define CD_MSG_HISTORY2_DATA	20		// Fehlerstatistik
#define CD_MSG_HISTORY2_DELETE	21		// Fehlerstatistik l�schen

#define CD_MSG_NOT_COMPLETE		-1
#define CD_MSG_UNKNOWN			-2


#ifdef __cplusplus
extern "C"
{
#endif

// -----------------------------------------------------------------------------
//  Funktion:       CD_Init
//  Beschreibung :  Setz die Art des Ger�tes
//  Parameter :     Zeichen f�r Ger�t 'T' f�r Terminal, 'D' f�r Diagnosesoftware
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_Init(TCHAR tchEquip);

// -----------------------------------------------------------------------------
//  Funktion:       CD_GetMsgLen
//  Beschreibung :  Liefert Anzahl Zeichen der Meldung
//  Parameter :     Meldung, L�nge
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_GetMsgLen(LPCTSTR ptszMsg, int *piLenData);

// -----------------------------------------------------------------------------
//  Funktion:       CD_GetMsgCount
//  Beschreibung :  Liefert die angeforderte Zeilenzahl
//  Parameter :     Meldung, Z�hler
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_GetMsgCount(LPCTSTR ptszMsg, int *piCount);

// -----------------------------------------------------------------------------
//  Funktion:       CD_GetMsgCheckSum
//  Beschreibung :  Liefert die in der Meldung gespeicherte Pr�fsumme
//  Parameter :     Meldung, L�nge, Pr�fsumme
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_GetMsgCheckSum(LPCTSTR ptszMsg, int iLenData, BYTE *pbyCheckSum);

// -----------------------------------------------------------------------------
//  Funktion:       CD_CalcCheckSumTotalMsg
//  Beschreibung :  berechnet die Pr�fsumme einer Meldung
//  Parameter :     Meldung, Anzahl Zeichen der Meldung samt Meldungskopf, Pr�fsumme
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_CalcCheckSumTotalMsg(LPCTSTR ptszMsg, int iLenMsg, BYTE *pbyCheckSum);

// -----------------------------------------------------------------------------
//  Funktion:       CD_CalcCheckSum
//  Beschreibung :  berechnet die Pr�fsumme einer Meldung
//  Parameter :     Meldung, Anzahl Zeichen der Meldung ohne Meldungskopf, Pr�fsumme
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_CalcCheckSum(LPCTSTR ptszMsg, int iLenMsg, BYTE *pbyCheckSum);

// -----------------------------------------------------------------------------
//  Funktion:       CD_SetMsgLen
//  Beschreibung :  setzt die L�nge Meldung an der entsprechenden Position
//  Parameter :     Meldung, L�nge
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_SetMsgLen(LPTSTR ptszMsg, int iLenData);

// -----------------------------------------------------------------------------
//  Funktion:       CD_SetCheckSum
//  Beschreibung :  setzt die Pr�fsumme einer Meldung an der entsprechenden Position
//  Parameter :     Meldung, Anzahl Zeichen Meldung ohne Meldungskopf, Pr�fsumme
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_SetCheckSum(LPTSTR ptszMsg, int iLenData, BYTE byCheckSum);

// -----------------------------------------------------------------------------
//  Funktion:       CD_GenerateConfirmMsg
//  Beschreibung :  erzeugt eine Best�tigungsmeldung
//  Parameter :     Meldung, Best�tigungsmeldung, L�nge Best�tigungsmeldung in Unicodezeichen, Anzahl Zeilen
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_GenerateConfirmMsg(LPCTSTR ptszMsg, LPTSTR ptszMsgConf, DWORD *pdwMsgConfChars, DWORD dwCount);

// -----------------------------------------------------------------------------
//  Funktion:       CD_CheckConfirmMsg
//  Beschreibung :  pr�ft eine Best�tigungsmeldung
//  Parameter :     Meldung, Typ-Orginalmeldung, Checksumme-Orginalmeldung, Anzahl Zeilen
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_CheckConfirmMsg(LPCTSTR ptszMsg, TCHAR tchType, BYTE byCheckSum, int *piCount);

// -----------------------------------------------------------------------------
//  Funktion:       CD_CheckMsg
//  Beschreibung :  pr�ft eine Meldung auf g�ltige Checksumme
//  Parameter :     Meldung
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_CheckMsg(LPCTSTR ptszMsg);

// -----------------------------------------------------------------------------
//  Funktion:       CD_CheckConfirmMsg2Msg
//  Beschreibung :  pr�ft eine Best�tigungsmeldung gegen Orginalmeldung
//  Parameter :     Best�tigungsmeldung, Orginalmeldung, Anzahl Zeilen
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_CheckConfirmMsg2Msg(LPCTSTR ptszMsgConf, LPCTSTR ptszMsgOrig, int *piCount);

// -----------------------------------------------------------------------------
//  Funktion:       CD_ParseMsg
//  Beschreibung :  pr�ft den Input-Buffer ob eine Meldung vorliegt
//  Parameter :     Lesebuffer in Bytes, Byteposition des Lesepuffers (f�r Anf�gen neuer Daten), 
//					Neue Meldung in Unicode, Typ der neuen Meldung
//  Returnwert :    TRUE..Meldung gefunden, FALSE..keine neue Meldung
BOOL CD_ParseMsg(BYTE *pbyBufIn, DWORD *dwPosIn, LPTSTR ptszMsgNew, int *piMsgType);

// -----------------------------------------------------------------------------
//  Funktion:       CD_GenerateDataMsg
//  Beschreibung :  erzeugt eine Daten Meldung
//  Parameter :     Meldung, L�nge Meldung in Bytes, Typ der neuen Meldung, Daten in Unicode, Anzahl Unicode Zeichen
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_GenerateDataMsg(LPTSTR ptszMsg, DWORD *pdwMsgBytes, int iMsgType, LPTSTR ptszData, int iDataLen);

// -----------------------------------------------------------------------------
//  Funktion:       CD_GenerateDataHeader
//  Beschreibung :  erzeugt den Kopf einer Daten Meldung
//  Parameter :     Meldung, gesamte L�nge Meldung in Unicodezeichen
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_GenerateDataHeader(LPTSTR ptszMsg, DWORD *pdwMsgChars, int iMsgType);

// -----------------------------------------------------------------------------
//  Funktion:       CD_GenerateDataTail
//  Beschreibung :  erzeugt das Ende der Daten Meldung (zus�tzlich Pr�fsumme hinzuf�gen)
//  Parameter :     Meldung, gesamte L�nge Meldung in Unicodezeichen
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_GenerateDataTail(LPTSTR ptszMsg, DWORD *pdwMsgChars);

// -----------------------------------------------------------------------------
//  Funktion:       CD_GenerateDataDate
//  Beschreibung :  F�gt das Datum als String an die aktuelle Position der Meldung an und setzt die 
//					aktuelle Position nach dem Datum, nach dem Datum wird zus�tzlich ein Komma als
//					Trennzeichen eingef�gt
//  Parameter :     Meldung, gesamte L�nge Meldung in Unicodezeichen, Datum
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_GenerateDataDate(LPTSTR ptszMsg, DWORD *pdwMsgChars, UINT32 uiDate);

// -----------------------------------------------------------------------------
//  Funktion:       CD_GenerateDataTime
//  Beschreibung :  F�gt die Uhrzeit als String an die aktuelle Position der Meldung an und setzt die 
//					aktuelle Position nach der Uhrzeit, nach der Uhrzeit wird zus�tzlich ein Komma als
//					Trennzeichen eingef�gt
//  Parameter :     Meldung, gesamte L�nge Meldung in Unicodezeichen, Datum
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_GenerateDataTime(LPTSTR ptszMsg, DWORD *pdwMsgChars, UINT32 uiTime);

// -----------------------------------------------------------------------------
//  Funktion:       CD_GenerateDataUINT32
//  Beschreibung :  F�gt dien Wert als String an die aktuelle Position der Meldung an und 
//					setzt die aktuelle Position nach dem Wert, nach dem Wert 
//					wird zus�tzlich ein Komma als Trennzeichen eingef�gt (Beispiel: Bertiebsstunden)
//  Parameter :     Meldung, gesamte L�nge Meldung in Unicodezeichen, UINT32-Wert
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_GenerateDataUINT32(LPTSTR ptszMsg, DWORD *pdwMsgChars, UINT32 uiVal);

// -----------------------------------------------------------------------------
//  Funktion:       CD_GenerateDataINT32
//  Beschreibung :  F�gt den Wert als String an die aktuelle Position der Meldung an und setzt die 
//					aktuelle Position nach dem Wert, nach dem Wert wird zus�tzlich ein Komma als
//					Trennzeichen eingef�gt (Beispiel: Drehzahl)
//  Parameter :     Meldung, gesamte L�nge Meldung in Unicodezeichen, INT32-Wert
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_GenerateDataINT32(LPTSTR ptszMsg, DWORD *pdwMsgChars, INT32 iVal);

// -----------------------------------------------------------------------------
//  Funktion:       CD_GetMsgData
//  Beschreibung :  Liefert die Daten einer Meldung
//  Parameter :     Meldung, Daten der Meldung
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_GetMsgData(LPCTSTR ptszMsg, LPTSTR ptszData);

// -----------------------------------------------------------------------------
//  Funktion:       CD_GenerateDataINT32Norm
//  Beschreibung :  F�gt den Wert mit Normierung als String an die aktuelle Position der Meldung an und setzt die 
//					aktuelle Position nach dem Wert, nach dem Wert wird zus�tzlich ein Komma als
//					Trennzeichen eingef�gt (Beispiel: Drehzahl)
//  Parameter :     Meldung, gesamte L�nge Meldung in Unicodezeichen, INT32-Wert
//  Returnwert :    TRUE..OK, FALSE..Fehler
BOOL CD_GenerateDataINT32Norm(LPTSTR ptszMsg, DWORD *pdwMsgChars, INT32 iVal, INT32 iNorm, INT32 iPreComma, INT32 iPostComma);





#ifdef __cplusplus
}
#endif

#endif
