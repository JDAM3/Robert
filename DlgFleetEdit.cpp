// DlgFleetEdit.cpp : implementation file
//

#include "stdafx.h"
#include "RailDiag.h"
#include "DlgFleetEdit.h"
#include "PhoneBook.h"
#include "DlgFleetNew.h"
#include "FileIO.h"

// CDlgFleetEdit dialog

IMPLEMENT_DYNAMIC(CDlgFleetEdit, CDialog)

	CStdioFile	file;
	
	CString		strFile, strFileNew;
	CStringArray	colArrTel, colArrFleet;
	CString		strMess1;
	CString		strMess2;


CDlgFleetEdit::CDlgFleetEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFleetEdit::IDD, pParent)

	, m_strFleetPhoneBookSt(_T(""))
	, m_strFleetFleetSt(_T(""))
{

}

CDlgFleetEdit::~CDlgFleetEdit()
{
}

void CDlgFleetEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SET_LOK, m_CtrlAddLok);
	DDX_Control(pDX, IDC_RESET_LOK, m_CtrlDelLok);
	DDX_Control(pDX, ID_DEL, m_CtrlDelFleet);
	DDX_Control(pDX, ID_NEW, m_CtrlNewFleet);
	DDX_Control(pDX, IDC_FLEET_NAME, m_CtrlComboFleetName);
	DDX_Control(pDX, IDC_PHONELIST_FLEET, m_CtrlListPhoneBook);
	DDX_Control(pDX, IDC_LIST_FLEET, m_CtrlListFleet);
	DDX_Control(pDX, IDOK, m_CtrlOk);
	DDX_Control(pDX, IDCANCEL, m_CtrlCancel);
	DDX_Control(pDX, ID_FLEET_IMPORT, m_CtrlImport);
	DDX_Control(pDX, ID_FLEET_EXPORT, m_CtrlExport);
	DDX_Text(pDX, IDC_ST_FLEET_PHONE_BOOK, m_strFleetPhoneBookSt);
	DDX_Text(pDX, IDC_ST_FLEET, m_strFleetFleetSt);
}


BEGIN_MESSAGE_MAP(CDlgFleetEdit, CDialog)
	ON_BN_CLICKED(ID_NEW, &CDlgFleetEdit::OnBnClickedNew)
	ON_BN_CLICKED(IDC_SET_LOK, &CDlgFleetEdit::OnBnClickedSetLok)
	ON_BN_CLICKED(IDC_RESET_LOK, &CDlgFleetEdit::OnBnClickedResetLok)
	ON_CBN_CLOSEUP(IDC_FLEET_NAME, &CDlgFleetEdit::OnCbnCloseupFleetName)
	ON_CBN_SELCHANGE(IDC_FLEET_NAME, &CDlgFleetEdit::OnCbnSelchangeFleetName)
	ON_BN_CLICKED(ID_DEL, &CDlgFleetEdit::OnBnClickedDel)
	ON_BN_CLICKED(ID_FLEET_IMPORT, &CDlgFleetEdit::OnBnClickedFleetImport)
	ON_BN_CLICKED(ID_FLEET_EXPORT, &CDlgFleetEdit::OnBnClickedFleetExport)
END_MESSAGE_MAP()


// CDlgFleetEdit message handlers

