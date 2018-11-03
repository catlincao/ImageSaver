// ImageSaverDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImageSaverDlg.h"
#include <string>
#include <vector>
#include <fstream>
using namespace std;



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CImageSaverDlg dialog
extern int cnum; // ���ƿ���һ��stream��ͼ������
extern int CINDEX; //���������������Ĭ�Ͽ�����0�����

CImageSaverDlg *CImageSaverDlg::instance = NULL;

CImageSaverDlg::CImageSaverDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CImageSaverDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_hFactory	= NULL;
	m_hCamera	= NULL; 
	m_hView		= NULL;
	m_hThread	= NULL;
	
	instance = this;
	/*ʹ��redis���������ȥ*/
	WSAData wsaData;
	WSAStartup(MAKEWORD(1, 1), &wsaData);
}

void CImageSaverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CImageSaverDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CImageSaverDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CImageSaverDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_START, &CImageSaverDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CImageSaverDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CImageSaverDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_NAMERESET, &CImageSaverDlg::OnBnClickedButtonNamereset)
END_MESSAGE_MAP()


// CImageSaverDlg message handlers

BOOL CImageSaverDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	// TODO: Add extra initialization here
	//CINDEX = 1;
	/////////////////////////////////////////////////////////////////////////////
	/*
	����ʾ����
	*/
	SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_HIDEWINDOW);
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
	////////////////////////////////////////////////////////////////////////////
	switch (CINDEX)
	{
	case 0:path = _T(".//images//camera1//cam1-"); SetWindowText(_T("camera1"));		break;
	case 1:path = _T(".//images//camera2//cam2-"); SetWindowText(_T("camera2"));		break;
	case 2:path = _T(".//images//camera3//cam3-"); SetWindowText(_T("camera3"));		break;
	case 3:path = _T(".//images//camera4//cam4-"); SetWindowText(_T("camera4"));		break;
	case 4:path = _T(".//images//camera5//cam5-"); SetWindowText(_T("camera5"));		break;
	case 5:path = _T(".//images//camera6//cam6-"); SetWindowText(_T("camera6"));		break;
	}

	GetDlgItem(IDC_BUTTON_NAMERESET)->EnableWindow(true);
	SetDlgItemInt(IDC_NAME, 0, 0);
	//��ʼ������
	m_ipAddr = _T("127.0.0.1");
	m_Port = 6379;
	if (!m_psM.Connect(m_ipAddr, m_Port))
	{
		AfxMessageBox("δ������redis���ݿ�");
		ExitProcess(1);
	}
	//Ĭ�ϼ���CCDCaptureSyncƵ��
	m_Channel = _T("CCDCaptureSync");
	m_psM.Subscribe(m_Channel);
	//��һ����ʱ������100ms��ʱ��������Ƿ�����Ϣ����
	SetTimer(1001, 100, NULL);
	OnBnClickedButton1();
	Sleep(400);
	OnBnClickedButtonStart();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CImageSaverDlg::OnPaint()
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
HCURSOR CImageSaverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CImageSaverDlg::StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo)
{
	string file;
	int temp;
	switch (CINDEX)
	{
	case 0: file = _T("cam1.txt");	break;
	case 1: file = _T("cam2.txt");	break;
	case 2: file = _T("cam3.txt");	break;
	case 3: file = _T("cam4.txt");	break;
	case 4: file = _T("cam5.txt");	break;
	case 5: file = _T("cam6.txt");	break;
	}
	fstream namefile(file);
	namefile >> temp;
	namefile.seekg(0, ios::beg);

	/*GetSystemTime(&time);   //��ʱ������
	tempCstring.Format(_T("%02i%02i%02i-%02i%02i%02i-%03i-UTC.bmp"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);*/

	tempCstring.Format(_T("%d.bmp"),temp);
	filename = path + tempCstring;

	temp++;
	namefile << temp; //ͼ�������ۼ�
	namefile.close();
	//if (cnum < 1)
	SaveImageToFile(pAqImageInfo, (LPCTSTR)filename);
	//cnum++;
}

