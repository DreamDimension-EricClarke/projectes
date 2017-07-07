#ifndef __cabinet_cpp__
#define __cabinet_cpp__

#include "cabinet.h"
#include "main.h"

SCab::SCab( int _num, int _count, int _state ):
num(_num), count(_count), state(_state) {
	
}

/*CCabinet构造函数创建一个柜子链模型
########################################################################################
参数:
========================================================================================
返回值:
*/
CCabinet::CCabinet( ):
commport(INVALID_HANDLE_VALUE) {
	
	//测试代码 
	cabs.Push( SCab( 1, 12, 0 ) );
	cabs.Push( SCab( 2, 5, 0 ) );
	cabs.Push( SCab( 4, 16, 0 ) );
}

/*析构函数释放所有资源
*/ 
CCabinet::~CCabinet( ) {
	
}

/*OpenComm打开串口
########################################################################################
参数:
	string name							串口号码
	------------------------------------------------------------------------------------
	DWORD baudrate						波特率
======================================================================================== 
返回值:
	bool								成功则返回true
										失败返回false 
*/ 
bool CCabinet::OpenComm( string name, DWORD baudrate ) {
	if( commport != INVALID_HANDLE_VALUE ) {
		return false;
	}
	
	commport = CreateFile( name.data(), GENERIC_READ|GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0 );
	if( commport == INVALID_HANDLE_VALUE ) {
		return false;
	}
	
	/*配置串口超时参数*/
	COMMTIMEOUTS comm_timeout;
	comm_timeout.ReadIntervalTimeout = 200;
	comm_timeout.ReadTotalTimeoutConstant = 2000;
	comm_timeout.ReadTotalTimeoutMultiplier = 0;
	comm_timeout.WriteTotalTimeoutConstant = 2000;
	comm_timeout.WriteTotalTimeoutMultiplier = 0;

	/*设置串口超时配置*/
	if( !SetCommTimeouts( commport, &comm_timeout ) ) {
		CloseHandle( commport );
		commport = INVALID_HANDLE_VALUE;
		return false;
	}
	
	DCB comm_param;
	memset( &comm_param, 0, sizeof(comm_param) );
	
	comm_param.DCBlength = sizeof(comm_param);
	comm_param.fBinary = TRUE;
	comm_param.BaudRate = baudrate;
	comm_param.ByteSize = 8;
	comm_param.StopBits = ONESTOPBIT;
	comm_param.fParity = FALSE;
	comm_param.Parity = NOPARITY;
	comm_param.fOutxCtsFlow = FALSE;
	comm_param.fRtsControl = RTS_CONTROL_DISABLE;
	/*设置串口配置*/
	if( !SetCommState( commport, &comm_param ) ) {
		CloseHandle( commport );
		commport = INVALID_HANDLE_VALUE;
		return false;
	}
	return true;
}

/*CloseComm关闭串口 
########################################################################################
参数:
========================================================================================
返回值:
	bool								关闭是否成功
*/ 
bool CCabinet::CloseComm( void ) {
	if( commport == INVALID_HANDLE_VALUE )
		return false;
	CloseHandle( commport );
	commport = INVALID_HANDLE_VALUE;
	return true;
}

/*IsCommOpen函数返回串口是否已经打开的信息
########################################################################################
*/
bool CCabinet::IsCommOpen( void ) {
	return (commport != INVALID_HANDLE_VALUE);
}

/*GetGateCount函数返回当前模型中柜门个数
########################################################################################
参数:
	int									指定柜子号码
										号码不能为0 
========================================================================================
返回值:
	int                                 柜门个数
*/
int CCabinet::GetGateCount( int cabnum ) {
	int count = 0;
	cabs.ForEach( [&]( SCab& scab ){
		if( scab.num == cabnum ) {
			count = scab.count;
			return false;
		}
		return true;
	} );
	return count;
}

/*QueryCount函数向下位机查询柜子的信息
########################################################################################
参数:
	int									指定柜子号码
										若号码为0则表示查询所有柜子的信息
========================================================================================
返回值:
	bool								查询是否成功
										若查询成功,则参数已经被存入了模型中 
*/
bool CCabinet::QueryInfo( int cabnum ) {
	
}

/*SetGateCount函数用来设置下位机柜门个数 
########################################################################################
参数:
	int cabnum							柜子编号 不能为0 
	------------------------------------------------------------------------------------ 
	int gatecount						柜门个数 
*/
bool CCabinet::SetGateCount( int cabnum, int gatecount ) {
	
}

/*OpenGate函数打开指定柜子的指定柜门
########################################################################################
参数:
	int cabnum							柜子编号
	------------------------------------------------------------------------------------
	int gatenum							门编号
========================================================================================
返回值:
	bool								开门是否成功
*/
bool CCabinet::OpenGate( int cabnum, int gatenum ) {
	
}