BOOL CDlgFleetEdit::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	int				i;

	//array list tel-book
	colArrTel.Add(pApp->m_stDlgFleetEdit.strDesc);
	colArrTel.Add(pApp->m_stDlgFleetEdit.strTelNumber);

	//array list fleet
	colArrFleet.Add(pApp->m_stDlgFleetEdit.strRowNumber);
	colArrFleet.Add(pApp->m_stDlgFleetEdit.strDesc);
	colArrFleet.Add(pApp->m_stDlgFleetEdit.strTelNumber);

	//texts
	m_strFleetPhoneBookSt = pApp->m_stDlgFleetEdit.strListPhoneBook;
	m_strFleetFleetSt = pApp->m_stDlgFleetEdit.strListFleet;
	SetWindowText(pApp->m_stDlgFleetEdit.strTitle);
	m_CtrlDelFleet.SetWindowText(pApp->m_stDlgFleetEdit.strFleetDel);
	m_CtrlNewFleet.SetWindowText(pApp->m_stDlgFleetEdit.strFleetNew);
	m_CtrlOk.SetWindowText(pApp->m_stDlgFleetEdit.strOk);
	m_CtrlCancel.SetWindowText(pApp->m_stDlgFleetEdit.strCancel);
	m_CtrlImport.SetWindowText(pApp->m_stDlgFleetEdit.strImport);
	m_CtrlExport.SetWindowText(pApp->m_stDlgFleetEdit.strExport);
	strMess1 = pApp->m_stDlgFleetEdit.strMess1;
	strMess2 = pApp->m_stDlgFleetEdit.strMess2;


	// PHONE BOOK
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
			col.cx = 145;
		}
		else if (i == 1)
		{
			col.cx = 150;
		}
		else
		{
			col.cx = 10;
		}

		col.iSubItem = i;
		col.pszText = colArrTel[i].GetBuffer(1);
		m_CtrlListPhoneBook.InsertColumn(i, &col);
		colArrTel[i].ReleaseBuffer();
	}

	for (i = m_strarrDescription.GetSize() -1 ; i >= 0; i--)
	{
		LV_ITEM	lvi;

		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.iItem = i;
		lvi.iSubItem = 0;
		lvi.lParam = i;
		lvi.pszText = _T("xx");
		m_CtrlListPhoneBook.InsertItem(&lvi);
	}
	
	for (i = m_strarrDescription.GetSize() -1 ; i >= 0; i--)
	{
		m_CtrlListPhoneBook.SetItemText(i, 0, m_strarrDescription.GetAt(i));
		m_CtrlListPhoneBook.SetItemText(i, 1, m_strarrNumber.GetAt(i));
	}

	m_CtrlListPhoneBook.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	// FLEETLIST
	// The other InsertColumn() override requires an initialized
	// LVCOLUMN structure.
	LVCOLUMN col_fleet;

   	col_fleet.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	col_fleet.fmt = LVCFMT_LEFT;
	col_fleet.cx = 80;

	for (i = 0; i < 3; i++)
	{
		if (i == 0)
		{
			col_fleet.cx = 25;
		}
		else if (i == 1)
		{
			col_fleet.cx = 145;
		}
		else if (i == 2)
		{
			col_fleet.cx = 145;
		}
		else
		{
			col_fleet.cx = 10;
		}

		col_fleet.iSubItem = i;
		col_fleet.pszText = colArrFleet[i].GetBuffer(1);
		m_CtrlListFleet.InsertColumn(i, &col_fleet);
		colArrFleet[i].ReleaseBuffer();
	}

	for (i = 0; i < m_strarrFleetDesc.GetCount(); i++)
	{
		m_CtrlComboFleetName.AddString(m_strarrFleetDesc.GetAt(i));
	}
	m_CtrlComboFleetName.SetCurSel(0);

	m_CtrlListFleet.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	UpdateFleetList();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

//-------------------------------------------------------------
// Events
void CDlgFleetEdit::OnBnClickedNew()
{
	CDlgFleetNew	FleetNew;
	CString			strTemp;
	int				i;
	
	for (i = 0; i < m_strarrFleetDesc.GetCount(); i++)
	{
		FleetNew.m_strarrFleetName.Add(m_strarrFleetDesc.GetAt(i));
	}

	if (FleetNew.DoModal() == IDOK)
	{
		strTemp = FleetNew.m_strFleetName;
		if (strTemp.GetLength() > 0)
		{
			m_CtrlComboFleetName.InsertString(-1, strTemp);
			m_CtrlComboFleetName.SetCurSel(m_CtrlComboFleetName.GetCount()-1);

			m_strarrFleetDesc.Add(strTemp);
			FLEET_ELEM *pFleetElemInsert = new FLEET_ELEM;
			m_ptrarrFleetElem.Add(pFleetElemInsert);

			UpdateFleetList();
		}
	}
}

