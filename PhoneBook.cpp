// PhoneBook.cpp : implementation file
//

#include "stdafx.h"
#include "raildiag.h"
#include "PhoneBook.h"
#include "PhoneBookEntry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPhoneBook dialog


CPhoneBook::CPhoneBook(CWnd* pParent /*=NULL*/)
	: CDialog(CPhoneBook::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPhoneBook)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_bModified = FALSE;
	m_bSelect = FALSE;
	m_nItem = -1;
	m_bSortAsc = TRUE;;
	m_iSortCol = -1;

}


void CPhoneBook::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPhoneBook)
	DDX_Control(pDX, IDC_DELETE_ENTRY, m_CtrlDeleteEntry);
	DDX_Control(pDX, IDC_EDIT_ENTRY, m_CtrlEditEntry);
	DDX_Control(pDX, IDC_NEW_ENTRY, m_CtrlNewEntry);
	DDX_Control(pDX, IDOK, m_CtrlOk);
	DDX_Control(pDX, IDCANCEL, m_CtrlCancel);
	DDX_Control(pDX, IDC_LIST_PHONE, m_ListCtrlPhone);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_PHB_EXPORT, m_CtrlExport);
	DDX_Control(pDX, IDC_PHB_IMPORT, m_CtrlImport);
}


BEGIN_MESSAGE_MAP(CPhoneBook, CDialog)
	//{{AFX_MSG_MAP(CPhoneBook)
	ON_BN_CLICKED(IDC_EDIT_ENTRY, OnEditEntry)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PHONE, OnDblclkListPhone)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PHONE, OnClickListPhone)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_PHONE, OnColumnclickListPhone)
	ON_BN_CLICKED(IDC_NEW_ENTRY, OnNewEntry)
	ON_BN_CLICKED(IDC_DELETE_ENTRY, OnDeleteEntry)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_PHB_IMPORT, &CPhoneBook::OnBnClickedPhbImport)
	ON_BN_CLICKED(IDC_PHB_EXPORT, &CPhoneBook::OnBnClickedPhbExport)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPhoneBook message handlers

BOOL CPhoneBook::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	int				i;
	CStringArray	colArr;

	colArr.Add(pApp->m_stDlgPhoneBook.strDesc);
	colArr.Add(pApp->m_stDlgPhoneBook.strNumber);

	SetWindowText(pApp->m_stDlgPhoneBook.strTitle);
	m_CtrlNewEntry.SetWindowText(pApp->m_stDlgPhoneBook.strNewEntry);
	m_CtrlEditEntry.SetWindowText(pApp->m_stDlgPhoneBook.strEditEntry);
	m_CtrlDeleteEntry.SetWindowText(pApp->m_stDlgPhoneBook.strDeleteEntry);
	m_CtrlOk.SetWindowText(pApp->m_stDlgPhoneBook.strOk);
	m_CtrlCancel.SetWindowText(pApp->m_stDlgPhoneBook.strCancel);
	m_CtrlImport.SetWindowText(pApp->m_stDlgPhoneBook.strImport);
	m_CtrlExport.SetWindowText(pApp->m_stDlgPhoneBook.strExport);

	// this code only works for a report-mode list view
	ASSERT(m_ListCtrlPhone.GetStyle() & LVS_REPORT);

	// Gain a reference to the list control itself

	// The other InsertColumn() override requires an initialized
	// LVCOLUMN structure.
	LVCOLUMN col;

   	col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	col.fmt = LVCFMT_LEFT;
	col.cx = 80;

	for (i = 0; i < 2; i++)
	{
		if (i == 0)
		{
			col.cx = 200;
		}
		else if (i == 1)
		{
			col.cx = 200;
		}
		else
		{
			col.cx = 80;
		}

		col.iSubItem = i;
		col.pszText = colArr[i].GetBuffer(1);
		m_ListCtrlPhone.InsertColumn(i, &col);
		colArr[i].ReleaseBuffer();
	}

	for (i = m_strarrDescription.GetSize() -1 ; i >= 0; i--)
	{
		LV_ITEM	lvi;

		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.iItem = i;
		lvi.iSubItem = 0;
		lvi.lParam = i;
		lvi.pszText = _T("xx");
		m_ListCtrlPhone.InsertItem(&lvi);
	}

	UpdateList();
	UpdateButtons();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPhoneBook::OnEditEntry() 
{
	TRACE(_T("CPhoneBook::OnEditEntry()\n"));

	EditEntry();
	UpdateList();
	UpdateButtons();
	
}

