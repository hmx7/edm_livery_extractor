
// EDM Livery Extractor GUIDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include <string>
#include <string.h>
#include <vector>
#include <map>
#include <algorithm>

// CEDMLiveryExtractorGUIDlg dialog
class CEDMLiveryExtractorGUIDlg : public CDialogEx
{
// Construction
public:
	CEDMLiveryExtractorGUIDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_EDMLIVERYEXTRACTORGUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedload();
	afx_msg void OnBnClickedextract();
//	afx_msg void OnBnClickedCancel();
//	afx_msg void OnFileOpen();
//	afx_msg void OnBnClickedAboutbox();

	// customized variables
	CButton uv_load;
	CButton uv_extract;
	CListBox uv_lstbox;

	std::vector<std::string> uv_filelst;
	std::map<CString, CString> uv_table;

	int savetogether;
	int advance_content;

	// customized functions
	int uf_extract2lua(char*);
	afx_msg void OnBnClickedCheck1();
	CButton uv_savetogether;
	//afx_msg void OnBnClickedCheck2();
	//CButton uv_advance_content;
};