void CDlgFleetEdit::OnBnClickedSetLok()
{
	CString strTempDesc;
	CString strTempNo;
	CString strTempTel;

	//get marked phoneitem
	POSITION pos = m_CtrlListPhoneBook.GetFirstSelectedItemPosition();
	int nItem = m_CtrlListPhoneBook.GetNextSelectedItem(pos);

	strTempDesc = m_CtrlListPhoneBook.GetItemText(nItem,0);
	strTempTel = m_CtrlListPhoneBook.GetItemText(nItem,1);

	if (nItem < 0)
		return;

	//get selected fleetitem
	pos = m_CtrlListFleet.GetFirstSelectedItemPosition();
	if ( (nItem = m_CtrlListFleet.GetNextSelectedItem(pos)) < 0 ) 
	{
		nItem = m_CtrlListFleet.GetItemCount();
	}

	int nSel = m_CtrlComboFleetName.GetCurSel();
	FLEET_ELEM *pFleetElem = (FLEET_ELEM *)m_ptrarrFleetElem.GetAt(nSel);
	if (nItem < pFleetElem->strarrPhoneBookDescr.GetCount())
	{
		pFleetElem->strarrPhoneBookDescr.InsertAt(nItem, strTempDesc);
		pFleetElem->strarrPhoneBookNumber.InsertAt(nItem, strTempTel);
	}
	else
	{
		pFleetElem->strarrPhoneBookDescr.Add(strTempDesc);
		pFleetElem->strarrPhoneBookNumber.Add(strTempTel);
	}

	UpdateFleetList();
}

void CDlgFleetEdit::OnBnClickedResetLok()
{
	//get selected fleetitem
	POSITION pos = m_CtrlListFleet.GetFirstSelectedItemPosition();
	int nItem = m_CtrlListFleet.GetNextSelectedItem(pos);
	if ( nItem < 0 ) 
		return;
	
	int nSel = m_CtrlComboFleetName.GetCurSel();
	FLEET_ELEM *pFleetElem = (FLEET_ELEM *)m_ptrarrFleetElem.GetAt(nSel);
	if (nItem < pFleetElem->strarrPhoneBookDescr.GetCount())
	{
		pFleetElem->strarrPhoneBookDescr.RemoveAt(nItem);
		pFleetElem->strarrPhoneBookNumber.RemoveAt(nItem);
	}

	UpdateFleetList();
}

void CDlgFleetEdit::OnCbnCloseupFleetName()
{
	UpdateFleetList();
}

void CDlgFleetEdit::OnCbnSelchangeFleetName()
{
	UpdateFleetList();
}

void CDlgFleetEdit::OnBnClickedDel()
{
	CString strTemp;
	int		nSel, nIdx;

	// Index der auswahl in der Combobox
	nSel = m_CtrlComboFleetName.GetCurSel();
	if (nSel < 0)
	{
		AfxMessageBox(strMess1);
		return;
	}

	// string in combobox löschen und markierung auf position neu setzen
	m_CtrlComboFleetName.DeleteString(nSel);
	nIdx = nSel;
	if (nIdx >= m_CtrlComboFleetName.GetCount())
	{
		nIdx--;
	}
	m_CtrlComboFleetName.SetCurSel(nIdx);

	// eintrag aus Listen löschen
	m_strarrFleetDesc.RemoveAt(nSel);
	FLEET_ELEM *pFleetElem = (FLEET_ELEM *)m_ptrarrFleetElem.GetAt(nSel);
	if(pFleetElem)
	{
		pFleetElem->strarrPhoneBookDescr.RemoveAll();
		pFleetElem->strarrPhoneBookNumber.RemoveAll();
	}
	m_ptrarrFleetElem.RemoveAt(nSel);

	UpdateFleetList();
}
//-----------------------------------------------------------------
//Functions Dj

