#ifndef __dream_dimension_sdk__
#define __dream_dimension_sdk__

/*
	Dream Dimension software development kit
	Comments were written in Chinese encoded
	using UTF-8, check method of your editor.
*/
// please set your encoding method to GBK or GB2312.
// change your encoding method until you can see Chinese under this line.
//        这是一句测试中文        //
 

namespace dream {
#ifndef __dream_type__
#define __dream_type__ 1


/*
*	Platform independent data types
*	They have confirmed certain length
*/
typedef unsigned char dByte;
typedef unsigned short dWord;
typedef unsigned long dDword;
typedef unsigned long long dLong;
#endif

#if BUILDING_DLL
	#define dreamapi __declspec(dllexport)
#else
	#define dreamapi __declspec(dllimport)
#endif

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

}
#endif
