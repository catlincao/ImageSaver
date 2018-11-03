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
extern int cnum; // 控制开启一次stream采图的数量
extern int CINDEX; //开启的相机索引，默认开启第0号相机

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
	/*使用redis必须添加上去*/
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
	不显示窗口
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
	//初始化连接
	m_ipAddr = _T("127.0.0.1");
	m_Port = 6379;
	if (!m_psM.Connect(m_ipAddr, m_Port))
	{
		AfxMessageBox("未能连接redis数据库");
		ExitProcess(1);
	}
	//默认监听CCDCaptureSync频道
	m_Channel = _T("CCDCaptureSync");
	m_psM.Subscribe(m_Channel);
	//打开一个定时器，以100ms的时间来检测是否有信息进来
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

	/*GetSystemTime(&time);   //以时间命名
	tempCstring.Format(_T("%02i%02i%02i-%02i%02i%02i-%03i-UTC.bmp"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);*/

	tempCstring.Format(_T("%d.bmp"),temp);
	filename = path + tempCstring;

	temp++;
	namefile << temp; //图像名字累加
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
		ShowErrorMsg(CString("分配缓冲区失败!"), status);
		return FALSE;
	}

	status = J_Image_FromRawToImage(pRawImageInfo, &cvnImageInfo);
	if(status != J_ST_SUCCESS) 
	{
		ShowErrorMsg(CString("将原始图像转换为图像格式失败!"), status);
		return FALSE;
	}

	status = J_Image_SaveFileEx(&cvnImageInfo, sFileName, J_FF_BMP);
	if(status != J_ST_SUCCESS) 
	{
		ShowErrorMsg(CString("保存图像失败!"), status);
		return FALSE;
	}

	status = J_Image_Free(&cvnImageInfo);
	if(status != J_ST_SUCCESS) 
	{
		ShowErrorMsg(CString("释放图像缓冲区失败!"), status);
		return FALSE;
	}
	
	return TRUE;

}

void CImageSaverDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error) //错误信息处理
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
		ShowErrorMsg(CString("打开factory失败！"), status);
		return FALSE;
	}

	//Device Discovery
	status = J_Factory_UpdateCameraList(m_hFactory, &bHasChanged); 

	if(status != J_ST_SUCCESS) 
	{
		ShowErrorMsg(CString("更新相机列表失败！"), status);
		return FALSE;
	}

	//Get number of cameras
	status = J_Factory_GetNumOfCameras(m_hFactory, &iNumOfCameras);
	if(status != J_ST_SUCCESS) 
	{
		ShowErrorMsg(CString("获取相机数目失败！"), status);
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
				ShowErrorMsg(CString("通过索引获取相机ID失败(相机列表中)！"), status);
				return FALSE;
			}

			int8_t   sCameraInfo[J_CAMERA_INFO_SIZE];
			uint32_t iCameraInfoSize = J_CAMERA_INFO_SIZE;
