// Win32Project1.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Win32Project1.h"

#include <WinSock2.h>

#pragma comment(lib, "WS2_32")

#define MAX_LOADSTRING 100

//函数声明
SOCKET InitConnectSocket();
void InitBM();

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
bool bConnect;

struct BM{
	BITMAPINFO bi;
	BITMAPFILEHEADER bfh;
} *PBMS;

LPVOID lpData = NULL;
HDC gHdc;
HWND hWnd;
HBITMAP hBitmap;

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WIN32PROJECT1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT1));	

	InitBM();
	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

void InitBM()
{
	PBMS = new BM();
	ZeroMemory(&(PBMS->bi), sizeof(BITMAPINFO));
	ZeroMemory(&(PBMS->bfh), sizeof(BITMAPFILEHEADER));
	
	gHdc = GetDC(hWnd);
}


//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT1));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WIN32PROJECT1);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_CONNECT:
			{
				bConnect = true;
				HANDLE hThread = CreateThread(NULL, 0, ThreadSocket, NULL, 0, NULL);
				CloseHandle(hThread);
			}	
			break;
		case IDM_DISCONNECT:
			bConnect = false;
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

SOCKET InitConnectSocket()
{
	//init socket
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	if (::WSAStartup(sockVersion, &wsaData) != 0)
	{
		exit(0);
	}

	USHORT nPort = 4568;//服务器监听的端口号

	//create socket
	SOCKET s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s == INVALID_SOCKET) 
	{ 
		exit(0);
	} 

	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(nPort);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;

	//bind the socket
	if (::bind(s, (sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		exit(0);
	}

	// 填写远程地址信息
	sockaddr_in servAddr; 
	servAddr.sin_family = AF_INET; 
	servAddr.sin_port = htons(4567); 
	// 注意，这里要填写服务器程序（TCPServer程序）所在机器的IP地址
	// 如果你的计算机没有联网，直接使用127.0.0.1即可
	servAddr.sin_addr.S_un.S_addr = inet_addr("192.168.166.60"); 
	if(::connect(s, (sockaddr*)&servAddr, sizeof(servAddr)) == -1) 
	{ 
		exit(0);
	} 

	return s;

}

DWORD WINAPI ThreadSocket(PVOID pvParam)
{
	SOCKET s = InitConnectSocket();

	char chInit = '1';
	send(s,&chInit, 1, 0);
	recv(s, (char*)PBMS,sizeof(BM), 0);

	DWORD dwSize = PBMS->bfh.bfSize-PBMS->bfh.bfOffBits;
	lpData = HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, dwSize);

	hBitmap = CreateCompatibleBitmap(gHdc, PBMS->bi.bmiHeader.biWidth, PBMS->bi.bmiHeader.biHeight) ;
	HDC hdcMem = CreateCompatibleDC(gHdc);
	
	while (bConnect)
	{	
		char a = '2';
		::send(s, &a, 1, 0);
		::recv(s, (char *)lpData, dwSize, 0);

		hBitmap = CreateDIBitmap(gHdc, &(PBMS->bi.bmiHeader), CBM_INIT, lpData, &(PBMS->bi), DIB_RGB_COLORS);
		SelectObject(hdcMem, hBitmap);
		BitBlt(gHdc, 0, 0, PBMS->bi.bmiHeader.biWidth, PBMS->bi.bmiHeader.biHeight, hdcMem, 0, 0, SRCCOPY);

		//Sleep(500);
	}

	//关闭套接字释放资源
	::closesocket(s); 
	DeleteDC(hdcMem);
	DeleteObject(hBitmap);
	HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, lpData);

	return 0;
}

