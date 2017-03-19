#include < reg51.h >
#include < intrins.h >

#define uchar unsigned char
#define uint  unsigned int

sbit DQ = P1^4 ;  //定义DS18B20端口DQ
sbit BEEP=P1^3  ; //蜂鸣器驱动线

bit presence  ;

sbit LCD_RS = P2^2 ;
sbit LCD_RW = P2^3 ;
sbit LCD_EN = P2^4 ;

uchar code  cdis1[ ] = {" welcome to JLDZ "} ;//嘉乐电子缩写
uchar code  cdis2[ ] = {" WENDU:    .  C "} ;
uchar code  cdis3[ ] = {" DS18B20  ERR0R "} ;
uchar code  cdis4[ ] = {"  PLEASE CHECK  "} ;

unsigned char data  temp_data[2] = {0x00,0x00} ;
unsigned char data  display[5] =   {0x00,0x00,0x00,0x00,0x00} ;
unsigned char code  ditab[16] =    {0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x04,
                                                        0x05,0x06,0x06,0x07,0x08,0x08,0x09,0x09} ;///*小数位表，四舍五入*/
//
void beep() ;
unsigned char code  mytab[8] = {0x0C,0x12,0x12,0x0C,0x00,0x00,0x00,0x00} ;

#define delayNOP() ; {_nop_() ;_nop_() ;_nop_() ;_nop_() ;} ;

/*************************************************************************************/
/********************************eeprom加入部分一*************************************/

uchar code word[]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x20};
uchar dispbuf[8],h=12,m,s,counter;

/*******更新缓冲区子程序*******/
void newbuf()
{
 dispbuf[0]=s%10;
 dispbuf[1]=s/10;
 dispbuf[3]=m%10;
 dispbuf[4]=m/10;
 dispbuf[6]=h%10;
 dispbuf[7]=h/10;
}

/***********************************eeprom加入的第三部分******************************/
/*************************************************************************************/
/*************************************************************************************/
#define ENABLE_ISP 0x80 //EEPROM操作参数设置：系统工作时钟<40MHz 时，对IAP_CONTR 寄存器设置此值

sfr IAP_DATA    = 0xe2;
sfr IAP_ADDRH   = 0xe3;
sfr IAP_ADDRL   = 0xe4;
sfr IAP_CMD     = 0xe5;
sfr IAP_TRIG    = 0xe6;
sfr IAP_CONTR   = 0xe7;
//定义Flash 操作等待时间及允许IAP/ISP/EEPROM 操作的常数

union union_temp16{unsigned int un_temp16;unsigned char un_temp8[2];}my_unTemp16;

//unsigned char temp[20]="";

unsigned char EEPROM_read(unsigned int add);   //读一字节，调用前需打开IAP 功能
void EEPROM_write(unsigned int, unsigned char ch); //字节编程，调用前需打开IAP 功能
void EEPROM_erase(unsigned int add);            //擦除扇区
void EEPROM_IAP_Disable();                       //关闭IAP 功能

void save_data_to_EEPROM(unsigned char num);

void send(unsigned char str[]);
void lcd_pos(uchar pos);
/************************************************    EEPROM操作函数   **************************************************/

unsigned char EEPROM_read(unsigned int add)	 //读一字节，调用前需打开IAP 功能，入口:DPTR = 字节地址，返回:A = 读出字节
{
    IAP_DATA = 0x00;
    IAP_CMD = 0x01;                 //IAP/ISP/EEPROM 字节读命令
    my_unTemp16.un_temp16 = add;
    IAP_ADDRH = my_unTemp16.un_temp8[0];    //设置目标单元地址的高8 位地址
    IAP_ADDRL = my_unTemp16.un_temp8[1];    //设置目标单元地址的低8 位地址
    //EA = 0;
    IAP_TRIG = 0x46;   //   46???   先送 5Ah,再送A5h 到ISP/IAP 触发寄存器,每次都需如此
    IAP_TRIG = 0xB9;   //   B9???   送完A5h 后，ISP/IAP 命令立即被触发起动
    _nop_();
    //EA = 1;
    return (IAP_DATA);
}


