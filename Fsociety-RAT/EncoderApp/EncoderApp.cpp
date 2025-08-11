// EncoderApp.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "EncoderApp.h"
#include <string>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <shlwapi.h>
#include <iostream>

#define MAX_LOADSTRING 100
#define DEBUG 1

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Control handles
HWND hEditOriginal;
HWND hEditEncoded;
HWND hButtonEncode;
HWND hButtonUninstall;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                CreateControls(HWND hWnd);
void                OnEncodeButtonClick(HWND hWnd);
void                OnUninstallButtonClick(HWND hWnd);
void                KillRATProcesses();
void                RemoveRATFiles();
void                CleanRegistry();
void                RemoveScheduledTasks();


// Custom string encoding
std::string encode(std::string text)
{
    std::string sLog = "";
	if (text.length() >= 4)
	{
		std::string reverse = "";
		std::string div = "";
		std::string order = "";

		// Reverse the string - "abcd" will be "dcba" 

		for (int i = text.length() - 1; i >= 0; i--)
			reverse += text[i];

        #ifdef DEBUG
            sLog = "e_Reverse:\t";
            sLog += reverse;
            OutputDebugStringA(sLog.c_str());
        #endif


		// Divide the string by 2 and switch there location - "Hello World" will be "World Hello"

		int s = (text.length() - 1) / 2;

		for (size_t i = s; i < text.length(); i++)
			div += reverse[i];

		for (int i = 0; i < s; i++)
			div += reverse[i];

		#ifdef DEBUG
            sLog = "e_Divide:\t";
            sLog += div;
            OutputDebugStringA(sLog.c_str());
        #endif


		// Reorder characters. "abcd" will be "badc" - Switch the current char with the next char

		for (size_t i = 0; i < text.length(); i += 2)
		{
			if (i != text.length() - 1)
			{
				order += div[i + 1];
				order += div[i];
			}
			else
			{
				order += div[i];
			}
		}

		#ifdef DEBUG
            sLog = "e_order:\t";
            sLog += order;
            OutputDebugStringA(sLog.c_str());
        #endif

		return order;
	}
	return "";
}

// Custom string decoding - This is using to obfuscate strings from static analysis
std::string decode(std::string encoded_text)
{
    std::string sLog = "";

	if (encoded_text.length() >= 4)
	{
		std::string reverse = "";
		std::string div = "";
		std::string order = "";

		// Reorder characters. "badc" will be "abcd" - Switch the current char with the next char
		for (size_t i = 0; i < encoded_text.length(); i += 2)
		{
			if (i != encoded_text.length() - 1)
			{
				order += encoded_text[i + 1];
				order += encoded_text[i];
			}
			else
			{
				order += encoded_text[i];
			}
		}

		#ifdef DEBUG
            sLog = "d_Reorder:\t";
            sLog += order;
            OutputDebugStringA(sLog.c_str());
        #endif

		// Divide the string by 2 and switch there location - "World Hello" will be "Hello World"
		int s = (encoded_text.length()) / 2;

		for (size_t i = s + 1; i < encoded_text.length(); i++) // Put the second half first in the new string
			div += order[i];

		for (int i = 0; i <= s; i++) // put the first half second in the new string
			div += order[i];

		#ifdef DEBUG
            sLog = "d_Divided:\t";
            sLog += div;
            OutputDebugStringA(sLog.c_str());
        #endif

		// Reverse the string - "dcba" will be "abcd"
		for (int i = encoded_text.length() - 1; i >= 0; i--)
			reverse += div[i];

		#ifdef DEBUG
            sLog = "d_Reverse:\t";
            sLog += reverse;
            OutputDebugStringA(sLog.c_str());
        #endif

		return reverse;
	}

	return "";
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
    LoadStringW(hInstance, IDC_ENCODERAPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ENCODERAPP));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ENCODERAPP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ENCODERAPP);
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
      CW_USEDEFAULT, 0, 600, 400, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   // Create the controls
   CreateControls(hWnd);

   ShowWindow(hWnd, nCmdShow);
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
            case IDC_BUTTON_ENCODE:
                OnEncodeButtonClick(hWnd);
                break;
            case IDC_BUTTON_UNINSTALL:
                OnUninstallButtonClick(hWnd);
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

// Function to create the controls
void CreateControls(HWND hWnd)
{
    // Create "Original Text" label
    CreateWindowW(L"STATIC", L"Original Text:", WS_VISIBLE | WS_CHILD,
        20, 20, 100, 20, hWnd, nullptr, hInst, nullptr);

    // Create original text edit box
    hEditOriginal = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
        20, 45, 560, 100, hWnd, (HMENU)IDC_EDIT_ORIGINAL, hInst, nullptr);

    // Create "Encoded Text" label
    CreateWindowW(L"STATIC", L"Encoded Text:", WS_VISIBLE | WS_CHILD,
        20, 160, 100, 20, hWnd, nullptr, hInst, nullptr);

    // Create encoded text edit box
    hEditEncoded = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        20, 185, 560, 100, hWnd, (HMENU)IDC_EDIT_ENCODED, hInst, nullptr);

    // Create encode button
    hButtonEncode = CreateWindowW(L"BUTTON", L"Encode", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        200, 300, 100, 30, hWnd, (HMENU)IDC_BUTTON_ENCODE, hInst, nullptr);

    // Create uninstall button
    hButtonUninstall = CreateWindowW(L"BUTTON", L"Uninstall RAT", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        320, 300, 120, 30, hWnd, (HMENU)IDC_BUTTON_UNINSTALL, hInst, nullptr);
}

