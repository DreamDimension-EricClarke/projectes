#ifndef __dream_dimension_sdk_cpp__
#define __dream_dimension_sdk_cpp__

#include <windows.h>
#include <string>

std::wstring	string2wstring( const std::string& value )
{
	using namespace std;
	wstring result;
	int length = MultiByteToWideChar( CP_ACP, 0, value.data(), -1, 0, 0 );
	if( length == 0 )
    	return result;
	wchar_t* tempbuf = new wchar_t[length];
	MultiByteToWideChar( CP_ACP, 0, value.data(), -1, tempbuf, length );
	result.assign( tempbuf, length - 1 );
	delete[] tempbuf;
	return result;
}

std::string	wstring2string( const std::wstring& value )
{
	using namespace std;
    string result;  
    BOOL tbool = FALSE;
    int length = WideCharToMultiByte( CP_ACP, 0, value.data(), -1, 0, 0, 0, &tbool );
    if( length == 0 )
    	return result;
    char *tempbuf = new char[length];  
    tbool = FALSE;
    WideCharToMultiByte( CP_OEMCP, 0, value.data(), -1, tempbuf, length, 0, &tbool );
    result.assign( tempbuf, length - 1 );
    delete[] tempbuf;
    return result;
}
#endif
