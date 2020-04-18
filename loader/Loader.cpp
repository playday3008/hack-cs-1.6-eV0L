///////////////////////////////////////////////////////////////////////////////
//
// File Description
//     Loader.cpp
//
// Copyright Notice
//     Copyright (c) 2009 Liu Zewei All Rights Reserved.
//
// Change log
//
//     08.02.2009 : Сreate
//
///////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "Injector.h"
#pragma comment( lib, "user32.lib" )
#pragma comment( lib, "shell32.lib" )

#define WM_TRAY ( WM_USER + 1 )
HINSTANCE appInst;
HWND loaderHwnd;
NOTIFYICONDATA trayIcon;

const char exeName[2][32] = { "cstrike.exe", "hl.exe" };
const char moduleName[32] = "hack.dll";

const char running[34] = "Injected successfully, running...";
const char monitor[41] = "Waiting for the target process to run...";

DWORD WINAPI InjectThread( LPVOID param );
LRESULT CALLBACK LoaderProcedure( HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam );

INT WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nShowCmd )
{
    HANDLE handle = CreateMutex( NULL, TRUE, "Laser" );
    if ( GetLastError() != ERROR_SUCCESS )
    {
        MessageBox( 0, "The program is already running!", "Information", MB_OK | MB_ICONINFORMATION );
        return 0;
    }
    
    // Create injection thread  
    CreateThread( NULL, 0, InjectThread, NULL, 0, new DWORD );
    
    // Create main dialog
    appInst = hInstance;    
    DialogBox( appInst, MAKEINTRESOURCE(IDD_LOADER), NULL, (DLGPROC)LoaderProcedure );
    
    return 0;
}

DWORD WINAPI InjectThread( LPVOID param )
{
    // Get current path
    char modulePath[MAX_PATH];
    GetModuleFileName( NULL, modulePath, MAX_PATH );
    char* index = strrchr( modulePath, '\\' );
    modulePath[ index - modulePath + 1 ] = 0;
    strcat_s( modulePath, MAX_PATH, moduleName );
    
    // Check if the module to be injected exists
    WIN32_FIND_DATA WFD;
    if ( FindFirstFile( modulePath, &WFD ) == INVALID_HANDLE_VALUE )
    {
        char msg[512];
        strcpy_s( msg, 512,"File " );
        strcat_s( msg, 512, moduleName );
        strcat_s( msg, 512, " does not exist!" );
        
        MessageBox( NULL, msg, "Error", MB_OK | MB_ICONERROR );
        
        PostMessage( loaderHwnd, WM_CLOSE, NULL, NULL );
    }
    
    // Monitor target process, Injection module
    while ( true )
    {
        static int interval = 0;
        static int returnValue = 0;
        
        if ( Injector::InjectModule( exeName[0], modulePath ) == true
             || Injector::InjectModule( exeName[1], modulePath ) == true )
        {
            interval = 2000;

            if ( strcmp( trayIcon.szTip, running ) != 0 )
            {
                strcpy_s( trayIcon.szTip, 64, running );
                Shell_NotifyIcon( NIM_MODIFY, &trayIcon );  
            }
        }
        else
        {
            interval = 10;

            if ( strcmp( trayIcon.szTip, monitor ) != 0 )
            {
                strcpy_s( trayIcon.szTip, 64, monitor );
                Shell_NotifyIcon( NIM_MODIFY, &trayIcon ); 
            }
        }
        
        Sleep( interval );
    }
    
    return 0;
}

LRESULT CALLBACK LoaderProcedure( HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
    HICON appIcon;
    RECT rect;

    switch ( msg )
    {
    case WM_INITDIALOG:         
        loaderHwnd = dlg;

        GetWindowRect( dlg, &rect );
        MoveWindow( dlg,
                    ( GetSystemMetrics(SM_CXSCREEN) - ( rect.right - rect.left ) ) / 2,
                    ( GetSystemMetrics(SM_CYSCREEN) - ( rect.bottom - rect.top ) ) / 2,
                    ( rect.right - rect.left ), ( rect.bottom - rect.top ),
                    true );
        
        appIcon = LoadIcon( appInst, (LPCTSTR)IDI_LOADER );
        SendMessage( dlg, WM_SETICON, ICON_BIG, (LPARAM)appIcon );   
        SendMessage( dlg, WM_SETICON, ICON_SMALL, (LPARAM)appIcon );  
        
        trayIcon.cbSize = sizeof( NOTIFYICONDATA );
        trayIcon.uCallbackMessage = WM_TRAY;
        trayIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
        trayIcon.uID = 1;
        trayIcon.hIcon = appIcon;
        strcpy_s( trayIcon.szTip, 64, "" );
        trayIcon.hWnd = dlg;
        Shell_NotifyIcon( NIM_ADD, &trayIcon );               
        
        break;   
        
    case WM_COMMAND:
        
        if ( LOWORD(wParam) == IDOK)
            ShellExecute( NULL, "open", "explorer", "https://discord.gg/gKEk9GV", NULL, SW_SHOWMAXIMIZED );
        
        break;

    case WM_CLOSE:

        Shell_NotifyIcon( NIM_DELETE, &trayIcon );
        EndDialog( dlg, LOWORD(wParam) );

        break;
        
    case WM_SYSCOMMAND:
        
        if ( wParam == SC_MINIMIZE )
        {
            ShowWindow( dlg, SW_HIDE );

            return 1;
        }
          
        break;
        
    case WM_TRAY:
        
        if ( (UINT)lParam == WM_LBUTTONDOWN 
            || (UINT)lParam == WM_LBUTTONDBLCLK 
            || (UINT)lParam == WM_RBUTTONDOWN 
            || (UINT)lParam == WM_RBUTTONDBLCLK )
        {
            if ( IsWindowVisible( dlg ) )
            {
                ShowWindow( dlg, SW_HIDE );            
            }
            else
            {
                ShowWindow( dlg, SW_SHOW );
                SetForegroundWindow( dlg );
            }
        }
        
        break;
    }
    
    return 0;
}