#include < reg51.h >
#include < intrins.h >

#define uchar unsigned char
#define uint  unsigned int

/*******************************����ȫ�ֱ���********************************/
//ADC0832������
sbit AD2_CS =P3^0;  //ADC0832 chip seclect
sbit AD2_DI =P3^4;  //ADC0832 k in
sbit AD2_DO =P3^4;  //ADC0832 k out
sbit AD2_CLK=P3^1; //ADC0832 clock signal
sbit LED2_=P0^5;
//ADC0832������
sbit AD1_CS =P3^5;  //ADC0832 chip seclect
sbit AD1_DI =P3^7;  //ADC0832 k in
sbit AD1_DO =P3^7;  //ADC0832 k out
sbit AD1_CLK=P3^6; //ADC0832 clock signal
sbit LED1_=P0^5;

sbit LED4_=P0^7;
sbit LED5_=P2^7;

int meanvalue1;
int meanvalue2; 


unsigned char data  temp_data1_[2] = {0x00,0x00} ;
unsigned char data  display1_[5] =   {0x00,0x00,0x00,0x00,0x00} ;
unsigned char data  temp_data2_[2] = {0x00,0x00} ;
unsigned char data  display2_[5] =   {0x00,0x00,0x00,0x00,0x00} ;
unsigned char code  ditab[16] =    {0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x04,
                                                        0x05,0x06,0x06,0x07,0x08,0x08,0x09,0x09} ;/*С��λ����������*/

/*******************************************************************/
void delay1(int ms)//�ӳٺ���

{ unsigned char y ;
  while(ms--)
  {
  for(y = 0 ; y<250 ; y++)
  {
   _nop_() ;
   _nop_() ;
   _nop_() ;
   _nop_() ;
  }
  }
 }

/****************************************************************/
/****************************************************************/
/****************************************************************/

sbit P1_0=P1^0;
sbit P1_1=P1^1;
sbit P1_2=P1^2;
sbit P1_3=P1^3;

static unsigned int count; //����
static int step_index; //������������ֵΪ0��7

char i,j;
char HadBeenDone;

static bit turn; //�������ת������
static bit stop_flag; //�������ֹͣ��־
static int speedlevel; //�������ת�ٲ�������ֵԽ���ٶ�Խ������СֵΪ1���ٶ����
static int spcount;    //�������ת�ٲ�������
void delay(unsigned int endcount); //��ʱ��������ʱΪendcount*0.5����
void gorun();          //����������Ʋ�������

void Motor_CompSlowlyUp(void);
void Motor_CompSlowlyDown(void);

/*************************************************************/

