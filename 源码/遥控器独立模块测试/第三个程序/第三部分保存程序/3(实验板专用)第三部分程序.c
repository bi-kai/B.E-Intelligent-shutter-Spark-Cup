/*在原来纯第三部分程序上加了两个部分，用以完成第三模块的测试*/
#include<reg51.h>
#include<intrins.h>

#define uchar unsigned char
#define uint  unsigned int


sbit PT2262_Data0=P1^0;
sbit PT2262_Data1=P1^1;
sbit PT2262_Data2=P1^2;
sbit PT2262_Data3=P1^5;
sbit PT2262_TE=P1^6;		/*模式三中要置为一*/
sbit System=P2^0;
sbit MoveUp=P2^1;
sbit MoveDown=P2^5;
sbit SelfAdd=P2^6;

uchar key=0;
uchar Button_Command;
sbit Button_Selfcontrol=P2^7;

uchar code Function0[ ]={" Happy everyday "};
uchar code Function1[ ]={"    Open UP    "};
uchar code Function2[ ]={"   Close Down   "};
uchar code Function3[ ]={"   Slowly Up   "};
uchar code Function4[ ]={"   Slowly Down  "};
uchar code Function5[ ]={"  Wrong Command "};
uchar code Function6[ ]={"  Self Control  "};


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

/*********************************/
/*********************键盘子程序***********************/

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
          return ((~scanh)+(~scanl));
         }
        else scanl=(scanl<<1)|0x01;
      }
    } 
  } 
  return 0;
}


/*********************************************************************************************/
/*中间为lcd加入部分程序*//*加入的第一部分*/
/*********************************************************************************************/
sbit LCD_RS = P2^2 ;
sbit LCD_RW = P2^3 ;
sbit LCD_EN = P2^4 ;

uchar code  cdis6[ ] = {"Position:model3."};

#define delayNOP() ; {_nop_() ;_nop_() ;_nop_() ;_nop_() ;} ;

/******************************************************************/
/*检查LCD忙状态                                                   */
/*lcd_busy为1时，忙，等待。lcd-busy为0时,闲，可写指令与数据。   */
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

/*写指令数据到LCD                                                  */
/*RS=L，RW=L，E=高脉冲，D0-D7=指令码。          */
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
/*写显示数据到LCD                            */
/*RS=H，RW=L，E=高脉冲，D0-D7=数据。       */
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


/*  LCD初始化设定                                                  */
/*******************************************************************/
void lcd_init()
{
    delay1(15) ;
    lcd_wcmd(0x01) ;      //清除LCD的显示内容
    lcd_wcmd(0x38) ;      //16*2显示，5*7点阵，8位数据
    delay1(5) ;
    lcd_wcmd(0x38) ;
    delay1(5) ;
    lcd_wcmd(0x38) ;
    delay1(5) ;

    lcd_wcmd(0x0c) ;      //显示开，关光标
    delay1(5) ;
    lcd_wcmd(0x06) ;      //移动光标
    delay1(5) ;
    lcd_wcmd(0x01) ;      //清除LCD的显示内容
    delay1(5) ;
}

/*  设定显示位置                                    */
/*******************************************************************/
void lcd_pos(uchar pos)
{
  lcd_wcmd(pos | 0x80) ;  //数据指针=80+地址变量
}
/*********************************************************************************************/
/*第一部分加入结束*/


