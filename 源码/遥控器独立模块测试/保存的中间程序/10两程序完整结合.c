#include < reg51.h >
#include < intrins.h >

#define uchar unsigned char
#define uint  unsigned int

sbit DQ = P1^2 ;  //����DS18B20�˿�DQ
sbit BEEP=P1^3  ; //������������

uchar Menu_INT1=1;
uchar count=0;

bit presence;
bit at=0;

sbit LCD_RS = P2^2 ;
sbit LCD_RW = P2^3 ;
sbit LCD_EN = P2^4 ;

uchar code  cdis1[ ] = {"Position:model1."};
uchar code  cdis2[ ] = {" WENDU:    .  C "};
uchar code  cdis3[ ] = {" DS18B20  ERR0R "};
uchar code  cdis4[ ] = {"  PLEASE CHECK  "};
uchar code  cdis5[ ] = {"Position:model2."};




uchar code PrintString[]={"CLOCK!"};
uchar code word[]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x20};
uchar dispbuf[8],h,m,s,counter;









unsigned char data  temp_data[2] = {0x00,0x00} ;
unsigned char data  display[5] =   {0x00,0x00,0x00,0x00,0x00} ;
unsigned char code  ditab[16] =    {0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x04,
                                                        0x05,0x06,0x06,0x07,0x08,0x08,0x09,0x09} ;///*С��λ����������*/
//
void beep() ;
unsigned char code  mytab[8] = {0x0C,0x12,0x12,0x0C,0x00,0x00,0x00,0x00} ;

#define delayNOP() ; {_nop_() ;_nop_() ;_nop_() ;_nop_() ;} ;

/*******************************************************************/
void delay1(int ms)
{
 uchar y;
  while(ms--)
 {
  for(y = 0 ; y<250 ; y++)
  {
   _nop_();
   _nop_();
   _nop_();
   _nop_();
  }
 }
}

/******************************************************************/
/*���LCDæ״̬                                                   */
/*lcd_busyΪ1ʱ��æ���ȴ���lcd-busyΪ0ʱ,�У���дָ�������ݡ�   */
/******************************************************************/
bit lcd_busy()
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

/*дָ�����ݵ�LCD                                                  */
/*RS=L��RW=L��E=�����壬D0-D7=ָ���롣          */
/*******************************************************************/
void lcd_wcmd(uchar cmd)
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

/*******************************************************************/
/*д��ʾ���ݵ�LCD                            */
/*RS=H��RW=L��E=�����壬D0-D7=���ݡ�       */
/*******************************************************************/
void lcd_wdat(uchar dat)
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
void lcd_init()
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
void lcd_pos(uchar pos)
{
  lcd_wcmd(pos | 0x80) ;  //����ָ��=80+��ַ����
}

/*�Զ����ַ�д��CGRAM                                   */
/*******************************************************************/
void  writetab()
{
    unsigned char i ;
    lcd_wcmd(0x40) ;            //дCGRAM
    for (i = 0 ; i< 8 ; i++)
    lcd_wdat(mytab[ i ]) ;
}

/*us����ʱ����                                        */
/*******************************************************************/

void Delay(unsigned int num)
{
  while( --num ) ;
}

/*��ʼ��ds1820                                      */
/*******************************************************************/
Init_DS18B20(void)
{
     DQ = 1 ;      //DQ��λ
     Delay(8) ;    //������ʱ

     DQ = 0 ;      //��Ƭ����DQ����
     Delay(90) ;   //��ȷ��ʱ ���� 480us

     DQ = 1 ;       //��������
     Delay(8) ;

     presence = DQ ;    //���=0���ʼ���ɹ� =1���ʼ��ʧ��
     Delay(100) ;
     DQ = 1 ;

     return(presence) ; //�����źţ�0=presence,1= no presence
}


/* ��һ���ֽ�                     */
/*******************************************************************/
 ReadOneChar(void)
{
unsigned char i = 0 ;
unsigned char dat = 0 ;

for (i = 8 ; i > 0 ; i--)
  {
    DQ = 0 ; // �������ź�
    dat >>= 1 ;
    DQ = 1 ; // �������ź�

    if(DQ)
     dat |= 0x80 ;
    Delay(4) ;
  }

    return (dat) ;
}

/* дһ���ֽ�                                         */
/*******************************************************************/
 WriteOneChar(unsigned char dat)
{
  unsigned char i = 0 ;
  for (i = 8 ; i > 0 ; i--)
  {
    DQ = 0 ;
    DQ = dat&0x01 ;
    Delay(5) ;

    DQ = 1 ;
    dat>>=1 ;
  }
}

/* ��ȡ�¶�                            */
/*******************************************************************/
 Read_Temperature(void)
{
     Init_DS18B20() ;

     WriteOneChar(0xCC) ;  // ����������кŵĲ���
     WriteOneChar(0x44) ;  // �����¶�ת��

     Init_DS18B20() ;
     WriteOneChar(0xCC) ;  //����������кŵĲ���
     WriteOneChar(0xBE) ;  //��ȡ�¶ȼĴ���

     temp_data[0] = ReadOneChar() ;   //�¶ȵ�8λ
     temp_data[1] = ReadOneChar() ;   //�¶ȸ�8λ
}