unsigned int Adc1_0832()     //ADת�������ؽ��
{
    uchar i=0;
    uchar j;
    uint dat=0;
    uchar ndat=0;
    AD1_DI=1;
    _nop_();
    _nop_();
    AD1_CS=0;//����CS��
    _nop_();
    _nop_();
    AD1_CLK=1;//����CLK��
    _nop_();
    _nop_();
    AD1_CLK=0;//����CLK��,�γ��½���1
    _nop_();
    _nop_();
    AD1_CLK=1;//����CLK��
    AD1_DI=0;	//ͨ��0
    _nop_();
    _nop_();
    AD1_CLK=0;//����CLK��,�γ��½���2
    _nop_();
    _nop_();
    AD1_CLK=1;//����CLK��
    AD1_DI=1;//ѡͨ��	1
    _nop_();
    _nop_();
    AD1_CLK=0;//����CLK��,�γ��½���3
    AD1_DI=1;//����������� 
    _nop_();
    _nop_();
    dat=0;
    for(i=0;i<8;i++)
    {
        dat|=AD1_DO;//������
        AD1_CLK=1;
        _nop_();
        _nop_();
        AD1_CLK=0;//�γ�һ��ʱ������
        _nop_();
        _nop_();
        dat<<=1;
        if(i==7)dat|=AD1_DO;
    }  
    for(i=0;i<8;i++)
    {
        j=0;
        j=j|AD1_DO;//������
        AD1_CLK=1;
        _nop_();
        _nop_();
        AD1_CLK=0;//�γ�һ��ʱ������
        _nop_();
        _nop_();
        j=j<<7;
        ndat=ndat|j;
        if(i<7)ndat>>=1;
    }
    AD1_CS=1;//����CS��
    AD1_CLK=0;//����CLK��
    AD1_DO=1;//�������ݶ�,�ص���ʼ״̬
    dat<<=8;
    dat|=ndat;
    return(dat);            //return dat
}
/************************************************************/

 unsigned int Adc2_0832()     //ADת�������ؽ��
{
    uchar i=0;
    uchar j;
    uint dat=0;
    uchar ndat=0;
    AD2_DI=1;
    _nop_();
    _nop_();
    AD2_CS=0;//����CS��
    _nop_();
    _nop_();
    AD2_CLK=1;//����CLK��
    _nop_();
    _nop_();
    AD2_CLK=0;//����CLK��,�γ��½���1
    _nop_();
    _nop_();
    AD2_CLK=1;//����CLK��
    AD2_DI=0;	//ͨ��0
    _nop_();
    _nop_();
    AD2_CLK=0;//����CLK��,�γ��½���2
    _nop_();
    _nop_();
    AD2_CLK=1;//����CLK��
    AD2_DI=1;//ѡͨ��	1
    _nop_();
    _nop_();
    AD2_CLK=0;//����CLK��,�γ��½���3
    AD2_DI=1;//����������� 
    _nop_();
    _nop_();
    dat=0;
    for(i=0;i<8;i++)
    {
        dat|=AD2_DO;//������
        AD2_CLK=1;
        _nop_();
        _nop_();
        AD2_CLK=0;//�γ�һ��ʱ������
        _nop_();
        _nop_();
        dat<<=1;
        if(i==7)dat|=AD2_DO;
    }  
    for(i=0;i<8;i++)
    {
        j=0;
        j=j|AD2_DO;//������
        AD2_CLK=1;
        _nop_();
        _nop_();
        AD2_CLK=0;//�γ�һ��ʱ������
        _nop_();
        _nop_();
        j=j<<7;
        ndat=ndat|j;
        if(i<7)ndat>>=1;
    }
    AD2_CS=1;//����CS��
    AD2_CLK=0;//����CLK��
    AD2_DO=1;//�������ݶ�,�ص���ʼ״̬
    dat<<=8;
    dat|=ndat;
    return(dat);            //return dat
}

/*								 ��ȡ����                         */  
/*******************************************************************/     
adc1_display(void) {
     temp_data1_[0] =  Adc1_0832();   //���ݵ�8λ


/* ����ת�����¶���ʾ                                              */
/*******************************************************************/
  
  display1_[4]=(temp_data1_[0]&0x0f) ;  //�����8λ������
  display1_[0]=ditab[display1_[4]]+0x30 ;     //����С��λ��ֵ

  display1_[4]=((temp_data1_[0]&0xf0)>>4)|((temp_data1_[1]&0x0f)<<4) ;
  display1_[3]=display1_[4]/100+0x30 ;
  display1_[1]=display1_[4]%100 ;
  display1_[2]=display1_[1]/10+0x30 ;
  display1_[1]=display1_[1]%10+0x30 ;

	  }







