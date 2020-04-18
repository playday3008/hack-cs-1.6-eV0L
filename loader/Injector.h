///////////////////////////////////////////////////////////////////////////////
//
// 文件描述
//     Injector.h
//     关于注入远程进程相关的一个库, 命名空间为 Injector
//     支持 Windows9x, WindowsME, WindowsNT, Windows2000, WindowsXP
//
// 版权声明
//     Copyright (c) 2009 刘泽围 All Rights Reserved.
//
// 更新记录
//
//     2009年02月08日 : 创建
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
    // 操作系统版本, 封装成结构体
    //
    struct OS_VER
    {
        bool IsWin98;
        bool IsWinMe;
        bool IsWinXp;
    };


    //
    // 跟注入相关的一组操作系统 API, 封装成结构体
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
    // 功能
    //     得到操作系统版本
    //  
    // 参数
    //
    // osVer
    //     输出操作系统版本结构体变量的指针
    //
    // 返回值
    //     false : 失败
    //     true  : 成功
    //
    bool GetOsVer( OS_VER *osVer );

    // 
    // 功能
    //     根据操作系统版本, 动态得到跟注入进程相关的一组接口统一的系统 API
    //     如果该操作系统不提供其中某个函数, 则会由库负责模拟一个
    //  
    // 参数
    //
    // osApi
    //     输出系统 API 结构体变量的指针
    //
    // 返回值
    //     false : 失败
    //     true  : 成功
    //
    bool GetOsApi( OS_API *osApi );

    // 
    // 功能
    //     获取操作目标进程的相关数据
    //  
    // 参数
    //
    // exeName
    //     目标进程的可执行文件名(不区分路径)
    // 
    // processInfo
    //     输出该进程 PROCESS_INFORMATION 结构体变量的指针
    //
    // 返回值
    //     false : 失败
    //     true  : 成功
    //
    bool GetProcessInfo( const char *exeName, PROCESS_INFORMATION *processInfo );

    // 
    // 功能
    //     获取目标进程中一个模块的相关数据
    //  
    // 参数
    //
    // exeName
    //     目标进程的可执行文件名(不区分路径)
    //
    // moduleName
    //     目标进程中一个模块的文件名(不区分路径)
    //
    // moduleEntry32
    //     输出该模块 MODULEENTRY32 结构体变量的指针
    //
    // 返回值
    //     false : 失败
    //     true  : 成功
    //
    bool GetModuleInfo( const char *exeName, const char *moduleName, MODULEENTRY32 *moduleEntry32 );

    // 
    // 功能
    //     将模块注入到目标进程
    // 
    // 参数
    //
    // exeName
    //     目标进程的可执行文件名(不区分路径)
    // 
    // modulePath
    //     待注入到目标进程的模块的绝对路径或相对于目标进程的相对路径
    //
    // 返回值
    //     false : 失败
    //     true  : 成功
    //
    bool InjectModule( const char *exeName, const char *modulePath );
}

#endif