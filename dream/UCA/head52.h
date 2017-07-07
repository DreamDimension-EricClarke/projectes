#ifndef	_HEAD52_H_
#define _HEAD52_H_
/********************
文件名: head52.h
包含内容 
  时钟初始化函数
  串口初始化函数
  IIC串行总线操作函数
  延时函数
命名规则:
  SCL IIC总线时钟端口
  SDA IIC总线数据端口
使用规则:
  使用前进行固定端口的必要定义
  时钟T1用来产生波特率
  时钟T2用来控制延时函数
作者:
	Eric.Clarke(王雨泽)
********************/

#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long

uchar code table[11]= //数码管字组
{
	0x3f,			//0	  
	0x06,			//1
	0x5b,			//2
	0x4f,			//3
	0x66,			//4
	0x6d,			//5
	0x7d,			//6
	0x07,			//7
	0x7f,			//8
	0x67,			//9
	0x80			//小数点,用或运算来激活小数点
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/********************
时钟0(程序计时器初始化)
********************/
typedef struct time_t  //时间结构体
{
	uint  msc;		   //毫秒(千分之一秒)
	uchar sec;		   //秒
	uchar min;		   //分
	uchar hor;		   //时
	unsigned long day; //天
}time;				   //时间结构体



/*计时器T0用到的全局变量*/
time local;			   //时间变量
uint initdat;		   //每次中断时装入T0的初值
uchar step;			   //每次中断local的毫秒值增长步长
uchar bcount;          //百分秒计数器
bit bslp;			   //休眠标志
unsigned long int slpt;//休眠时间
/********************
功能:计时器T0的初始化
参数:晶振频率
返回值:无
********************/
void t0Init(float frequent)
{
	 if(frequent==11.0592f)
	 {
	  	initdat=65536-9216;
		step=10;
	 }
	 else
	 {
	  	initdat=65536-1000;
		step=1;
	 }
	 TMOD&=0xf0;
	 TMOD|=0x01;
	 TH0=initdat/256;
	 TL0=initdat%256;
	 ET0=1;
	 TR0=1;
}


/*计时器T0的中断系统*/
void cutT0(void)  interrupt 1
{
	TH0=initdat/256;
	TL0=initdat%256;
	local.msc+=step;
	if(bslp==1)bcount+=step;
	if(bcount>=10&&bslp==1)
	{
		bcount=0;
		slpt+=1;
	}
	if(local.msc==1000)
	{
		local.sec+=1;
		local.msc=0;
		if(local.sec==60)
		{
			local.min+=1;
			local.sec=0;
			if(local.min==60)
			{
				local.hor+=1;
				local.min=0;
				if(local.hor==24)
				{
					local.day+=1;
					local.hor=0;
				}
			}
		}
	}
}
/********************
依靠T0的延时函数
功能:以百分之一秒为单位延时
参数:length 延时时间
返回值:无
********************/
void sleep(unsigned long int length)
{
	slpt=0;
	bslp=1;
	while(slpt<length);
	bslp=0;
	slpt=0;
	bcount=0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	   
/*IIC总线操作函数*/

/********************
IIC总线初始化
功能:初始化总线
参数:无
返回值:无
********************/
void iicInit(void)
{
	SDA=1;
	sleep(1);
	SCL=1;
	sleep(1);
}


/*******************
IIC总线启动
功能:发出启动传输的IIC总线信号
参数:无
返回值:无
*******************/
void iicStart(void)
{
	SDA=1;
	sleep(0);
	SCL=1;
	sleep(0);
	SDA=0;
	sleep(0);
	SCL=0;
	sleep(0);
}

/*******************
IIC总线等待应答函数
功能:用于发送每一帧的最后一位同时等待应答
参数:无
返回值: 有应答 1
		无应答 0
		无时钟 2
*******************/
uchar iicAck()
{
	unsigned char i;
	SDA=1;
	sleep(0);
	SCL=1;
	while(SDA&&(i++)<250);
	return SDA==0?1:0;
}
/*******************
IIC总线停止
功能:用于发送IIC停止符号
参数:无
返回值:无
*******************/
void iicStop(void)
{
	SDA=0;
	sleep(0);
	SCL=1;
	sleep(0);
	SDA=1;
	sleep(0);
	SCL=0;
}
/*******************
IIC总线发送
功能:用于在IIC总线上发送一个字节
参数:要发送的数据
返回值:无
*******************/
void iicSend(uchar dat)
{
	uchar i;
	for(i=8;i>0;i--)
	{
		SCL=0;
		sleep(0);
		SDA=(dat>>(i-1))&1;
		SCL=1;
		sleep(0);
	}
	SCL=0;
}
/*******************
IIC总线非应答
功能:IIC总线非应答信号
参数:无
返回值:无
*******************/
void iicNoack(void)
{
	 SDA = 1;
	 sleep(0);
	 SCL = 1;
	 sleep(0);
	 SCL = 0;
	 sleep(0);
}
/*******************
IIC总线读取一个字节
功能:IIC总线读取一个字节
参数:无
返回值:读取的数据
*******************/
uchar iicRead(void)
{
	uchar i,dat=0,c;
	SDA=1;
	for(i=8;i>0;i--)
	{
		SCL=0;
		sleep(0);
		SCL=1;
		sleep(0);
		c=SDA;
		dat|=(c<<(i-1));
	}
	SCL=0;
	return dat;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*串口通信*/

/*******************
串口通信初始化
功能:初始化T1计时器和串口中断
参数:
	frequent:晶振频率
	botrate:波特率
	scon:串口设置
返回值:0失败
	   1成功
*******************/
uchar commInit(unsigned long int frequent,unsigned long int botrate,uchar scon,uchar smod)
{
	unsigned long int outrate;

	if(smod>1) return 0;

	outrate=(32-(16*smod))*botrate;
	if(outrate==0) return 0;

	TH1=256.0-frequent/(12.0*outrate);
	TL1=256.0-frequent/(12.0*outrate);

	TMOD&=0x0f;
	TMOD|=0x20;

	PCON|=(smod<<7);
	SCON=scon;

	TR1=1;
	ES=1;

	return 1;
}
		
#endif
