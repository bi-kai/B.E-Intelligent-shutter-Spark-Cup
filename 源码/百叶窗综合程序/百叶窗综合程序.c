#include < reg51.h >
#include < intrins.h >

#define uchar unsigned char
#define uint  unsigned int

sbit PT2272_Data0=P2^0;
sbit PT2272_Data1=P2^1;
sbit PT2272_Data2=P2^2;
sbit PT2272_Data3=P2^3;

/******************************************/

//ADC0832������
sbit AD2_CS =P1^4;  //ADC0832 chip seclect
sbit AD2_DI =P3^4;  //ADC0832 k in
sbit AD2_DO =P3^4;  //ADC0832 k out
sbit AD2_CLK=P1^5; //ADC0832 clock signal
sbit LED2_=P0^5;
//ADC0832������
sbit AD1_CS =P3^5;  //ADC0832 chip seclect
sbit AD1_DI =P3^7;  //ADC0832 k in
sbit AD1_DO =P3^7;  //ADC0832 k out
sbit AD1_CLK=P3^6; //ADC0832 clock signal
sbit LED1_=P0^6;

sbit Speaker=P1^7;
sbit P3_2=P3^2;

sbit LED4_=P0^7;  //�Զ�����ָʾ��
sbit LED5_=P2^7;  //�Զ�����ָʾ��

sbit LED6_=P0^0;  //��ȫ��ָʾ��
sbit LED7_=P0^1;  //��ȫ�ر�ָʾ��

int meanvalue1;
int meanvalue2; 


unsigned char data  temp_data1_[2] = {0x00,0x00} ;
unsigned char data  display1_[5] =   {0x00,0x00,0x00,0x00,0x00} ;
unsigned char data  temp_data2_[2] = {0x00,0x00} ;
unsigned char data  display2_[5] =   {0x00,0x00,0x00,0x00,0x00} ;
unsigned char code  ditab[16] =    {0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x04,
                                                        0x05,0x06,0x06,0x07,0x08,0x08,0x09,0x09} ;/*С��λ����������*/


/******************��ʱ���ܲ���************************/

uchar T1_h,T1_m,T1_s,T1_counter;

void T1_init()
{

  TMOD=0x11;
  TH1=0x94;
  TL1=0x00;
  EA=1;
  ET1=1;
  TR1=1;
  T1_counter=0;
  T1_h=12;T1_m=0;T1_s=0;

}

/******************************************/

bit SignalControl_Flag1=1;		//������־λ
bit SignalControl_Flag2=1;		//���������һ�ε�������־λ
bit SelfControl_Flag=0;     	//�Զ����Ʊ�־λ
bit Check_ForOnce=1;		//ÿ�ΰ����жϺ�ֻ����˿�ɨ��һ��
bit Base_Flag=0;			//�ж�ʱ���Ƿ�������Χ֮�ڱ�־λ

char INT1_Counter=0;
//char Motor_StepCounter=0;

void Motor_CompSlowlyUp(void);
void Motor_CompSlowlyDown(void);
void MainModel2(void);
void MainModel1(void);
void Motor_OpenUp(void);
void Motor_CloseDown(void);
int Motor_StepCounter=90;


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


 /*************************������1***************************/

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




 /***************************************��������AD�ۺϲ��ֵĳ���********************************************************************/
/************************************************************************************************************************************/
/************************************************************************************************************************************/
/************************************************************************************************************************************/
/************************************************************************************************************************************/

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

	LED1_=~LED1_;delay1(50);LED1_=~LED1_;delay1(50);LED1_=~LED1_;

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

		LED2_=~LED2_;delay1(50);LED2_=~LED2_;delay1(50);LED2_=~LED2_;
	
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






 /**************************************************************/

void main(void){

/*************������2******************/
Speaker=1;
P3_2=1;

T1_init();

count = 0;
step_index = 0;
spcount = 0;
stop_flag = 0;

P1_0 = 0;
P1_1 = 0;
P1_2 = 0;
P1_3 = 0;

IT1=1;
PX1=1;
EX1=1;
EA = 1;            //����CPU�ж� 
TMOD = 0x11; //�趨ʱ��0��1Ϊ16λģʽ1 
ET0 = 1;           //��ʱ��0�ж����� 

TH0 = 0xFE;
TL0 = 0x0C;   //�趨ʱÿ��0.5ms�ж�һ��

/***************************************/


	PT2272_Data0=1;
	PT2272_Data1=1;
	PT2272_Data2=1;
	PT2272_Data3=1;

while(1){

if((T1_h<=7)||(T1_h>=19)){	Motor_CloseDown();Base_Flag=1;}
else Base_Flag=0;


	if	((SelfControl_Flag==0)&&(Base_Flag==0)){ MainModel1(); }
	else     {  MainModel2();}

		  }

		  
		  
		  
		  
		  
		  
		  
}


