// =========================================================================
//                      (C) Kinz Elektronik
//
// Produkt:      QLib
//
// Modulname:    ComLib.h
//
// Funktion:     COM-Bibliothek
//
// Autor:
//      Name:    F. Kinz
//      Datum:   2.8.2000
//
// Updates:
//      Version:	1.0
//      Name:		R. Bernecker
//      Datum:		23.01.2002   
//      Grund:		Simulation schreibt Werte auf Serielle Schnittstelle 
//
//      Version:	1.1
//      Name:		R. Bernecker
//      Datum:		05.06.2002   
//      Grund:		Funktionen um DTR Signal ein und auszuschalten
//
// Hardware:            PC
// Betriebsystem:       Windows NT/2000
// Compiler:            Visual C++
//
// =========================================================================


#ifndef _COMLIB_H
#define _COMLIB_H


#ifdef __cplusplus
extern "C"
{
#endif

// --------------------------------------------------------------------------
//  Funktion:       ComOpen()
//  Beschreibung :  Schnittstelle öffnen. 
//  Parameter:      byComNr : Schnittstellen-Nummer (0..9)
//                  Schnittstellenparameter
//  Returnwert :     0  Funktion in Bearbeitung
//					 1  Funktion erfolgreich abgeschlossen
//					-1	Fehler

int ComOpen(BYTE  byComNr,		// 1 - 0(10)
			 BYTE  byMode,		// 0..RS232 ohne HS, 1..RS232 mit HS
			 DWORD dwBaud,		// 300 - 115200
			 BYTE  byDataSize,	// 4 - 8
			 BYTE  byStopBits,	// 1, 2
			 BYTE  byParity,	// 0..no, 1..ungerade, 2..gerade
			 DWORD dwBufferSize	// Puffergrösse Ein/Aus in Bytes
			);


// --------------------------------------------------------------------------
//  Funktion:       ComClose()
//  Beschreibung :  Schnittstelle schliessen
//  Parameter:      byComNr : Schnittstellen-Nummer (0..9)
//  Returnwert :     0  Funktion in Bearbeitung
//					 1  Funktion erfolgreich abgeschlossen
//					-1	Fehler

int ComClose(BYTE byComNr);


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

DWORD ComSend(BYTE byComNr, LPVOID lpvBuffer, DWORD dwLength, DWORD *lpdwBytesWritten);


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

DWORD ComGetStatus(BYTE byComNr, DWORD *lpdwOutCount, DWORD *lpdwInCount);


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

DWORD ComReceive(BYTE byComNr, LPVOID lpvBuffer, DWORD dwMaxLength, DWORD *lpdwBytesRead);


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

DWORD ComGet485Error(BYTE byComNr);


// --------------------------------------------------------------------------
//  Funktion:       ComClear()
//  Beschreibung :  Der Inhalt des Eingabe- und Ausgabepuffers wird gelöscht
//  Parameter:      byComNr : Schnittstellen-Nummer (0..9)
//	Returnwert:		 1  Funktion erfolgreich abgeschlossen
//					-1	Fehler

int ComClear(BYTE byComNr);


// --------------------------------------------------------------------------
//  Funktion:       ComActivateDTR()
//  Beschreibung :  Setzen und löschen des DTR-Signales
//  Parameter:      byComNr : Schnittstellen-Nummer (0..9)
//					bActivate : Signal setzen (TRUE) oder löschen (FALSE)
//	Returnwert:		 1  Funktion erfolgreich abgeschlossen
//					-1	Fehler

int ComActivateDTR(BYTE byComNr, BOOL bActivate);


// --------------------------------------------------------------------------
//  Funktion:       ComChangeBaud()
//  Beschreibung :  Ändern der Baudrate einer seriellen Schnittstelle
//  Parameter:      byComNr : Schnittstellen-Nummer (0..9)
//					iBaudRate : neue Baudrate
//	Returnwert:		 1  Funktion erfolgreich abgeschlossen
//					-1	Fehler

int ComChangeBaud(BYTE byComNr, int iBaudRate);


// -----------------------------------------------------------------------------
//  Funktion:       ComLibIinit()
//  Beschreibung :  Init-Funktion
//  Parameter :     kein
//  Returnwert :    kein

void ComLibInit(void);


// -----------------------------------------------------------------------------
//  Funktion:       ComLibDeinit()
//  Beschreibung :  Deinit-Funktion
//  Parameter :     kein
//  Returnwert :    kein

void ComLibDeinit(void);


#ifdef __cplusplus
}
#endif

#endif