adc2_display(void) {
     temp_data2_[0] =  Adc2_0832();   //���ݵ�8λ


/* 							����ת��                        */
/*******************************************************************/
  
  display2_[4]=(temp_data2_[0]&0x0f) ;  //�����8λ������
  display2_[0]=ditab[display2_[4]]+0x30 ;     //����С��λ��ֵ

  display2_[4]=((temp_data2_[0]&0xf0)>>4)|((temp_data2_[1]&0x0f)<<4) ;
  display2_[3]=display2_[4]/100+0x30 ;
  display2_[1]=display2_[4]%100 ;
  display2_[2]=display2_[1]/10+0x30 ;
  display2_[1]=display2_[1]%10+0x30 ;
   
	  }

  void main(void)
  { 
count = 0;
step_index = 0;
spcount = 0;
stop_flag = 0;

P1_0 = 0;
P1_1 = 0;
P1_2 = 0;
P1_3 = 0;

EA = 1;            //����CPU�ж� 
TMOD = 0x11; //�趨ʱ��0��1Ϊ16λģʽ1 
ET0 = 1;           //��ʱ��0�ж����� 

TH0 = 0xFE;
TL0 = 0x0C;   //�趨ʱÿ��0.5ms�ж�һ��


	while(1) {   
	adc1_display();			
	adc2_display();

meanvalue1=((char)((display1_[1]-0x30+display2_[1]-0x30)/2)+0x30);
meanvalue2=(display2_[2]-0x30+display1_[2]-0x30+0x30);


if(meanvalue2>=0x31){LED5_=1;LED4_=~LED4_;delay1(100);Motor_CompSlowlyUp();}
else if(meanvalue1>=0x39){LED5_=1;LED4_=~LED4_;delay1(100);Motor_CompSlowlyUp();}
else if(meanvalue1>=0x35){LED4_=0;LED5_=0;delay1(100);}
else   {LED4_=1;LED5_=~LED5_;delay1(100);Motor_CompSlowlyDown();}
	 }
	}



/*****************************************************************/
/****************************************************************/
/****************************************************************/

void Motor_CompSlowlyDown(void){
turn=1;
spcount=0;
    speedlevel = 2;
	 for(i=0;i<=7;i++)
	 	for(j=0;j<=15;j++){HadBeenDone=0;TH0=0xFE;TL0=0x0c;TR0=1;delay(2);
		if(HadBeenDone==1){
		if(spcount<=0){spcount=speedlevel;
		gorun();}}}
		TR0=0;
}


/****************************************/

void Motor_CompSlowlyUp(void){
turn=0;
spcount=0;
    speedlevel = 2;
	 for(i=0;i<=7;i++)
	 	for(j=0;j<=15;j++){HadBeenDone=0;TH0=0xFE;TL0=0x0c;TR0=1;delay(2);
		if(HadBeenDone==1){
		if(spcount<=0){spcount=speedlevel;
		gorun();}}}
		TR0=0;
}

//��ʱ��0�жϴ��� 
void timeint(void) interrupt 1 
{ 
TH0=0xFE;
TL0=0x0C; //�趨ʱÿ��0.5ms�ж�һ��


count++;
if(HadBeenDone==0){HadBeenDone=1;
spcount--;
}

}

void delay(unsigned int endcount)
{
count=0;
do{}while(count<endcount);
}

void gorun()
{
if (stop_flag==1)
{
    P1_0 = 0;
    P1_1 = 0;
    P1_2 = 0;
    P1_3 = 0;
    return;
}

switch(step_index)
{
case 0: //0
    P1_0 = 1;
    P1_1 = 0;
    P1_2 = 0;
    P1_3 = 0;
    break;
case 1: //0��1
    P1_0 = 1;
    P1_1 = 1;
    P1_2 = 0;
    P1_3 = 0;
    break;
case 2: //1
    P1_0 = 0;
    P1_1 = 1;
    P1_2 = 0;
    P1_3 = 0;
    break;
case 3: //1��2
    P1_0 = 0;
    P1_1 = 1;
    P1_2 = 1;
    P1_3 = 0;
    break;
case 4: //2
    P1_0 = 0;
    P1_1 = 0;
    P1_2 = 1;
    P1_3 = 0;
    break;
case 5: //2��3
    P1_0 = 0;
    P1_1 = 0;
    P1_2 = 1;
    P1_3 = 1;
    break;
case 6: //3
    P1_0 = 0;
    P1_1 = 0;
    P1_2 = 0;
    P1_3 = 1;
    break;
case 7: //3��0
    P1_0 = 1;
    P1_1 = 0;
    P1_2 = 0;
    P1_3 = 1;
}

if (turn==0)
{
    step_index++;
    if (step_index>7)
      step_index=0;
}
else
{
    step_index--;
    if (step_index<0)
      step_index=7;
}
    
}
