/***********************************************
wmgl.h
Eric.Clarke
2014.03.24
v 1.0.13
************************************************/
#ifndef __WMGL_H_
#define __WMGL_H_
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <wingdi.h>
#include <winuser.h>



HGLRC mgEnable(  HWND hWnd,  HDC*hdc)//�����������Ϊ��������OpenGL 
         //hWnd				����  ��������OpenGL�Ĵ��ھ�� 
         //hdc				����  ���Դ�����ָ���ڵ��豸������� 
         //����ֵ��OpenGL����Ⱦ������ 
{
	if(hdc==NULL) return NULL;// ���ָ���Ƿ������ 
	
	*hdc=GetDC(hWnd);
	HGLRC hrc;
	int iFormat;
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd,0,sizeof pfd);
	pfd.nSize=sizeof pfd;
	pfd.nVersion=1;
	pfd.dwFlags=PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL ;
	pfd.iPixelType=PFD_TYPE_RGBA;
	pfd.cColorBits=24;
	pfd.cDepthBits=16;
    pfd.iLayerType=PFD_MAIN_PLANE;
    
    iFormat=ChoosePixelFormat (*hdc, &pfd);
    SetPixelFormat (*hdc,iFormat, &pfd);//  ѡ���ʺ�OpenGL�����ظ�ʽ 
    
    
    hrc=wglCreateContext(*hdc);
    wglMakeCurrent(*hdc,hrc);
    
    
    return hrc;
}

int mgPut(wchar_t text,HDC hdc)//��OpenGL�����ظ�ʽ��ʾһ���ַ� .
                   //text       Ҫ��ʾ���ַ�.
                   //hdc        Ҫ��ʾ�ַ����豸�������.
                   //
                   //����ֵ 
                   //          -1 û����ʾ�б���.
                   //          -2 Ŀ���豸��֧��OpenGL
				   //          -3 ������ʾ�б�ʱʧ�� 
				   //           0 ��ʾ�ɹ� 
{
	int list=glGenLists(1);
	if(list==0)
	  return -1;
	  
	  PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd,0,sizeof pfd);
	DescribePixelFormat(hdc,GetPixelFormat(hdc),sizeof pfd,&pfd);
	if(pfd.dwFlags&PFD_SUPPORT_OPENGL==0)
	  return -2;
	  
	  
	if(!wglUseFontBitmapsW(hdc,text,1,list))
	  return -3;
	glCallList(list);
	glDeleteLists(list,1);
	return 0;
}

int mgPuts(char*text,long int length,HDC hdc)//��Opengl ��ʾһ���ַ��� .
         //text              ָ��Ҫ������ַ�����ָ��.
         //length            �ַ�������,����'\0' .
         //hdc               Ҫ��ʾ�ַ������豸.
         //return  0 ��ʾ���� .
         //return -1 ��ʾ����û��Ҫ��ʵ���ַ���textΪNULL��lengthΪС�ڵ�������� .
         //return -2 ��ʾû�й���ռ����ɿ��ִ���.
         //return -3 û����ʾ�б��� .
         //return -4 ת�����ֽڵ�ʱ�������.
         //return -5 ������ʾ�б��ʱ�����.
         //return -6 �ַ����Ľ�β����'\0',length����.
         //return -7 �豸��֧��OpenGL 
{
	if(text==NULL||length<=0)
	  return -1;
	
	int i,lan=0;
	wchar_t*ct;
	
	for(i=0;text[i]!='\0'&&i<length;++i)
	{
		if(IsDBCSLeadByte(text[i]))
		  ++i;
		++lan;
	}
	if(i==length&&text[i]!='\0')
	  return -6;
	
	ct=(wchar_t*)malloc((lan+1)*sizeof(wchar_t));
	if(ct==NULL)
	  return -2;
	if(MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,text,-1,ct,lan)!=0)
	  return -4;
	ct[lan+1]='\0';
	for(i=0;i<lan;++i)
	{
		int ret=mgPut(ct[i],hdc);
		if(ret==-1)
		  return -3;
		else if(ret==-2)
		  return -7;
		else if(ret==-3)
		  return -5;
	}
	free(ct);
	return 0;
}

