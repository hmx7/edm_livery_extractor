
// EDM Livery Extractor GUI.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CEDMLiveryExtractorGUIApp:
// See EDM Livery Extractor GUI.cpp for the implementation of this class
//

class CEDMLiveryExtractorGUIApp : public CWinApp
{
public:
	CEDMLiveryExtractorGUIApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CEDMLiveryExtractorGUIApp theApp;