void EEPROM_write(unsigned int add, unsigned char ch) //字节编程，调用前需打开IAP 功能，入口:DPTR = 字节地址, A= 须编程字节的数据
{
    IAP_CMD = 0x02;                 //IAP/ISP/EEPROM 字节编程命令
    my_unTemp16.un_temp16 = add;
   	IAP_ADDRH = my_unTemp16.un_temp8[0];    //设置目标单元地址的高8 位地址
   	IAP_ADDRL = my_unTemp16.un_temp8[1];    //设置目标单元地址的低8 位地址
   	IAP_DATA = ch;                  //要编程的数据先送进IAP_DATA 寄存器
   	//EA = 0;
   	IAP_TRIG = 0x46;   //   46???   先送 5Ah,再送A5h 到ISP/IAP 触发寄存器,每次都需如此
   	IAP_TRIG = 0xB9;   //   B9???   送完A5h 后，ISP/IAP 命令立即被触发起动
    _nop_();
   	//EA = 1;
}


void EEPROM_erase(unsigned int add)//擦除扇区, 入口:DPTR = 扇区地址
{
	IAP_CMD = 0x03;                 //IAP/ISP/EEPROM 扇区擦除命令
    my_unTemp16.un_temp16 = add;
    IAP_ADDRH = my_unTemp16.un_temp8[0];    //设置目标单元地址的高8 位地址
    IAP_ADDRL = my_unTemp16.un_temp8[1];    //设置目标单元地址的低8 位地址

    //EA = 0;
    IAP_TRIG = 0x46;   //   46???   先送 5Ah,再送A5h 到ISP/IAP 触发寄存器,每次都需如此
    IAP_TRIG = 0xB9;   //   B9???   送完A5h 后，ISP/IAP 命令立即被触发起动
    _nop_();
    //EA = 1;
}

void EEPROM_IAP_Disable()
{
    //关闭IAP 功能, 清相关的特殊功能寄存器,使CPU 处于安全状态
    //一次连续的IAP 操作完成之后建议关闭IAP 功能,不需要每次都关
	IAP_CONTR = 0;      //关闭IAP 功能
	IAP_CMD   = 0;      //清命令寄存器,使命令寄存器无命令,此句可不用
	IAP_TRIG = 0;      //清命令触发寄存器,使命令触发寄存器无触发,此句可不用
	IAP_ADDRH = 0;
	IAP_ADDRL = 0;
}

void send(unsigned char str[])
{
	unsigned int i=0;
	while(str[i]!='\0')
	{
		SBUF=str[i];
		while(!TI);
		TI=0;
		i++;
		}
}

/**********************************自己加入的函数*********************************************/
uint eeprom_address=0x2000;
uchar Model4_WTime[8];
uchar Model4_WTem[8];
uchar Model4_RTime[9];
uchar Model4_RTem[9];
uchar Model4_rkey;
uchar sp=0;
long k=0;				//擦出扇区计数器
uint Model4_EEPROM_Address=0x2000;
uchar Model4_rkey;
bit Model4_SendFlag=0;		//为0允许发送，为1不允许发送（标志位）
uchar Model4_Key=0;			//模式四键盘扫描
/*******************************/
sbit Bu_SetZero=P1^7;			//模式四复用：重新发送
sbit Bu_MoveUp=P3^6;				//模式四复用：开始发送信息
sbit Bu_MoveDown=P3^7;			//模式四复用：停止发送信息
/**************************函数声明************************/
void INIT_NewTemp(void);
void send_data_from_EEPROM(void);
uchar Model4_ChangeForSave(uchar ucfs);
void send_data_from_EEPROM(void);
uchar m4_n;	//模式四中显示计数
/*****************************/
void delay1(int ms);
void lcd_wdat(uchar dat);
void lcd_pos(uchar pos);
uchar Menu_INT1=0;
uchar BK_count; //报警专用
uchar code  cdis7[ ] = {"Position:model4."};
uchar code Function12[ ]={"  loading...... "};
uchar code Function13[ ]={"  Sending...... "};
uchar code Function14[ ]={"  Stop Sending."};
uchar code Function15[ ]={"  EEPROM ready  "};
uchar code Function16[ ]={"  Restart......"};