void CDlgFleetEdit::RenumFleetList()
{
	int nItem;
	CString strTempNo;

	for (nItem = 0; nItem < m_CtrlListFleet.GetItemCount(); nItem++)
	{
		strTempNo.Format(_T("%d"),nItem);	
		m_CtrlListFleet.SetItemText(nItem,0,strTempNo);
	}
}


void CDlgFleetEdit::UpdateFleetList()
{
	//update fleetlist
	CString strFleet,strTemp,strLine;
	int nItem;
	LV_ITEM	lvi;
	int nSel;
	int	nSelList;

	nSel = m_CtrlComboFleetName.GetCurSel();
	
	// index der selektierten Zeile
	POSITION pos = m_CtrlListFleet.GetFirstSelectedItemPosition();
	nSelList = m_CtrlListFleet.GetNextSelectedItem(pos);

	//delete fleetlist
	m_CtrlListFleet.DeleteAllItems();

	if (nSel < 0)
	{
		AfxMessageBox(strMess1);
		return;
	}
	m_CtrlComboFleetName.GetLBText(nSel,strFleet);
	
	// aktuelle Liste übernehmen
	if (nSel < m_ptrarrFleetElem.GetCount())
	{
		FLEET_ELEM *pFleetElem = (FLEET_ELEM *)m_ptrarrFleetElem.GetAt(nSel);

		if (pFleetElem)
		{
			for (nItem = 0; nItem < pFleetElem->strarrPhoneBookDescr.GetCount(); nItem++)
			{
				//insert Items in Fleetlist
				//i = m_CtrlListFleet.GetItemCount();
				if (nItem >= m_CtrlListFleet.GetItemCount())
				{
					lvi.mask = LVIF_TEXT | LVIF_PARAM;
					lvi.iItem = nItem;
					lvi.iSubItem = 0;
					lvi.lParam = nItem;
					lvi.pszText = _T("xxx");
					m_CtrlListFleet.InsertItem(&lvi);
				}
				
				//set no
				strTemp.Format(_T("%d"),nItem+1);
				m_CtrlListFleet.SetItemText(nItem,0,strTemp);

				//set desc
				m_CtrlListFleet.SetItemText(nItem,1,pFleetElem->strarrPhoneBookDescr.GetAt(nItem));

				//set Tel
				m_CtrlListFleet.SetItemText(nItem,2,pFleetElem->strarrPhoneBookNumber.GetAt(nItem));
			}

			if (nSelList >= 0)
			{
				if (nSelList >= pFleetElem->strarrPhoneBookDescr.GetCount())
				{
					nSelList = pFleetElem->strarrPhoneBookDescr.GetCount() - 1;
				}
				m_CtrlListFleet.SetItemState(nSelList, LVIS_SELECTED, LVIS_SELECTED);
			}
		}
	}

}