BOOL CImageSaverDlg::SaveImageToFile(J_tIMAGE_INFO* pRawImageInfo, LPCTSTR sFileName)
{
	J_tIMAGE_INFO cvnImageInfo;
	J_STATUS_TYPE status = J_ST_SUCCESS;

	status = J_Image_Malloc(pRawImageInfo, &cvnImageInfo);
	if(status != J_ST_SUCCESS) 
	{
		ShowErrorMsg(CString("���仺����ʧ��!"), status);
		return FALSE;
	}

	status = J_Image_FromRawToImage(pRawImageInfo, &cvnImageInfo);
	if(status != J_ST_SUCCESS) 
	{
		ShowErrorMsg(CString("��ԭʼͼ��ת��Ϊͼ���ʽʧ��!"), status);
		return FALSE;
	}

	status = J_Image_SaveFileEx(&cvnImageInfo, sFileName, J_FF_BMP);
	if(status != J_ST_SUCCESS) 
	{
		ShowErrorMsg(CString("����ͼ��ʧ��!"), status);
		return FALSE;
	}

	status = J_Image_Free(&cvnImageInfo);
	if(status != J_ST_SUCCESS) 
	{
		ShowErrorMsg(CString("�ͷ�ͼ�񻺳���ʧ��!"), status);
		return FALSE;
	}
	
	return TRUE;

}

void CImageSaverDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error) //������Ϣ����
{
	CString errorMsg;
	errorMsg.Format(_T("%s: Error = %d: "), message, error);

	switch (error)
	{
	case J_ST_INVALID_BUFFER_SIZE:	errorMsg += "Invalid buffer size ";	                break;
	case J_ST_INVALID_HANDLE:		errorMsg += "Invalid handle ";		                break;
	case J_ST_INVALID_ID:			errorMsg += "Invalid ID ";			                break;
	case J_ST_ACCESS_DENIED:		errorMsg += "Access denied ";		                break;
	case J_ST_NO_DATA:				errorMsg += "No data ";				                break;
	case J_ST_ERROR:				errorMsg += "Generic error ";		                break;
	case J_ST_INVALID_PARAMETER:	errorMsg += "Invalid parameter ";	                break;
	case J_ST_TIMEOUT:				errorMsg += "Timeout ";				                break;
	case J_ST_INVALID_FILENAME:		errorMsg += "Invalid file name ";	                break;
	case J_ST_INVALID_ADDRESS:		errorMsg += "Invalid address ";		                break;
	case J_ST_FILE_IO:				errorMsg += "File IO error ";		                break;
	case J_ST_GC_ERROR:				errorMsg += "GenICam error ";		                break;
	case J_ST_VALIDATION_ERROR:		errorMsg += "Settings File Validation Error ";		break;
	case J_ST_VALIDATION_WARNING:	errorMsg += "Settings File Validation Warning ";    break;
	}

	AfxMessageBox(errorMsg, MB_OKCANCEL | MB_ICONINFORMATION);

}


BOOL CImageSaverDlg::OpenFactory()
{
	J_STATUS_TYPE	status = J_ST_SUCCESS;
	bool8_t			bHasChanged = false;
	uint32_t		iNumOfCameras =	0;

	//Open Factory and get list of cameras
	status = J_Factory_Open((int8_t*)"", &m_hFactory);
	if(status != J_ST_SUCCESS) 
	{
		ShowErrorMsg(CString("��factoryʧ�ܣ�"), status);
		return FALSE;
	}

	//Device Discovery
	status = J_Factory_UpdateCameraList(m_hFactory, &bHasChanged); 

	if(status != J_ST_SUCCESS) 
	{
		ShowErrorMsg(CString("��������б�ʧ�ܣ�"), status);
		return FALSE;
	}

	//Get number of cameras
	status = J_Factory_GetNumOfCameras(m_hFactory, &iNumOfCameras);
	if(status != J_ST_SUCCESS) 
	{
		ShowErrorMsg(CString("��ȡ�����Ŀʧ�ܣ�"), status);
		return FALSE;
	}

	if(iNumOfCameras == 0)
	{
		AfxMessageBox(_T("No camera found!"), MB_OK | MB_ICONINFORMATION);
		return FALSE;
	}
	else if(iNumOfCameras > 0)
	{

		for (uint32_t index = 0; index < iNumOfCameras; index++)
		{
			uint32_t iCameraIdSize = J_CAMERA_ID_SIZE;

			status = J_Factory_GetCameraIDByIndex(m_hFactory, index, m_CameraId, &iCameraIdSize);
			if(status != J_ST_SUCCESS) 
			{
				ShowErrorMsg(CString("ͨ��������ȡ���IDʧ��(����б���)��"), status);
				return FALSE;
			}

			int8_t   sCameraInfo[J_CAMERA_INFO_SIZE];
			uint32_t iCameraInfoSize = J_CAMERA_INFO_SIZE;
//			CString ListItemStr;
			
			//get model name
			status = J_Factory_GetCameraInfo(m_hFactory, m_CameraId, CAM_INFO_MODELNAME, sCameraInfo, &iCameraInfoSize);
			if(status != J_ST_SUCCESS) 
			{
				ShowErrorMsg(CString("��ȡ�����Ϣʧ�ܣ�"), status);
				return FALSE;
			}
/*			
			ListItemStr.Format(_T("%s"),sCameraInfo);

			//check if the interface is filter driver or socket driver
			if (strstr(m_CameraId, _T("INT=>FD")))
				ListItemStr += " (Filter Driver)";
			else if (strstr(m_CameraId, "INT=>SD"))
				ListItemStr += " (Socket Driver)";
			else 
            {
                return FALSE;
            }
			*/
		}//end of camera index
	}
	return TRUE;
}


