#include < reg51.h >
#include < intrins.h >

#define uchar unsigned char
#define uint  unsigned int

/*******************************定义全局变量********************************/
unsigned char dat=0x00 ;      //AD值

unsigned char channel;       //通道变量
//ADC0832的引脚
sbit AD1_CS =P1^4;  //ADC0832 chip seclect
sbit AD1_DI =P1^6;  //ADC0832 k in
sbit AD1_DO =P1^6;  //ADC0832 k out
sbit AD1_CLK=P1^5; //ADC0832 clock signal
sbit LED1_=P0^4;

unsigned char data  temp_data1_[2] = {0x00,0x00} ;
unsigned char data  display1_[5] =   {0x00,0x00,0x00,0x00,0x00} ;
unsigned char code  ditab[16] =    {0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x04,
                                                        0x05,0x06,0x06,0x07,0x08,0x08,0x09,0x09} ;/*小数位表，四舍五入*/

void beep() ;
/*******************************************************************/
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

 unsigned int Adc1_0832()     //AD转换，返回结果
{
    uchar i=0;
    uchar j;
    uint dat=0;
    uchar ndat=0;
    AD1_DI=1;
    _nop_();
    _nop_();
    AD1_CS=0;//拉低CS端
    _nop_();
    _nop_();
    AD1_CLK=1;//拉高CLK端
    _nop_();
    _nop_();
    AD1_CLK=0;//拉低CLK端,形成下降沿1
    _nop_();
    _nop_();
    AD1_CLK=1;//拉高CLK端
    AD1_DI=0;	//通道0
    _nop_();
    _nop_();
    AD1_CLK=0;//拉低CLK端,形成下降沿2
    _nop_();
    _nop_();
    AD1_CLK=1;//拉高CLK端
    AD1_DI=1;//选通道	1
    _nop_();
    _nop_();
    AD1_CLK=0;//拉低CLK端,形成下降沿3
    AD1_DI=1;//控制命令结束 
    _nop_();
    _nop_();
    dat=0;
    for(i=0;i<8;i++)
    {
        dat|=AD1_DO;//收数据
        AD1_CLK=1;
        _nop_();
        _nop_();
        AD1_CLK=0;//形成一次时钟脉冲
        _nop_();
        _nop_();
        dat<<=1;
        if(i==7)dat|=AD1_DO;
    }  
    for(i=0;i<8;i++)
    {
        j=0;
        j=j|AD1_DO;//收数据
        AD1_CLK=1;
        _nop_();
        _nop_();
        AD1_CLK=0;//形成一次时钟脉冲
        _nop_();
        _nop_();
        j=j<<7;
        ndat=ndat|j;
        if(i<7)ndat>>=1;
    }
    AD1_CS=1;//拉低CS端
    AD1_CLK=0;//拉低CLK端
    AD1_DO=1;//拉高数据端,回到初始状态
    dat<<=8;
    dat|=ndat;
    return(dat);            //return ad k
}

/* 读取温度                            */  
/*******************************************************************/     
adc1_display(void) {
     temp_data1_[0] =  Adc1_0832();   //温度低8位


/* 数据转换与温度显示                                              */
/*******************************************************************/
  
  display1_[4]=(temp_data1_[0]&0x0f) ;  //换算成8位二进制
  display1_[0]=ditab[display1_[4]]+0x30 ;     //查表得小数位的值

  display1_[4]=((temp_data1_[0]&0xf0)>>4)|((temp_data1_[1]&0x0f)<<4) ;
  display1_[3]=display1_[4]/100+0x30 ;
  display1_[1]=display1_[4]%100 ;
  display1_[2]=display1_[1]/10+0x30 ;
  display1_[1]=display1_[1]%10+0x30 ;
    if(display1_[3]==0x30)        //高位为0，不显示
   {
     display1_[3]=0x20 ;
     if(display1_[2]==0x30)      //次高位为0，不显示
     display1_[2]=0x20 ;
   }
	  }








  void main()
  { 
	while(1) {   
	adc1_display();
		if(display1_[1]>=0x37){LED1_=~LED1_; delay1(100);}
else LED1_=0;
	 }
	}




