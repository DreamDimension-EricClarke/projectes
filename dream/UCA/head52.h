#ifndef	_HEAD52_H_
#define _HEAD52_H_
/********************
�ļ���: head52.h
�������� 
  ʱ�ӳ�ʼ������
  ���ڳ�ʼ������
  IIC�������߲�������
  ��ʱ����
��������:
  SCL IIC����ʱ�Ӷ˿�
  SDA IIC�������ݶ˿�
ʹ�ù���:
  ʹ��ǰ���й̶��˿ڵı�Ҫ����
  ʱ��T1��������������
  ʱ��T2����������ʱ����
����:
	Eric.Clarke(������)
********************/

#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long

uchar code table[11]= //���������
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
	0x80			//С����,�û�����������С����
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/********************
ʱ��0(�����ʱ����ʼ��)
********************/
typedef struct time_t  //ʱ��ṹ��
{
	uint  msc;		   //����(ǧ��֮һ��)
	uchar sec;		   //��
	uchar min;		   //��
	uchar hor;		   //ʱ
	unsigned long day; //��
}time;				   //ʱ��ṹ��



/*��ʱ��T0�õ���ȫ�ֱ���*/
time local;			   //ʱ�����
uint initdat;		   //ÿ���ж�ʱװ��T0�ĳ�ֵ
uchar step;			   //ÿ���ж�local�ĺ���ֵ��������
uchar bcount;          //�ٷ��������
bit bslp;			   //���߱�־
unsigned long int slpt;//����ʱ��
/********************
����:��ʱ��T0�ĳ�ʼ��
����:����Ƶ��
����ֵ:��
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


/*��ʱ��T0���ж�ϵͳ*/
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
����T0����ʱ����
����:�԰ٷ�֮һ��Ϊ��λ��ʱ
����:length ��ʱʱ��
����ֵ:��
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
/*IIC���߲�������*/

/********************
IIC���߳�ʼ��
����:��ʼ������
����:��
����ֵ:��
********************/
void iicInit(void)
{
	SDA=1;
	sleep(1);
	SCL=1;
	sleep(1);
}


/*******************
IIC��������
����:�������������IIC�����ź�
����:��
����ֵ:��
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
IIC���ߵȴ�Ӧ����
����:���ڷ���ÿһ֡�����һλͬʱ�ȴ�Ӧ��
����:��
����ֵ: ��Ӧ�� 1
		��Ӧ�� 0
		��ʱ�� 2
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
IIC����ֹͣ
����:���ڷ���IICֹͣ����
����:��
����ֵ:��
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
IIC���߷���
����:������IIC�����Ϸ���һ���ֽ�
����:Ҫ���͵�����
����ֵ:��
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
IIC���߷�Ӧ��
����:IIC���߷�Ӧ���ź�
����:��
����ֵ:��
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
IIC���߶�ȡһ���ֽ�
����:IIC���߶�ȡһ���ֽ�
����:��
����ֵ:��ȡ������
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
/*����ͨ��*/

/*******************
����ͨ�ų�ʼ��
����:��ʼ��T1��ʱ���ʹ����ж�
����:
	frequent:����Ƶ��
	botrate:������
	scon:��������
����ֵ:0ʧ��
	   1�ɹ�
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
