///////////////////////////////////////////////////////////////////////////////
//
// 文件描述
//     Injector.cpp
//
// 版权声明
//     Copyright (c) 2009 刘泽围 All Rights Reserved.
//
// 更新记录
//
//     2009年02月08日 : 创建
//
///////////////////////////////////////////////////////////////////////////////
#include "Injector.h"

// 操作系统版本
Injector::OS_VER OsVer;
bool IsGotOsVer = false;

// 注入相关系统 API
Injector::OS_API OsApi = { NULL };
bool IsGotOsApi = false;

// 注入代码
DWORD LoadLibraryAddress = NULL;
#pragma pack( 1 ) // 使注入代码字节有序
struct INJECT_CODE
{
    BYTE  PushOpc;
    DWORD PushAdr;
    BYTE  CallOpc;
    DWORD CallAdr;
    BYTE  DoneOpc;
    WORD  DoneAdr;
    char  ModulePath[MAX_PATH];
};
#pragma pack()

// Windows9x 系统上模拟 OpenThread
HANDLE WINAPI OpenThread9x( DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId );

// Windows9x 系统上模拟 VirtualAllocEx
LPVOID WINAPI VirtualAllocEx9x( HANDLE hProcess, LPVOID lpAddress, DWORD dwSize, DWORD flAllocationType, DWORD flProtect );

// Windows9x 系统上模拟 VirtualFreeEx
BOOL WINAPI VirtualFreeEx9x( HANDLE hProcess, LPVOID lpAddress, DWORD dwSize, DWORD dwFreeType );

// 通过特征串从内存中搜索到其起始地址
DWORD SearchMemory( DWORD start, DWORD length, BYTE *pattern, CHAR *mask );

// 通过比较判断两个模块的名字判断两个模块是否为同一个模块
// 注意 : 此比较为完全忽略路径与大小写的比较
bool IsSameName( const char *targetString, const char *sourceString );

bool Injector::GetOsVer( OS_VER *osVer )
{
    OSVERSIONINFO OSVI;
    OSVI.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    if ( !GetVersionEx( &OSVI ) )
        return false;

    osVer->IsWin98 = ( OSVI.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS && OSVI.dwMajorVersion==4 && OSVI.dwMinorVersion<=10 ) ? true : false;
    osVer->IsWinMe = ( OSVI.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS && OSVI.dwMajorVersion==4 && OSVI.dwMinorVersion>=90 ) ? true : false;
    osVer->IsWinXp = ( OSVI.dwPlatformId==VER_PLATFORM_WIN32_NT ) ? true : false;

    return true;
}

