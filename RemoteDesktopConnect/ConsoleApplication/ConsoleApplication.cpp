// ConsoleApplication.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <WinSock2.h>
#include <string>
#include <windows.h>
#pragma comment(lib, "WS2_32")

using namespace std;

bool HandlerMessage(const char &message, const SOCKET &s);
//void GetScreemBitmap(LPBITMAPINFO lpBMI);
void GetScreemBitmap();
void InitBM();
void ReleaseBM();

struct BM{
	BITMAPINFO bi;
	BITMAPFILEHEADER bfh;
} *PBMS;

LPVOID lpData = NULL;
HBITMAP hBitmap;
HDC screenDC;
HDC hdcMem;
int xRes;
int yRes;
DWORD dwBmSize;

int _tmain(int argc, _TCHAR* argv[])
{
	//init socket
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	if (::WSAStartup(sockVersion, &wsaData) != 0)
	{
		exit(0);
	}

	USHORT nPort = 4567;//服务器监听的端口号

	//create socket
	SOCKET sListen = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sListen == INVALID_SOCKET) 
	{ 
		exit(0); 
	} 

	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(nPort);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;

	//bind the socket
	if (::bind(sListen, (sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		exit(0);
	}

	if (::listen(sListen, 1) == SOCKET_ERROR)
	{
		exit(0);
	}
	
	//******
	sockaddr_in remoteAddr;
	int nAddrLen = sizeof(remoteAddr);
	SOCKET sClient;
	char szText[] = "Succeed!!!\r\n";
	InitBM();
	while(true) 
	{ 
		// 接受新连接
		sClient = ::accept(sListen, (SOCKADDR*)&remoteAddr, &nAddrLen); 
		if(sClient == INVALID_SOCKET) 
		{ 
			continue; 
		} 
		
		//::send(sClient, szText, strlen(szText), 0); 

		while (true)
		{
			char cMsg; 
			int nRecv = ::recv(sClient, &cMsg, 1, 0); 
			if(nRecv == 0 || nRecv == SOCKET_ERROR) 
			{ 
				break;
			} 
			if (!HandlerMessage(cMsg, sClient))
			{
				break;
			}
			
		}
		
		// 关闭同客户端的连接
		::closesocket(sClient); 
	} 


	// 关闭监听套接字
	::closesocket(sListen);

	//end
	::WSACleanup();

	ReleaseBM();

	return 0;
}



bool HandlerMessage(const char &message, const SOCKET &s)
{
	switch (message)
	{
	case '0'://0则结束
		return false;
		break;
	case '1'://1则返回截图信息
		{		
			::send(s, (char *)PBMS, sizeof(BM), 0);
		}
		return true;
		break;
	case '2':
		{		
			GetScreemBitmap();
			::send(s, (char*)lpData, dwBmSize, 0);
		}
		return true;
		break;
	default:
		return true;
		break;
	}
	return true;
}

void InitBM()
{
	PBMS = new BM();
	ZeroMemory(&(PBMS->bi), sizeof(BITMAPINFO));
	ZeroMemory(&(PBMS->bfh), sizeof(BITMAPFILEHEADER));

	screenDC = CreateDC(_T("DISPLAY"),NULL, NULL, NULL);
	xRes = GetDeviceCaps(screenDC, HORZRES);
	yRes = GetDeviceCaps(screenDC, VERTRES);
	hBitmap = CreateCompatibleBitmap(screenDC, xRes, yRes) ;
	hdcMem = CreateCompatibleDC(screenDC);
	SelectObject(hdcMem, hBitmap);
	dwBmSize = ((xRes * 16)>>3) * yRes;

	PBMS->bi.bmiHeader.biSize = sizeof(BITMAPINFO);
	PBMS->bi.bmiHeader.biBitCount = 16;
	PBMS->bi.bmiHeader.biPlanes = 1;
	PBMS->bi.bmiHeader.biWidth = xRes;
	PBMS->bi.bmiHeader.biHeight = yRes;
	PBMS->bi.bmiHeader.biCompression = BI_RGB;
	DWORD dwBitsOffset = sizeof(BITMAPINFO) + sizeof(BITMAPINFOHEADER);
	PBMS->bfh.bfOffBits = dwBitsOffset;
	PBMS->bfh.bfType = MAKEWORD('B', 'M');
	PBMS->bfh.bfSize = dwBitsOffset + dwBmSize;
	lpData = HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, dwBmSize);
}

void ReleaseBM()
{
	DeleteObject(hBitmap);
	DeleteDC(screenDC);
	DeleteDC(hdcMem);
	HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, lpData);
	delete PBMS;
}

void GetScreemBitmap()
{

	BitBlt(hdcMem, 0, 0, xRes, yRes, screenDC, 0, 0, SRCCOPY);
	ZeroMemory(lpData, dwBmSize);
	GetDIBits(hdcMem, 
			  hBitmap, 
			  0L,
			  yRes,
			  lpData,
			  &(PBMS->bi),
			  DIB_RGB_COLORS);
}
