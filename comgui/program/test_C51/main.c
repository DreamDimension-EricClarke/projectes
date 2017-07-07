#include <reg52.h>

#define frequence 11059200UL		//使用22.1184M晶体
#define baudrate   9600UL		  	//波特率定义为9600
						   
unsigned char zifu = 'a';		//待显示字符
unsigned char rcvbuf[32];
int ptr;
/*
void delay(unsigned char i)
{
	unsigned char j,k;

	for(j=i; j>0; j--)
		for(k=90; k>0; k--);
}*/

void init(void)					//串口初始化
{
	EA    = 0;					//暂时关闭中断
	TMOD &= 0x0F;				//定时器1模式控制在高4位
	TMOD |= 0x20;				//定时器1工作在模式2，自动重装模式
	SCON  = 0x50;				//串口工作在模式1
	TH1   = 256-frequence/(baudrate*12*16);  //计算定时器重装值
	TL1   = 256-frequence/(baudrate*12*16);
	PCON |= 0x80;				//串口波特率加倍
	ES    = 1;					//串行中断允许
	TR1   = 1;					//启动定时器1
	REN   = 1;					//允许接收 
	EA    = 1;					//允许中断
}

void sdbyte(unsigned char d)	//发送一个字节的数据，形参d即为待发送数据。
{
	SBUF = d; 			//将数据写入到串口缓冲
	while(!TI); 		//等待发送完毕
	TI = 0;
}

void send( unsigned char* buf, int leng )
{
	int i;
	for( i = 0; i < leng; i++ )
		sdbyte( buf[i] );
}
int recv(  unsigned char* buf )
{
	int i;
	REN = 0;
	for( i = 0; i < ptr; i++ )
		buf[i] = rcvbuf[i];
	ptr = 0;
	REN = 1;
	return i;
}
void uart(void) interrupt 4	//串口发送中断
{
	if(RI)
	{
		RI = 0;
		rcvbuf[ptr++] = SBUF;
	}
}

void main( )
{
	unsigned char cmd[32];
	unsigned short addr;
	init();
	while( 1 )
	{
		while( ptr < 6 );
		recv( cmd );
		addr = cmd[1] + (((int)cmd[2])<<8);
		cmd[0] = 0x3a;
		cmd[4] = 1;
		cmd[5] = 2;
		cmd[6] = addr%2;
		cmd[7] = (addr+1)%2;
		cmd[8] = cmd[0] ^ cmd[1] ^ cmd[2] ^ cmd[3] ^ cmd[4] ^ cmd[5] ^ cmd[6] ^ cmd[7];
		cmd[9] = 0x23;
		send( cmd, 10 );
	}
	return;
}