void CImageSaverDlg::CloseFactoryAndCamera()
{

	J_STATUS_TYPE	status = J_ST_SUCCESS;

	// close camera
	if(m_hCamera)
	{
		status = J_Camera_Close(m_hCamera);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("�ر����ʧ�ܣ�"), status);
			return;
		}
		m_hCamera = NULL;
	}

	// close factory
	if(m_hFactory)
	{
		status = J_Factory_Close(m_hFactory);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("�ر�factoryʧ�ܣ�"), status);
			return;
		}
		m_hFactory = NULL;
	}
}



void CImageSaverDlg::OnBnClickedOk()
{
	CloseFactoryAndCamera();
	OnDestroy();
	OnOK();
}

void CImageSaverDlg::OnBnClickedButton1()
{
	if (OpenFactory())
	{
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON1)->EnableWindow(false);

	}
}

void CImageSaverDlg::PrepareSwTrigSetting()
{
	J_STATUS_TYPE status;
	if (m_hCamera)
	{
		NODE_HANDLE hNode;
		uint32_t iNumOfEnumEntries = 0;
		int64_t int64Val = 0;
		hNode = NULL;
		status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"TriggerSelector", &hNode);

		if ((status == J_ST_SUCCESS) && (hNode != NULL))
		{
			// Here we assume that this is GenICam SFNC trigger so we do the following:
			// TriggerSelector=FrameStart
			// TriggerMode=On
			// TriggerSource=Software
			// SequenceMode=Off

			// Set TriggerSelector="FrameStart"
			status = J_Camera_SetValueString(m_hCamera, (int8_t*)"TriggerSelector", (int8_t*)"FrameStart");
			if (status != J_ST_SUCCESS)
			{
				ShowErrorMsg(CString("����TriggerSelector=FrameStartʧ��"), status);
				return;
			}

			// Set TriggerMode="On"
			status = J_Camera_SetValueString(m_hCamera, (int8_t*)"TriggerMode", (int8_t*)"On");
			if (status != J_ST_SUCCESS)
			{
				ShowErrorMsg(CString("����TriggerMode=Onʧ�ܣ�"), status);
				return;
			}

			// Set TriggerSource="Software" 
			status = J_Camera_SetValueString(m_hCamera, (int8_t*)"TriggerSource", (int8_t*)"Software");
			if (status != J_ST_SUCCESS)
			{
				ShowErrorMsg(CString("����TriggerSource=Softwareʧ�ܣ�"), status);
				return;
			}

			//Set SequenceMode="Off"  �л����������ʱ������ᱨ����ע�͵� 2018��7��18��
			/* status = J_Camera_SetValueString(m_hCamera, (int8_t*)"SequenceMode", (int8_t*)"Off");
			if(status != J_ST_SUCCESS)
			{
			ShowErrorMsg(CString("Could not set SequenceMode!"), status);
			return;
			}*/
		}
		else
		{
			
			status = J_Camera_SetValueString(m_hCamera, (int8_t*)"ExposureMode", (int8_t*)"EdgePreSelect");
			if (status != J_ST_SUCCESS)
			{
				ShowErrorMsg(CString("����ExposureMode=EdgePreSelectʧ��"), status);
				return;
			}

			status = J_Camera_SetValueString(m_hCamera, (int8_t*)"LineSelector", (int8_t*)"CameraTrigger0");
			if (status != J_ST_SUCCESS)
			{
				ShowErrorMsg(CString("����LineSelector=CameraTrigger0ʧ��"), status);
				return;
			}

			status = J_Camera_SetValueString(m_hCamera, (int8_t*)"LineSource", (int8_t*)"SoftwareTrigger0");
			if (status != J_ST_SUCCESS)
			{
				ShowErrorMsg(CString("����LineSource=SoftwareTrigger0ʧ��"), status);
				return;
			}
		}

	
	} 
}