/*uchar KeyScan(void){

	System=0;MoveUp=1;MoveDown=1;SelfAdd=1;

	if(MoveUp==0){
		delay1(5);
		if(MoveUp==0){
			System=1;
			if(MoveUp==0)key=6;
			else key=1;
			while(MoveUp==0);
			goto a;
		}
	}


	if(MoveDown==0){
	
		delay1(5);
		if(MoveDown==0){
			System=1;
			if(MoveDown==0)key=7;
			else key=2;
			while(MoveDown==0);
			goto a;
		}
	}


  if(SelfAdd==0){
  
	 delay1(5);
  	 if(SelfAdd==0){
	 System=1;
	 if(SelfAdd==0)key=8;
	 else key=3;
	 while(SelfAdd==0);
	 goto a;
	 }
  }


	System=1;MoveUp=0;MoveDown=1;SelfAdd=1;  

	if(MoveDown==0){
	
		delay1(5);
		if(MoveDown==0){
		MoveUp=1;
		if(MoveDown==0)key=7;
		else key=4;
		while(MoveDown==0);
		goto a;
		}
	}


System=1;MoveUp=1;MoveDown=1;SelfAdd=1;  

	if(System==0){
	
	 delay1(5);
	 if(System==0)key=5;
	 while(System==0);
	 goto a;
	}


a:  return key;

}
*/
/*模式三，初始化显示子函数*/
/*********************************************************************************/

void Self_BK_Define3(void){    //用于模式三，显示按键操作

	uchar  m ;
    lcd_pos(0) ;                //设置显示位置为第一行的第1个字符
     m = 0 ;
    while(cdis6[m]!= '\0')
     {                         //显示字符
       lcd_wdat(cdis6[m]) ;
       m++ ;
     }

    lcd_pos(0x40) ;             //设置显示位置为第二行第1个字符
     m = 0 ;
    while(Function0[m] != '\0')
     {
       lcd_wdat(Function0[m]) ;      //显示字符
       m++ ;
     }
}


/******************************************************************************/

void MainModel3(void){
	uchar n;
	Self_BK_Define3();
	delay1(4000);
while(1){
	PT2262_Data0=1;PT2262_Data1=1;PT2262_Data2=1;PT2262_Data3=1;
	PT2262_TE=1;
 //  Self_BK_Define3();
	Button_Command=keypro();
	if(Button_Command==0x18){PT2262_Data0=1;PT2262_Data1=1;PT2262_Data2=1;PT2262_Data3=1;
		
	lcd_pos(0x40);
	n=0;
	while(Function6[n]!='\0'){
	lcd_wdat(Function6[n]);
			n++;
			}
}	

else if(Button_Command==0x28){PT2262_Data0=1;PT2262_Data1=1;PT2262_Data2=1;PT2262_Data3=0;
		
	lcd_pos(0x40);
	n=0;
	while(Function1[n]!='\0'){
	lcd_wdat(Function1[n]);
			n++;
			}
}	

else if(Button_Command==0x48){PT2262_Data0=0;PT2262_Data1=1;PT2262_Data2=1;PT2262_Data3=0;
		
	lcd_pos(0x40);
	n=0;
	while(Function2[n]!='\0'){
	lcd_wdat(Function2[n]);
			n++;
			}
}	


else if(Button_Command==0x88){PT2262_Data0=1;PT2262_Data1=1;PT2262_Data2=0;PT2262_Data3=0;
		
	lcd_pos(0x40);
	n=0;
	while(Function3[n]!='\0'){
	lcd_wdat(Function3[n]);
			n++;
			}
}	


else if(Button_Command==0x11){PT2262_Data0=0;PT2262_Data1=1;PT2262_Data2=0;PT2262_Data3=0;
		
	lcd_pos(0x40);
	n=0;
	while(Function4[n]!='\0'){
	lcd_wdat(Function4[n]);
			n++;
			}
}	



else {PT2262_Data0=0;PT2262_Data1=0;PT2262_Data2=0;PT2262_Data3=0;
		
	lcd_pos(0x40);
	n=0;
	while(Function5[n]!='\0'){
	lcd_wdat(Function5[n]);
			n++;
			}
}	
delay1(40);
}
}//模块3最后一行




/*********************************************************************************/
/*第二块添加的部分*/

void main(void){
	lcd_init();
	while(1){
	MainModel3();
	}
}
/*********************************************************************************/












































































