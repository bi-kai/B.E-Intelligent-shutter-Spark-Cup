#include <reg51.h>
#include < intrins.h >

#define uchar unsigned char

uchar T1_h,T1_m,T1_s,T1_counter;

 
sbit LED1_=P0^6;
sbit LED4_=P0^7;
sbit LED5_=P2^7;

void delay1(int ms)//延迟函数

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

/*********************初始化子程序**********************/
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

 

 

 

/***************************主程序************************/

void main(void)
{
  T1_init();

  while(1)
  {
  if(T1_m<=1){LED5_=1;LED4_=~LED4_;delay1(40);} 
		else if(T1_m<=2){LED4_=1;LED5_=~LED5_;delay1(40);}
		else if(T1_m<=3){LED1_=~LED1_;delay1(40);}

   }
}

 

 

 


/*************************定时器0的中断**********************/
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