void delay500(void){

	uchar i;
	for(i=250;i>0;i--){_nop_();}
}


/**********************************************************/

void save_data_to_EEPROM(void){

	uint i=0,j=0,disc=0;

	INIT_NewTemp();
	IAP_CONTR = ENABLE_ISP;         //打开IAP 功能, 设置Flash 操作等待时间

	while(i<8){
	EEPROM_write(eeprom_address,Model4_WTime[i]);
	eeprom_address++;
	i++;
	k++;
	if((k%508)==0){disc=(0x2000+((k/511)+1)*0x200+256);EEPROM_erase(disc);}
	if(eeprom_address>=(0x2fdf)){eeprom_address=0x2000;k=0;
	IAP_CONTR = ENABLE_ISP; 
	EEPROM_erase(0x2000);
	EEPROM_IAP_Disable();}
	}
	while(j<8){
	EEPROM_write(eeprom_address,Model4_WTem[j]);
	eeprom_address++;
	j++;
	k++;
	if((k%508)==0){disc=(0x2000+((k/511)+1)*0x200+256);EEPROM_erase(disc);}
	if(eeprom_address>=(0x2fdf)){eeprom_address=0x2000;k=0;
	IAP_CONTR = ENABLE_ISP; 
	EEPROM_erase(0x2000);
	EEPROM_IAP_Disable();
	}
	}
	
	EEPROM_IAP_Disable(); //关闭IAP 功能, 清相关的特殊功能寄存器,使CPU 处于安全状态
	}

void INIT_NewTemp(void){


	Model4_WTem[0]=display[3];
	Model4_WTem[1]=display[2];
	Model4_WTem[2]=display[1];
	Model4_WTem[3]='\x2E';
	Model4_WTem[4]=display[0];
	Model4_WTem[5]='\x60';
	Model4_WTem[6]='\x43';
	Model4_WTem[7]='\x20';	

	newbuf();
	Model4_WTime[0]=Model4_ChangeForSave(dispbuf[7]);
	Model4_WTime[1]=Model4_ChangeForSave(dispbuf[6]);
	Model4_WTime[2]='\x3A';	//冒号
	Model4_WTime[3]=Model4_ChangeForSave(dispbuf[4]);
	Model4_WTime[4]=Model4_ChangeForSave(dispbuf[3]);
	Model4_WTime[5]='\x3A';	//冒号
	Model4_WTime[6]=Model4_ChangeForSave(dispbuf[1]);
	Model4_WTime[7]=Model4_ChangeForSave(dispbuf[0]);

	}

uchar Model4_ChangeForSave(uchar ucfs){

	uchar tmp1;
	tmp1=ucfs;
	tmp1=word[tmp1];
	return tmp1;

}

void send_data_from_EEPROM(void){

uchar si=0,sj=0;
if(Model4_EEPROM_Address>=0x2fdf){Model4_EEPROM_Address=0x2000;}
	if(Model4_EEPROM_Address<0x2fdf){
	
		IAP_CONTR = ENABLE_ISP;         //打开IAP 功能, 设置Flash 操作等待时间
		while(si<8){
		Model4_RTime[si]=EEPROM_read(Model4_EEPROM_Address); 

		si++;
		Model4_EEPROM_Address++;
		}
		Model4_RTime[8]='\0';
	send(Model4_RTime);
	send("\x20");


	
		while(sj<8){
		
			Model4_RTem[sj]=EEPROM_read(Model4_EEPROM_Address); 
			sj++;
			Model4_EEPROM_Address++;
	
}
	Model4_RTem[8]='\0';
send(Model4_RTem);

send("\t");
send("\t");
//send("\t");

sp++;
if((sp%3)==0)send("\n");
if(sp==9)sp=0;
	}
}

