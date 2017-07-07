#ifndef __ic_pc_USART_Demo__
#define __ic_pc_USART_Demo__

#include <windows.h>
#include <stdio.h>
#include <conio.h>

typedef enum{ ctExit, ctOpen, ctSet, ctClose, ctTalk }Command;
unsigned char led = 0;
int NumComm = 0;
char CommName[32];
HANDLE CommPort = INVALID_HANDLE_VALUE;
DCB CommDCB;
COMMTIMEOUTS CommTimeout;

void cll( int l );
void gto( int x, int y );
void cls( int, int, int, int, char );
Command menu( void );
void Xopen( void );
int Xexit( void );
void Xinit( void );
void Xclose( void );
void Xset( void );
void Xtalk( void );

int main( int argc, char ** argv )
{
	Command cmd = ctExit;
	
	CONSOLE_CURSOR_INFO cursor_info = {1, 0}; 
 	SetConsoleCursorInfo( GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
	
	while( true )
	{
		cmd = menu();
		
		switch( cmd )
		{
			case ctExit:
				return Xexit();
				break;
			case ctOpen:
				Xopen();
				break;
			case ctClose:
				Xclose();
				break;
			case ctSet:
				Xset();
				break;
		}
	}
	
}

void cll( int l, char c )
{
	for( int i = 0; i < 80; i++ )
	{
		gto( i, l );
		putchar( c );
	}
	return;
}

void clr( int r, char c )
{
	for( int i = 0; i < 20; i++ )
	{
		gto( r, i );
		putchar( c );
	}
	return;
}

void gto(int x,int y)
{
	COORD c;
	c.X=x;
	c.Y=y;
	SetConsoleCursorPosition( GetStdHandle( STD_OUTPUT_HANDLE ), c );
}

Command menu( void )
{
	int cmd;
	system( "cls" );
	gto( 28, 2 );
	puts( "ic_pc_USART_Demo" );
	gto( 26, 3 );
	puts( "ic_pc_串行口演示程序( menu )");
	cll( 4, '-' );
	cll( 17, '-' );
	cll( 19, '-' );
	cll( 16, '_' );
	cll( 14, '_' );
	if( INVALID_HANDLE_VALUE == CommPort )
	{
		gto( 4, 5 );
		puts( "串口尚未打开" );
		return ctOpen;
	}
	
	GetCommState( CommPort, &CommDCB );
	GetCommTimeouts( CommPort, &CommTimeout );
	
	gto( 4, 5 );
	printf( "串口名字:%s", CommName );
	gto( 8, 6 );
	printf( "波特率\t\t%d", CommDCB.BaudRate );
	gto( 8, 7 );
	printf( "数据位数\t%d", CommDCB.ByteSize );
	gto( 8, 8 );
	printf( "奇偶校验位\t%s", (TRUE == CommDCB.fParity)?"是":"否" );
	gto( 8, 9 );
	if( TRUE == CommDCB.fParity )
	{
		printf( "奇偶校验位\t\t%d", CommDCB.Parity );
		gto( 8, 10 );
	}
	printf( "停止位\t\t%d", CommDCB.StopBits );
	
	gto( 40, 6);//读串口间隔超时、读串口总超时乘数、读串口总超时常数（ｍｓ）、写串口总超时乘数、写串口总超时常数（ｍｓ）。
	printf( "读串口间隔超时:%d", CommTimeout.ReadIntervalTimeout );
	gto( 40, 7 );
	printf( "读串口总超时:%d", CommTimeout.ReadTotalTimeoutConstant );
	gto( 40, 8 );
	printf( "读串口超时乘数:%d", CommTimeout.ReadTotalTimeoutMultiplier );
	gto( 40, 9 );
	printf( "写串口总超时:%d", CommTimeout.WriteTotalTimeoutConstant );
	gto( 40, 10 );
	printf( "写串口超时乘数:%d", CommTimeout.WriteTotalTimeoutMultiplier );
	
	gto( 0, 15 );
	puts( "Hint: 1.配置串口 2.关闭串口 3.开始通信 0.退出" );
	gto( 0, 18 );
	printf( "-->" );
	while( true )
	{
		cmd = getch();
		switch( cmd )
		{
			case '1':return ctSet;
			case '2':return ctClose;
			case '3':return ctTalk;
			case '0':return ctExit;
		}
	}
}

void Xopen( void )
{
		cll( 3, 0 );
		gto( 26, 3 );
		puts( "ic_pc_串行口演示程序( open )");
		gto( 0, 15 );
		puts( "Hint:请输入要打开的串口号码" );
		gto( 0, 18 );
		printf( "-->" );
		scanf( "%d", &NumComm );
		//生成串口名 
		sprintf( CommName, "COM%d", NumComm );
		//打开串口 
		CommPort = CreateFile( 
		CommName, GENERIC_READ | GENERIC_WRITE ,
		0, NULL, OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED, NULL );
		
		cll( 15, ' ' );
		gto( 0, 15 );
		if( INVALID_HANDLE_VALUE == CommPort )
			puts( "Error:串口打开失败." );
		else
			puts( "Result:串口打开成功.");
		while( kbhit() )
			getch();
		getch();
		return;
}

int Xexit( void )
{
	if( INVALID_HANDLE_VALUE != CommPort )
		CloseHandle( CommPort );
	return 0;
}

void Xclose( void )
{
	CloseHandle( CommPort );
	CommPort = INVALID_HANDLE_VALUE;
	return;
}

void Xset( void )
{
	int cmd = 0;
	int sel = 1;
	bool bBR = false;
	bool bBS = false;
	bool bP = false;
	bool bSB = false;
	
	while( true )
	{
		system( "cls" );
		gto( 28, 2 );
		puts( "ic_pc_USART_Demo" );
		cll( 3, 0 );
		gto( 26, 3 );
		puts( "ic_pc_串行口演示程序( set )");
		gto( 4, 5 );
		printf( "\t串口名字:%s", CommName );
		
		cll( 4, '-' );
		cll( 17, '-' );
		cll( 19, '-' );
		cll( 16, '_' );
		cll( 14, '_' );
		
		gto( 8, 6 );
		if( 1 == sel )
			putchar( '>' );
		printf( "\t波特率\t\t%d", CommDCB.BaudRate );
		gto( 8, 7 );
		if( 2 == sel )
			putchar( '>' );
		printf( "\t数据位数\t%d", CommDCB.ByteSize );
		gto( 8, 8 );
		if( 3 == sel )
			printf( ">\t奇偶校验位\t<%s>", (TRUE == CommDCB.fParity)?"是":"否" );
		else
			printf( "\t奇偶校验位\t%s", (TRUE == CommDCB.fParity)?"是":"否" );
		gto( 8, 9 );
		if( TRUE == CommDCB.fParity )
		{
			if( 4 == sel )
				putchar( '>' );
			printf( "\t\t奇偶校验位\t\t%d", CommDCB.Parity );
			gto( 8, 10 );
			if( 5 == sel )
				putchar( '>' );
		}
		else if( 4 == sel )
			putchar( '>' );
		printf( "\t停止位\t\t%d", CommDCB.StopBits );
		
		gto( 40, 6);//读串口间隔超时、读串口总超时乘数、读串口总超时常数（ｍｓ）、写串口总超时乘数、写串口总超时常数（ｍｓ）。
		printf( "读串口间隔超时:%d", CommTimeout.ReadIntervalTimeout );
		gto( 40, 7 );
		printf( "读串口总超时:%d", CommTimeout.ReadTotalTimeoutConstant );
		gto( 40, 8 );
		printf( "读串口超时乘数:%d", CommTimeout.ReadTotalTimeoutMultiplier );
		gto( 40, 9 );
		printf( "写串口总超时:%d", CommTimeout.WriteTotalTimeoutConstant );
		gto( 40, 10 );
		printf( "写串口超时乘数:%d", CommTimeout.WriteTotalTimeoutMultiplier );
		
		gto( 0, 15 );
		puts( "Hint: 方向键选择 esc返回" );
		gto( 0, 18 );
		printf( "-->" );
		
		cmd = getch();
		switch( cmd )
		{
			case 72://上
				if( 1 < sel ) sel -= 1;
				break;
			case 80://下
				if(TRUE == CommDCB.fParity)
				{
					if( 5 > sel ) sel += 1;
				}
				else
					if( 4 > sel ) sel += 1;
				break;
			case 75:
			case 77:
				if( 3 == sel )
					CommDCB.fParity = (( TRUE == CommDCB.fParity )? FALSE : TRUE);
				break;
			case '0' ... '9':
				switch( sel )
				{
					case 1:
						if( !bBR )
							CommDCB.BaudRate = cmd - '0', bBR = true;
						else
							CommDCB.BaudRate = CommDCB.BaudRate*10 + cmd - '0';
						break;
					case 2:
						if( !bBS )
							CommDCB.ByteSize = cmd - '0', bBS = true;
						else
							CommDCB.ByteSize = CommDCB.ByteSize*10 + cmd - '0';
						break;
					case 4:
						if( TRUE == CommDCB.fParity )
						{
							if( !bP )
								CommDCB.Parity = cmd - '0', bP = true;
							else
								CommDCB.Parity = CommDCB.Parity*10 + cmd - '0';
							break;
						}
						if( !bSB )
							CommDCB.StopBits = cmd - '0', bSB = true;
						else
							CommDCB.StopBits = CommDCB.StopBits*10 + cmd - '0';
						break;
					case 5:
						if( !bSB )
							CommDCB.StopBits = cmd - '0', bSB = true;
						else
							CommDCB.StopBits = CommDCB.StopBits*10 + cmd - '0';
						break;
				}
				break;
			case 27:
				if( FALSE == SetCommState( CommPort, &CommDCB ) )
				{
					cll( 15, 0 );
					gto( 0, 15 );
					puts( "Error:配置失败,请检查配置项." );
					while( kbhit() )
						getch();
					getch();
				}
				else
					return;
		}
	}
}

void Xtalk( void )
{
	unsigned char sel = 1;
	long suc = 0
	system( "cls" );
	gto( 28, 2 );
	puts( "ic_pc_USART_Demo" );
	cll( 3, 0 );
	gto( 26, 3 );
	puts( "ic_pc_串行口演示程序( talk )");
	
	gto( 4, 4 );
	printf( "LED状态: %s", 0 == led?"关":"开" );
	
	gto( 4, 5 );
	printf( "按回车键改变led状态, esc键返回" );
	while( true )
	{
		switch( getch() )
		{
			case 12:
				sel = ( 0 == sel ? 1: 0 );
				WriteFile( CommPort, &sel, 1, &suc, NULL );
		}
	}
}
#endif