void CImageSaverDlg::Swtrigger()
{
	J_STATUS_TYPE status = J_ST_SUCCESS;
	NODE_HANDLE hNode = NULL;
	status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"TriggerSoftware", &hNode);

	if ((status == J_ST_SUCCESS) && (hNode != NULL))
	{
		status = J_Camera_SetValueString(m_hCamera, (int8_t*)"TriggerSelector", (int8_t*)"FrameStart");
		if (status != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("����TriggerSelector=FrameStartʧ��"), status);
			return;
		}
		status = J_Camera_ExecuteCommand(m_hCamera, (int8_t*)"TriggerSoftware");
		if (status != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("ִ��TriggerSoftwareʧ�ܣ�"), status);
			return;
		}
	}
	else
	{

		status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SoftwareTrigger0", 1);
		if (status != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("0.����SoftwareTrigger0=1ʧ��"), status);
			return;
		}

		status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SoftwareTrigger0", 0);
		if (status != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("����SoftwareTrigger0=0ʧ��"), status);
			return;
		}
	}
}

void CImageSaverDlg::OnBnClickedButtonStart()
{
	J_STATUS_TYPE status = J_ST_SUCCESS;
	int64_t		widthMax = 10;
	int64_t		heightMax = 10;
	NODE_HANDLE hNode; 
	CString editStr;

	if(!m_hCamera)
	{
		uint32_t iCameraIdSize = J_CAMERA_ID_SIZE;

		status = J_Factory_GetCameraIDByIndex(m_hFactory, CINDEX, m_CameraId, &iCameraIdSize);//ʹ������ΪINDEX��Ĭ��Ϊ0�������
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("ͨ��������ȡ���IDʧ�ܣ�"), status);
			return;
		}

		//open the selected camera
		status = J_Camera_Open(m_hFactory, m_CameraId, &m_hCamera);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("�����ʧ�ܣ�"), status);
			return;
		}
	}
	PrepareSwTrigSetting();
	if(m_hCamera)
	{	
		char buff[80];
		uint32_t iSize = sizeof(buff);
			
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(true);
		//Set Width to max width
		J_Camera_GetNodeByName(m_hCamera, (int8_t*)"Width", &hNode);
		J_Node_GetMaxInt64(hNode, &widthMax); 
		J_Node_SetValueInt64(hNode, 0, widthMax);

		//Set Height to max height
		J_Camera_GetNodeByName(m_hCamera, (int8_t*)"Height", &hNode);
		J_Node_GetMaxInt64(hNode, &heightMax); 
		J_Node_SetValueInt64(hNode, 0, heightMax);		

		status = J_Image_OpenStream(m_hCamera, 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this), reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CImageSaverDlg::StreamCBFunc), &m_hThread, (uint32_t)(heightMax * widthMax * 6));
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("��Streamʧ�ܣ�"), status);
			return;
		}
		status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"AcquisitionStart", &hNode);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("�����ֵõ��ڵ�ʧ�ܣ�AcquisitionStart����"), status);
			return;
		}
		J_Node_ExecuteCommand(hNode);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(false);
	}
	//cnum = 0;
}

void CImageSaverDlg::OnBnClickedButtonStop()
{
	J_STATUS_TYPE status = J_ST_SUCCESS;
	NODE_HANDLE hNode;
	//cnum = 0;
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(false);


	// Stop Acquision
	if (m_hCamera) {
 		status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"AcquisitionStop", &hNode);
		status = J_Node_ExecuteCommand(hNode);
	}

	// Close stream
	if(m_hThread)
	{
		status = J_Image_CloseStream(m_hThread);
		m_hThread = NULL;
	}
}





