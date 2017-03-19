#include < reg51.h >
#include < intrins.h >

#define uchar unsigned char
#define uint  unsigned int

//ADC0832的引脚
sbit AD1_CS =P3^5;  //ADC0832 chip seclect
sbit AD1_DI =P3^7;  //ADC0832 k in
sbit AD1_DO =P3^7;  //ADC0832 k out
sbit AD1_CLK=P3^6; //ADC0832 clock signal
sbit LED1_=P0^6;
sbit LED4_=P0^7;
sbit LED5_=P2^7;
sbit AD2_CS =P3^0;  //ADC0832 chip seclect
sbit AD2_DI =P3^4;  //ADC0832 k in
sbit AD2_DO =P3^4;  //ADC0832 k out
sbit AD2_CLK=P3^1; //ADC0832 clock signal
sbit LED2_=P0^5;
//ADC0832的引脚
sbit AD3_CS =P1^4;  //ADC0832 chip seclect
sbit AD3_DI =P1^6;  //ADC0832 k in
sbit AD3_DO =P1^6;  //ADC0832 k out
sbit AD3_CLK=P1^5; //ADC0832 clock signal
sbit LED3_=P0^4;

uchar meanvalue;

unsigned char data  temp_data1_[2] = {0x00,0x00} ;
unsigned char data  display1_[5] =   {0x00,0x00,0x00,0x00,0x00} ;
unsigned char data  temp_data2_[2] = {0x00,0x00} ;
unsigned char data  display2_[5] =   {0x00,0x00,0x00,0x00,0x00} ;
unsigned char data  temp_data3_[2] = {0x00,0x00} ;
unsigned char data  display3_[5] =   {0x00,0x00,0x00,0x00,0x00} ;

unsigned char code  ditab[16] =    {0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x04,
                                                        0x05,0x06,0x06,0x07,0x08,0x08,0x09,0x09} ;/*小数位表，四舍五入*/
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

 unsigned int Adc1_0832()     //AD转换,返回结果
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
/*********************加入部分1***********************************************/
 unsigned int Adc2_0832()     //AD转换，返回结果
{
    uchar i=0;
    uchar j;
    uint dat=0;
    uchar ndat=0;
    AD2_DI=1;
    _nop_();
    _nop_();
    AD2_CS=0;//拉低CS端
    _nop_();
    _nop_();
    AD2_CLK=1;//拉高CLK端
    _nop_();
    _nop_();
    AD2_CLK=0;//拉低CLK端,形成下降沿1
    _nop_();
    _nop_();
    AD2_CLK=1;//拉高CLK端
    AD2_DI=0;	//通道0
    _nop_();
    _nop_();
    AD2_CLK=0;//拉低CLK端,形成下降沿2
    _nop_();
    _nop_();
    AD2_CLK=1;//拉高CLK端
    AD2_DI=1;//选通道	1
    _nop_();
    _nop_();
    AD2_CLK=0;//拉低CLK端,形成下降沿3
    AD2_DI=1;//控制命令结束 
    _nop_();
    _nop_();
    dat=0;
    for(i=0;i<8;i++)
    {
        dat|=AD2_DO;//收数据
        AD2_CLK=1;
        _nop_();
        _nop_();
        AD2_CLK=0;//形成一次时钟脉冲
        _nop_();
        _nop_();
        dat<<=1;
        if(i==7)dat|=AD2_DO;
    }  
    for(i=0;i<8;i++)
    {
        j=0;
        j=j|AD2_DO;//收数据
        AD2_CLK=1;
        _nop_();
        _nop_();
        AD2_CLK=0;//形成一次时钟脉冲
        _nop_();
        _nop_();
        j=j<<7;
        ndat=ndat|j;
        if(i<7)ndat>>=1;
    }
    AD2_CS=1;//拉低CS端
    AD2_CLK=0;//拉低CLK端
    AD2_DO=1;//拉高数据端,回到初始状态
    dat<<=8;
    dat|=ndat;
    return(dat);            //return ad k
}
  