bool Injector::GetOsApi( OS_API *osApi )
{
    //
    // 得到操作系统版本
    //
    OS_VER osVer;
    if ( GetOsVer( &osVer ) == false )
        return false;
    
    //
    // 根据操作系统版本动态得到跟注入相关的操作系统 API
    //
    HINSTANCE kernel32 = GetModuleHandle( "kernel32.dll" );
    if ( kernel32 == NULL )
    {
        kernel32 = LoadLibrary( "kernel32.dll" );
        if ( kernel32 == NULL )
            return false;
    }

   	*(PDWORD)(&osApi->CreateToolhelp32Snapshot) = (DWORD)GetProcAddress( kernel32, "CreateToolhelp32Snapshot" );
    *(PDWORD)(&osApi->Process32First) = (DWORD)GetProcAddress( kernel32, "Process32First" );
    *(PDWORD)(&osApi->Process32Next)  = (DWORD)GetProcAddress( kernel32, "Process32Next" );
    *(PDWORD)(&osApi->Module32First)  = (DWORD)GetProcAddress( kernel32, "Module32First" );
    *(PDWORD)(&osApi->Module32Next)   = (DWORD)GetProcAddress( kernel32, "Module32Next" );
    *(PDWORD)(&osApi->Thread32First)  = (DWORD)GetProcAddress( kernel32, "Thread32First" );
    *(PDWORD)(&osApi->Thread32Next)   = (DWORD)GetProcAddress( kernel32, "Thread32Next" );
    *(PDWORD)(&osApi->OpenProcess)    = (DWORD)GetProcAddress( kernel32, "OpenProcess" );
    *(PDWORD)(&osApi->OpenThread)     = (DWORD)GetProcAddress( kernel32, "OpenThread" );
    *(PDWORD)(&osApi->VirtualAllocEx) = (DWORD)GetProcAddress( kernel32, "VirtualAllocEx" );
    *(PDWORD)(&osApi->VirtualFreeEx)  = (DWORD)GetProcAddress( kernel32, "VirtualFreeEx" );
    
    if ( osVer.IsWin98 )
    {
        *(PDWORD)(&osApi->OpenThread) = (DWORD)OpenThread9x;
        *(PDWORD)(&osApi->VirtualAllocEx) = (DWORD)VirtualAllocEx9x;
        *(PDWORD)(&osApi->VirtualFreeEx)  = (DWORD)VirtualFreeEx9x;        
    }
    else if ( osVer.IsWinMe )
    {
        *(PDWORD)(&osApi->VirtualAllocEx) = (DWORD)VirtualAllocEx9x;
        *(PDWORD)(&osApi->VirtualFreeEx)  = (DWORD)VirtualFreeEx9x;
    }
    
    return ( osApi->CreateToolhelp32Snapshot
             && osApi->Process32First
             && osApi->Process32Next
             && osApi->Module32First 
             && osApi->Module32Next
             && osApi->Thread32First
             && osApi->Thread32Next
             && osApi->OpenProcess
             && osApi->OpenThread
             && osApi->VirtualAllocEx
             && osApi->VirtualFreeEx             
           );
}

bool Injector::GetProcessInfo( const char *exeName, PROCESS_INFORMATION *processInfo )
{
    // 得到相关系统 API
    if ( IsGotOsApi == false )
        if ( ( IsGotOsApi = GetOsApi( &OsApi ) ) == false )
            return false;

    //
    // 枚举所有进程找出目标进程
    //
    HANDLE snapshotProcess = OsApi.CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if ( snapshotProcess == INVALID_HANDLE_VALUE )
        return false;

    PROCESSENTRY32 PE32;
    PE32.dwSize = sizeof( PROCESSENTRY32 );

    bool isExist = false;
    if ( OsApi.Process32First( snapshotProcess, &PE32 ) )
    {
        do
        {
            if ( IsSameName( PE32.szExeFile, exeName ) )
            {
                CloseHandle( snapshotProcess );
                isExist = true;

                break;
            }
        } while ( isExist == false && OsApi.Process32Next( snapshotProcess, &PE32 ) );
    }
    
    CloseHandle( snapshotProcess );

    if ( isExist == false ) 
        return false;

    //
    // 枚举所有线程找出目标进程中的任一线程
    //
    HANDLE snapshotThread = OsApi.CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 );
    if ( snapshotThread == INVALID_HANDLE_VALUE )
        return false;

    THREADENTRY32 TE32;
    TE32.dwSize = sizeof( THREADENTRY32 );

    isExist = false;
    if ( OsApi.Thread32First( snapshotThread, &TE32 ) )
    {
        do
        {
            if ( TE32.th32OwnerProcessID == PE32.th32ProcessID )
            {
                CloseHandle( snapshotThread );
                isExist = true;

                break;
            }
        } while ( isExist == false && OsApi.Thread32Next( snapshotThread, &TE32 ) );
    }

    CloseHandle( snapshotThread );

    if ( isExist == false )
        return false;

    //
    // 分别通过 Id 找到其句柄, 填充 processInfo, 输出结果
    //
    processInfo->dwProcessId = PE32.th32ProcessID;
    processInfo->hProcess = OsApi.OpenProcess( PROCESS_ALL_ACCESS, FALSE, PE32.th32ProcessID );
    if ( processInfo->hProcess == NULL )
        return false;

    processInfo->dwThreadId = TE32.th32ThreadID;
    processInfo->hThread = OsApi.OpenThread( THREAD_ALL_ACCESS, FALSE, TE32.th32ThreadID );
    if ( processInfo->hThread == NULL )
    {
        CloseHandle( processInfo->hProcess );

        return false;
    }

    return true;
}

