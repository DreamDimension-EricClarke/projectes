#ifndef __cabinet_cpp__
#define __cabinet_cpp__

#include "cabinet.h"
#include "main.h"
#include <math.h>

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
	
	/*测试代码
	cabs.Push( SCab( 1, 12, 0b0101'0011 ) );
	cabs.Push( SCab( 2, 5, 0b0011'0011 ) );
	cabs.Push( SCab( 4, 16, 0b1000'0000 ) );
	/**/
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
	
	/*测试代码
	commport = (HANDLE)1;
	return true; 
	/**/
	
	commport = CreateFile( name.data(), GENERIC_READ|GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0 );
	if( commport == INVALID_HANDLE_VALUE ) {
		return false;
	}
	
	/*配置串口超时参数*/
	COMMTIMEOUTS comm_timeout;
	comm_timeout.ReadIntervalTimeout = 800;
	comm_timeout.ReadTotalTimeoutConstant = 5000;
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

/*GetGateState函数返回当前模型中柜门个数
########################################################################################
参数:
	int									指定柜子号码
										号码不能为0 
========================================================================================
返回值:
	int                                 柜门的状态 
*/
int CCabinet::GetGateState( int cabnum ) {
	int state = 0;
	cabs.ForEach( [&]( SCab& scab ){
		if( scab.num == cabnum ) {
			state = scab.state;
			return false;
		}
		return true;
	} );
	return state;
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
	
	DWORD count;
	char cmd[2] = { 0x03, (char)cabnum };
	char ret[128];
	
	layout.Log( "[日志]读取柜子信息" );
	
	if( !IsCommOpen() ) {
		return false;
	}
	
	//发送查询指令 
	if( !WriteFile( commport, cmd, 2, &count, 0 ) ) {
		layout.Log( "[错误]指令发送失败" );
		return false;
	}
	
	memset( ret, 0, 128 );
	//读取反馈数据量 
	if( !ReadFile( commport, ret, 2, &count, 0 ) ) {
		layout.Log( "[错误]接收反馈指令错误" );
		return false;
	}
	if( count < 2 ) {
		layout.Log( "[错误]反馈指令长度错误" );
		return false;
	}
	//读取反馈数据 
	if( !ReadFile( commport, ret + 2, ret[1]*5, &count, 0 ) ) {
		layout.Log( "[错误]接收反馈数据错误" );
		return false;
	}
	if( count < ret[1]*5 ) {
		layout.Log( "[错误]反馈数据长度从错误" );
		return false;
	}
	
	if( cabnum == 0 ) {
		cabs.Clear();
	}
	
	for( int ct = 0; ct < ret[1]; ct++ ) {
		char *base = ret+2+ct*5;
		SCab tcab( base[1], base[2], base[3]<<8+base[4] );
		int pos = -1;
		cabs.ForEach( [&]( SCab& icab ) {
			if( icab.num == tcab.num ) {
				cabs.GetIndex( icab, pos );
				return false;
			}
			return true;
		} );
		if( pos >= 0 ) {
			cabs.Remove( pos );
		}
		for( pos = 0; pos < cabs.Count(); pos++ ) {
			if( cabs[pos].num > tcab.num ) {
				break;
			}
		}
		cabs.Insert( tcab, pos );
	}
	
	return true;
}