/************************加入部分2*******************************************/
 unsigned int Adc3_0832()     //AD转换，返回结果
{
    uchar i=0;
    uchar j;
    uint dat=0;
    uchar ndat=0;
    AD3_DI=1;
    _nop_();
    _nop_();
    AD3_CS=0;//拉低CS端
    _nop_();
    _nop_();
    AD3_CLK=1;//拉高CLK端
    _nop_();
    _nop_();
    AD3_CLK=0;//拉低CLK端,形成下降沿1
    _nop_();
    _nop_();
    AD3_CLK=1;//拉高CLK端
    AD3_DI=0;	//通道0
    _nop_();
    _nop_();
    AD3_CLK=0;//拉低CLK端,形成下降沿2
    _nop_();
    _nop_();
    AD3_CLK=1;//拉高CLK端
    AD3_DI=1;//选通道	1
    _nop_();
    _nop_();
    AD3_CLK=0;//拉低CLK端,形成下降沿3
    AD3_DI=1;//控制命令结束 
    _nop_();
    _nop_();
    dat=0;
    for(i=0;i<8;i++)
    {
        dat|=AD3_DO;//收数据
        AD3_CLK=1;
        _nop_();
        _nop_();
        AD3_CLK=0;//形成一次时钟脉冲
        _nop_();
        _nop_();
        dat<<=1;
        if(i==7)dat|=AD3_DO;
    }  
    for(i=0;i<8;i++)
    {
        j=0;
        j=j|AD3_DO;//收数据
        AD3_CLK=1;
        _nop_();
        _nop_();
        AD3_CLK=0;//形成一次时钟脉冲
        _nop_();
        _nop_();
        j=j<<7;
        ndat=ndat|j;
        if(i<7)ndat>>=1;
    }
    AD3_CS=1;//拉低CS端
    AD3_CLK=0;//拉低CLK端
    AD3_DO=1;//拉高数据端,回到初始状态
    dat<<=8;
    dat|=ndat;
    return(dat);            //return ad k
}


/**************************************************************/
     
adc1_display(void) {

	LED1_=~LED1_;delay1(50);LED1_=~LED1_;delay1(50);LED1_=~LED1_;
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

/**********************************************************************/
adc2_display(void) {

	LED2_=~LED2_;delay1(50);LED2_=~LED2_;delay1(50);LED2_=~LED2_;
     temp_data2_[0] =  Adc2_0832();


/* 数据转换与温度显示                                              */
/*******************************************************************/
  
  display2_[4]=(temp_data2_[0]&0x0f) ;  //换算成8位二进制
  display2_[0]=ditab[display2_[4]]+0x30 ;     //查表得小数位的值

  display2_[4]=((temp_data2_[0]&0xf0)>>4)|((temp_data2_[1]&0x0f)<<4) ;
  display2_[3]=display2_[4]/100+0x30 ;
  display2_[1]=display2_[4]%100 ;
  display2_[2]=display2_[1]/10+0x30 ;
  display2_[1]=display2_[1]%10+0x30 ;
    if(display2_[3]==0x30)        //高位为0，不显示
   {
     display2_[3]=0x20 ;
     if(display2_[2]==0x30)      //次高位为0，不显示
     display2_[2]=0x20 ;
   }
	  }

/*********************************************************************/
adc3_display(void) {

	LED3_=~LED3_;delay1(50);LED3_=~LED3_;delay1(50);LED3_=~LED3_;
     temp_data3_[0] =  Adc3_0832();   //温度低8位


/* 数据转换与温度显示                                              */
/*******************************************************************/
  
  display3_[4]=(temp_data3_[0]&0x0f) ;  //换算成8位二进制
  display3_[0]=ditab[display3_[4]]+0x30 ;     //查表得小数位的值

  display3_[4]=((temp_data3_[0]&0xf0)>>4)|((temp_data3_[1]&0x0f)<<4) ;
  display3_[3]=display3_[4]/100+0x30 ;
  display3_[1]=display3_[4]%100 ;
  display3_[2]=display3_[1]/10+0x30 ;
  display3_[1]=display3_[1]%10+0x30 ;
    if(display3_[3]==0x30)        //高位为0，不显示
   {
     display3_[3]=0x20 ;
     if(display3_[2]==0x30)      //次高位为0，不显示
     display3_[2]=0x20 ;
   }
	  }



/********************************************************************/






  void main()
  { 
	while(1) { 
	adc1_display();
	adc2_display();
	adc3_display();


	meanvalue=((display1_[1]+display2_[1]+display3_[1])/3);
	if(meanvalue>=0x37){LED5_=1;LED4_=~LED4_;delay1(100);}
else if(meanvalue<=0x33){LED4_=1;LED5_=~LED5_;delay1(100);}
else {LED4_=0;LED5_=0;delay1(100);}

}
	}




