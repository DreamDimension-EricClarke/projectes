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
		static HWND wndShow;	//������ʾͼ��Ĵ��� 
		static bool bSwaped;
		static HDC hDC;
		
		DWORD m_dwWidth;		//λͼ��� 
		DWORD m_dwHeight;		//λͼ�߶� 
		WORD m_wColorBits;		//λͼ��ɫλ�� 
		GLuint m_uTex;			//λͼ�������ݻ��� 
	public:
		Image( const string& strFileName )
		{
			HANDLE hFile;
			BITMAPFILEHEADER bmpHeader;
			BITMAPINFO bmpInfo;
			DWORD dwRead;
			
			/***��λͼ�ļ�***/ 
			hFile = CreateFile( strFileName.data(), GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0 );
			if( hFile == INVALID_HANDLE_VALUE )
				throw invalid_argument( "�ļ���ʧ��" );
			
			
			/***��ȡλͼ����***/ 
			SetFilePointer( hFile, 0, 0, FILE_BEGIN );
			ReadFile( hFile, &bmpHeader, sizeof(bmpHeader), &dwRead, 0 );
			ReadFile( hFile, &bmpInfo, sizeof(bmpInfo), &dwRead, 0 );
			
			/***���ó�Ա����***/ 
			m_dwWidth = bmpInfo.bmiHeader.biWidth;
			m_dwHeight = bmpInfo.bmiHeader.biHeight;
			m_wColorBits = bmpInfo.bmiHeader.biBitCount;
			if( m_wColorBits != 24 )
			{
				throw invalid_argument( "��ʱֻ֧��24λλͼ" );
			}
			BYTE *pcBuffer = new BYTE[bmpInfo.bmiHeader.biSizeImage];

			/***��ȡÿ��������***/
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
			
			/***��λͼ��������***/ 
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, m_dwWidth,m_dwHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pcBuffer );
			
			/***�����������***/ 
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
			
			/***�����ṩ���,����ԭ��С����***/ 
			if( height == -1 )
				height = m_dwHeight;
			if( width == -1 )
				width = m_dwWidth;
				
			/***���ս��й�ˢ��������***/ 
			if( bSwaped )
			{
				glClear( GL_COLOR_BUFFER_BIT );
				bSwaped = false;
			}
			
			
			/***���浱ǰͶӰ����***/ 
			glPushMatrix();
			RECT rect;
			GetWindowRect( wndShow, &rect );	/***��ȡ������״***/ 
			glTranslatef( -1.0f, 1.0f, 0.0f );	/***������ԭ���봰��ԭ���غ�***/ 
			glScalef( 2.0/(rect.right-rect.left), -2.0/(rect.bottom-rect.top), 1.0f );	/***���������������ڴ�С***/ 
			glBindTexture( GL_TEXTURE_2D, m_uTex );
			
			/***����λͼ***/ 
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
			/***��ԭͶӰ����***/ 
			glPopMatrix();
			return true;
		}
		
		/**������������ʾ�ڻ�����***/ 
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
			/***��ʼ�����ظ�ʽ***/
			memset( &pfd, 0, sizeof( PIXELFORMATDESCRIPTOR ) );
			pfd.cColorBits = 24;
			pfd.cDepthBits = 16;
			pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
			pfd.nSize = sizeof( pfd );
			pfd.nVersion = 1;
			
			/***�������ظ�ʽ***/ 
			int iFormat = ChoosePixelFormat( hDC, &pfd );
			if( !SetPixelFormat( hDC, iFormat, &pfd ) )
				return false;
			
			/***������Ⱦ����***/ 
			HGLRC hRC = wglCreateContext( hDC );
			wglMakeCurrent( hDC, hRC );
			
			glEnable( GL_TEXTURE_2D );
			glClearColor( 0.0f, 0.2f, 0.0f, 0.0f );
			glClear( GL_COLOR_BUFFER_BIT );
			return true;
		}
};

/***��̬��Ա�����ĳ�ʼ��***/ 
HWND Image::wndShow = 0;
bool Image::bSwaped = false;
HDC Image::hDC = 0;
