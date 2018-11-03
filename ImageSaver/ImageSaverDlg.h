// ImageSaverDlg.h : header file
//

#pragma once
#include "PubSubManager.h"
#include <Jai_Factory.h>
#include "ImageSaver.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "Windows.h"
#include "afxdialogex.h"
#include <afxwin.h>

// CImageSaverDlg dialog
class CImageSaverDlg : public CDialogEx
{
// Construction
public:
	CImageSaverDlg(CWnd* pParent = NULL);	// standard constructor

	//redis 的参数定义
	CString m_ipAddr;
	int m_Port;
	CString m_Channel;
	CString m_message;
	static void SaveAPicture();
	
	CString filename; 
	CString path; 
	CString tempCstring;
	SYSTEMTIME time;
	FACTORY_HANDLE  m_hFactory;	// Factory handle
	CAM_HANDLE      m_hCamera;	// Camera handle 
	VIEW_HANDLE		m_hView;	// View handle
	THRD_HANDLE		m_hThread;	// Thread handle
	int8_t			m_CameraId[J_CAMERA_ID_SIZE]; // camera ID string array
	BOOL OpenFactory();
	void CloseFactoryAndCamera();
	void ShowErrorMsg(CString message, J_STATUS_TYPE error);
	void StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo);
	BOOL SaveImageToFile(J_tIMAGE_INFO* pRawImageInfo, LPCTSTR sFileName);
	void PrepareSwTrigSetting();
	void Swtrigger();

// Dialog Data
	enum { IDD = IDD_IMAGESAVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	PubSubManager m_psM;
	static CImageSaverDlg *instance;

// Implementation
protected:
	HICON m_hIcon;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonNamereset();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