void CPhoneBook::OnClickListPhone(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TRACE(_T("CPhoneBook::OnClickListPhone()\n"));

	POSITION pos = m_ListCtrlPhone.GetFirstSelectedItemPosition();
	if (pos == NULL)
		TRACE0("No items were selected!\n");
	else
	{
		m_nItem = m_ListCtrlPhone.GetNextSelectedItem(pos);
		TRACE1("Item %d was selected!\n", m_nItem);
	}

	UpdateButtons();

	*pResult = 0;
}

void CPhoneBook::OnDblclkListPhone(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TRACE(_T("CPhoneBook::OnDblclkListPhone()\n"));

	if (m_bSelect)
	{
		CDialog::OnOK();
	}
	else
	{
		EditEntry();
		UpdateList();
	}


	*pResult = 0;
}

void CPhoneBook::OnColumnclickListPhone(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	TRACE(_T("CPhoneBook::OnColumnclickListPhone()\n"));
	TRACE(_T("Item %d, Subitem %d\n"), pNMListView->iItem, pNMListView->iSubItem);
	
	m_bModified = TRUE;
	if (m_iSortCol == pNMListView->iSubItem)
	{
		m_bSortAsc = !m_bSortAsc;
	}

	m_iSortCol = pNMListView->iSubItem;

	m_ListCtrlPhone.SortItems(CompareProc, (LPARAM)this);

	*pResult = 0;
}

void CPhoneBook::EditEntry()
{
	CString strDescriptionOld, strNumberOld;

	POSITION pos = m_ListCtrlPhone.GetFirstSelectedItemPosition();
	if (pos == NULL)
		TRACE0("No items were selected!\n");
	else
	{
		LV_ITEM	lvi;
		int		iIdx;
		int nItem = m_ListCtrlPhone.GetNextSelectedItem(pos);
		TRACE1("Item %d was selected!\n", nItem);

		CPhoneBookEntry	PhoneBookEntry;

		lvi.mask = LVIF_PARAM;
		lvi.iItem = nItem;
		lvi.iSubItem = 0;

		iIdx = m_ListCtrlPhone.GetItem(&lvi);

		strDescriptionOld = PhoneBookEntry.m_strDescription = m_strarrDescription.GetAt(lvi.lParam);
		strNumberOld = PhoneBookEntry.m_strNumber = m_strarrNumber.GetAt(lvi.lParam);

		if (PhoneBookEntry.DoModal() == IDOK)
		{
			lvi.mask = LVIF_PARAM;
			lvi.iItem = nItem;
			lvi.iSubItem = 0;

			iIdx = m_ListCtrlPhone.GetItem(&lvi);

			// update Application data
			m_strarrDescription.SetAt(lvi.lParam, PhoneBookEntry.m_strDescription);
			m_strarrNumber.SetAt(lvi.lParam, PhoneBookEntry.m_strNumber);

			m_ListCtrlPhone.SetItemText(nItem, 0, m_strarrDescription.GetAt(lvi.lParam));
			m_ListCtrlPhone.SetItemText(nItem, 1, m_strarrNumber.GetAt(lvi.lParam));

			CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
			pApp->UpdateDescNumber(strDescriptionOld, strNumberOld, 
				PhoneBookEntry.m_strDescription, PhoneBookEntry.m_strNumber);


			// mark es modified
			m_bModified = TRUE;
		}
	}

}

BOOL CPhoneBook::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_strarrDescription.RemoveAll();
	m_strarrNumber.RemoveAll();

	for (int i = 0; i < m_ListCtrlPhone.GetItemCount(); i++)
	{
		m_strarrDescription.Add(m_ListCtrlPhone.GetItemText(i, 0));
		m_strarrNumber.Add(m_ListCtrlPhone.GetItemText(i, 1));
	}

	return CDialog::DestroyWindow();
}