/****************************模式四按键扫描*****************************************/
uchar Model4_KeyScan(void){

Bu_SetZero=1;			//模式四复用：重新发送
Bu_MoveUp=1;				//模式四复用：开始发送信息
Bu_MoveDown=1;			//模式四复用：停止发送信息
Model4_Key=0;

if(Bu_MoveUp==0){
delay1(5);
if(Bu_MoveUp==0)
Model4_Key=1;
while(Bu_MoveUp==0);
}


if(Bu_MoveDown==0){
delay1(5);
if(Bu_MoveDown==0)
Model4_Key=2;
while(Bu_MoveDown==0);
}


if(Bu_SetZero==0){
delay1(5);
if(Bu_SetZero==0)
Model4_Key=3;
while(Bu_SetZero==0);
}

return Model4_Key;	
}


/***********************************************/
/***********************************************/
/***********************************************/
/***********************************************/
/***********************************************/
/***********************************************/
/***********************************************/
void Self_BK_Define4(void){    //用于模式四，显示按键操作

	uchar  m ;
    lcd_pos(0) ;                //设置显示位置为第一行的第1个字符
     m = 0 ;
    while(cdis7[m]!= '\0')
     {                         //显示字符
       lcd_wdat(cdis7[m]) ;
       m++ ;
     }

    lcd_pos(0x40) ;             //设置显示位置为第二行第1个字符
     m = 0 ;
    while(Function12[m] != '\0')
     {
       lcd_wdat(Function12[m]) ;      //显示字符
       m++ ;
     }
}




void MainModel4(void){

Self_BK_Define4();
delay1(600);	


while(1){

Model4_rkey=Model4_KeyScan();

if(Model4_rkey==1){
while(1){send_data_from_EEPROM();
Model4_rkey=Model4_KeyScan();

if(Model4_rkey==3){Model4_EEPROM_Address=0x2000;

lcd_pos(0x40);
	m4_n=0;
	while(Function16[m4_n]!='\0'){
	lcd_wdat(Function16[m4_n]);
			m4_n++;
			}
delay1(100);

}

lcd_pos(0x40);
	m4_n=0;
	while(Function13[m4_n]!='\0'){
	lcd_wdat(Function13[m4_n]);
			m4_n++;
			}
if((Model4_rkey==2)/*||(((Menu_INT1)%4)==1)||(((Menu_INT1)%4)==0)||(((Menu_INT1)%4)==2)*/)break;
    if(((Menu_INT1)%2)==0)break;	
			}
			}

else if(Model4_rkey==2){

while(1){

Model4_rkey=Model4_KeyScan();
lcd_pos(0x40);
	m4_n=0;
	while(Function14[m4_n]!='\0'){
	lcd_wdat(Function14[m4_n]);
			m4_n++;
			}
if((Model4_rkey==1)/*||(((Menu_INT1)%4)==1)||(((Menu_INT1)%4)==0)||(((Menu_INT1)%4)==2)*/)break;
    if(((Menu_INT1)%2)==0)break;		
}
			}


else {
lcd_pos(0x40);
	m4_n=0;
	while(Function15[m4_n]!='\0'){
	lcd_wdat(Function15[m4_n]);
			m4_n++;
			}
}





    if(((Menu_INT1)%2)==0)break;
//if((((Menu_INT1)%4)==1)||(((Menu_INT1)%4)==0)||(((Menu_INT1)%4)==2))break;

}
}





