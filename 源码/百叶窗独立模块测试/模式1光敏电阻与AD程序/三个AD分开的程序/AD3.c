#include < reg51.h >
#include < intrins.h >

#define uchar unsigned char
#define uint  unsigned int

/*******************************����ȫ�ֱ���********************************/
//ADC0832������
sbit AD3_CS =P3^0;  //ADC0832 chip seclect
sbit AD3_DI =P3^4;  //ADC0832 k in
sbit AD3_DO =P3^4;  //ADC0832 k out
sbit AD3_CLK=P3^1; //ADC0832 clock signal
sbit LED3_=P0^5;

unsigned char data  temp_data3_[2] = {0x00,0x00} ;
unsigned char data  display3_[5] =   {0x00,0x00,0x00,0x00,0x00} ;
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

 unsigned int Adc3_0832()     //ADת�������ؽ��
{
    uchar i=0;
    uchar j;
    uint dat=0;
    uchar ndat=0;
    AD3_DI=1;
    _nop_();
    _nop_();
    AD3_CS=0;//����CS��
    _nop_();
    _nop_();
    AD3_CLK=1;//����CLK��
    _nop_();
    _nop_();
    AD3_CLK=0;//����CLK��,�γ��½���1
    _nop_();
    _nop_();
    AD3_CLK=1;//����CLK��
    AD3_DI=0;	//ͨ��0
    _nop_();
    _nop_();
    AD3_CLK=0;//����CLK��,�γ��½���2
    _nop_();
    _nop_();
    AD3_CLK=1;//����CLK��
    AD3_DI=1;//ѡͨ��	1
    _nop_();
    _nop_();
    AD3_CLK=0;//����CLK��,�γ��½���3
    AD3_DI=1;//����������� 
    _nop_();
    _nop_();
    dat=0;
    for(i=0;i<8;i++)
    {
        dat|=AD3_DO;//������
        AD3_CLK=1;
        _nop_();
        _nop_();
        AD3_CLK=0;//�γ�һ��ʱ������
        _nop_();
        _nop_();
        dat<<=1;
        if(i==7)dat|=AD3_DO;
    }  
    for(i=0;i<8;i++)
    {
        j=0;
        j=j|AD3_DO;//������
        AD3_CLK=1;
        _nop_();
        _nop_();
        AD3_CLK=0;//�γ�һ��ʱ������
        _nop_();
        _nop_();
        j=j<<7;
        ndat=ndat|j;
        if(i<7)ndat>>=1;
    }
    AD3_CS=1;//����CS��
    AD3_CLK=0;//����CLK��
    AD3_DO=1;//�������ݶ�,�ص���ʼ״̬
    dat<<=8;
    dat|=ndat;
    return(dat);            //return ad k
}

/* ��ȡ�¶�                            */  
/*******************************************************************/     
adc3_display(void) {
     temp_data3_[0] =  Adc3_0832();   //�¶ȵ�8λ


/* ����ת�����¶���ʾ                                              */
/*******************************************************************/
  
  display3_[4]=(temp_data3_[0]&0x0f) ;  //�����8λ������
  display3_[0]=ditab[display3_[4]]+0x30 ;     //����С��λ��ֵ

  display3_[4]=((temp_data3_[0]&0xf0)>>4)|((temp_data3_[1]&0x0f)<<4) ;
  display3_[3]=display3_[4]/100+0x30 ;
  display3_[1]=display3_[4]%100 ;
  display3_[2]=display3_[1]/10+0x30 ;
  display3_[1]=display3_[1]%10+0x30 ;
    if(display3_[3]==0x30)        //��λΪ0������ʾ
   {
     display3_[3]=0x20 ;
     if(display3_[2]==0x30)      //�θ�λΪ0������ʾ
     display3_[2]=0x20 ;
   }
	  }








  void main()
  { 
	while(1) {   
	adc3_display();
		if(display3_[1]>=0x37){LED3_=~LED3_; delay1(100);}
else LED3_=0;
	 }
	}



