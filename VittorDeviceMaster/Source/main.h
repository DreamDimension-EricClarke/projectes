#ifndef __main_h__
#define __main_h__

#include <windows.h>
#include <wingdi.h>
#include <gl/gl.h>
#include <gl/glu.h> 
#include "cabinet.h"
#include "Layout.h"

extern CLayout	layout;
extern CCabinet cab;
extern int		curcabnum;
extern MSG 		global_msg;
extern POINTS	pts; 
extern DWORD	FontHeight;

LRESULT OnCommSwitch( HWND hwnd, WORD code );
LRESULT OnLastCab( HWND hwnd, WORD code );
LRESULT OnNextCab( HWND hwnd, WORD code );
LRESULT OnSetCab( HWND hwnd, WORD code );
LRESULT OnRefresh( HWND hwnd, WORD code );
LRESULT OnFontUp( HWND hwnd, WORD code );
LRESULT OnFontDown( HWND hwnd, WORD code );
LRESULT OnSaveLog( HWND hwnd, WORD code );
LRESULT OnClearLog( HWND hwnd, WORD code );


LRESULT CALLBACK MainProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK SubWindowProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

DWORD WINAPI RenderProc( LPVOID lpVoid );

void EnumCommPort( void );

void DrawText( string tstr );

#endif
