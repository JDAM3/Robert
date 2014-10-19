// Util.h: interface for the CUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTIL_H__A16A3A07_5B43_4190_B5FF_70AE3C0F5992__INCLUDED_)
#define AFX_UTIL_H__A16A3A07_5B43_4190_B5FF_70AE3C0F5992__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

char *UnicodeString2CharString(CString strIn);

class CUtil  
{
public:
	CUtil();
	virtual ~CUtil();
public:
	void SeparateComma(CString string, CStringArray &stringArr);

};

#endif // !defined(AFX_UTIL_H__A16A3A07_5B43_4190_B5FF_70AE3C0F5992__INCLUDED_)