bool Injector::GetModuleInfo( const char *exeName, const char *moduleName, MODULEENTRY32 *moduleEntry32 )
{
    PROCESS_INFORMATION processInfo;

    // 得到相关系统 API
    if ( IsGotOsApi == false )
        if ( ( IsGotOsApi = GetOsApi( &OsApi ) ) == false )
            return false;

    // 获取操作目标进程的相关数据
    if ( GetProcessInfo( exeName, &processInfo ) == false )
        return false;

    HANDLE snapshotModule = OsApi.CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, processInfo.dwProcessId );
    if ( snapshotModule == INVALID_HANDLE_VALUE )
    {
        CloseHandle( processInfo.hThread );
        CloseHandle( processInfo.hProcess );        

        return false;
    }

    MODULEENTRY32 ME32;
    ME32.dwSize = sizeof( MODULEENTRY32 );
    if ( OsApi.Module32First( snapshotModule, &ME32 ) )
    {
        do
        {            
            if ( IsSameName( ME32.szExePath, moduleName ) )
            {	
                if ( moduleEntry32 != NULL )
                    *moduleEntry32 = ME32;

                CloseHandle( processInfo.hThread );
                CloseHandle( processInfo.hProcess );
                CloseHandle( snapshotModule );
                
                return true;
            }
        } while ( OsApi.Module32Next( snapshotModule, &ME32 ) );
    }

    CloseHandle( processInfo.hThread );
    CloseHandle( processInfo.hProcess );
    CloseHandle( snapshotModule );

    return false;
}

