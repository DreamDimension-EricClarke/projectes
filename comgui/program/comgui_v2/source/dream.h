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

}
#endif