void CPhoneBook::OnNewEntry() 
{
	// TODO: Add your control notification handler code here
	CPhoneBookEntry	PhoneBookEntry;

	PhoneBookEntry.m_strDescription = _T("");
	PhoneBookEntry.m_strNumber = _T("");

	if (PhoneBookEntry.DoModal() == IDOK)
	{
		// update Application data
		m_strarrDescription.Add(PhoneBookEntry.m_strDescription);
		m_strarrNumber.Add(PhoneBookEntry.m_strNumber);

		// update List
		int			iIdx;
		LV_ITEM		lvi;

		iIdx = m_strarrDescription.GetSize() - 1;
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.iItem = iIdx;
		lvi.iSubItem = 0;
		lvi.lParam = iIdx;
		lvi.pszText = _T("xx");
		m_ListCtrlPhone.InsertItem(&lvi);

		m_ListCtrlPhone.SetItemText(iIdx, 0, m_strarrDescription.GetAt(iIdx));
		m_ListCtrlPhone.SetItemText(iIdx, 1, m_strarrNumber.GetAt(iIdx));

		// mark es modified
		m_bModified = TRUE;
		m_iSortCol = -1; // neue sortierung mit alten kriterien ermöglichen
	}

	UpdateList();
}

void CPhoneBook::UpdateList()
{
	int				i;

	// texte updaten
	for (i = 0; i < m_ListCtrlPhone.GetItemCount(); i++)
	{
		LV_ITEM	lvi;
		int		iIdx;

		lvi.mask = LVIF_PARAM;
		lvi.iItem = i;
		lvi.iSubItem = 0;

		iIdx = m_ListCtrlPhone.GetItem(&lvi);

		m_ListCtrlPhone.SetItemText(i, 0, m_strarrDescription.GetAt(lvi.lParam));
		m_ListCtrlPhone.SetItemText(i, 1, m_strarrNumber.GetAt(lvi.lParam));
	}
//	m_ListCtrlPhone.Invalidate(TRUE);
}

void CPhoneBook::UpdateButtons()
{
	if (m_bSelect)
	{
		m_CtrlEditEntry.EnableWindow(FALSE);
		m_CtrlDeleteEntry.EnableWindow(FALSE);
		m_CtrlNewEntry.EnableWindow(FALSE);

		if (m_ListCtrlPhone.GetSelectedCount() > 0)
		{
			m_CtrlOk.EnableWindow(TRUE);
		}
		else
		{
			m_CtrlOk.EnableWindow(FALSE);
		}

	}
	else
	{
		if (m_ListCtrlPhone.GetSelectedCount() > 0)
		{
			m_CtrlEditEntry.EnableWindow(TRUE);
			m_CtrlDeleteEntry.EnableWindow(TRUE);
		}
		else
		{
			m_CtrlEditEntry.EnableWindow(FALSE);
			m_CtrlDeleteEntry.EnableWindow(FALSE);
		}
	}
}

void CPhoneBook::OnDeleteEntry() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_ListCtrlPhone.GetFirstSelectedItemPosition();
	if (pos == NULL)
		TRACE0("No items were selected!\n");
	else
	{
		int nItem = m_ListCtrlPhone.GetNextSelectedItem(pos);
		TRACE1("Item %d was selected!\n", nItem);

		m_ListCtrlPhone.DeleteItem(nItem);

		// mark es modified
		m_bModified = TRUE;

		UpdateList();
		UpdateButtons();
	}
	
}

int CALLBACK CPhoneBook::CompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	// lParamSort contains a pointer to the list view control.
	// The lParam of an item is just its index.
	TRACE(_T("CPhoneBook::CompareProc lParam1 %d, lParam2 %d\n"), lParam1, lParam2);

	CPhoneBook* pPhoneBook = (CPhoneBook*) lParamSort;
	int			iResult;

	CString    strItem1;
	CString    strItem2;

	if (pPhoneBook->m_iSortCol == 0)
	{
		strItem1 = pPhoneBook->m_strarrDescription.GetAt(lParam1);
		strItem2 = pPhoneBook->m_strarrDescription.GetAt(lParam2);
	}
	else
	{
		strItem1 = pPhoneBook->m_strarrNumber.GetAt(lParam1);
		strItem2 = pPhoneBook->m_strarrNumber.GetAt(lParam2);
	}

	strItem1.MakeUpper();
	strItem2.MakeUpper();

	if (pPhoneBook->m_bSortAsc)
	{
		iResult = _tcscmp(strItem1, strItem2);
	}
	else
	{
		iResult = _tcscmp(strItem2, strItem1);
	}

	return iResult;
}


