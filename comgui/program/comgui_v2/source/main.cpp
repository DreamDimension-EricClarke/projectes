#include <windows.h>
#include "layoutlexer.h"
#include "commport.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
	layoutlexer layout;
	commport	comp( layout );
	
	comp.init( "init.dxml" );
	
	return comp.run();
}
