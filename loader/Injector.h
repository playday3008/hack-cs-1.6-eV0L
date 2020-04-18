///////////////////////////////////////////////////////////////////////////////
//
// File Description
//     Injector.h
//     Regarding the injection of a library related to the remote process, the namespace is Injector
//     Support Windows9x, WindowsME, WindowsNT, Windows2000, WindowsXP
//
// Copyright Notice
//     Copyright (c) 2009 Liu Zewei All Rights Reserved.
//
// Change log
//
//     08.02.2009 : Create
//
///////////////////////////////////////////////////////////////////////////////
#ifndef INJECTOR_H
#define INJECTOR_H

#pragma warning(disable:4996)
#include <stddef.h>
#include <windows.h>
#include <TLHELP32.H>

namespace Injector
{
    //
    // Operating system version, packaged as a structure
    //
    struct OS_VER
    {
        bool IsWin98;
        bool IsWinMe;
        bool IsWinXp;
    };


    //
    // A set of operating system APIs related to injection, encapsulated into a structure
    //
    struct OS_API
    {
        HANDLE  ( WINAPI *CreateToolhelp32Snapshot )( DWORD  dwFlags, DWORD th32ProcessID );
        BOOL    ( WINAPI *Process32First )          ( HANDLE hSnapshot, LPPROCESSENTRY32 lppe ); 
        BOOL    ( WINAPI *Process32Next )           ( HANDLE hSnapshot, LPPROCESSENTRY32 lppe );
        BOOL    ( WINAPI *Module32First )           ( HANDLE hSnapshot, LPMODULEENTRY32 lpme ); 
        BOOL    ( WINAPI *Module32Next )            ( HANDLE hSnapshot, LPMODULEENTRY32 lpme );
        BOOL    ( WINAPI *Thread32First )           ( HANDLE hSnapshot, LPTHREADENTRY32 lpte );
        BOOL    ( WINAPI *Thread32Next )            ( HANDLE hSnapshot, LPTHREADENTRY32 lpte );
        HANDLE  ( WINAPI *OpenProcess )             ( DWORD dwDesiredAccess,
                                                      BOOL  bInheritHandle,
                                                      DWORD dwProcessId
                                                    );
        HANDLE  ( WINAPI *OpenThread )              ( DWORD dwDesiredAccess,
                                                      BOOL  bInheritHandle,
                                                      DWORD dwThreadId
                                                    );
        LPVOID  ( WINAPI *VirtualAllocEx )          ( HANDLE hProcess,
                                                      LPVOID lpAddress,
                                                      SIZE_T dwSize,
                                                      DWORD  flAllocationType,
                                                      DWORD  flProtect
                                                    );
        BOOL    ( WINAPI *VirtualFreeEx )           ( HANDLE hProcess,
                                                      LPVOID lpAddress,
                                                      SIZE_T dwSize,
                                                      DWORD  dwFreeType
                                                    );        
    };

    // 
    // Features
    //     Get the operating system version
    //  
    // parameter
    //
    // osVer
    //     Pointer to output operating system version structure variable
    //
    // return value
    //     false : failure
    //     true  : success
    //
    bool GetOsVer( OS_VER *osVer );

    // 
    // Features
    //     According to the operating system version, dynamically obtain a set of unified system APIs related to the injection process
    //     If the operating system does not provide one of these functions, the library is responsible for simulating one
    //  
    // parameter
    //
    // osApi
    //     Pointer to output system API structure variable
    //
    // return value
    //     false : failed
    //     true  : success
    //
    bool GetOsApi( OS_API *osApi );

    // 
    // Features
    //     Obtain the relevant data of the operation target process
    //  
    // parameter
    //
    // exeName
    //     The executable file name of the target process (path indistinguishable)
    // 
    // processInfo
    //     Output a pointer to the process PROCESS_INFORMATION structure variable
    //
    // return value
    //     false : failed
    //     true  : success
    //
    bool GetProcessInfo( const char *exeName, PROCESS_INFORMATION *processInfo );

    // 
    // Features
    //     Get data about a module in the target process
    //  
    // parameter
    //
    // exeName
    //     The executable file name of the target process (path indistinguishable)
    //
    // moduleName
    //     File name of a module in the target process (path indistinguishable)
    //
    // moduleEntry32
    //     Output the pointer of the module variable of this module MODULEENTRY32
    //
    // return value
    //     false : failed
    //     true  : success
    //
    bool GetModuleInfo( const char *exeName, const char *moduleName, MODULEENTRY32 *moduleEntry32 );

    // 
    // Features
    //     Inject the module into the target process
    // 
    // parameter
    //
    // exeName
    //     The executable file name of the target process (path indistinguishable)
    // 
    // modulePath
    //     The absolute path of the module to be injected into the target process or the relative path relative to the target process
    //
    // return value
    //     false : failed
    //     true  : success
    //
    bool InjectModule( const char *exeName, const char *modulePath );
}

#endif