// File: Program.cs

using System;
using System.Runtime.InteropServices;

class Program
{
    // DllMain function signature
    [DllImport("HelloWorldPrinter.dll", CallingConvention = CallingConvention.StdCall)]
    public static extern bool DllMain(IntPtr hModule, uint ul_reason_for_call, IntPtr lpReserved);

    static void Main()
    {
        // Load the DLL
        IntPtr moduleHandle = LoadLibrary("HelloWorldPrinter.dll");

        // Call DllMain to print "Hello"
        DllMain(moduleHandle, 1, IntPtr.Zero);

        // Unload the DLL
        FreeLibrary(moduleHandle);
    }

    // LoadLibrary function
    [DllImport("kernel32.dll")]
    public static extern IntPtr LoadLibrary(string dllToLoad);

    // FreeLibrary function
    [DllImport("kernel32.dll")]
    public static extern bool FreeLibrary(IntPtr hModule);
}
