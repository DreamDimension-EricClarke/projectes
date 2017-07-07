#ifndef __MAIN_H__
#define __MAIN_H__
#include "Device.h"
#include "Layout.h"
#include "CChain.h"
#include <windows.h>
#define RES_BMP_VITTOR 0
#define RES_BMP_RED 1
#define RES_BMP_GREEN 2
#define RES_BMP_SOCKET 3
#define RES_BMP_HTTPD 4
#define RES_BMP_MESSAGE 5

extern CDevice device;
extern CLayout layout;
extern bool bRunning;
extern CChain<char*> combuf;
extern volatile bool brecv;
extern HANDLE TaskThread;
extern LRESULT (*TaskFunc)(HWND, WORD);
extern HWND TaskWnd;
extern WORD TaskNCode;


LRESULT CALLBACK MainProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

LRESULT OnSubFunc( HWND hwnd, WORD notify_code );
LRESULT OnCommSwitch( HWND, WORD );
LRESULT OnWorkMode( HWND, WORD );
LRESULT OnFuncCheck( HWND, WORD );
LRESULT OnEnterConfigState( HWND, WORD );
LRESULT OnEnterCommunicateState( HWND, WORD );
LRESULT OnQueryParam( HWND, WORD );
LRESULT OnSetParam( HWND, WORD );
LRESULT OnApn( HWND, WORD );
LRESULT SetCheck( const string& name, bool checked );
LRESULT OnTemplate( HWND hWnd, WORD motify_code );
LRESULT OnSendCmd( HWND hwnd, WORD motify_code );

void EnumCommPort( void );
void WriteComConfig( void );
void ReadComConfig( void );

bool SetText( const string& name, const string& text );
bool GetCheck( const string& name );
bool GetText( const string& name, string& text );
string GetText( const string& name );
void ShowParam( const SDeviceParam& param );
void PullParam( SDeviceParam& param );

bool Text2Hex( string& text );
char* CutArg( char* arg );

DWORD WINAPI TaskProc( LPVOID p );
#endif
