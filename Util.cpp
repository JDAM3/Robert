// Util.cpp: implementation of the CUtil class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "raildiag.h"
#include "Util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUtil::CUtil()
{

}

CUtil::~CUtil()
{

}

void CUtil::SeparateComma(CString string, CStringArray &stringArr)
{
    int length, pos = 0;

    length = string.GetLength();
    while (!string.IsEmpty())
    {
        if ((pos == length) || (string.GetAt(pos) == ','))
        {
            CString tmp;

            tmp = string.Left(pos);
            tmp.TrimLeft();
            tmp.TrimRight();
            stringArr.Add(tmp);

			if (length - pos - 1 > 0)
			{
				string = string.Right(length - pos - 1);
			}
			else
			{
				string.Empty();
			}
            length = string.GetLength();
            pos = 0;
        }
        else
        {
            pos++;
        }
    }
}

char *UnicodeString2CharString(CString strIn)
{
	WCHAR	*szuBuf;
	char	*szBuf;
	int		iConvStatus;
	int		iLen;
	CString	str;
		
	iLen = strIn.GetLength();

	szuBuf = new WCHAR[iLen+1];
	szBuf = new char[iLen+1];

	for (int i = 0; i < iLen; i++)
		szuBuf[i] = strIn.GetAt(i);
	szuBuf[iLen] = '\0';

	iConvStatus = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, szuBuf, -1, szBuf, iLen+1, NULL, NULL);
	if (iConvStatus != iLen+1)
	{
		szBuf[0] = '\0';
	}

	delete szuBuf;

	return szBuf;
}