/* ����ת�����¶���ʾ                                              */
/*******************************************************************/
 Disp_Temperature()
{
  display[4]=temp_data[0]&0x0f ;
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
     lcd_wdat(display[3]) ;        //��λ����ʾ
     lcd_pos(0x49) ;
     lcd_wdat(display[2]) ;        //ʮλ����ʾ
  lcd_pos(0x4a) ;
     lcd_wdat(display[1]) ;        //��λ����ʾ
  lcd_pos(0x4c) ;
     lcd_wdat(display[0]) ;        //С��λ����ʾ
}

/*******************************************************************/
/* ��������һ��                 */
/*******************************************************************/
void beep()
  {
    unsigned char y ;
    for (y=0 ;y<100 ;y++)
    {
      Delay(60) ;
      BEEP=!BEEP ;                //BEEPȡ��
    }
    BEEP=1 ;                      //�رշ�����
 Delay(40000) ;
  }

/* DS18B20 OK ��ʾ�˵�                                             */
/*******************************************************************/
void  Ok_Menu()
{
    uchar  m ;
    lcd_init() ;                //��ʼ��LCD

    lcd_pos(0) ;                //������ʾλ��Ϊ��һ�еĵ�1���ַ�
     m = 0 ;
    while(cdis1[m] != '\0')
     {                         //��ʾ�ַ�
       lcd_wdat(cdis1[m]) ;
       m++ ;
     }

    lcd_pos(0x40) ;             //������ʾλ��Ϊ�ڶ��е�1���ַ�
     m = 0 ;
    while(cdis2[m] != '\0')
     {
       lcd_wdat(cdis2[m]) ;      //��ʾ�ַ�
       m++ ;
     }

     writetab() ;               //�Զ����ַ�д��CGRAM
     delay1(5) ;
     lcd_pos(0x4d) ;
     lcd_wdat(0x00) ;          //��ʾ�Զ����ַ�
}


/* DS18B20 ERROR ��ʾ�˵�                             */
/*******************************************************************/
void  Error_Menu ()
{
     uchar  m ;
     lcd_init() ;                //��ʼ��LCD

    lcd_pos(0) ;                //������ʾλ��Ϊ��һ�еĵ�1���ַ�
     m = 0 ;
     while(cdis3[m] != '\0')
     {                         //��ʾ�ַ�
       lcd_wdat(cdis3[m]) ;
       m++ ;
     }

     lcd_pos(0x40) ;             //������ʾλ��Ϊ�ڶ��е�1���ַ�
     m = 0 ;
     while(cdis4[m] != '\0')
     {
       lcd_wdat(cdis4[m]) ;      //��ʾ�ַ�
       m++ ;
     }
}






/*******���»������ӳ���*******/
void newbuf()
{
 dispbuf[0]=s%10;
 dispbuf[1]=s/10;
 dispbuf[3]=m%10;
 dispbuf[4]=m/10;
 dispbuf[6]=h%10;
 dispbuf[7]=h/10;
}

 

/*******��ʾ�ӳ���**********/
void disp(uchar dispadd)
{
   uchar tmp;
   lcd_wcmd(dispadd);
   tmp=dispbuf[7];
   tmp=word[tmp];
   lcd_wdat(tmp);
   tmp=dispbuf[6];
   tmp=word[tmp];
   lcd_wdat(tmp);
   tmp=dispbuf[5];
   tmp=word[tmp];
   lcd_wdat(tmp);
   tmp=dispbuf[4];
   tmp=word[tmp];
   lcd_wdat(tmp);
   tmp=dispbuf[3];
   tmp=word[tmp];
   lcd_wdat(tmp);
   tmp=dispbuf[2];
   tmp=word[tmp];
   lcd_wdat(tmp);
   tmp=dispbuf[1];
   tmp=word[tmp];
   lcd_wdat(tmp);
   tmp=dispbuf[0];
   tmp=word[tmp];
   lcd_wdat(tmp); 
}

 

 

/*********************�����ӳ���***********************/

uchar keypro(void)
{
  uchar scanl,scanh;
  P1=0xf0;
  if((P1&0xf0)!=0xf0)
  {
    delay1(1);
    if((P1&0xf0)!=0xf0)
    {
      scanl=0xfe;
      while((scanl&0x10)!=0)
      {
        P1=scanl;
        if((P1&0xf0)!=0xf0)
        {
          scanh=(P1&0xf0)|0x0f;
          return (~scanh)+(~scanl);
         }
        else scanl=(scanl<<1)|0x01;
      }
    } 
  } 
  return 0;
}


