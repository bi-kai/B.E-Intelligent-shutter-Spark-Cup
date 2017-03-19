#include <reg51.h>
#define uchar unsigned char

sbit LCD_RW=P2^3;
sbit LCD_RS=P2^2;
sbit LCD_EN=P2^4;
bit at=0;
//sbit busy_bit=P1^7;
uchar code PrintString[]={"CLOCK!"};
uchar code word[]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x20};
uchar dispbuf[8],h,m,s,counter;

 

void delay()
{
  uchar i;
 for(i=0;i<255;i++);
}

 

/*******д����**********/
void lcd_wmc(uchar i)
{

  P0=i;
  LCD_RS=0;
  LCD_RW=0;
  LCD_EN=0;
  delay();
   LCD_EN=1;
}

 

/*******д����***********/

void lcd_wmd(uchar i)
{

  P0=i;
   LCD_RS=1;
   LCD_RW=0;
   LCD_EN=0;
  delay();
   LCD_EN=1;
}

 


/*******��ʼ��Һ��*******/
void lcd_init()
{
 
  lcd_wmc(0x01);
  lcd_wmc(0x38);
  lcd_wmc(0x0c);
  lcd_wmc(0x06);
 
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
   lcd_wmc(dispadd);
   tmp=dispbuf[7];
   tmp=word[tmp];
   lcd_wmd(tmp);
   tmp=dispbuf[6];
   tmp=word[tmp];
   lcd_wmd(tmp);
   tmp=dispbuf[5];
   tmp=word[tmp];
   lcd_wmd(tmp);
   tmp=dispbuf[4];
   tmp=word[tmp];
   lcd_wmd(tmp);
   tmp=dispbuf[3];
   tmp=word[tmp];
   lcd_wmd(tmp);
   tmp=dispbuf[2];
   tmp=word[tmp];
   lcd_wmd(tmp);
   tmp=dispbuf[1];
   tmp=word[tmp];
   lcd_wmd(tmp);
   tmp=dispbuf[0];
   tmp=word[tmp];
   lcd_wmd(tmp); 
}

 

 

/*********************�����ӳ���***********************/

uchar keypro(void)
{
  uchar scanl,scanh;
  P1=0xf0;
  if((P1&0xf0)!=0xf0)
  {
    delay();
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
   case 0x88: if(!at){add=0xc1;EA=0;lcd_wmc(0xc1);lcd_wmc(0x0f);at=1;}
               else {lcd_wmc(0xc0);lcd_wmc(0x0c);at=0;EA=1;} break;

   case 0x48:  if(at)
             {
       if(add==0xc1){add=0xc7; lcd_wmc(add);}
                 else {add=add-3;lcd_wmc(add);}
               }
         break;
  
  case 0x28:  if(at)
             {
               if(add==0xc7) {add=0xc1;lcd_wmc(add);}
                 else {add=add+3;lcd_wmc(add);}
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
      lcd_wmc(add);
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

 

 

 

/*********************��ʼ���ӳ���**********************/
void init()
{
  uchar i;
  TMOD=0x01;
  TH0=0x4c;
  TL0=0x00;
  EA=1;
  ET0=1;
  TR0=1;
  counter=0;
  h=12;m=0;s=0;
  dispbuf[2]=10;
  dispbuf[5]=10;
   lcd_wmc(0xc9);
  for(i=0;i<6;i++)
  lcd_wmd(PrintString[i]);
  lcd_wmc(0xc0);
}

 

 

 

/***************************������************************/

void main(void)
{
  lcd_init();
  init();

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