/*************************************************************************************/
/***********************************eeprom加入部分一结束*******************************/
/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/
void delay1(int ms)
{
 unsigned char y ;
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

/******************************************************************/
/*检查LCD忙状态                                                   */
/*lcd_busy为1时，忙，等待。lcd-busy为0时,闲，可写指令与数据。   */
/******************************************************************/
bit lcd_busy()
 {
    bit result ;
    LCD_RS = 0 ;
    LCD_RW = 1 ;    LCD_EN = 1 ;
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

/*自定义字符写入CGRAM                                   */
/*******************************************************************/
void  writetab()
{
    unsigned char i ;
    lcd_wcmd(0x40) ;            //写CGRAM
    for (i = 0 ; i< 8 ; i++)
    lcd_wdat(mytab[ i ]) ;
}

/*us级延时函数                                        */
/*******************************************************************/

void Delay(unsigned int num)
{
  while( --num ) ;
}

/*初始化ds1820                                      */
/*******************************************************************/
Init_DS18B20(void)
{
     DQ = 1 ;      //DQ复位
     Delay(8) ;    //稍做延时

     DQ = 0 ;      //单片机将DQ拉低
     Delay(90) ;   //精确延时 大于 480us

     DQ = 1 ;       //拉高总线
     Delay(8) ;

     presence = DQ ;    //如果=0则初始化成功 =1则初始化失败
     Delay(100) ;
     DQ = 1 ;

     return(presence) ; //返回信号，0=presence,1= no presence
}


/* 读一个字节                     */
/*******************************************************************/
 ReadOneChar(void)
{
unsigned char i = 0 ;
unsigned char dat = 0 ;

for (i = 8 ; i > 0 ; i--)
  {
    DQ = 0 ; // 给脉冲信号
    dat >>= 1 ;
    DQ = 1 ; // 给脉冲信号

    if(DQ)
     dat |= 0x80 ;
    Delay(4) ;
  }

    return (dat) ;
}

/* 写一个字节                                         */
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

/* 读取温度                            */
/*******************************************************************/
 Read_Temperature(void)
{
     Init_DS18B20() ;

     WriteOneChar(0xCC) ;  // 跳过读序号列号的操作
     WriteOneChar(0x44) ;  // 启动温度转换

     Init_DS18B20() ;
     WriteOneChar(0xCC) ;  //跳过读序号列号的操作
     WriteOneChar(0xBE) ;  //读取温度寄存器

     temp_data[0] = ReadOneChar() ;   //温度低8位
     temp_data[1] = ReadOneChar() ;   //温度高8位
}

/* 数据转换与温度显示                                              */
/*******************************************************************/
 Disp_Temperature()
{
  display[4]=temp_data[0]&0x0f ;
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

     lcd_pos(0x48) ;
     lcd_wdat(display[3]) ;        //百位数显示
     lcd_pos(0x49) ;
     lcd_wdat(display[2]) ;        //十位数显示
  lcd_pos(0x4a) ;
     lcd_wdat(display[1]) ;        //个位数显示
  lcd_pos(0x4c) ;
     lcd_wdat(display[0]) ;        //小数位数显示
}

/*******************************************************************/
/* 蜂鸣器响一声                 */
/*******************************************************************/
void beep()
  {
    unsigned char y ;
    for (y=0 ;y<100 ;y++)
    {
      Delay(60) ;
      BEEP=!BEEP ;                //BEEP取反
    }
    BEEP=1 ;                      //关闭蜂鸣器
 Delay(40000) ;
  }

/* DS18B20 OK 显示菜单                                             */
/*******************************************************************/
void  Ok_Menu ()
{
    uchar  m ;
    lcd_init() ;                //初始化LCD

    lcd_pos(0) ;                //设置显示位置为第一行的第1个字符
     m = 0 ;
    while(cdis1[m] != '\0')
     {                         //显示字符
       lcd_wdat(cdis1[m]) ;
       m++ ;
     }

    lcd_pos(0x40) ;             //设置显示位置为第二行第1个字符
     m = 0 ;
    while(cdis2[m] != '\0')
     {
       lcd_wdat(cdis2[m]) ;      //显示字符
       m++ ;
     }

     writetab() ;               //自定义字符写入CGRAM
     delay1(5) ;
     lcd_pos(0x4d) ;
     lcd_wdat(0x00) ;          //显示自定义字符
}


/* DS18B20 ERROR 显示菜单                             */
/*******************************************************************/
void  Error_Menu ()
{
     uchar  m ;
     lcd_init() ;                //初始化LCD

    lcd_pos(0) ;                //设置显示位置为第一行的第1个字符
     m = 0 ;
     while(cdis3[m] != '\0')
     {                         //显示字符
       lcd_wdat(cdis3[m]) ;
       m++ ;
     }

     lcd_pos(0x40) ;             //设置显示位置为第二行第1个字符
     m = 0 ;
     while(cdis4[m] != '\0')
     {
       lcd_wdat(cdis4[m]) ;      //显示字符
       m++ ;
     }
}
/********************************************************************************************************/
void Self_BK_Define2(void){  
	 uchar  m ;
    lcd_pos(0) ;                //设置显示位置为第一行的第1个字符
     m = 0 ;
    while(cdis1[m] != '\0')
     {                         //显示字符
       lcd_wdat(cdis1[m]) ;
       m++ ;
     }

    lcd_pos(0x40) ;             //设置显示位置为第二行第1个字符
     m = 0 ;
    while(cdis2[m] != '\0')
     {
       lcd_wdat(cdis2[m]) ;      //显示字符
       m++ ;
     }

     writetab() ;               //自定义字符写入CGRAM
     delay1(5) ;
     lcd_pos(0x4d) ;
     lcd_wdat(0x00) ;          //显示自定义字符
}



void MainModel1(void){
 
Self_BK_Define2();
//System=0;
//LED_Model1=1;
//Bu_CloseDown=1;

 do
  {
      Read_Temperature() ;
  	   Disp_Temperature() ;
			
  if(display[2]>=51){for(BK_count=200;BK_count>0;BK_count--){
  
  				BEEP=~BEEP;
  				delay500();  
  }
  		for(BK_count=200;BK_count>0;BK_count--){
		
			BEEP=~BEEP;
			delay500();
			delay500();
		}
    }	
	

save_data_to_EEPROM();
		
    if(((Menu_INT1)%2)==1)break;
     }
    while(!presence);

     Error_Menu();

 do
  {
    Init_DS18B20();
    beep();
	     if(((Menu_INT1)%2)==1)break;
     }
 while(presence);
/*System=1;
LED_Model1=0;
Bu_CloseDown=1;
*/ 
//    if(((Menu_INT1)%2)==1)break;
}

void init(void){
  TMOD=0x01;
  TH0=0x4c;
  TL0=0x00;
   IT1=1;
	PX1=1;
  EX1=1;
	EA=1;
  ET0=1;
  TR0=1;
}
/* 主函数     */
/************************************/
void main()
 {
     Ok_Menu () ;
	  init();
/**********************************加入的第二部分*************************************/
/****************************************************************************************/
SCON=0x50;//串口方式1，允许接收
   TMOD=0x20;//定时器1，定时方式2
//   TCON=0x40;//设定时器1开始计数
	ET1=1;
   TR1=1;
   TH1=0xE8;//11.0592MHz, 1200波特率
   TL1=0xE8;
   TI=1;
   TR1=1;
	IAP_CONTR = ENABLE_ISP; 
	EEPROM_erase(0x2000);
EEPROM_IAP_Disable();
/************加入的第二部分结束**********************************************************/
while(1){ 
if(((Menu_INT1)%2)==0){	MainModel1();}
else 	MainModel4();
}

}
/*外部中断*/
void int1_Menu_INT1() interrupt 2  
	{
		Menu_INT1++;
		
		if(Menu_INT1==8)Menu_INT1=0;
	
	}
