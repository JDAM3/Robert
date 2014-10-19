
bool TranslateNextLine(CStdioFile &file, CString &key, CString &param);
bool FindSection(CStdioFile &file, CString section, bool bFromBegin = true);
void SeparateComma(CString string, CStringArray &stringArr);
void GetFleetNames(CString &strFile, CComboBox &m_ComboBox);

bool TranslateSection(CStdioFile &file, CString &section);
