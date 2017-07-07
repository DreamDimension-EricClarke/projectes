#include <iostream>
#include <string>
#include <stdexcept> 

#include <windows.h>
#include <wingdi.h>
#include <gl/gl.h>
#include <gl/glu.h>

using namespace std;

class Image {
	private:
		static HWND wndShow;	//用来显示图像的窗口 
		static bool bSwaped;
		static HDC hDC;
		
		DWORD m_dwWidth;		//位图宽度 
		DWORD m_dwHeight;		//位图高度 
		WORD m_wColorBits;		//位图颜色位数 
		GLuint m_uTex;			//位图像素数据缓冲 
	public:
		Image( const string& strFileName )
		{
			HANDLE hFile;
			BITMAPFILEHEADER bmpHeader;
			BITMAPINFO bmpInfo;
			DWORD dwRead;
			
			/***打开位图文件***/ 
			hFile = CreateFile( strFileName.data(), GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0 );
			if( hFile == INVALID_HANDLE_VALUE )
				throw invalid_argument( "文件打开失败" );
			
			
			/***读取位图参数***/ 
			SetFilePointer( hFile, 0, 0, FILE_BEGIN );
			ReadFile( hFile, &bmpHeader, sizeof(bmpHeader), &dwRead, 0 );
			ReadFile( hFile, &bmpInfo, sizeof(bmpInfo), &dwRead, 0 );
			
			/***设置成员变量***/ 
			m_dwWidth = bmpInfo.bmiHeader.biWidth;
			m_dwHeight = bmpInfo.bmiHeader.biHeight;
			m_wColorBits = bmpInfo.bmiHeader.biBitCount;
			if( m_wColorBits != 24 )
			{
				throw invalid_argument( "暂时只支持24位位图" );
			}
			BYTE *pcBuffer = new BYTE[bmpInfo.bmiHeader.biSizeImage];

			/***读取每像素数据***/
			SetFilePointer( hFile, bmpHeader.bfOffBits, 0, FILE_BEGIN );
			ReadFile( hFile, pcBuffer, bmpInfo.bmiHeader.biSizeImage, &dwRead, 0 );
			for( int i = 0; i < bmpInfo.bmiHeader.biSizeImage; i += 3 )
			{
				pcBuffer[i+2] ^= pcBuffer[i];
				pcBuffer[i] ^= pcBuffer[i+2];
				pcBuffer[i+2] ^= pcBuffer[i];
			}
			CloseHandle( hFile );
			
			glGenTextures( 1, &m_uTex );
			glBindTexture( GL_TEXTURE_2D, m_uTex );
			
			/***将位图读入纹理***/ 
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, m_dwWidth,m_dwHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pcBuffer );
			
			/***设置纹理参数***/ 
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		    
		    delete[] pcBuffer;
		}
		
		~Image()
		{
			glDeleteTextures( 1, &m_uTex );
		}
		
		bool Show( DWORD x, DWORD y, DWORD width = -1, DWORD height = -1 )
		{
			if( wndShow == 0 )
				return false;
			
			/***若不提供宽高,则按照原大小绘制***/ 
			if( height == -1 )
				height = m_dwHeight;
			if( width == -1 )
				width = m_dwWidth;
				
			/***若刚进行过刷新则清屏***/ 
			if( bSwaped )
			{
				glClear( GL_COLOR_BUFFER_BIT );
				bSwaped = false;
			}
			
			
			/***保存当前投影矩阵***/ 
			glPushMatrix();
			RECT rect;
			GetWindowRect( wndShow, &rect );	/***获取窗口形状***/ 
			glTranslatef( -1.0f, 1.0f, 0.0f );	/***将画布原点与窗口原点重合***/ 
			glScalef( 2.0/(rect.right-rect.left), -2.0/(rect.bottom-rect.top), 1.0f );	/***将画布缩放至窗口大小***/ 
			glBindTexture( GL_TEXTURE_2D, m_uTex );
			
			/***绘制位图***/ 
			glBegin( GL_QUADS );
			glTexCoord2f( 0, 1 );
			glVertex2i( x, y );
			glTexCoord2f( 1, 1 );
			glVertex2i( x+width, y );
			glTexCoord2f( 1, 0 );
			glVertex2i( x+width, y+height );
			glTexCoord2f( 0, 0 );
			glVertex2i( x, y+height );
			glEnd();
			/***还原投影矩阵***/ 
			glPopMatrix();
			return true;
		}
		
		/**将绘制命令显示在画面上***/ 
		static void Flush()
		{
			if( wndShow != 0 )
			{
				SwapBuffers( hDC );
				bSwaped = true;
			}
		}
		
		static bool BindWindow( HWND hwnd )
		{
			if( hwnd == 0 )
				return false;
			wndShow = hwnd;
			hDC = GetWindowDC( hwnd );
			PIXELFORMATDESCRIPTOR pfd;
			/***初始化像素格式***/
			memset( &pfd, 0, sizeof( PIXELFORMATDESCRIPTOR ) );
			pfd.cColorBits = 24;
			pfd.cDepthBits = 16;
			pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
			pfd.nSize = sizeof( pfd );
			pfd.nVersion = 1;
			
			/***设置像素格式***/ 
			int iFormat = ChoosePixelFormat( hDC, &pfd );
			if( !SetPixelFormat( hDC, iFormat, &pfd ) )
				return false;
			
			/***创建渲染环境***/ 
			HGLRC hRC = wglCreateContext( hDC );
			wglMakeCurrent( hDC, hRC );
			
			glEnable( GL_TEXTURE_2D );
			glClearColor( 0.0f, 0.2f, 0.0f, 0.0f );
			glClear( GL_COLOR_BUFFER_BIT );
			return true;
		}
};

/***静态成员参数的初始化***/ 
HWND Image::wndShow = 0;
bool Image::bSwaped = false;
HDC Image::hDC = 0;
