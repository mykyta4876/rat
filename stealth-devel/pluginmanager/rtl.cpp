#include <windows.h>
#include <stdarg.h>


// Force the linker to include USER32.LIB
#pragma comment(linker, "/defaultlib:user32.lib")
#pragma comment(linker, "/nodefaultlib" )
#include "rtl.h"


int __cdecl _purecall(void)
{
    return 0;
}

void * __cdecl operator new(unsigned int s)
{
  return HeapAlloc( GetProcessHeap(), 0, s );
}

void __cdecl operator delete(void* p)
{
  if (p)
    HeapFree(GetProcessHeap(), 0, p );
}

void * __cdecl malloc(size_t size)
{
    return HeapAlloc( GetProcessHeap(), 0, size );
}

 #pragma function(memcpy)
 __declspec(naked) void * __cdecl memcpy(void *,const void *,size_t)
  {
    __asm
    {
      push    esi;
      push    edi;

      mov     edi, [esp+12];
      mov     eax, edi;
      mov     esi, [esp+16];
      mov     ecx, [esp+20];
      shr     ecx, 2;
      jz      memcpy_tail;
      rep     movsd;

memcpy_tail:
      mov     ecx, [esp+20];
      and     ecx, 3;
      rep     movsb;

      pop     edi;
      pop     esi;
      ret;
    }
  }

 #pragma function(memset)
 __declspec(naked) void * __cdecl memset(void *,int,size_t)
  {
    __asm
    {
      push    edi;

      mov     edi, [esp+8];
      movzx   eax, byte ptr [esp+12];
      mov     edx, eax;
      shl     edx, 8;
      or      eax, edx;
      mov     edx, eax;
      shl     edx, 16;
      or      eax, edx;
      mov     ecx, [esp+16];
      shr     ecx, 2;
      jz      memset_tail;
      rep     stosd;

memset_tail:
      mov     ecx, [esp+16];
      and     ecx, 3;
      rep     stosb;

      mov     eax, [esp+8];
      pop     edi;
      ret;
    }
  }

void __cdecl free(void *ptr)
{
    HeapFree(GetProcessHeap(),0,ptr);
}

void * __cdecl realloc(void * p, size_t size)
{
    if (p)
        return HeapReAlloc( GetProcessHeap(), 0, p, size );
    else    // 'p' is 0, and HeapReAlloc doesn't act like realloc() here
        return HeapAlloc( GetProcessHeap(), 0, size );
}

int __cdecl _printf(const char * format, ...)
{
    char szBuff[1024];
    int retValue;
    DWORD cbWritten;
    va_list argptr;

    va_start( argptr, format );
    retValue = wvsprintf( szBuff, format, argptr );
    va_end( argptr );

    WriteFile(  GetStdHandle(STD_OUTPUT_HANDLE), szBuff, retValue,
                &cbWritten, 0 );

    return retValue;
}