/*SetGateCount函数用来设置下位机柜门个数 
########################################################################################
参数:
	int cabnum							柜子编号 不能为0 
	------------------------------------------------------------------------------------ 
	int gatecount						柜门个数 
*/
bool CCabinet::SetGateCount( int cabnum, int gatecount ) {
	DWORD count;
	unsigned char cmd[4] = { 0x04, 0xff, (unsigned char)cabnum, (unsigned char)gatecount };
	char ret[4];
	
	layout.Log( "[日志]设置柜子门数量" );
	
	if( !IsCommOpen() or cabnum <= 0 or gatecount <= 1 ) {
		return false;
	}
	
	//发送查询指令 
	if( !WriteFile( commport, cmd, 4, &count, 0 ) ) {
		layout.Log( "[错误]指令发送失败" );
		return false;
	}
	
	memset( ret, 0, 128 );
	//读取反馈数据量 
	if( !ReadFile( commport, ret, 4, &count, 0 ) ) {
		layout.Log( "[错误]接收反馈指令错误" );
		return false;
	}
	if( count < 4 ) {
		layout.Log( "[错误]反馈指令长度错误" );
		return false;
	}
	if( ret[3] == 0 )  {
		layout.Log( "[错误]设置失败" );
		return false;
	} else {
		layout.Log( "[正确]设置成功" );
		return true;
	}
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
	SCab *tag = 0;
	cabs.ForEach( [&]( SCab& cab ) {
		if( cab.num == cabnum ) {
			tag = &cab;
			return false;
		}
		return true;
	} );
	
	if( tag == 0 ) {
		return false;
	}
	
	//计算被累加的门编号 
	gatenum += (cabnum-1)*12;
	cabs.ForEach( [&]( SCab& cab ) {
		if( cab.num < cabnum ) {
			gatenum += cab.count - 12;
		}
		return true;
	} );
	
	DWORD cnt;
	char cmd[3] = { 0x02, (char)gatenum, 0x00 };
	WriteFile( commport, cmd, 3, &cnt, 0 );
	if( !ReadFile( commport, cmd, 2,  &cnt, 0 ) ) {
		layout.Log( "[错误]接受反馈时出错" );
		return false;
	}
	if( cnt < 2 or cmd[0] != 0x02 or cmd[1] != gatenum ) {
		char buf[64]; 
		layout.Log( "[错误]动作执行失败" );
		sprintf( buf, "0x%2X 0x%2X", cmd[0], cmd[1] );
		layout.Log( string(buf) );
		
		return false;
	}
	layout.Log( "[正确]操作成功" );
	return true;
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
	int gatestate = 0;
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
		gatestate = GetGateState( cabnum );
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
			
			if( ((gatestate>>i)&0x01) == 0 )
				glColor3ub( 210, 255, 210 );
			else
				glColor3ub( 255, 210, 210 );
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
			
			
			if( bK < 0 ) { 		//鼠标左键被按下了 
				if( ((gatestate>>(cabnum-1))&0x01) == 0 )
					glColor3ub( 195, 255, 195 );
				else
					glColor3ub( 255, 195, 195 );
			} else {
				if( ((gatestate>>(cabnum-1))&0x01) == 0 )
					glColor3ub( 225, 255, 225 );
				else
					glColor3ub( 255, 225, 225 );
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
	if( count == 0 or commport == INVALID_HANDLE_VALUE ) {
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

/*DrawList函数绘制柜体列表
########################################################################################
参数:
	------------------------------------------------------------------------------------
========================================================================================
返回值: 
*/
void CCabinet::DrawList( void ) {
	char buf[32];
	int cabnum = CatchList( pts );
	glColor3f( 0.75, 0.75, 0.75 );
	glBegin( GL_LINE_STRIP );
	glVertex2i( list_x, list_y );
	glVertex2i( list_x+list_width, list_y );
	glVertex2i( list_x+list_width, list_y+list_height );
	glVertex2i( list_x, list_y+list_height );
	glVertex2i( list_x, list_y );
	glEnd();
	
	glBegin( GL_QUADS );
	if( pts.x >= list_x and pts.x <= list_x+list_width and pts.y >= list_y and pts.y <= list_y+25 ) {
		if( GetKeyState( VK_LBUTTON ) < 0 ) {
			glColor3ub( 220, 220, 220 );
		} else {
			glColor3ub( 250, 250, 250 );
		}
	} else {
		glColor3ub( 240, 240, 240 );
	}
	glVertex2i( list_x+1, list_y+1 );
	glVertex2i( list_x+list_width-1, list_y+1 );
	glVertex2i( list_x+list_width-1, list_y+25-1 );
	glVertex2i( list_x+1, list_y+25-1 );
	
	
	if( pts.x >= list_x and pts.x <= list_x+list_width and pts.y >= list_y+list_height-25 and pts.y <= list_y+list_height ) {
		if( GetKeyState( VK_LBUTTON ) < 0 ) {
			glColor3ub( 195, 195, 195 );
		} else {
			glColor3ub( 235, 235, 235 );
		}
	} else {
		glColor3ub( 215, 215, 215 );
	}
	glVertex2i( list_x+1, list_y+list_height-1 );
	glVertex2i( list_x+list_width-1, list_y+list_height-1 );
	glVertex2i( list_x+list_width-1, list_y+list_height-25+1 );
	glVertex2i( list_x+1, list_y+list_height-25+1 );
	glEnd();
	
	glColor3ub( 115, 115, 115 );
	glBegin( GL_LINE_STRIP );
	glVertex2i( list_x+1, list_y+1 );
	glVertex2i( list_x+list_width-1, list_y+1 );
	glVertex2i( list_x+list_width-1, list_y+25-1 );
	glVertex2i( list_x+1, list_y+25-1 );
	glEnd();
	
	glBegin( GL_LINE_STRIP );
	glVertex2i( list_x+1, list_y+list_height-1 );
	glVertex2i( list_x+list_width-1, list_y+list_height-1 );
	glVertex2i( list_x+list_width-1, list_y+list_height-25+1 );
	glVertex2i( list_x+1, list_y+list_height-25+1 );
	glEnd();
	
	glBegin( GL_POLYGON );
	glColor3ub( 255, 255, 255 );
	glVertex2i( list_x+10, list_y+23 );
	glVertex2i( list_x+10, list_y+18 );
	glVertex2i( list_x+list_width/2, list_y+3 );
	glVertex2i( list_x+list_width/2, list_y+8 );
	glEnd();
	glBegin( GL_POLYGON );
	glColor3ub( 255, 255, 255 );
	glVertex2i( list_x+list_width-10, list_y+23 );
	glVertex2i( list_x+list_width-10, list_y+18 );
	glVertex2i( list_x+list_width/2, list_y+3 );
	glVertex2i( list_x+list_width/2, list_y+8 );
	glEnd();
	glBegin( GL_POLYGON );
	glColor3ub( 255, 255, 255 );
	glVertex2i( list_x+10, list_y+list_height-23 );
	glVertex2i( list_x+10, list_y+list_height-18 );
	glVertex2i( list_x+list_width/2, list_y+list_height-3 );
	glVertex2i( list_x+list_width/2, list_y+list_height-8 );
	glEnd();
	glBegin( GL_POLYGON );
	glColor3ub( 255, 255, 255 );
	glVertex2i( list_x+list_width-10, list_y+list_height-23 );
	glVertex2i( list_x+list_width-10, list_y+list_height-18 );
	glVertex2i( list_x+list_width/2, list_y+list_height-3 );
	glVertex2i( list_x+list_width/2, list_y+list_height-8 );
	glEnd();
	
	
	//int tagcab = CatchList( pts );
	for( int off = 0; off < 4; off++ ) {
		SCab* pcab = cabs.GetPointer( list_offset + off );
		if( pcab != 0 ) {
			if( curcabnum == pcab->num )
				glColor3ub( 215, 215, 215 );
			else
				glColor3ub( 195, 195, 195 ) ;
			
			glBegin( GL_QUADS );
			glVertex2i( list_x+4, list_y+25+off*100+10 );
			glVertex2i( list_x+list_width-4, list_y+25+off*100+10 );
			glVertex2i( list_x+list_width-4, list_y+25+off*100+90 );
			glVertex2i( list_x+4, list_y+25+off*100+90 );
			glEnd();
			
			char buf[64];
			glColor3ub( 255, 255, 255 );
			glRasterPos2i( list_x+25, list_y+25+off*100+35 );
			sprintf( buf, "%d号柜", pcab->num );
			DrawText( buf );
			glRasterPos2i( list_x+25, list_y+25+off*100+55 );
			sprintf( buf, "%d门", pcab->count );
			DrawText( buf );
			if( curcabnum == pcab->num ) {
				glColor3ub( 255, 0, 0 );
				glBegin( GL_POLYGON );
				glVertex2i( list_x, list_y+25+off*100+48 );
				glVertex2i( list_x, list_y+25+off*100+52 );
				glVertex2i( list_x+20, list_y+25+off*100+52 );
				glVertex2i( list_x+20, list_y+25+off*100+48 );
				glEnd();
			}
		}
	}
}

/*CatchCab函数从鼠标相对于窗口的坐标抓取一个柜体
#########################################################################################
参数:
	POINTS pos							鼠标坐标
=========================================================================================
返回值:
	int									柜体编号
*/
int CCabinet::CatchList( POINTS pos ) {
	int tag = 0;
	if( pos.x >= list_x and pos.x <= list_x+list_width and pos.y >= list_y and pos.y <= list_y+list_height ) {
		tag = (pos.y-list_y-25)/100 + list_offset;
		if( tag >= cabs.Count() ) {
			tag = 0;
		} else {
			tag = cabs[tag].num;
		}
	}
	return tag;
}

/*GetLastCab函数从列表中获取目标柜号相邻的柜号
########################################################################################
参数:
	int cabnum							基准柜号
========================================================================================
返回值:
	int 								若失败则返回0,成功则返回新的柜子号码
*/
int CCabinet::GetLastCab( int cabnum ) {
	int ncab = -1;
	for( int i = cabnum-1; i > 0; i-- ) {
		cabs.ForEach( [&]( SCab& cab ) {
			if( cab.num == i ) {
				ncab = cab.num;
				return false;
			}
			return true;
		} );
		if( ncab != -1 ) {
			break; 
		}
	}
	return ncab;
}

/*GetNextCab函数从列表中获取目标柜号相邻的柜号
########################################################################################
参数:
	int cabnum							基准柜号
========================================================================================
返回值:
	int 								若失败则返回0,成功则返回新的柜子号码
*/
int CCabinet::GetNextCab( int cabnum ) {
	int ncab = -1;
	for( int i = cabnum+1; i < 240; i++ ) {
		cabs.ForEach( [&]( SCab& cab ) {
			if( cab.num == i ) {
				ncab = cab.num;
				return false;
			}
			return true;
		} );
		if( ncab != -1 ) {
			break; 
		}
	}
	return ncab;
}
/*GetCabCount函数获取柜子总数 
########################################################################################
参数:
========================================================================================
返回值:
	int 								若失败则返回0,成功则返回柜子总数 
*/
int CCabinet::GetCabCount( void ) {
	return cabs.Count();
}
#endif
