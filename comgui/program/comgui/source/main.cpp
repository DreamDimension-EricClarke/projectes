#include "comgui.h"
#include <iostream>
using namespace std;
using namespace dream;

extern comgui gui;
int main( int argc, char **argv )
{
	MSG msg;
	gui.initial( "config.ini" );
	while( GetMessage( &msg, 0, 0, 0 ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
		gui.process();
	}
	return msg.wParam;
}