BYTE* mgBitmap(const char*filename,BITMAPFILEHEADER*pbit,BITMAPINFO*pmap)//print bitmap on window
                                       //x 
                                       //y
                                       //filename the file going to display .
{
	FILE*fp=fopen(filename,"rb");
	if(fp==NULL)return NULL;
	else
	{
		fread(pbit,sizeof(BITMAPFILEHEADER),1,fp);
		fread(pmap,sizeof(BITMAPINFO),1,fp);
		
		BYTE*cmd=(BYTE*)malloc((pmap->bmiHeader).biSizeImage);
		if(cmd==NULL)return NULL;
		
		fseek(fp,(pbit->bfOffBits),0);
		int i;
		for(i=0;i<((pmap->bmiHeader).biSizeImage);i+=3)
		{
		      
			  fread(&cmd[i+2],1,1,fp);
			  fread(&cmd[i+1],1,1,fp);
			  fread(&cmd[i],1,1,fp);
		}
		fclose(fp);
		return cmd;
	}
}

/*WORD mgLoadTexture2D(const char *filename,LONG *width,LONG *height)
{
	GLuint map=0;
	glGenTextures(1,&map);
	BITMAPFILEHEADER mapheader;
	BITMAPINFO mapinfo;
	if(width!=NULL)*width=mapinfo.bmiHeader.biWidth;
	if(height!=NULL)*height=mapinfo.bmiHeader.biHeight;
	BYTE *data=mgBitmap(filename,&mapheader,&mapinfo);
	glBindTexture(GL_TEXTURE_2D,map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mapinfo.bmiHeader.biWidth,mapinfo.bmiHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	free(data);
	return map;
}
*/
BOOL Ipl2BMP(IplImage *Iplmg,BITMAPINFO **pbmpinfo,BYTE **pbmpdata)
{
    BYTE    *p=new BYTE[2048];
    *pbmpinfo=(BITMAPINFO*)p;

    (*pbmpinfo)->bmiHeader.biSize         =sizeof(BITMAPINFOHEADER);
    (*pbmpinfo)->bmiHeader.biBitCount     =Iplmg->nChannels*8;
    (*pbmpinfo)->bmiHeader.biClrUsed      =0;
    (*pbmpinfo)->bmiHeader.biCompression  =BI_RGB;
    (*pbmpinfo)->bmiHeader.biHeight       =Iplmg->height;
    (*pbmpinfo)->bmiHeader.biWidth        =Iplmg->width;
    (*pbmpinfo)->bmiHeader.biPlanes       =1;
    (*pbmpinfo)->bmiHeader.biSizeImage    =0;
    (*pbmpinfo)->bmiHeader.biXPelsPerMeter=0;
    (*pbmpinfo)->bmiHeader.biYPelsPerMeter=0;


    int ImgSize=(((*pbmpinfo)->bmiHeader.biWidth  *  (*pbmpinfo)->bmiHeader.biBitCount +  31 )/32) * 4  * (*pbmpinfo)->bmiHeader.biHeight;
    
     (*pbmpdata)=new BYTE[ImgSize+1];

    memcpy(*pbmpdata,Iplmg->imageData,ImgSize);

    return TRUE;

}

WORD mgLoadTexture2D(const char *filename,LONG *width,LONG *height)
{
	GLuint map=0;
	glGenTextures(1,&map);
	BITMAPINFO *pmapinfo;
	IplImage *pImg=0;
	BYTE *data=0;
	pImg=cvLoadImage(filename,1);
	if(!Ipl2BMP(pImg,&pmaginfo,&data))return 0;
	if(width!=NULL)*width=mapinfo.bmiHeader.biWidth;
	if(height!=NULL)*height=mapinfo.bmiHeader.biHeight;
	glGenTextures(1,&map);
	glBindTexture(GL_TEXTURE_2D,map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mapinfo.bmiHeader.biWidth,mapinfo.bmiHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	free(data);
	cvReleaseImage(pImg);
	return map;
}

BOOL mgFullScreen(HWND hWnd)
{
	if(hWnd==NULL)return FALSE;
	
	HDC hdc=GetDC(hWnd);
	SetWindowLong(hWnd,GWL_STYLE,GetWindowLong(hWnd,GWL_STYLE)&(~WS_OVERLAPPEDWINDOW));
	SetWindowPos(hWnd,HWND_TOPMOST,0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),SWP_SHOWWINDOW);
    
	return TRUE;
}
#endif
