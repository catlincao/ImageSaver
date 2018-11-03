// ImageSaver.h : main header file for the PROJECT_NAME application
//

#pragma once
#include "Resource.h"		// main symbols
#include "ImageSaverDlg.h"
#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif


#ifndef _cnum_CINDEX_
#define _cnum_CINDEX_
#endif _cnum_CINDEX_

// CImageSaverApp:
// See ImageSaver.cpp for the implementation of this class
//

class CImageSaverApp : public CWinApp
{
	
public:
	CImageSaverApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CImageSaverApp theApp;