bool Injector::InjectModule( const char *exeName, const char *modulePath )
{
    PROCESS_INFORMATION processInfo;    
    DWORD beginPosition, endPosition;
    INJECT_CODE injectCode;
    
    // 得到系统版本
    if ( IsGotOsVer == false )
        if ( ( IsGotOsVer = GetOsVer( &OsVer ) ) == false )
            return false;

    // 得到相关系统 API
    if ( IsGotOsApi == false )
        if ( ( IsGotOsApi = GetOsApi( &OsApi ) ) == false )
            return false;    

    // 检查待注入模块是否已经被加载
    if ( GetModuleInfo( exeName, modulePath, NULL ) == true )
        return true;

    // 获取操作目标进程的相关数据
    if ( GetProcessInfo( exeName, &processInfo ) == false )
        return false;   

    //
    // 注入目标进程
    //
    if ( LoadLibraryAddress == NULL )
    {
        HINSTANCE kernel32 = GetModuleHandle( "kernel32.dll" );
        if ( kernel32 == NULL )
        {
            kernel32 = LoadLibrary( "kernel32.dll" );
            if ( kernel32 == NULL )
                return false;
        }

        if ( ( LoadLibraryAddress = (DWORD)GetProcAddress( kernel32, "LoadLibraryA" ) ) == NULL )
        {
            CloseHandle( processInfo.hThread );
            CloseHandle( processInfo.hProcess );

            return false;
        }
    }

    beginPosition = (DWORD)OsApi.VirtualAllocEx( processInfo.hProcess, NULL, sizeof(INJECT_CODE), MEM_COMMIT, PAGE_EXECUTE_READWRITE );
    if ( beginPosition == NULL )
    {
        CloseHandle( processInfo.hThread );
        CloseHandle( processInfo.hProcess );

        return false;
    }

    endPosition = beginPosition + offsetof( INJECT_CODE, DoneOpc );

    // 根据操作系统版本填充相应注入代码结构体
    injectCode.PushOpc = 0x68;   // 0x68 是 push 的机器码
    injectCode.PushAdr = beginPosition + offsetof( INJECT_CODE, ModulePath );
    injectCode.CallOpc = 0xE8;   // 0xE8 是相对地址 call 的机器码
    injectCode.CallAdr = LoadLibraryAddress - endPosition;
    if ( OsVer.IsWin98 || OsVer.IsWinMe )
    {        
        injectCode.DoneOpc = 0xEB;
        injectCode.DoneAdr = 0xFE; // 0xFEEB 是跳转到当前行, 也就是原地循环
    }
    else
    {
        injectCode.DoneOpc = 0xC2;   // 0xC2 是返回
        injectCode.DoneAdr = 0x0004; // 返回值
    }
    strcpy_s( injectCode.ModulePath, MAX_PATH, modulePath );

    // 向目标进程写入注入代码
    if ( !WriteProcessMemory( processInfo.hProcess, (VOID*)beginPosition, &injectCode, sizeof(INJECT_CODE), NULL ) )
    {
        OsApi.VirtualFreeEx( processInfo.hProcess, (VOID*)beginPosition, sizeof(INJECT_CODE), MEM_DECOMMIT );
        CloseHandle( processInfo.hThread );
        CloseHandle( processInfo.hProcess );

        return false;
    }

    // 根据操作系统版本用相应的方式运行已写入的注入代码
    if ( OsVer.IsWin98 || OsVer.IsWinMe )
    {
        // 检查目标进程是否已经启动并初始化好, 这里以目标进程加载了 Kernel32.dll 为
        // 其启动成功的标志, 且判断后再 Sleep() 给其启动时间, 或许还有其他更好更稳
        // 健的方法
        if ( GetModuleInfo( exeName, "kernel32.dll", NULL ) == false )
        {
            OsApi.VirtualFreeEx( processInfo.hProcess, (VOID*)beginPosition, sizeof(INJECT_CODE), MEM_DECOMMIT );
            CloseHandle( processInfo.hThread );
            CloseHandle( processInfo.hProcess );

            return false;
        }
        Sleep( 50 );                       
        
        //
        // 通过修改目标进程中某一线程的线程环境的方法, 使目标进程执行我们写入的代码
        // 注意: 以下任一句 ResumeThread() 函数调用失败, 目标线程都将被永久挂起, 必
        //       造成目标进程不可预计的严重错误, 建议的做法是重新启动目标进程.
        //
        INT countSuspend;
        CONTEXT orgContext, runContext;

        if ( SuspendThread( processInfo.hThread ) == 0xFFFFFFFF )
            return false;

        countSuspend = 1;
        
        orgContext.ContextFlags = CONTEXT_FULL;
        if ( !GetThreadContext( processInfo.hThread, &orgContext ) )
        {
            ResumeThread( processInfo.hThread );      
            OsApi.VirtualFreeEx( processInfo.hProcess, (VOID*)beginPosition, sizeof(INJECT_CODE), MEM_DECOMMIT );
            CloseHandle( processInfo.hThread );
            CloseHandle( processInfo.hProcess );

            return false;
        }
        
   	    runContext = orgContext;
        runContext.Eip = beginPosition;
        if ( !SetThreadContext( processInfo.hThread, &runContext ) )
        {               
            ResumeThread( processInfo.hThread );
            OsApi.VirtualFreeEx( processInfo.hProcess, (VOID*)beginPosition, sizeof(INJECT_CODE), MEM_DECOMMIT );
            CloseHandle( processInfo.hThread );
            CloseHandle( processInfo.hProcess );

            return false;
        }
        
        while ( runContext.Eip != endPosition )
        {
            // 确保并调整该线程到运行状态
            while ( true )
            {
                DWORD returnValue = ResumeThread( processInfo.hThread );
            
                if ( returnValue <= 1 )
                {
                    if ( returnValue == 1 )
                        countSuspend--;

                    break;
                }
                else if ( returnValue < 0xFFFFFFFF )
                {
                    countSuspend--;
                }
                else if ( returnValue == 0xFFFFFFFF )
                {
                    OsApi.VirtualFreeEx( processInfo.hProcess, (VOID*)beginPosition, sizeof(INJECT_CODE), MEM_DECOMMIT );
                    CloseHandle( processInfo.hThread );
                    CloseHandle( processInfo.hProcess );

                    return false;
                }
            }
            
            // 让线程运行我们注入的代码
            Sleep( 50 );

            // 确保并调整该线程到暂停状态
            if ( SuspendThread( processInfo.hThread ) == 0xFFFFFFFF )
            {
                OsApi.VirtualFreeEx( processInfo.hProcess, (VOID*)beginPosition, sizeof(INJECT_CODE), MEM_DECOMMIT );
                CloseHandle( processInfo.hThread );
                CloseHandle( processInfo.hProcess );

                return false;        
            }
            countSuspend++;

            if ( !GetThreadContext( processInfo.hThread, &runContext ) )
            {            
                if ( countSuspend > 0 )
                    while ( countSuspend-- > 0 && ResumeThread( processInfo.hThread ) != 0xFFFFFFFF );
                else if ( countSuspend < 0 )
                    while ( countSuspend++ < 0 && SuspendThread( processInfo.hThread ) != 0xFFFFFFFF );
            
                OsApi.VirtualFreeEx( processInfo.hProcess, (VOID*)beginPosition, sizeof(INJECT_CODE), MEM_DECOMMIT );
                CloseHandle( processInfo.hThread );
                CloseHandle( processInfo.hProcess );

                return false;        
            }            
        }

        if ( !SetThreadContext( processInfo.hThread, &orgContext ) )
        {
            if ( countSuspend > 0 )
                while ( countSuspend-- > 0 && ResumeThread( processInfo.hThread ) != 0xFFFFFFFF );
            else if ( countSuspend < 0 )
                while ( countSuspend++ < 0 && SuspendThread( processInfo.hThread ) != 0xFFFFFFFF );
        
            OsApi.VirtualFreeEx( processInfo.hProcess, (VOID*)beginPosition, sizeof(INJECT_CODE), MEM_DECOMMIT );
            CloseHandle( processInfo.hThread );
            CloseHandle( processInfo.hProcess );

            return false;
        }

        //
        // 操作成功, 恢复线程到初始状态
        //
        if ( countSuspend > 0 )
            while ( countSuspend-- > 0 && ResumeThread( processInfo.hThread ) != 0xFFFFFFFF );
        else if ( countSuspend < 0 )
            while ( countSuspend++ < 0 && SuspendThread( processInfo.hThread ) != 0xFFFFFFFF );
    }
    else // Windows NT, Windows2000, WindowsXP 采用创建远程线程的方法运行注入代码
    {
        HANDLE remoteThread = CreateRemoteThread( processInfo.hProcess, NULL, 0,
            (LPTHREAD_START_ROUTINE)beginPosition, NULL, 0, NULL );
        if ( remoteThread == NULL )
        {
            OsApi.VirtualFreeEx( processInfo.hProcess, (VOID*)beginPosition, sizeof(INJECT_CODE), MEM_DECOMMIT );
            CloseHandle( processInfo.hThread );
            CloseHandle( processInfo.hProcess );

            return false;
        }

        WaitForSingleObject( remoteThread, INFINITE );

        DWORD moduleBase;
        if ( !GetExitCodeThread( remoteThread, &moduleBase ) || !moduleBase )
        {
            OsApi.VirtualFreeEx( processInfo.hProcess, (VOID*)beginPosition, sizeof(INJECT_CODE), MEM_DECOMMIT );
            CloseHandle( processInfo.hThread );
            CloseHandle( processInfo.hProcess );

            return false;
        }  
    }
    
    OsApi.VirtualFreeEx( processInfo.hProcess, (VOID*)beginPosition, sizeof(INJECT_CODE), MEM_DECOMMIT );
    CloseHandle( processInfo.hThread );
    CloseHandle( processInfo.hProcess );
    
    return true;
}

