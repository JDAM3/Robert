

#include "stdafx.h"
#include "FileIO.h"

bool TranslateNextLine(CStdioFile &file, CString &key, CString &param)
{
	CString	line;
	DWORD	dwFilePos = (DWORD)file.GetPosition();

    while (file.ReadString(line))
	{
		line.TrimLeft();
		line.TrimRight();

		if (line.IsEmpty())
			continue;
		if (line[0] == TCHAR(';'))
			continue;
		if (line[0] == TCHAR('['))
		{
			file.Seek(dwFilePos, CFile::begin);
			return false;
		}

        // "line" ist jetzt Keyword oder Datenzeile
        int posEq = line.Find(_T("="));
        if (posEq != -1)
        {
            // Keyword
            param = line.Right(line.GetLength() - posEq - 1);
            param.TrimLeft();
            param.TrimRight();

            key = line.Left(posEq);
            key.TrimLeft();
            key.TrimRight();

			return true;
		}

		dwFilePos = (DWORD)file.GetPosition();
	}

	return false;
}

bool FindSection(CStdioFile &file, CString section, bool bFromBegin)
{
    CString line, sec;

    sec = _T("[") + section + _T("]");

	if (bFromBegin)
		file.SeekToBegin();
    while (file.ReadString(line))
    {
		if (line.GetLength() >= 1 && line.Left(1) == 0xFEFF)
		{
			line = line.Right(line.GetLength() - 1);
		}
        line.MakeUpper();
        line.TrimLeft();

        if (line.Find(sec) == 0)
            return true;
    }

    return false;
}

void SeparateComma(CString string, CStringArray &stringArr)
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
            string = string.Right(length - pos - 1);
            length = string.GetLength();
            pos = 0;
        }
        else
        {
            pos++;
        }
    }
}

BOOL FindTitle(CStdioFile &file)
{
    CString line, sec;

    sec = _T("[") ;

	file.SeekToBegin();
    while (file.ReadString(line))
    {
		if (line.GetLength() >= 1 && line.Left(1) == 0xFEFF)
		{
			line = line.Right(line.GetLength() - 1);
		}
        line.MakeUpper();
        line.TrimLeft();

        if (line.Find(sec) == 0)
            return true;
    }

    return false;
}

void GetFleetNames(CString &strFile, CComboBox &m_ComboBox )
{
	CStdioFile	file;
	CString strLine, strSec;
	int i;

	if (m_ComboBox.GetCount() > 0)
		m_ComboBox.ResetContent();
	
	if (file.Open(strFile, CFile::modeRead | CFile::typeBinary))
	{

	    strSec = _T("[");
		i = 0;
		file.SeekToBegin();
	    while (file.ReadString(strLine))
	    {
			if (strLine.GetLength() >= 1 && strLine.Left(1) == 0xFEFF)
			{
				strLine = strLine.Right(strLine.GetLength() - 1);
			}	

			if (strLine.Find(strSec) == 0)
			{
				strLine = strLine.Right(strLine.GetLength() - 1);
				strLine = strLine.Left(strLine.GetLength() - 2);
				m_ComboBox.InsertString(-1, strLine);
			}
		}
		file.Close();
	}

    return;
}

bool TranslateSection(CStdioFile &file, CString &section)
{
	CString	line;

    while (file.ReadString(line))
	{
		line.TrimLeft();
		line.TrimRight();

		if (line.IsEmpty())
			continue;
		if (line[0] == TCHAR(';'))
			continue;
		if (line[0] == TCHAR('[') || line[1] == '[')
		{
			int posEnd = line.Find(_T("]"));
			if (-1 != posEnd)
			{
				section = line.Left(posEnd);
				section.SetAt(0, ' ');
				if (section[1] == '[')
				{
					section.SetAt(1, ' ');
				}
				section.Trim();

				return true;
			}
		}
	}

	return false;
}

