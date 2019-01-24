
// EDM Livery Extractor GUIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EDM Livery Extractor GUI.h"
#include "EDM Livery Extractor GUIDlg.h"
#include "afxdialogex.h"
//#include "afxdlgs.h""

#include "edmextractor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CEDMLiveryExtractorGUIDlg dialog



CEDMLiveryExtractorGUIDlg::CEDMLiveryExtractorGUIDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEDMLiveryExtractorGUIDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEDMLiveryExtractorGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, ID_load, uv_load);
	DDX_Control(pDX, ID_extract, uv_extract);
	DDX_Control(pDX, IDC_LIST1, uv_lstbox);
	DDX_Control(pDX, IDC_CHECK1, uv_savetogether);
	//DDX_Control(pDX, IDC_CHECK2, uv_advance_content);
}

BEGIN_MESSAGE_MAP(CEDMLiveryExtractorGUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_load, &CEDMLiveryExtractorGUIDlg::OnBnClickedload)
	ON_BN_CLICKED(ID_extract, &CEDMLiveryExtractorGUIDlg::OnBnClickedextract)
//	ON_BN_CLICKED(IDCANCEL, &CEDMLiveryExtractorGUIDlg::OnBnClickedCancel)
//	ON_COMMAND(ID_FILE_OPEN, &CEDMLiveryExtractorGUIDlg::OnFileOpen)
//  ON_BN_CLICKED(IDD_ABOUTBOX, &CEDMLiveryExtractorGUIDlg::OnBnClickedAboutbox)
ON_BN_CLICKED(IDC_CHECK1, &CEDMLiveryExtractorGUIDlg::OnBnClickedCheck1)
//  ON_BN_CLICKED(IDC_CHECK2, &CEDMLiveryExtractorGUIDlg::OnBnClickedCheck2)
END_MESSAGE_MAP()


// CEDMLiveryExtractorGUIDlg message handlers

BOOL CEDMLiveryExtractorGUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	uv_extract.EnableWindow(false);
	uv_filelst.clear();

	savetogether = 0;
	advance_content = 0;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEDMLiveryExtractorGUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEDMLiveryExtractorGUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEDMLiveryExtractorGUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//#include "edmextractor.h"

int CEDMLiveryExtractorGUIDlg::uf_extract2lua(char* filename)
{
	// TODO: Add your control notification handler code here

	return 0;
}


void CEDMLiveryExtractorGUIDlg::OnBnClickedload()
{
	// TODO: Add your control notification handler code here


	CFileDialog FileDlg(TRUE, _T("edm"), NULL, OFN_ALLOWMULTISELECT, _T("Rule Profile (*.edm)|*.edm*||"));
	DWORD MAXFILE = 2562;
	FileDlg.m_ofn.nMaxFile = MAXFILE;
	TCHAR* pc = new TCHAR[MAXFILE];
	FileDlg.m_ofn.lpstrFile = pc;
	FileDlg.m_ofn.lpstrFile[0] = NULL;

	if (FileDlg.DoModal() == IDOK)
	{
		uv_filelst.clear();
		uv_lstbox.ResetContent();
		POSITION pos = FileDlg.GetStartPosition();

		if (pos) {
			uv_extract.EnableWindow(true);

			do{
				CString PathName = FileDlg.GetNextPathName(pos);

				CT2CA pszConvertedAnsiString(PathName);
				std::string strStd(pszConvertedAnsiString);

				uv_filelst.push_back(strStd);
			} while (pos);
		}
	}

	return;
}


void CEDMLiveryExtractorGUIDlg::OnBnClickedextract()
{
	// TODO: Add your control notification handler code here
	unsigned int i;
	int ret;

	if (uv_filelst.size() == 0){
		uv_extract.EnableWindow(false);
	}
	else{
		for (i = 0; i < uv_filelst.size(); i++){
			ret = edmextract(uv_filelst[i].c_str(), savetogether, advance_content);

			std::size_t found = uv_filelst[i].find_last_of("\\");
			std::string tmpfn = uv_filelst[i].substr(found + 1);

			std::transform(tmpfn.begin(), tmpfn.end(), tmpfn.begin(), ::tolower);
			std::size_t nlength = tmpfn.size();

			std::size_t i, flen = 60;

			if (ret == 1){
				for (i = 0; i < flen - nlength; i++){
					tmpfn += ".";
				}
				tmpfn += "completed";

				CString FileName(tmpfn.c_str());
				uv_lstbox.AddString(FileName);
			}
			else if (ret == 2){
				for (i = 0; i < flen - nlength; i++){
					tmpfn += ".";
				}
				tmpfn += "ignored";

				CString FileName(tmpfn.c_str());
				uv_lstbox.AddString(FileName);
			}
			else if (ret == 0){
				for (i = 0; i < flen - nlength; i++){
					tmpfn += ".";
				}
				tmpfn += "failed";

				CString FileName(tmpfn.c_str());
				uv_lstbox.AddString(FileName);
			}

		}

		uv_filelst.clear();
		uv_extract.EnableWindow(false);
		//uv_lstbox.ResetContent();
	}

}





void CEDMLiveryExtractorGUIDlg::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here
	UINT nCheck = uv_savetogether.GetState();
	if ((nCheck & BST_CHECKED) != 0)	{
		// Button is checked
		savetogether = 1;
	}
	else{
		// Button is unchecked
		savetogether = 0;
	}
}


/*void CEDMLiveryExtractorGUIDlg::OnBnClickedCheck2()
{
	// TODO: Add your control notification handler code here	
	UINT nCheck = uv_advance_content.GetState();
	if ((nCheck & BST_CHECKED) != 0)	{
		// Button is checked
		advance_content = 1;
	}
	else{
		// Button is unchecked
		advance_content = 0;
	}
}*/
