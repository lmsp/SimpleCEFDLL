/* Test_SimpleCEFDLL.cpp

Author: Luis Miguel Santamaría Pérez

The MIT License (MIT)

Copyright (c) 2016 Luis Miguel Santamaría Pérez

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
Status API Training Shop Blog About Pricing
© 2016 GitHub, Inc. Terms Privacy Security 

*/

#include "stdafx.h"
#include "Test_SimpleCEFDLL.h"

#define MAX_LOADSTRING 100

HINSTANCE hInst;								
TCHAR szTitle[MAX_LOADSTRING];					
TCHAR szWindowClass[MAX_LOADSTRING];			

ATOM				MyRegisterClass(HINSTANCE hInstance);

//Make a change in InitInstance add return parameter HWND
BOOL				InitInstance(HINSTANCE, int, HWND&);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

//Functions that we will load from the DLL
typedef void (*SimpleCEFDLL_Initialize)(HINSTANCE);
typedef void (*SimpleCEFDLL_CreateBrowser)(HWND,const wchar_t*);
typedef void (*SimpleCEFDLL_ResizeBrowser)(HWND);
typedef void (*SimpleCEFDLL_Shutdown)(bool);

//Make it accesible globally ( Not a good solution, better declare it in a class )
SimpleCEFDLL_Initialize g_SimpleCEFDLL_Initialize = NULL;
SimpleCEFDLL_CreateBrowser g_SimpleCEFDLL_CreateBrowser = NULL;
SimpleCEFDLL_ResizeBrowser g_SimpleCEFDLL_ResizeBrowser = NULL;
SimpleCEFDLL_Shutdown g_SimpleCEFDLL_Shutdown = NULL;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	//Load the functions
	HINSTANCE hDLL = LoadLibrary(L"SimpleCEFDLL.dll");
	g_SimpleCEFDLL_Initialize = (SimpleCEFDLL_Initialize)GetProcAddress((HMODULE)hDLL, "SimpleCEFDLL_Initialize");
	g_SimpleCEFDLL_CreateBrowser = (SimpleCEFDLL_CreateBrowser)GetProcAddress((HMODULE)hDLL, "SimpleCEFDLL_CreateBrowser");
	g_SimpleCEFDLL_ResizeBrowser = (SimpleCEFDLL_ResizeBrowser)GetProcAddress((HMODULE)hDLL, "SimpleCEFDLL_ResizeBrowser");
	g_SimpleCEFDLL_Shutdown = (SimpleCEFDLL_Shutdown)GetProcAddress((HMODULE)hDLL, "SimpleCEFDLL_Shutdown");
	
	//Initialize CEF
	(*g_SimpleCEFDLL_Initialize)( hInstance );

	MSG msg;
	HACCEL hAccelTable;

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TEST_SIMPLECEFDLL, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	HWND hWnd;
	if (!InitInstance (hInstance, nCmdShow, hWnd))
	{
		return FALSE;
	}

	//Create the browser asociated to our window
	(*g_SimpleCEFDLL_CreateBrowser)( hWnd, L"http:\\www.google.es" );

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TEST_SIMPLECEFDLL));

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	//Shutdown CEF and terminate process
	(*g_SimpleCEFDLL_Shutdown)( true );

	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TEST_SIMPLECEFDLL));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TEST_SIMPLECEFDLL);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND& hWnd)
{
   hInst = hInstance;

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
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_SIZE:
	{
		//When we resize call this method
		(*g_SimpleCEFDLL_ResizeBrowser)( hWnd );
	}
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
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