HANDLE WINAPI OpenThread9x( DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId )
{
    //
    // 说明 : 
    //
    // 1> 系统 API OpenProcess() 做了什么?
    //    1、检查目标是否真的是一个进程
    //    2、调用微软未公开的一个系统函数 GetHandle()
    //
    // 2> 我们的 OpenThread9x() 做了什么?
    //    直接得到线程的 TDB, 然后调用 OpenProcess() 调用的 GetHandle() 得到线程句柄
    //
    DWORD  processID, obsfucator, *pThreadDataBase;
    HANDLE hThread;
    HANDLE ( WINAPI *pInternalOpenProcess )( DWORD, BOOL, DWORD );
    
    processID = GetCurrentProcessId();
    __asm mov eax,fs:[0x30];
    __asm xor eax,processID;
    __asm mov obsfucator,eax;
    
    pThreadDataBase = ( DWORD* ) ( dwThreadId ^ obsfucator );
    if ( IsBadReadPtr( pThreadDataBase, sizeof(DWORD) ) || ( ( *pThreadDataBase & 0x7 ) != 0x7 ) )
        return NULL;
    
    *(PDWORD)(&pInternalOpenProcess) = SearchMemory( (DWORD)OpenProcess, 0xFF, (BYTE*)"\xB9\x00\x00\x00\x00", "xxxxx" );
    if ( pInternalOpenProcess == NULL )
        return NULL;
    
    __asm mov   eax, pThreadDataBase;
    __asm push  dwThreadId;
    __asm push  bInheritHandle;
    __asm push  dwDesiredAccess;
    __asm call  pInternalOpenProcess;
    __asm mov   hThread, eax;
      
    return hThread;
}