/********************ʱ������ӳ���********************/
void adjustime()
{
  uchar k;
  static uchar add;
  k=keypro();
  switch(k)
  {
   case 0x88: if(!at){add=0xc1;EA=0;lcd_wcmd(0xc1);lcd_wcmd

(0x0f);at=1;}
               else {lcd_wcmd(0xc0);lcd_wcmd

(0x0c);at=0;EA=1;} break;

   case 0x48:  if(at)
             {
       if(add==0xc1){add=0xc7; lcd_wcmd(add);}
                 else {add=add-3;lcd_wcmd(add);}
               }
         break;
  
  case 0x28:  if(at)
             {
               if(add==0xc7) {add=0xc1;lcd_wcmd(add);}
                 else {add=add+3;lcd_wcmd(add);}
           }
       break;

   case 0x18: if(at)
            {
              if(add==0xc1) h++;
                if(h==24)
          h=0;
                if(add==0xc4) m++;
         if(m==60)
           m=0;
                if(add==0xc7) s++;
          if(s==60)
           s=0;
           newbuf();
      disp(0xc0);
      lcd_wcmd(add);
              }
               break;
  default: break;
  }
  if(k!=0)
   {
  while((P1&0xf0)!=0xf0)
  P1=0xf0;
 }

}

void Self_BK_Define2(void){
	 uchar  m ;
    lcd_pos(0) ;                //������ʾλ��Ϊ��һ�еĵ�1���ַ�
     m = 0 ;
    while(cdis1[m] != '\0')
     {                         //��ʾ�ַ�
       lcd_wdat(cdis1[m]) ;
       m++ ;
     }

    lcd_pos(0x40) ;             //������ʾλ��Ϊ�ڶ��е�1���ַ�
     m = 0 ;
    while(cdis2[m] != '\0')
     {
       lcd_wdat(cdis2[m]) ;      //��ʾ�ַ�
       m++ ;
     }

     writetab() ;               //�Զ����ַ�д��CGRAM
     delay1(5) ;
     lcd_pos(0x4d) ;
     lcd_wdat(0x00) ;          //��ʾ�Զ����ַ�
}

void Self_BK_Define1(void){
  uchar i=0;
  lcd_pos(0);
  while(cdis5[i]!='\0'){
  lcd_wdat(cdis5[i]);
  i++;
  }
  lcd_wcmd(0xc9);
  for(i=0;i<6;i++)
  lcd_wdat(PrintString[i]);
  lcd_wcmd(0xc0);
}

/*********************��ʼ���ӳ���**********************/
void init()
{
  TMOD=0x01;
  TH0=0x4c;
  TL0=0x00;
  EX1=1;
  IT1=1;
  EA=1;
  ET0=1;
  TR0=1;
  counter=0;
  h=12;m=0;s=0;
  dispbuf[2]=10;
  dispbuf[5]=10;
   }

void MainModel2(void){
 Self_BK_Define1();
  while(1)
  {
   adjustime();
   if(!at)
 {
     //��˸
      if(counter<10)        
      {
       dispbuf[2]=10;
       dispbuf[5]=10;
      }
      else
      {            
       dispbuf[2]=11;
       dispbuf[5]=11;
      }
      //������ʾ��������������ʾ����
      if(counter==0)
      {
       newbuf();
       disp(0xc0);
       }
      else if(counter==10)
      disp(0xc0);  
    }
	 if((((Menu_INT1)%3)==0)||(((Menu_INT1)%3)==2))break;
   }
} 

void dely500(void){

	uchar i;
	for(i=250;i>0;i--){_nop_();
		
}
}


void MainModel1(void){
 
while(1){
Self_BK_Define2();
 do
  {
      Read_Temperature() ;
   Disp_Temperature() ;

		if(display[2]>=51){
		for(count=120;count>0;count--){
			BEEP=~BEEP;
			dely500();
					}
		for(count=120;count>0;count--){
			
			BEEP=~BEEP;
			dely500();
			dely500();
		}			
	
	}
	if((((Menu_INT1)%3)==1)||(((Menu_INT1)%3)==2))break;
     }
    while(!presence);

     Error_Menu();

 do
  {
    Init_DS18B20();
    beep();
     }
 while(presence);
 
 if((((Menu_INT1)%3)==1)||(((Menu_INT1)%3)==2))break;
}
}


/* ������     */
/************************************/
void main()
 {
 	Ok_Menu();
	init();


	while(1){
	if(((Menu_INT1)%3)==0){
   	MainModel1(); }
	else /*(((Menu_INT1)%3)==1)*/
	{
		MainModel2(); }






}
}









		
/*************************��ʱ��0���ж�**********************/
void Time0() interrupt 1 using 2    //�ٴ�ǿ���ж��ӳ���ִ��ʱ��Խ��Խ��
{
  TH0=(65536-46075)/256;
  TL0=(65536-46075)%256;
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


/*�ⲿ�ж�*/
void int2_Menu_INT1(void) interrupt 2
	{
		Menu_INT1++;
		
		if(Menu_INT1==9)Menu_INT1=0;
	
	}
