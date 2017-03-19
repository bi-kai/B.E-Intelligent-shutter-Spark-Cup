#include < REG51.h >
#include < intrins.h >

#define uchar unsigned char
#define uint  unsigned int
 uint temp;
bit presence;	
/*******************************����ȫ�ֱ���********************************/
unsigned char dat=0x00 ;      //ADֵ
unsigned char count = 0x00;      //��ʱ������
unsigned char channel;       //ͨ������
//ADC0832������
sbit ADCS =P1^4;  //ADC0832 chip seclect
sbit ADDI =P1^6;  //ADC0832 k in
sbit ADDO =P1^6;  //ADC0832 k out
sbit ADCLK=P1^5; //ADC0832 clock signal

sbit LCD_RS = P2^2 ;
sbit LCD_RW = P2^3 ;
sbit LCD_EN = P2^4 ;


unsigned char data  temp_data[2] = {0x00,0x00} ;
unsigned char data  display[5] =   {0x00,0x00,0x00,0x00,0x00} ;
unsigned char code  ditab[16] =    {0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x04,
                                                        0x05,0x06,0x06,0x07,0x08,0x08,0x09,0x09} ;///*С��λ����������*/
//
void beep() ;
unsigned char code  mytab[8] = {0x0C,0x12,0x12,0x0C,0x00,0x00,0x00,0x00} ;

#define delayNOP() ; {_nop_() ;_nop_() ;_nop_() ;_nop_() ;} ;

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
bit lcd_busy()//��æ���
 {
    bit result ;
    LCD_RS = 0 ;
    LCD_RW = 1 ;
    LCD_EN = 1 ;
    delayNOP() ;
    result = (bit)(P0&0x80) ;
    LCD_EN = 0 ;
    return(result) ;
 }
void lcd_wcmd(uchar cmd)//ָ���������
{
   while(lcd_busy()) ;
    LCD_RS = 0 ;
    LCD_RW = 0 ;
    LCD_EN = 0 ;
    _nop_() ;
    _nop_() ;
    P0 = cmd ;
    delayNOP() ;
    LCD_EN = 1 ;
    delayNOP() ;
    LCD_EN = 0 ;
}
void lcd_wdat(uchar dat)//�����������
{
   while(lcd_busy()) ;
    LCD_RS = 1 ;
    LCD_RW = 0 ;
    LCD_EN = 0 ;
    P0 = dat ;
    delayNOP() ;
    LCD_EN = 1 ;
    delayNOP() ;
    LCD_EN = 0 ;
}


/*  LCD��ʼ���趨                                                  */
/*******************************************************************/
void lcd_init()//lcd��ʼ����䣬�����ȫ������
{
    delay1(15) ;
    lcd_wcmd(0x01) ;      //���LCD����ʾ����
    lcd_wcmd(0x38) ;      //16*2��ʾ��5*7����8λ����
    delay1(5) ;
    lcd_wcmd(0x38) ;
    delay1(5) ;
    lcd_wcmd(0x38) ;
    delay1(5) ;

    lcd_wcmd(0x0c) ;      //��ʾ�����ع��
    delay1(5) ;
    lcd_wcmd(0x06) ;      //�ƶ����
    delay1(5) ;
    lcd_wcmd(0x01) ;      //���LCD����ʾ����
    delay1(5) ;
}

/*  �趨��ʾλ��                                    */
/*******************************************************************/
void lcd_pos(uchar pos)//���ַ����趨
{
  lcd_wcmd(pos | 0x80) ;  //����ָ��=80+��ַ����
}

/******************************************************************
void Delay(unsigned int num)
{
  while( --num ) ;
}
********/
 unsigned int Adc0832()     //ADת�������ؽ��
{
    uchar i=0;
    uchar j;
    uint dat=0;
    uchar ndat=0;
    ADDI=1;
    _nop_();
    _nop_();
    ADCS=0;//����CS��
    _nop_();
    _nop_();
    ADCLK=1;//����CLK��
    _nop_();
    _nop_();
    ADCLK=0;//����CLK��,�γ��½���1
    _nop_();
    _nop_();
    ADCLK=1;//����CLK��
    ADDI=0;	//ͨ��0
    _nop_();
    _nop_();
    ADCLK=0;//����CLK��,�γ��½���2
    _nop_();
    _nop_();
    ADCLK=1;//����CLK��
    ADDI=1;//ѡͨ��	1
    _nop_();
    _nop_();
    ADCLK=0;//����CLK��,�γ��½���3
    ADDI=1;//����������� 
    _nop_();
    _nop_();
    dat=0;
    for(i=0;i<8;i++)
    {
        dat|=ADDO;//������
        ADCLK=1;
        _nop_();
        _nop_();
        ADCLK=0;//�γ�һ��ʱ������
        _nop_();
        _nop_();
        dat<<=1;
        if(i==7)dat|=ADDO;
    }  
    for(i=0;i<8;i++)
    {
        j=0;
        j=j|ADDO;//������
        ADCLK=1;
        _nop_();
        _nop_();
        ADCLK=0;//�γ�һ��ʱ������
        _nop_();
        _nop_();
        j=j<<7;
        ndat=ndat|j;
        if(i<7)ndat>>=1;
    }
    ADCS=1;//����CS��
    ADCLK=0;//����CLK��
    ADDO=1;//�������ݶ�,�ص���ʼ״̬
    dat<<=8;
    dat|=ndat;
    return(dat);            //return ad k
}

/* ��ȡ�¶�                            */  
/*******************************************************************/     
adc_display(void) {
     temp_data[0] =  Adc0832();   //�¶ȵ�8λ


/* ����ת�����¶���ʾ                                              */
/*******************************************************************/
  
  display[4]=(temp_data[0]&0x0f) ;  //�����8λ������
  display[0]=ditab[display[4]]+0x30 ;     //����С��λ��ֵ

  display[4]=((temp_data[0]&0xf0)>>4)|((temp_data[1]&0x0f)<<4) ;
  display[3]=display[4]/100+0x30 ;
  display[1]=display[4]%100 ;
  display[2]=display[1]/10+0x30 ;
  display[1]=display[1]%10+0x30 ;
    if(display[3]==0x30)        //��λΪ0������ʾ
   {
     display[3]=0x20 ;
     if(display[2]==0x30)      //�θ�λΪ0������ʾ
     display[2]=0x20 ;
   }

     lcd_pos(0x48) ;
     lcd_wdat(display[3]) ;       //��λ����ʾ
     lcd_pos(0x49) ;
     lcd_wdat(display[2]) ;        //ʮλ����ʾ
  lcd_pos(0x4a) ;
     lcd_wdat(display[1]) ;        //��λ����ʾ
  lcd_pos(0x4c) ;
     lcd_wdat(display[0]) ;        //С��λ����ʾ
}
  void main()
  {

    lcd_init() ;  
	while(1) {   
	adc_display();
	 }
	}