void CPhoneBook::OnBnClickedPhbImport()
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CString			strFile;
	CStringArray	strarrDescription, strarrNumber;

	strFile = _T("Phone_Book.rpb");

	CFileDialog	FileDialog(TRUE, _T("rpb"), strFile, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
						   _T("Import Files (*.rpb)|*.rpb|All Files (*.*)|*.*||"));
	int			iStatus;
	CString		strPath;

	FileDialog.m_ofn.lpstrInitialDir = pApp->m_strInitialDirectory;

	FileDialog.m_ofn.lpstrTitle = _T("PhoneBook Import");
	iStatus = FileDialog.DoModal();
	if (iStatus == IDOK)
	{
//		TRACE(_T("Exporting file \n"));
		strPath = FileDialog.GetPathName();

		BOOL		bRet = TRUE;
		CStdioFile	file;
		CWaitCursor	wait;

		if (file.Open(strPath, CFile::modeRead | CFile::typeText))
		{
			bRet = pApp->ReadPhoneBookIni(file, strarrDescription, strarrNumber);
			file.Close();
		}
		else
			bRet = FALSE;

		if (!bRet)
		{
			CString	csMsg;

			csMsg.Format(_T("Importing file failed: %s"), strPath);
			AfxMessageBox(csMsg);
		}

		int	i, j, k;
		bool found, same;
		CString strDescription;
		for (i = 0; i < strarrDescription.GetCount(); i++)
		{
			found = false;
			same = false;
			strDescription = strarrDescription.GetAt(i);

			for (j = 0; !found && j < m_strarrDescription.GetCount(); j++)
			{
				if (m_strarrDescription.GetAt(j) == strDescription)
				{
					found = true;
					if (m_strarrNumber.GetAt(j) == strarrNumber.GetAt(i))
					{
						same = true;
					}
				}
			}

			// neuen Description erzeugen und erneut suchen
			for (k = 1; k < 10 && found && !same; k++)
			{
				if (k == 1)
				{
					strDescription.Format(_T("%s copy"), strarrDescription.GetAt(i));
				}
				else
				{
					strDescription.Format(_T("%s copy (%d)"), strarrDescription.GetAt(i), k);
				}

				found = same = false;
				for (j = 0; !found && j < m_strarrDescription.GetCount(); j++)
				{
					if (m_strarrDescription.GetAt(j) == strDescription)
					{
						found = true;
						if (m_strarrNumber.GetAt(j) == strarrNumber.GetAt(i))
						{
							same = true;
						}
					}
				}
			}

			// eintrag hinzufügen
			if (!found)
			{ 
				m_strarrDescription.Add(strDescription);
				m_strarrNumber.Add(strarrNumber.GetAt(i));

				// update List
				int			iIdx;
				LV_ITEM		lvi;

				iIdx = m_strarrDescription.GetSize() - 1;
				lvi.mask = LVIF_TEXT | LVIF_PARAM;
				lvi.iItem = iIdx;
				lvi.iSubItem = 0;
				lvi.lParam = iIdx;
				lvi.pszText = _T("xx");
				m_ListCtrlPhone.InsertItem(&lvi);

				m_ListCtrlPhone.SetItemText(iIdx, 0, m_strarrDescription.GetAt(iIdx));
				m_ListCtrlPhone.SetItemText(iIdx, 1, m_strarrNumber.GetAt(iIdx));

				// mark es modified
				m_bModified = TRUE;
				m_iSortCol = -1; // neue sortierung mit alten kriterien ermöglichen
			}
		}

		UpdateList();
		UpdateButtons();
	}
}

void CPhoneBook::OnBnClickedPhbExport()
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CString			strFile;

	strFile = _T("Phone_Book.rpb");

	CFileDialog	FileDialog(FALSE, _T("rpb"), strFile, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
						   _T("Export Files (*.rpb)|*.rpb|All Files (*.*)|*.*||"));
	int			iStatus;
	CString		strPath;

	FileDialog.m_ofn.lpstrInitialDir = pApp->m_strInitialDirectory;

	FileDialog.m_ofn.lpstrTitle = _T("PhoneBook Export");
	iStatus = FileDialog.DoModal();
	if (iStatus == IDOK)
	{
//		TRACE(_T("Exporting file \n"));
		strPath = FileDialog.GetPathName();

		BOOL		bRet = TRUE;
		CStdioFile	file;
		CWaitCursor	wait;

		if (file.Open(strPath, CFile::modeWrite | CFile::modeCreate | CFile::typeText))
		{
			bRet = pApp->WritePhoneBookIni(file, m_strarrDescription, m_strarrNumber);
			file.Close();
		}
		else
			bRet = FALSE;

		if (!bRet)
		{
			CString	csMsg;

			csMsg.Format(_T("Exporting file failed: %s"), strPath);
			AfxMessageBox(csMsg);
		}
	}
}
