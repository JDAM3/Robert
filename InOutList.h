// InOutList.h: interface for the CInOutList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INOUTLIST_H__A2248E25_5AAE_460B_84A7_4431655242E4__INCLUDED_)
#define AFX_INOUTLIST_H__A2248E25_5AAE_460B_84A7_4431655242E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CInOutList  
{
public:
	void ResetRetryCount();
	int SetCount4NewSend(void);
	BOOL AllDataRecived(void);
	BOOL SetRecived(int iReciveIndex, BOOL *pbSendAnother);
	void GetNext2Send(int *piSendIndex, BOOL *pbSendAnother);
	void DeInit(void);
	void Init(int iTopMax, int iMaxWaiting, int iMarkAsRead);
	CInOutList();
	virtual ~CInOutList();

protected:
	int			m_iWaitingMax;
	int			m_iWaiting;
	int			m_iSendCount;
	int			m_iIdxBottomAct;
	int			m_iIdxTopAct;
	int			m_iIdxTopMax;
	int			m_iNewSend;
	CUIntArray	m_uiarrState;
	BOOL		m_bInit;
};

#endif // !defined(AFX_INOUTLIST_H__A2248E25_5AAE_460B_84A7_4431655242E4__INCLUDED_)