void CDlgFleetEdit::OnBnClickedFleetImport()
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CString			strFile;
	CStringArray	strarrFleetDesc;
	CPtrArray		ptrarrFleetElem;

	strFile = _T("Fleet_List.rfl");

	CFileDialog	FileDialog(TRUE, _T("rfl"), strFile, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
						   _T("Import Files (*.rfl)|*.rfl|All Files (*.*)|*.*||"));
	int			iStatus;
	CString		strPath;

	FileDialog.m_ofn.lpstrInitialDir = pApp->m_strInitialDirectory;

	FileDialog.m_ofn.lpstrTitle = _T("FleetList Import");
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
			bRet = pApp->ReadFleetIni(file, strarrFleetDesc, ptrarrFleetElem);
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

		int	i, j, k, l, m;
		int	iFleetElemIdx;
		bool found, same;
		bool foundFleet;
		CString strFleetName;
		CString	strDesc;
		FLEET_ELEM *pFleetElem, *pFleetElemImp;

		for (l = 0; l < strarrFleetDesc.GetCount(); l++)
		{
			strFleetName = strarrFleetDesc.GetAt(l);
			foundFleet = false;
			for (m = 0; !foundFleet && m < m_strarrFleetDesc.GetCount(); m++)
			{
				if (m_strarrFleetDesc.GetAt(m) == strFleetName)
				{
					foundFleet = true;
					iFleetElemIdx = m;
				}
			}

			if (foundFleet)
			{
				pFleetElemImp = (FLEET_ELEM *)ptrarrFleetElem.GetAt(l);
				pFleetElem = (FLEET_ELEM *)m_ptrarrFleetElem.GetAt(iFleetElemIdx);
				if (pFleetElemImp && pFleetElem)
				{
					// flotte gefunden nur fehlende Telefoneinträge einfügen
					for (i = 0; i < pFleetElemImp->strarrPhoneBookDescr.GetCount(); i++)
					{
						found = false;
						same = false;
						strDesc = pFleetElemImp->strarrPhoneBookDescr.GetAt(i);

						for (j = 0; !found && j < pFleetElem->strarrPhoneBookDescr.GetCount(); j++)
						{
							if (pFleetElem->strarrPhoneBookDescr.GetAt(j) == strDesc)
							{
								found = true;
								if (pFleetElem->strarrPhoneBookNumber.GetAt(j) == 
									pFleetElemImp->strarrPhoneBookNumber.GetAt(i))
								{
									same = true;
								}
							}
						}

						// neuen flotte erzeugen und erneut suchen
						for (k = 1; k < 10 && found && !same; k++)
						{
							if (k == 1)
							{
								strDesc.Format(_T("%s copy"), pFleetElemImp->strarrPhoneBookDescr.GetAt(i));
							}
							else
							{
								strDesc.Format(_T("%s copy (%d)"), pFleetElemImp->strarrPhoneBookDescr.GetAt(i), k);
							}

							found = same = false;
							for (j = 0; !found && j < pFleetElem->strarrPhoneBookDescr.GetCount(); j++)
							{
								if (pFleetElem->strarrPhoneBookDescr.GetAt(j) == strDesc)
								{
									found = true;
									if (pFleetElem->strarrPhoneBookNumber.GetAt(j) == 
										pFleetElemImp->strarrPhoneBookNumber.GetAt(i))
									{
										same = true;
									}
								}
							}
						}

						// eintrag hinzufügen
						if (!found)
						{ 
							pFleetElem->strarrPhoneBookDescr.Add(strDesc);
							pFleetElem->strarrPhoneBookNumber.Add(pFleetElemImp->strarrPhoneBookNumber.GetAt(i));
						}
					}
				}
			}
			else
			{
				// flotte nicht gefunden komplett hinzufügen
				pFleetElemImp = (FLEET_ELEM *)ptrarrFleetElem.GetAt(l);
				pFleetElem = (FLEET_ELEM *)new FLEET_ELEM;

				for (i = 0; i < pFleetElemImp->strarrPhoneBookDescr.GetCount(); i++)
				{
					pFleetElem->strarrPhoneBookDescr.Add(pFleetElemImp->strarrPhoneBookDescr.GetAt(i));
					pFleetElem->strarrPhoneBookNumber.Add(pFleetElemImp->strarrPhoneBookNumber.GetAt(i));
				}

				m_CtrlComboFleetName.InsertString(-1, strFleetName);

				m_strarrFleetDesc.Add(strFleetName);
				m_ptrarrFleetElem.Add(pFleetElem);
			}
		}

		UpdateFleetList();
	}
}

void CDlgFleetEdit::OnBnClickedFleetExport()
{
	CRailDiagApp	*pApp = (CRailDiagApp *)AfxGetApp();
	CString			strFile;

	strFile = _T("Fleet_List.rfl");

	CFileDialog	FileDialog(FALSE, _T("rfl"), strFile, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
						   _T("Export Files (*.rfl)|*.rfl|All Files (*.*)|*.*||"));
	int			iStatus;
	CString		strPath;

	FileDialog.m_ofn.lpstrInitialDir = pApp->m_strInitialDirectory;

	FileDialog.m_ofn.lpstrTitle = _T("FleetList Export");
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
			bRet = pApp->WriteFleetIni(file, m_strarrFleetDesc, m_ptrarrFleetElem);
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
