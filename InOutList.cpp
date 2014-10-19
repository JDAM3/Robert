// InOutList.cpp: implementation of the CInOutList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "raildiag.h"
#include "InOutList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define STATE_NOT_SEND		0xFFFFFFFF
#define STATE_DATA_RECIVED	0xFFFFFFFE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInOutList::CInOutList()
{
	m_bInit = FALSE;
	m_iNewSend = 0;
}

CInOutList::~CInOutList()
{
	m_uiarrState.RemoveAll();
}

void CInOutList::Init(int iTopMax, int iMaxWaiting, int iMarkAsRead)
{
	int		i;

	if (m_bInit)
	{
		DeInit();
	}
	ASSERT (!m_bInit); // DeInit first

	m_iNewSend = 0;
	m_bInit = TRUE;

	m_iIdxTopMax = iTopMax - 1;
	m_uiarrState.SetSize(iTopMax);
	m_iIdxTopAct = -1;
	m_iIdxBottomAct = 0;

	// alle als ungesendet markieren
	for (i = 0; i < iTopMax; i++)
	{
		m_uiarrState[i] = STATE_NOT_SEND;
	}
	
	// bestimmte als gelesen markieren
	for (i = 0; i < iMarkAsRead; i++)
	{
		m_uiarrState[i] = STATE_DATA_RECIVED;
		m_iIdxTopAct = i;
		m_iIdxBottomAct = i;
	}

	m_iWaiting = 0;
	m_iWaitingMax = iMaxWaiting;
	m_iSendCount = 0;

}

void CInOutList::DeInit()
{
	m_bInit = FALSE;

	m_uiarrState.RemoveAll();
}

void CInOutList::GetNext2Send(int *piSendIndex, BOOL *pbSendAnother)
{
	int		i;

	if (!m_bInit)
	{
		*piSendIndex = -1;
		*pbSendAnother = FALSE;
		return;
	}

	ASSERT(m_bInit);

	// neue Datensätze sind anzufordern
	if (m_iWaiting < m_iWaitingMax)
	{
		if (m_iIdxTopAct - m_iIdxBottomAct > m_iWaitingMax + 2) // Sätze noch nicht erhalten (ausständig in Reihenfolge)
		{
			for (i = m_iIdxBottomAct; i <= m_iIdxTopAct - m_iWaitingMax; i++)
			{
				if (m_uiarrState[i] != STATE_DATA_RECIVED && 
					m_uiarrState[i] < (UINT)(m_iSendCount - (m_iWaitingMax - 1))) // Satz neu anfordern
				{
					m_iSendCount++;
					m_uiarrState[i] = m_iSendCount;
					m_iWaiting++;

					if (m_iWaiting < m_iWaitingMax)
					{
						*pbSendAnother = TRUE;
					}
					*piSendIndex = i + 1;
					TRACE(_T("CInOutList::GetNext2Send Pos repeat old # %d, another %d\n"), *piSendIndex, *pbSendAnother);
					return;
				}
			}
		}

/*
		// prüfen ob Sätze neu angefordert werden müssen
		if (m_iIdxTopAct - m_iIdxBottomAct > 1) // Sätze noch nicht erhalten (ausständig in Reihenfolge)
		{
			for (i = m_iIdxBottomAct; i <= m_iIdxTopAct; i++)
			{
				if (m_uiarrState[i] != STATE_DATA_RECIVED && 
					m_uiarrState[i] < (UINT)(m_iSendCount - (m_iWaitingMax - 1))) // Satz neu anfordern
				{
					m_iSendCount++;
					m_uiarrState[i] = m_iSendCount;
					m_iWaiting++;

					if (m_iWaiting < m_iWaitingMax)
					{
						*pbSendAnother = TRUE;
					}
					*piSendIndex = i + 1;
					TRACE(_T("CInOutList::GetNext2Send Pos repeat old 2 # %d, another %d\n"), *piSendIndex, *pbSendAnother);
					return;
				}
			}
		}
*/

		if (m_iIdxTopAct < m_iIdxTopMax) // weiteren Satz anfordern
		{
			m_iSendCount++;
			m_iIdxTopAct++;
			m_uiarrState[m_iIdxTopAct] = m_iSendCount;
			m_iWaiting++;

			if (m_iWaiting <= m_iWaitingMax)
			{
				*pbSendAnother = TRUE;
			}
			*piSendIndex = m_iIdxTopAct + 1;
			TRACE(_T("CInOutList::GetNext2Send Pos get next # %d, another %d\n"), *piSendIndex, *pbSendAnother);
			return;
		}
		else // noch nicht erhaltene Datensätze neu anfordern
		{
			for (i = m_iIdxBottomAct; i <= m_iIdxTopAct; i++)
			{
				if (m_uiarrState[i] != STATE_DATA_RECIVED && 
					m_uiarrState[i] < (UINT)(m_iSendCount - (m_iWaitingMax - 1))) // Satz neu anfordern
				{
					m_iSendCount++;
					m_uiarrState[i] = m_iSendCount;
					m_iWaiting++;

					if (m_iWaiting < m_iWaitingMax)
					{
						*pbSendAnother = TRUE;
					}
					*piSendIndex = i + 1;
					TRACE(_T("CInOutList::GetNext2Send Pos get last # %d, another %d\n"), *piSendIndex, *pbSendAnother);
					return;
				}
			}

//			m_iSendCount++; // damit die letzten Sätze eventuell neu angefordert werden
		}
	}


	*piSendIndex = -1;	// keinen Eintrag anfordern
	*pbSendAnother = FALSE;
}

BOOL CInOutList::SetRecived(int iReciveIndex, BOOL *pbSendAnother)
{
	int		iIdxRecive;
	int		i;
	BOOL	bEnd;
	BOOL	bNewData = FALSE;

	if (!m_bInit)
	{
		*pbSendAnother = FALSE;
		return bNewData;
	}

	ASSERT(m_bInit);

	m_iNewSend = 0;
	iIdxRecive = iReciveIndex - 1;

	m_iWaiting--;
	
	if (iIdxRecive <= m_iIdxTopMax)
	{
		if (m_uiarrState[iIdxRecive] != STATE_DATA_RECIVED)
		{
			m_uiarrState[iIdxRecive] = STATE_DATA_RECIVED;
			bNewData = TRUE;
		}

		if (m_iIdxBottomAct <= iIdxRecive)
		{
			bEnd = FALSE;

			for (i = m_iIdxBottomAct; !bEnd && i <= m_iIdxTopMax; i++)
			{
				if (m_uiarrState[i] == STATE_DATA_RECIVED) // Satz als vorhanden markieren
				{
					m_iIdxBottomAct = i;
				}
				else
				{
					bEnd = TRUE;
				}
			}
		}
	}

	return bNewData;
}

BOOL CInOutList::AllDataRecived()
{
	if (m_iIdxBottomAct == m_iIdxTopMax)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int CInOutList::SetCount4NewSend()
{
	m_iWaiting = 0;
	m_iSendCount += m_iWaitingMax;
	m_iNewSend ++;

	return m_iNewSend;
}

void CInOutList::ResetRetryCount()
{
	m_iNewSend = 0;
}