/**********************************************����ĵ�һ����*****************************************************************************/

void MainModel1(void){ 

Speaker=1;
P3_2=1;

while(1) {   
	adc1_display();			
	adc2_display();


if		 (Motor_StepCounter>=90){LED6_=0;LED7_=1;}
else if(Motor_StepCounter<=0){LED6_=1;LED7_=0;}
else   {LED6_=1;LED7_=1;}

meanvalue1=((char)((display1_[1]-0x30+display2_[1]-0x30)/2)+0x30);
meanvalue2=(display2_[2]-0x30+display1_[2]-0x30+0x30);



if(meanvalue2>=0x31){Motor_StepCounter--;
stop_flag=0;if(Motor_StepCounter<=0){stop_flag=1;}
LED5_=1;LED4_=~LED4_;delay1(100);
Motor_CompSlowlyUp();}

else if(meanvalue1>=0x39){Motor_StepCounter--;
stop_flag=0;
if(Motor_StepCounter<=0){stop_flag=1;}
LED5_=1;LED4_=~LED4_;delay1(100);
Motor_CompSlowlyUp();}

else if(meanvalue1>=0x35){LED4_=0;LED5_=0;delay1(100);}

else   {Motor_StepCounter++;
stop_flag=0;
if(Motor_StepCounter>=90){stop_flag=1;}
LED4_=1;LED5_=~LED5_;delay1(100);
Motor_CompSlowlyDown();}

if(P3_2==0){Speaker=0;}
else if(P3_2==1){Speaker=1;}

if(SelfControl_Flag==1)break;
	 }

}

/*************************����ĵ�һ���ֽ���**********************************/

void MainModel2(void){

Speaker=1;
P3_2=1;
stop_flag=0;

	while(1){
	PT2272_Data0=1;
	PT2272_Data1=1;
	PT2272_Data2=1;
	PT2272_Data3=1;

	
if	   (Motor_StepCounter>=90){LED6_=0;LED7_=1;}
else if(Motor_StepCounter<=0){LED6_=1;LED7_=0;}
else   {LED6_=1;LED7_=1;}


	if(Check_ForOnce==0){

			if		 ((PT2272_Data0==1)&&(PT2272_Data1==0)&&(PT2272_Data2==0)&&(PT2272_Data3==0))
				  	 {SignalControl_Flag1=0;
					 SignalControl_Flag2=1;
					 }

			else if((PT2272_Data0==0)&&(PT2272_Data1==0)&&(PT2272_Data2==1)&&(PT2272_Data3==0))
					 {SignalControl_Flag1=0;
					 SignalControl_Flag2=1;
					 }

			else if((PT2272_Data0==1)&&(PT2272_Data1==1)&&(PT2272_Data2==1)&&(PT2272_Data3==0))
					 {SignalControl_Flag2=0;
					 SignalControl_Flag1=1;
					 }

			else if((PT2272_Data0==0)&&(PT2272_Data1==1)&&(PT2272_Data2==1)&&(PT2272_Data3==0))
					 {SignalControl_Flag2=0;
					 SignalControl_Flag1=1;
					 }

			else if((PT2272_Data0==1)&&(PT2272_Data1==1)&&(PT2272_Data2==0)&&(PT2272_Data3==0))
					 {SignalControl_Flag2=0;
					 SignalControl_Flag1=1;
					 }

			else if((PT2272_Data0==0)&&(PT2272_Data1==1)&&(PT2272_Data2==0)&&(PT2272_Data3==0))
					 {SignalControl_Flag2=0;
					 SignalControl_Flag1=1;
					 }

			else if((PT2272_Data0==0)&&(PT2272_Data1==0)&&(PT2272_Data2==0)&&(PT2272_Data3==0))
					 {SignalControl_Flag2=0;
					 SignalControl_Flag1=1;
					 }
	
	Check_ForOnce=1;}
	/****************************************************************************************************/	

			if		 ((SignalControl_Flag1==0)&&((PT2272_Data0==1)&&(PT2272_Data1==0)&&(PT2272_Data2==0)&&(PT2272_Data3==0)))
					 {if(Motor_StepCounter<=0)SignalControl_Flag1=1;
					  else SignalControl_Flag1=0;
					 Motor_CompSlowlyUp();
					 Motor_StepCounter--;
					 
					 }

			else if((SignalControl_Flag1==0)&&((PT2272_Data0==0)&&(PT2272_Data1==0)&&(PT2272_Data2==1)&&(PT2272_Data3==0)))
					 {if(Motor_StepCounter>=90)SignalControl_Flag1=1;
					  else SignalControl_Flag1=0;
					 Motor_CompSlowlyDown();
					 Motor_StepCounter++;
					 }

		 	else if((SignalControl_Flag2==0)&&((PT2272_Data0==1)&&(PT2272_Data1==1)&&(PT2272_Data2==1)&&(PT2272_Data3==0)))
					 {Motor_OpenUp();
					 SignalControl_Flag2=1;
					 }

			else if((SignalControl_Flag2==0)&&((PT2272_Data0==0)&&(PT2272_Data1==1)&&(PT2272_Data2==1)&&(PT2272_Data3==0)))
					 {Motor_CloseDown();
					 SignalControl_Flag2=1;
					 }

			else if((SignalControl_Flag2==0)&&((PT2272_Data0==1)&&(PT2272_Data1==1)&&(PT2272_Data2==0)&&(PT2272_Data3==0)))
					 {Motor_CompSlowlyUp();
					 Motor_StepCounter--;
					 SignalControl_Flag2=1;
					 }

			else if((SignalControl_Flag2==0)&&((PT2272_Data0==0)&&(PT2272_Data1==1)&&(PT2272_Data2==0)&&(PT2272_Data3==0)))
					 {Motor_CompSlowlyDown();
					 Motor_StepCounter++;
					 SignalControl_Flag2=1;
					 }

if(P3_2==0){Speaker=0;}
else if(P3_2==1){Speaker=1;}


	delay1(100);
	if(SelfControl_Flag==0)break;
	
	}

}