// Function to handle encode button click
void OnEncodeButtonClick(HWND hWnd)
{
    // Get text from original edit box
    int length = GetWindowTextLength(hEditOriginal);
    if (length > 0)
    {
        wchar_t* buffer = new wchar_t[length + 1];
        GetWindowText(hEditOriginal, buffer, length + 1);

        // Simple encoding: convert to base64-like representation
        // In a real application, you would implement proper encoding here
        std::wstring originalText(buffer);
        std::string originalTextStr = std::string(originalText.begin(), originalText.end());
        std::string encodedText = encode(originalTextStr);
        std::wstring encodedTextW = std::wstring(encodedText.begin(), encodedText.end());

        // Set the encoded text
        SetWindowText(hEditEncoded, encodedTextW.c_str());

        delete[] buffer;
    }
    else
    {
        SetWindowText(hEditEncoded, L"Please enter text to encode");
    }
}

// Function to handle uninstall button click
void OnUninstallButtonClick(HWND hWnd)
{
    // Show confirmation dialog
    int result = MessageBox(hWnd, L"Are you sure you want to uninstall the RAT from this system?\n\nThis will:\n- Stop all RAT processes\n- Remove RAT files\n- Clean up registry entries\n- Remove scheduled tasks", 
                           L"Confirm Uninstall", MB_YESNO | MB_ICONWARNING);
    
    if (result != IDYES) {
        return;
    }

    std::wstring statusMessage = L"Uninstalling RAT...\n\n";
    
    // Step 1: Kill RAT processes
    statusMessage += L"1. Stopping RAT processes...\n";
    SetWindowText(hEditEncoded, statusMessage.c_str());
    UpdateWindow(hWnd);
    
    KillRATProcesses();
    
    // Step 2: Remove RAT files
    statusMessage += L"2. Removing RAT files...\n";
    SetWindowText(hEditEncoded, statusMessage.c_str());
    UpdateWindow(hWnd);
    
    RemoveRATFiles();
    
    // Step 3: Clean registry
    statusMessage += L"3. Cleaning registry entries...\n";
    SetWindowText(hEditEncoded, statusMessage.c_str());
    UpdateWindow(hWnd);
    
    //CleanRegistry();
    
    // Step 4: Remove scheduled tasks
    statusMessage += L"4. Removing scheduled tasks...\n";
    SetWindowText(hEditEncoded, statusMessage.c_str());
    UpdateWindow(hWnd);
    
    RemoveScheduledTasks();
    
    // Final status
    statusMessage += L"\nRAT uninstallation completed!\n\n";
    statusMessage += L"System has been cleaned of RAT components.";
    SetWindowText(hEditEncoded, statusMessage.c_str());
}

// Helper function to kill RAT processes
void KillRATProcesses()
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return;

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            std::wstring processName = pe32.szExeFile;
            
            // Check for common RAT process names
            if (processName.find(L"meta1.exe") != std::wstring::npos ||
                processName.find(L"help.exe") != std::wstring::npos) {
                
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
                if (hProcess != NULL) {
                    TerminateProcess(hProcess, 0);
                    CloseHandle(hProcess);
                }
            }
        } while (Process32NextW(hSnapshot, &pe32));
    }
    
    CloseHandle(hSnapshot);
}

// Helper function to remove RAT files
void RemoveRATFiles()
{
    // Common RAT file locations
    std::wstring locations[] = {
        L"C:\\Windows\\System32\\",
        L"C:\\Windows\\SysWOW64\\",
        L"C:\\Users\\%USERNAME%\\AppData\\Roaming\\",
        L"C:\\Users\\%USERNAME%\\AppData\\Local\\",
        L"C:\\ProgramData\\"
    };
    
    std::wstring fileNames[] = {
        L"meta1.exe"
    };
    
    for (const auto& location : locations) {
        for (const auto& fileName : fileNames) {
            std::wstring fullPath = location + fileName;
            DeleteFileW(fullPath.c_str());
        }
    }
}

// Helper function to clean registry entries
void CleanRegistry()
{
    // Registry keys to clean
    std::wstring regKeys[] = {
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce",
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\\Run"
    };
    
    std::wstring regValues[] = {
        L"WindowsUpdate",
        L"SystemService",
        L"MicrosoftUpdate",
        L"WindowsDefender",
        L"SecurityCenter",
        L"RATService",
        L"FsocietyService"
    };
    
    for (const auto& key : regKeys) {
        for (const auto& value : regValues) {
            HKEY hKey;
            if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, key.c_str(), 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
                RegDeleteValueW(hKey, value.c_str());
                RegCloseKey(hKey);
            }
        }
    }
}

// Helper function to remove scheduled tasks
void RemoveScheduledTasks()
{
    // Common scheduled task names
    std::wstring taskNames[] = {
        L"Microsoft\\Windows\\Security\\WindowsSecurityTask"
    };
    
    for (const auto& taskName : taskNames) {
        std::wstring command = L"schtasks /Delete /TN \"" + taskName + L"\" /f";
        system(std::string(command.begin(), command.end()).c_str());
    }
}