/*DrawCab函数绘制柜子模型
########################################################################################
参数:
	int cabnum							绘制的柜子的编号
========================================================================================
返回值:
========================================================================================
注意:
	绘制使用窗口坐标系,即原点在窗口左上角,x轴从左向右,y轴从上到下 
	被鼠标关注的柜门会被特殊绘制 
*/
void CCabinet::DrawCab( int cabnum ) {
	char buf[32];
	glColor3f( 0.75, 0.75, 0.75 );
	glBegin( GL_LINE_STRIP );
	glVertex2i( x, y );
	glVertex2i( x+width, y );
	glVertex2i( x+width, y+height );
	glVertex2i( x, y+height );
	glVertex2i( x, y );
	glEnd();
	
	if( commport == INVALID_HANDLE_VALUE ) {
		glColor3ub( 255, 155, 155 );
		glBegin( GL_QUADS );
		glVertex2i( x, y );
		glVertex2i( x+width, y );
		glVertex2i( x+width, y+height );
		glVertex2i( x, y+height );
		glEnd();
		
		glColor3f( 1.0f, 1.0f, 1.0f );
		glRasterPos2i( x+width*0.35, y+height*0.5 );
		DrawText( "串口尚未打开" );
		return;
	}
	
	glColor3ub( 155, 255, 155 );
	//glBegin( GL_QUADS );
	//glVertex2i( x, y );
	//glVertex2i( x+width, y );
	//glVertex2i( x+width, y+height );
	//glVertex2i( x, y+height );
	//glEnd();
	
	int count = GetGateCount( cabnum );
	if( count == 0 ) {
		glColor3f( 1.0f, 0.0f, 0.0f );
		glRasterPos2i( x+width*0.35, y+height*0.5 );
		DrawText( "柜号码无效" );
	} else {	
		int col = count/2 + (count%2 == 0?0:1);
		int dh = (height-8)/col;
		if( dh >= width/2 ) {
			dh = width/2;
		}
		glBegin( GL_QUADS );
		glColor3ub( 215, 215, 215 );
		glVertex2i( x+width*0.5-dh-8, y+height-1 );
		glVertex2i( x+width*0.5-dh-8, y+1 );
		glVertex2i( x+width*0.5-dh, y+1 );
		glVertex2i( x+width*0.5-dh, y+height-1 );
		
		glVertex2i( x+width*0.5+dh+8, y+height-1 );
		glVertex2i( x+width*0.5+dh+8, y+1 );
		glVertex2i( x+width*0.5+dh, y+1 );
		glVertex2i( x+width*0.5+dh, y+height-1 );
		
		glVertex2i( x+width*0.5-dh-8, y+1 );
		glVertex2i( x+width*0.5+dh+8, y+1 );
		glVertex2i( x+width*0.5+dh+8, y+9 );
		glVertex2i( x+width*0.5-dh-8, y+9 );
		glEnd();
	
		
		for( int i = 0; i < count; i++ ) {
			int dx = x + width*0.5 - dh + i%2*dh;
			int dy = y+8 + i/2*dh;
			
			glColor3ub( 245, 245, 245 );
			glBegin( GL_QUADS );
			glVertex2i( dx+1, dy+1 );
			glVertex2i( dx+dh-1, dy+1 );
			glVertex2i( dx+dh-1, dy+dh-1 );
			glVertex2i( dx+1, dy+dh-1 );
			glEnd();
			glColor3ub( 200, 200, 200 );
			glRasterPos2i( dx+2, dy+30 );
			sprintf( buf, "%d号门", i+1 );
			DrawText( string(buf) );
		}
		cabnum = CatchGate( cabnum, pts );
		if( cabnum != 0 ) {
			int dx = x + width*0.5 - dh + (cabnum-1)%2*dh;
			int dy = y+8 + (cabnum-1)/2*dh;
			SHORT bK = GetKeyState( VK_LBUTTON );
			
			glColor3ub( 240, 240, 240 );
			glBegin( GL_LINE_STRIP );
			glVertex2i( dx, dy );
			glVertex2i( dx+dh, dy );
			glVertex2i( dx+dh, dy+dh );
			glVertex2i( dx, dy+dh );
			glVertex2i( dx, dy );
			glEnd();
			
			
			if( bK < 0 ) {
				glColor3ub( 240, 240, 240 );
			} else {
				glColor3ub( 250, 250, 250 );
			}
				glBegin( GL_QUADS );
				glVertex2i( dx+1, dy+1 );
				glVertex2i( dx+dh-1, dy+1 );
				glVertex2i( dx+dh-1, dy+dh-1 );
				glVertex2i( dx+1, dy+dh-1 );
				glEnd();
				glColor3ub( 200, 200, 200 );
				glRasterPos2i( dx+2, dy+30 );
				sprintf( buf, "%d号门", cabnum );
				DrawText( string(buf) );
		}
	} 
	
	glColor3f( 0, 0, 0 );
	glRasterPos2i( x+width*0.35, 570 );
	sprintf( buf, "当前柜编号:%d", curcabnum );
	DrawText( string(buf) );
}

/*CatchGate函数通过鼠标相对于窗口的坐标抓取一个柜门
########################################################################################
参数:
	int cabnum							要抓取的柜子号码
	------------------------------------------------------------------------------------
	POINTS pos							鼠标相对于窗口的坐标 
======================================================================================== 
返回值:
	int									柜门号码,若返回值为0则表示没有检索到任何柜门
*/
int	CCabinet::CatchGate( int cabnum, POINTS pos ) {
	int count = GetGateCount( cabnum );
	char buf[32];
//	glColor3f( 0.0f, 0.0f, 0.0f );
//	glRasterPos2i( x+width*0.35, 575 );
//	sprintf( buf, "x:%d y:%d", pos.x, pos.y );
//	DrawText( string(buf) );
	if( count == 0 ) {
		return 0;
	}
	int col = count/2 + (count%2 == 0?0:1);
	int dh = (height-8)/col;
	if( dh >= width/2 ) {
		dh = width/2;
	}
	if( pos.x < (x + width*0.5 - dh) or pos.x > (x + width*0.5 + dh) ) {
		return 0;
	}
	if( pos.y < y or pos.y > y+height ) {
		return 0;
	}
	
	int dx = (pos.x - x - width*0.5 + dh)/dh;
	int dy = (pos.y - y)/dh;
	int rel = dy*2+dx+1;
	if( rel > count ) {
		rel = 0;
	}
	return rel;
}
#endif