LPVOID WINAPI VirtualAllocEx9x( HANDLE hProcess, LPVOID lpAddress, DWORD dwSize, DWORD flAllocationType, DWORD flProtect )
{
    LPVOID ( WINAPI *pVirtualAlloc )( LPVOID, SIZE_T, DWORD, DWORD );

    HINSTANCE kernel32 = GetModuleHandle( "kernel32.dll" );
    if ( kernel32 == NULL )
        kernel32 = LoadLibrary( "kernel32.dll" );

    *(PDWORD)(&pVirtualAlloc) = (DWORD)GetProcAddress( kernel32, "VirtualAlloc" );
    
    return pVirtualAlloc( lpAddress, dwSize, flAllocationType|0x8000000, flProtect );          
}

BOOL WINAPI VirtualFreeEx9x( HANDLE hProcess, LPVOID lpAddress, DWORD dwSize, DWORD dwFreeType )
{
    BOOL ( WINAPI *pVirtualFree )( LPVOID, SIZE_T, DWORD );

    HINSTANCE kernel32 = GetModuleHandle( "kernel32.dll" );
    if ( kernel32 == NULL )
        kernel32 = LoadLibrary( "kernel32.dll" );

    *(PDWORD)(&pVirtualFree) = (DWORD)GetProcAddress( kernel32, "VirtualFree" );
    
    return pVirtualFree( lpAddress, dwSize, dwFreeType );
}

DWORD SearchMemory( DWORD start, DWORD length, BYTE *pattern, CHAR *mask )
{
    BYTE *currentAddress;
    BYTE *currentPattern;
    CHAR *currentMask;

    for ( DWORD i=0; i<length; i++ )
    {
        currentAddress = (BYTE*)(start+i);
        currentPattern = pattern;
        currentMask    = mask;
        for ( ; *currentMask; currentAddress++,currentPattern++,currentMask++ )
        {
            if ( *currentMask=='x' && *currentAddress!=*currentPattern )
                break;
        }
        if ( *currentMask == NULL ) return ( start + i );
    }
    
    return NULL;
}

bool IsSameName( const char *targetString, const char *sourceString )
{
    const char *index, *i, *j;
    
    for( index=i=targetString; *index; index++ )
        if ( *index == '\\' )
            i = index+1;
        
    for( index=j=sourceString; *index; index++ )
        if ( *index == '\\' )
            j = index+1;
            
    for ( ; *i && *j ; i++,j++ )
    {
        if ( tolower( *i ) != tolower( *j ) )
            return false;
    }
            
    return ( *j == 0 );
}