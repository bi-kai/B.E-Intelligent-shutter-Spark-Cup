#include < reg51.h >
#include < intrins.h >

#define uchar unsigned char
#define uint  unsigned int

/*******************************定义全局变量********************************/
unsigned char dat=0x00 ;      //AD值

unsigned char channel;       //通道变量
//ADC0832的引脚
sbit ADCS =P3^5;  //ADC0832 chip seclect
sbit ADDI =P3^7;  //ADC0832 k in
sbit ADDO =P3^7;  //ADC0832 k out
sbit ADCLK=P3^6; //ADC0832 clock signal
sbit LED=P0^6;

unsigned char data  temp_data[2] = {0x00,0x00} ;
unsigned char data  display[5] =   {0x00,0x00,0x00,0x00,0x00} ;
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

 unsigned int Adc0832()     //AD转换，返回结果
{
    uchar i=0;
    uchar j;
    uint dat=0;
    uchar ndat=0;
    ADDI=1;
    _nop_();
    _nop_();
    ADCS=0;//拉低CS端
    _nop_();
    _nop_();
    ADCLK=1;//拉高CLK端
    _nop_();
    _nop_();
    ADCLK=0;//拉低CLK端,形成下降沿1
    _nop_();
    _nop_();
    ADCLK=1;//拉高CLK端
    ADDI=0;	//通道0
    _nop_();
    _nop_();
    ADCLK=0;//拉低CLK端,形成下降沿2
    _nop_();
    _nop_();
    ADCLK=1;//拉高CLK端
    ADDI=1;//选通道	1
    _nop_();
    _nop_();
    ADCLK=0;//拉低CLK端,形成下降沿3
    ADDI=1;//控制命令结束 
    _nop_();
    _nop_();
    dat=0;
    for(i=0;i<8;i++)
    {
        dat|=ADDO;//收数据
        ADCLK=1;
        _nop_();
        _nop_();
        ADCLK=0;//形成一次时钟脉冲
        _nop_();
        _nop_();
        dat<<=1;
        if(i==7)dat|=ADDO;
    }  
    for(i=0;i<8;i++)
    {
        j=0;
        j=j|ADDO;//收数据
        ADCLK=1;
        _nop_();
        _nop_();
        ADCLK=0;//形成一次时钟脉冲
        _nop_();
        _nop_();
        j=j<<7;
        ndat=ndat|j;
        if(i<7)ndat>>=1;
    }
    ADCS=1;//拉低CS端
    ADCLK=0;//拉低CLK端
    ADDO=1;//拉高数据端,回到初始状态
    dat<<=8;
    dat|=ndat;
    return(dat);            //return ad k
}

/* 读取温度                            */  
/*******************************************************************/     
adc_display(void) {
     temp_data[0] =  Adc0832();   //温度低8位


/* 数据转换与温度显示                                              */
/*******************************************************************/
  
  display[4]=(temp_data[0]&0x0f) ;  //换算成8位二进制
  display[0]=ditab[display[4]]+0x30 ;     //查表得小数位的值

  display[4]=((temp_data[0]&0xf0)>>4)|((temp_data[1]&0x0f)<<4) ;
  display[3]=display[4]/100+0x30 ;
  display[1]=display[4]%100 ;
  display[2]=display[1]/10+0x30 ;
  display[1]=display[1]%10+0x30 ;
    if(display[3]==0x30)        //高位为0，不显示
   {
     display[3]=0x20 ;
     if(display[2]==0x30)      //次高位为0，不显示
     display[2]=0x20 ;
   }
	  }








  void main()
  { 
	while(1) {   
	adc_display();
		if(display[1]>=0x37){LED=~LED; delay1(100);}
else LED=0;
	 }
	}



