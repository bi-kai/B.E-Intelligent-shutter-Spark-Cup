#include <reg51.h>
#include < intrins.h >

#define uchar unsigned char

uchar dispbuf[8],h,m,s,counter;

 
sbit LED1_=P0^6;
sbit LED4_=P0^7;
sbit LED5_=P2^7;

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

/*********************��ʼ���ӳ���**********************/
void T0_init()
{

  TMOD=0x01;
  TH0=0x94;
  TL0=0x00;
  EA=1;
  ET0=1;
  TR0=1;
  counter=0;
  h=12;m=0;s=0;

}

 

 

 

/***************************������************************/

void main(void)
{ 
  T0_init();

  while(1)
  {
  if(m<=1){LED5_=1;LED4_=~LED4_;delay1(40);} 
		else if(m<=2){LED4_=1;LED5_=~LED5_;delay1(40);}
		else if(m<=3){LED1_=~LED1_;delay1(40);}

   }
}

 

 

 


/*************************��ʱ��0���ж�**********************/
void Time0() interrupt 1   //�ٴ�ǿ���ж��ӳ���ִ��ʱ��Խ��Խ��
{
  TH0=0x94;
  TL0=0x00;
  counter++;
  if(counter==20)
  {
    s++;
  counter=0;
  if(s==60)
  { 
    m++;
    s=0;
      if(m==60)
    {
      h++;
    m=0;
    if(h==24)
     h=0;
         }
       }

  }
}