//			CString ListItemStr;
			
			//get model name
			status = J_Factory_GetCameraInfo(m_hFactory, m_CameraId, CAM_INFO_MODELNAME, sCameraInfo, &iCameraInfoSize);
			if(status != J_ST_SUCCESS) 
			{
				ShowErrorMsg(CString("获取相机信息失败！"), status);
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
			ShowErrorMsg(CString("关闭相机失败！"), status);
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
			ShowErrorMsg(CString("关闭factory失败！"), status);
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
				ShowErrorMsg(CString("设置TriggerSelector=FrameStart失败"), status);
				return;
			}

			// Set TriggerMode="On"
			status = J_Camera_SetValueString(m_hCamera, (int8_t*)"TriggerMode", (int8_t*)"On");
			if (status != J_ST_SUCCESS)
			{
				ShowErrorMsg(CString("设置TriggerMode=On失败！"), status);
				return;
			}

			// Set TriggerSource="Software" 
			status = J_Camera_SetValueString(m_hCamera, (int8_t*)"TriggerSource", (int8_t*)"Software");
			if (status != J_ST_SUCCESS)
			{
				ShowErrorMsg(CString("设置TriggerSource=Software失败！"), status);
				return;
			}

			//Set SequenceMode="Off"  切换成软件触发时，这里会报错，先注释掉 2018年7月18日
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
				ShowErrorMsg(CString("设置ExposureMode=EdgePreSelect失败"), status);
				return;
			}

			status = J_Camera_SetValueString(m_hCamera, (int8_t*)"LineSelector", (int8_t*)"CameraTrigger0");
			if (status != J_ST_SUCCESS)
			{
				ShowErrorMsg(CString("设置LineSelector=CameraTrigger0失败"), status);
				return;
			}

			status = J_Camera_SetValueString(m_hCamera, (int8_t*)"LineSource", (int8_t*)"SoftwareTrigger0");
			if (status != J_ST_SUCCESS)
			{
				ShowErrorMsg(CString("设置LineSource=SoftwareTrigger0失败"), status);
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
			ShowErrorMsg(CString("设置TriggerSelector=FrameStart失败"), status);
			return;
		}
		status = J_Camera_ExecuteCommand(m_hCamera, (int8_t*)"TriggerSoftware");
		if (status != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("执行TriggerSoftware失败！"), status);
			return;
		}
	}
	else
	{

		status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SoftwareTrigger0", 1);
		if (status != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("0.设置SoftwareTrigger0=1失败"), status);
			return;
		}

		status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SoftwareTrigger0", 0);
		if (status != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("设置SoftwareTrigger0=0失败"), status);
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

		status = J_Factory_GetCameraIDByIndex(m_hFactory, CINDEX, m_CameraId, &iCameraIdSize);//使用索引为INDEX（默认为0）的相机
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("通过索引获取相机ID失败！"), status);
			return;
		}

		//open the selected camera
		status = J_Camera_Open(m_hFactory, m_CameraId, &m_hCamera);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("打开相机失败！"), status);
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
			ShowErrorMsg(CString("打开Stream失败！"), status);
			return;
		}
		status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"AcquisitionStart", &hNode);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("由名字得到节点失败（AcquisitionStart）！"), status);
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
			ShowErrorMsg(CString("设置TriggerSelector=FrameStart失败"), status);
			return;
		}
		status = J_Camera_ExecuteCommand(m_hCamera, (int8_t*)"TriggerSoftware");
		if (status != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("执行TriggerSoftware失败！"), status);
			return;
		}
	}
	else
	{

		status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SoftwareTrigger0", 1);
		if (status != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("1.设置SoftwareTrigger0=1失败"), status);
			return;
		}

		status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SoftwareTrigger0", 0);
		if (status != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("设置SoftwareTrigger0=0失败"), status);
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
	temp = GetDlgItemInt(IDC_NAME, NULL, 0);//读取第一个名字的序号
	ofstream namefile(file, ios::trunc);	/*由于重置的数字长度可能小于TXT文件中保留的数字长度，如果直接把文件光标移到最开始（下一句代码），则会导致命名错误。如:
										TXT文件中保存的数字是100，如果想重置为20，输入20后，光标移到最开始，TXT文件会被重写为200，显然不对，所以需要清空。而在
										前面保存图像的代码中，TXT文件的数字是用“++”来实现累加的，所以后面重写的数字长度一定不比之前的短，所以没问题。
										*/
	/*namefile.seekg(0, ios::beg);*/
	namefile << temp;
	namefile.close();
}



void CImageSaverDlg::SaveAPicture() //用于redis的保存函数（静态成员函数）
{
	J_STATUS_TYPE status = J_ST_SUCCESS;
	NODE_HANDLE hNode = NULL;
	status = J_Camera_GetNodeByName(instance->m_hCamera, (int8_t*)"TriggerSoftware", &hNode);

	if ((status == J_ST_SUCCESS) && (hNode != NULL))
	{
		status = J_Camera_SetValueString(instance->m_hCamera, (int8_t*)"TriggerSelector", (int8_t*)"FrameStart");
		if (status != J_ST_SUCCESS)
		{
			instance->ShowErrorMsg(CString("设置TriggerSelector=FrameStart失败"), status);
			return;
		}
		status = J_Camera_ExecuteCommand(instance->m_hCamera, (int8_t*)"TriggerSoftware");
		if (status != J_ST_SUCCESS)
		{
			instance->ShowErrorMsg(CString("执行TriggerSoftware失败！"), status);
			return;
		}
	}
	else
	{

		status = J_Camera_SetValueInt64(instance->m_hCamera, (int8_t*)"SoftwareTrigger0", 1);
		if (status != J_ST_SUCCESS)
		{
			instance->ShowErrorMsg(CString("2.设置SoftwareTrigger0=1失败"), status);
			return;
		}

		status = J_Camera_SetValueInt64(instance->m_hCamera, (int8_t*)"SoftwareTrigger0", 0);
		if (status != J_ST_SUCCESS)
		{
			instance->ShowErrorMsg(CString("设置SoftwareTrigger0=0失败"), status);
			return;
		}
	}
}

void CImageSaverDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	m_psM.Unsubscribe(m_Channel);
	// TODO:  在此处添加消息处理程序代码
}

void CImageSaverDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	//每次时间到都使用PubSubManager类的处理函数来判断是否有消息发布
	m_psM.OnTimer();
	CDialogEx::OnTimer(nIDEvent);
}

void SAVEAPICTURE()
{
	//全局函数，供PubSubManager类里面的消息处理回调函数进行调用
	CImageSaverDlg::SaveAPicture();
}