void SelfControl_int(void) interrupt 2
{
	delay1(100);
	Check_ForOnce=0;
	if((PT2272_Data0==0)&&(PT2272_Data1==0)&&(PT2272_Data2==0)&&(PT2272_Data3==1))
			{SelfControl_Flag=0;}
	else if((PT2272_Data0==0)&&(PT2272_Data1==0)&&(PT2272_Data2==1)&&(PT2272_Data3==1))
			{SelfControl_Flag=1;}
			
			/*	
		INT1_Counter++;
		if(INT1_Counter==5)INT1_Counter=1;	
	}
if((INT1_Counter%2)==1){SelfControl_Flag=1;}
else SelfControl_Flag=0;
*/
}

/***************************************���¼���������*******************************************/

void Motor_CloseDown(void){
turn=1;
while(Motor_StepCounter<=90){
turn=1;
	Motor_CompSlowlyDown();
	Motor_StepCounter++;

if	   (Motor_StepCounter>=90){LED6_=0;LED7_=1;}
else if(Motor_StepCounter<=0){LED6_=1;LED7_=0;}
else   {LED6_=1;LED7_=1;}

}

}


/*****************************************************************************************************/

void Motor_OpenUp(void){
turn=0;
while(Motor_StepCounter>=0){
turn=0;
	Motor_CompSlowlyUp();
	Motor_StepCounter--;

if	   (Motor_StepCounter>=90){LED6_=0;LED7_=1;}
else if(Motor_StepCounter<=0){LED6_=1;LED7_=0;}
else   {LED6_=1;LED7_=1;}

}

}


/*****************************************************************************************************/

void Motor_CompSlowlyDown(void){
turn=1;
spcount=0;
    speedlevel=2;
	 for(i=0;i<=15;i++)
	 	for(j=0;j<=15;j++){HadBeenDone=0;TH0=0xFE;TL0=0x0c;TR0=1;delay(2);
		if(HadBeenDone==1){
		if(spcount<=0){spcount=speedlevel;
		gorun();}}}
		TR0=0;
}


/**************************************************************************************************/

void Motor_CompSlowlyUp(void){
turn=0;
spcount=0;
    speedlevel=2;
	 for(i=0;i<=15;i++)
	 	for(j=0;j<=15;j++){HadBeenDone=0;TH0=0xFE;TL0=0x0c;TR0=1;delay(2);
		if(HadBeenDone==1){
		if(spcount<=0){spcount=speedlevel;
		gorun();}}}
		TR0=0;
}




/*************************************************************************************/

/*************************��ʱ��1���ж�**********************/
void Time0() interrupt 3  
{
  TH1=0x94;
  TL1=0x00;
  T1_counter++;
  if(T1_counter==20)
  {
    T1_s++;
  T1_counter=0;
  if(T1_s==60)
  { 
    T1_m++;
    T1_s=0;
      if(T1_m==60)
    {
      T1_h++;
    T1_m=0;
    if(T1_h==24)
     T1_h=0;
         }
       }

  }
}


/****************��ʱ��0�жϴ���*****************************/ 
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


