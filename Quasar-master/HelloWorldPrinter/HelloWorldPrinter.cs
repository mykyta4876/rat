/*
using System;
using System.Runtime.InteropServices;

namespace ExampleDll
{
    public class HelloWorldPrinter
    {
        // DllMain function
        [DllImport("kernel32.dll")]
        public static extern bool DllMain(IntPtr hModule, uint ul_reason_for_call, IntPtr lpReserved);

        // PrintHello function
        public static void PrintHello()
        {
            Console.WriteLine("Hello");
        }
    }
}
*/

using System;

namespace ExampleDll
{
    public class HelloWorldPrinter
    {
        // Static constructor
        static HelloWorldPrinter()
        {
            Console.WriteLine("Hello from DLL");
        }

        // PrintHello function
        public static void PrintHello()
        {
            Console.WriteLine("Hello");
        }
    }
}