void CImageSaverDlg::OnBnClickedButtonSave()
{
	J_STATUS_TYPE status = J_ST_SUCCESS;
	NODE_HANDLE hNode = NULL;
	status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"TriggerSoftware", &hNode);

	if ((status == J_ST_SUCCESS) && (hNode != NULL))
	{
		status = J_Camera_SetValueString(m_hCamera, (int8_t*)"TriggerSelector", (int8_t*)"FrameStart");
		if (status != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("����TriggerSelector=FrameStartʧ��"), status);
			return;
		}
		status = J_Camera_ExecuteCommand(m_hCamera, (int8_t*)"TriggerSoftware");
		if (status != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("ִ��TriggerSoftwareʧ�ܣ�"), status);
			return;
		}
	}
	else
	{

		status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SoftwareTrigger0", 1);
		if (status != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("1.����SoftwareTrigger0=1ʧ��"), status);
			return;
		}

		status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SoftwareTrigger0", 0);
		if (status != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("����SoftwareTrigger0=0ʧ��"), status);
			return;
		}
	}
}



void CImageSaverDlg::OnBnClickedButtonNamereset()
{
	string file;
	int temp;
	switch (CINDEX)
	{
	case 0: file = _T("cam1.txt");	break;
	case 1: file = _T("cam2.txt");	break;
	case 2: file = _T("cam3.txt");	break;
	case 3: file = _T("cam4.txt");	break;
	case 4: file = _T("cam5.txt");	break;
	case 5: file = _T("cam6.txt");	break;
	}
	temp = GetDlgItemInt(IDC_NAME, NULL, 0);//��ȡ��һ�����ֵ����
	ofstream namefile(file, ios::trunc);	/*�������õ����ֳ��ȿ���С��TXT�ļ��б��������ֳ��ȣ����ֱ�Ӱ��ļ�����Ƶ��ʼ����һ����룩����ᵼ������������:
										TXT�ļ��б����������100�����������Ϊ20������20�󣬹���Ƶ��ʼ��TXT�ļ��ᱻ��дΪ200����Ȼ���ԣ�������Ҫ��ա�����
										ǰ�汣��ͼ��Ĵ����У�TXT�ļ����������á�++����ʵ���ۼӵģ����Ժ�����д�����ֳ���һ������֮ǰ�Ķ̣�����û���⡣
										*/
	/*namefile.seekg(0, ios::beg);*/
	namefile << temp;
	namefile.close();
}



void CImageSaverDlg::SaveAPicture() //����redis�ı��溯������̬��Ա������
{
	J_STATUS_TYPE status = J_ST_SUCCESS;
	NODE_HANDLE hNode = NULL;
	status = J_Camera_GetNodeByName(instance->m_hCamera, (int8_t*)"TriggerSoftware", &hNode);

	if ((status == J_ST_SUCCESS) && (hNode != NULL))
	{
		status = J_Camera_SetValueString(instance->m_hCamera, (int8_t*)"TriggerSelector", (int8_t*)"FrameStart");
		if (status != J_ST_SUCCESS)
		{
			instance->ShowErrorMsg(CString("����TriggerSelector=FrameStartʧ��"), status);
			return;
		}
		status = J_Camera_ExecuteCommand(instance->m_hCamera, (int8_t*)"TriggerSoftware");
		if (status != J_ST_SUCCESS)
		{
			instance->ShowErrorMsg(CString("ִ��TriggerSoftwareʧ�ܣ�"), status);
			return;
		}
	}
	else
	{

		status = J_Camera_SetValueInt64(instance->m_hCamera, (int8_t*)"SoftwareTrigger0", 1);
		if (status != J_ST_SUCCESS)
		{
			instance->ShowErrorMsg(CString("2.����SoftwareTrigger0=1ʧ��"), status);
			return;
		}

		status = J_Camera_SetValueInt64(instance->m_hCamera, (int8_t*)"SoftwareTrigger0", 0);
		if (status != J_ST_SUCCESS)
		{
			instance->ShowErrorMsg(CString("����SoftwareTrigger0=0ʧ��"), status);
			return;
		}
	}
}

void CImageSaverDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	m_psM.Unsubscribe(m_Channel);
	// TODO:  �ڴ˴������Ϣ����������
}

void CImageSaverDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//ÿ��ʱ�䵽��ʹ��PubSubManager��Ĵ��������ж��Ƿ�����Ϣ����
	m_psM.OnTimer();
	CDialogEx::OnTimer(nIDEvent);
}

void SAVEAPICTURE()
{
	//ȫ�ֺ�������PubSubManager���������Ϣ����ص��������е���
	CImageSaverDlg::SaveAPicture();
}