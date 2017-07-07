#include <reg52.h>

#define frequence 11059200UL		//ʹ��22.1184M����
#define baudrate   9600UL		  	//�����ʶ���Ϊ9600
						   
unsigned char zifu = 'a';		//����ʾ�ַ�
unsigned char rcvbuf[32];
int ptr;
/*
void delay(unsigned char i)
{
	unsigned char j,k;

	for(j=i; j>0; j--)
		for(k=90; k>0; k--);
}*/

void init(void)					//���ڳ�ʼ��
{
	EA    = 0;					//��ʱ�ر��ж�
	TMOD &= 0x0F;				//��ʱ��1ģʽ�����ڸ�4λ
	TMOD |= 0x20;				//��ʱ��1������ģʽ2���Զ���װģʽ
	SCON  = 0x50;				//���ڹ�����ģʽ1
	TH1   = 256-frequence/(baudrate*12*16);  //���㶨ʱ����װֵ
	TL1   = 256-frequence/(baudrate*12*16);
	PCON |= 0x80;				//���ڲ����ʼӱ�
	ES    = 1;					//�����ж�����
	TR1   = 1;					//������ʱ��1
	REN   = 1;					//������� 
	EA    = 1;					//�����ж�
}

void sdbyte(unsigned char d)	//����һ���ֽڵ����ݣ��β�d��Ϊ���������ݡ�
{
	SBUF = d; 			//������д�뵽���ڻ���
	while(!TI); 		//�ȴ��������
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
void uart(void) interrupt 4	//���ڷ����ж�
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

