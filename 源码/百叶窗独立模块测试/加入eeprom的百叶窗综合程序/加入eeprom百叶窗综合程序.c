#include < reg51.h >
#include < intrins.h >

#define ENABLE_ISP 0x80 //EEPROM操作参数设置：系统工作时钟<40MHz 时，对IAP_CONTR 寄存器设置此值


#define uchar unsigned char
#define uint  unsigned int

sbit PT2272_Data0=P2^0;
sbit PT2272_Data1=P2^1;
sbit PT2272_Data2=P2^2;
sbit PT2272_Data3=P2^3;

/******************************************/

//ADC0832的引脚
sbit AD2_CS =P1^4;  //ADC0832 chip seclect
sbit AD2_DI =P3^4;  //ADC0832 k in
sbit AD2_DO =P3^4;  //ADC0832 k out
sbit AD2_CLK=P1^5; //ADC0832 clock signal
sbit LED2_=P0^5;
//ADC0832的引脚
sbit AD1_CS =P3^5;  //ADC0832 chip seclect
sbit AD1_DI =P3^7;  //ADC0832 k in
sbit AD1_DO =P3^7;  //ADC0832 k out
sbit AD1_CLK=P3^6; //ADC0832 clock signal
sbit LED1_=P0^6;

sbit Speaker=P1^7;
sbit P3_2=P3^2;

sbit LED4_=P0^7;  //自动调节指示灯
sbit LED5_=P2^7;  //自动调节指示灯

sbit LED6_=P0^0;  //完全打开指示灯
sbit LED7_=P0^1;  //完全关闭指示灯

int meanvalue1;
int meanvalue2; 

char eeprom_meanvalue1;
char eeprom_meanvalue2;
char eeprom_meanvalue3;

unsigned char data  temp_data1_[2] = {0x00,0x00} ;
unsigned char data  display1_[5] =   {0x00,0x00,0x00,0x00,0x00} ;
unsigned char data  temp_data2_[2] = {0x00,0x00} ;
unsigned char data  display2_[5] =   {0x00,0x00,0x00,0x00,0x00} ;
unsigned char code  ditab[16] =    {0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x04,
                                                        0x05,0x06,0x06,0x07,0x08,0x08,0x09,0x09} ;/*小数位表，四舍五入*/


/******************定时功能部分************************/

uchar T1_h,T1_m,T1_s,T1_counter;

 /**************************************************************/
 /**********************eeprom加入的第一部分********************/
 /**************************************************************/

uchar Cleaner=0;
uint Mainaddress;
uchar Model4_WTemprature[3];
bit Model4_SendFlag=0;		//为0允许发送，为1不允许发送（标志位）
uchar allow_erase=0;
bit allow_eraser=0;
bit Model4_SendFlag1=0;
uchar Model4_STemprature[4];
uint Model4_SendData_Counter=0x2000;
uint Model4_Eeprom_Counter=0x2000;

 /**************************************************************/

sfr IAP_DATA    = 0xe2;
sfr IAP_ADDRH   = 0xe3;
sfr IAP_ADDRL   = 0xe4;
sfr IAP_CMD     = 0xe5;
sfr IAP_TRIG    = 0xe6;
sfr IAP_CONTR   = 0xe7;
//定义Flash 操作等待时间及允许IAP/ISP/EEPROM 操作的常数

union union_temp16{unsigned int un_temp16;unsigned char un_temp8[2];}my_unTemp16;


unsigned char EEPROM_read(unsigned int add);   //读一字节，调用前需打开IAP 功能
void EEPROM_write(unsigned int, unsigned char ch); //字节编程，调用前需打开IAP 功能
void EEPROM_erase(unsigned int add);            //擦除扇区
void EEPROM_IAP_Disable();                       //关闭IAP 功能

//void save_data_to_EEPROM(unsigned char num);

void send(unsigned char str[]);



/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/


void T1_init()
{

  TMOD=0x21;
// TH1=0xE8;
// TL1=0xE8;
  EA=1;
//ET1=1;
// TR1=1;
  T1_counter=0;
  T1_h=12;T1_m=0;T1_s=0;

}

/******************************************/

bit SignalControl_Flag1=1;		//连动标志位
bit SignalControl_Flag2=1;		//仅允许调用一次电机程序标志位
bit SelfControl_Flag=0;     	//自动控制标志位
bit Check_ForOnce=1;		//每次按键中断后只允许端口扫描一次
bit Base_Flag=0;			//判断时间是否在允许范围之内标志位

char INT1_Counter=0;
//char Motor_StepCounter=0;

void Motor_CompSlowlyUp(void);
void Motor_CompSlowlyDown(void);
void MainModel2(void);
void MainModel1(void);
void Motor_OpenUp(void);
void Motor_CloseDown(void);
int Motor_StepCounter=45;


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


 /*************************加入电机1***************************/

sbit P1_0=P1^0;
sbit P1_1=P1^1;
sbit P1_2=P1^2;
sbit P1_3=P1^3;

static unsigned int count; //计数
static int step_index; //步进索引数，值为0－7

char i,j;
char HadBeenDone;

static bit turn; //步进电机转动方向
static bit stop_flag; //步进电机停止标志
static int speedlevel; //步进电机转速参数，数值越大速度越慢，最小值为1，速度最快
static int spcount;    //步进电机转速参数计数
void delay(unsigned int endcount); //延时函数，延时为endcount*0.5毫秒
void gorun();          //步进电机控制步进函数




 /***************************************加入两个AD综合部分的程序********************************************************************/
/************************************************************************************************************************************/
/************************************************************************************************************************************/
/************************************************************************************************************************************/
/************************************************************************************************************************************/

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
    return(dat);            //return dat
}
/************************************************************/

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
    return(dat);            //return dat
}

/*								 读取数据                         */  
/*******************************************************************/     
adc1_display(void) {

	LED1_=~LED1_;delay1(50);LED1_=~LED1_;delay1(50);LED1_=~LED1_;

     temp_data1_[0] =  Adc1_0832();   //数据低8位


/* 数据转换与温度显示                                              */
/*******************************************************************/
  
  display1_[4]=(temp_data1_[0]&0x0f) ;  //换算成8位二进制
  display1_[0]=ditab[display1_[4]]+0x30 ;     //查表得小数位的值

  display1_[4]=((temp_data1_[0]&0xf0)>>4)|((temp_data1_[1]&0x0f)<<4) ;
  display1_[3]=display1_[4]/100+0x30 ;
  display1_[1]=display1_[4]%100 ;
  display1_[2]=display1_[1]/10+0x30 ;
  display1_[1]=display1_[1]%10+0x30 ;    

	  }







adc2_display(void) {

		LED2_=~LED2_;delay1(50);LED2_=~LED2_;delay1(50);LED2_=~LED2_;
	
     temp_data2_[0] =  Adc2_0832();   //数据低8位


/* 							数据转换                        */
/*******************************************************************/
  
  display2_[4]=(temp_data2_[0]&0x0f) ;  //换算成8位二进制
  display2_[0]=ditab[display2_[4]]+0x30 ;     //查表得小数位的值

  display2_[4]=((temp_data2_[0]&0xf0)>>4)|((temp_data2_[1]&0x0f)<<4) ;
  display2_[3]=display2_[4]/100+0x30 ;
  display2_[1]=display2_[4]%100 ;
  display2_[2]=display2_[1]/10+0x30 ;
  display2_[1]=display2_[1]%10+0x30 ;      
   
	  }

/**************************************************************/
/**********************eeprom加入的第二部分*********************/
/**************************************************************/
/**************************************************************/


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

void New_Temprature(void){

Model4_WTemprature[0]=eeprom_meanvalue1;//百位
Model4_WTemprature[1]=eeprom_meanvalue2;//十位
Model4_WTemprature[2]=eeprom_meanvalue3;//个位

}


void Model4_Save_Data(void){

	uchar M4_Wi;
	uint M4_Wj=0,disc=0;
/*if(allow_erase==0){   
	for(disc=0;disc<8;disc++){
	Mainaddress=0x2000+disc*0x200;
	EEPROM_erase(Mainaddress);
	disc++;
	}
	EEPROM_IAP_Disable();
	allow_erase=1;
}
	
    if(Model4_WTemprature[0]==0x30)        //高位为0，不显示
   {
     Model4_WTemprature[0]=0x20 ;
     if(Model4_WTemprature[1]==0x30)      //次高位为0，不显示
     Model4_WTemprature[1]=0x20 ;
   }

*/

	if(Model4_Eeprom_Counter<=0x2fff){
			New_Temprature();
				IAP_CONTR = ENABLE_ISP; 
			for(M4_Wi=0;M4_Wi<=2;M4_Wi++){
				EEPROM_write(Model4_Eeprom_Counter,Model4_WTemprature[M4_Wi]);
//				EEPROM_write(Model4_Eeprom_Counter,'\x2a');
				Model4_Eeprom_Counter++;
				M4_Wj++;
			}	
	}

else {M4_Wj=0;}

//if((M4_Wj%509)==1){disc=(0x2000+((M4_Wj/511)+2)*0x200+256);EEPROM_erase(disc);}

EEPROM_IAP_Disable(); //关闭IAP 功能, 清相关的特殊功能寄存器,使CPU 处于安全状态
}




void Model4_Send_Data(void){

	uchar M4_Si;//M4_Sj;
	if((Model4_SendFlag1==0)&&(Model4_SendData_Counter<0x2fff)){
		IAP_CONTR = ENABLE_ISP;         //打开IAP 功能, 设置Flash 操作等待时间
		for(M4_Si=0;M4_Si<=2;M4_Si++){
			Model4_STemprature[M4_Si]=EEPROM_read(Model4_SendData_Counter);
			Model4_SendData_Counter++;
		//	M4_Si++;
				
		}
	Model4_STemprature[3]='\0';
	send(Model4_STemprature);
	send("\x20");
//	Model4_rkey=Model4_KeyScan();
		EEPROM_IAP_Disable();
	}

else if(Model4_SendFlag1==0){Model4_SendFlag1=1;send("Sanding Over");}




//send("Send Over");
		}


/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/

void main(void){

/*************加入电机2******************/
Speaker=1;
P3_2=1;

T1_init();

/***********eeprom加入的第三部分*****/

   SCON=0x50;//串口方式1，允许接收
   TMOD=0x21;//定时器1，定时方式2
// TCON=0x40;//设定时器1开始计数
	TH1=0xE8;//11.0592MHz, 1200波特率
   TL1=0xE8;
   TI=1;
   TR1=1;

	  	IAP_CONTR = ENABLE_ISP;

   for(Cleaner=0;Cleaner<8;Cleaner++){
	Mainaddress=0x2000+Cleaner*0x200;
	EEPROM_erase(Mainaddress);
//	Cleaner++;
	}

	EEPROM_IAP_Disable();
/***********************************/

count = 0;
step_index = 0;
spcount = 0;
stop_flag = 0;

P1_0 = 0;
P1_1 = 0;
P1_2 = 0;
P1_3 = 0;

IT1=1;
PX1=1;
EX1=1;
EA = 1;            //允许CPU中断 
TMOD = 0x21; //设定时器0和1为16位模式1、2
ET0 = 1;           //定时器0中断允许 

TH0 = 0xFE;
TL0 = 0x0C;   //设定时每隔0.5ms中断一次

/***************************************/


	PT2272_Data0=1;
	PT2272_Data1=1;
	PT2272_Data2=1;
	PT2272_Data3=1;

while(1){

if((T1_h<=7)||(T1_h>=19)){	Motor_CloseDown();Base_Flag=1;}
else Base_Flag=0;


	if	((SelfControl_Flag==0)&&(Base_Flag==0)){ MainModel1(); }
	else     {  MainModel2();}

		  }

		  
		  
		  
		  
		  
		  
		  
}


/**********************************************加入的第一部分*****************************************************************************/

void MainModel1(void){ 

Speaker=1;
P3_2=1;

while(1) {   
	adc1_display();			
	adc2_display();


if		 (Motor_StepCounter>=45){LED6_=0;LED7_=1;}
else if(Motor_StepCounter<=0){LED6_=1;LED7_=0;}
else   {LED6_=1;LED7_=1;}

meanvalue1=((char)((display1_[1]-0x30+display2_[1]-0x30)/2)+0x30);
meanvalue2=(display2_[2]-0x30+display1_[2]-0x30+0x30);

/***************************eeprom部分******************************************************/

eeprom_meanvalue1=((char)((display1_[2]-0x30+display2_[2]-0x30)/2)+0x30);
eeprom_meanvalue2=meanvalue1;
eeprom_meanvalue3=((char)((display1_[0]-0x30+display2_[0]-0x30)/2)+0x30);

Model4_Save_Data();
Model4_Send_Data();
//send("Send Over");

/*******************************************************************************************/

if(meanvalue2>=0x31){Motor_StepCounter--;
stop_flag=0;if(Motor_StepCounter<=0){stop_flag=1;}
LED5_=1;LED4_=~LED4_;delay1(100);
Motor_CompSlowlyUp();}

else if(meanvalue1>=0x39){Motor_StepCounter--;
stop_flag=0;
if(Motor_StepCounter<=0){stop_flag=1;}
LED5_=1;LED4_=~LED4_;delay1(100);
Motor_CompSlowlyUp();}

else if(meanvalue1>=0x35){LED4_=0;LED5_=0;delay1(100);}

else   {Motor_StepCounter++;
stop_flag=0;
if(Motor_StepCounter>=45){stop_flag=1;}
LED4_=1;LED5_=~LED5_;delay1(100);
Motor_CompSlowlyDown();}

if(P3_2==0){Speaker=0;}
else if(P3_2==1){Speaker=1;}

if(SelfControl_Flag==1)break;
	 }

}

/*************************加入的第一部分结束**********************************/

void MainModel2(void){

Speaker=1;
P3_2=1;
stop_flag=0;

	while(1){
	PT2272_Data0=1;
	PT2272_Data1=1;
	PT2272_Data2=1;
	PT2272_Data3=1;

	
if	   (Motor_StepCounter>=45){LED6_=0;LED7_=1;}
else if(Motor_StepCounter<=0){LED6_=1;LED7_=0;}
else   {LED6_=1;LED7_=1;}


	if(Check_ForOnce==0){

			if		 ((PT2272_Data0==1)&&(PT2272_Data1==0)&&(PT2272_Data2==0)&&(PT2272_Data3==0))
				  	 {SignalControl_Flag1=0;
					 SignalControl_Flag2=1;
					 }

			else if((PT2272_Data0==0)&&(PT2272_Data1==0)&&(PT2272_Data2==1)&&(PT2272_Data3==0))
					 {SignalControl_Flag1=0;
					 SignalControl_Flag2=1;
					 }

			else if((PT2272_Data0==1)&&(PT2272_Data1==1)&&(PT2272_Data2==1)&&(PT2272_Data3==0))
					 {SignalControl_Flag2=0;
					 SignalControl_Flag1=1;
					 }

			else if((PT2272_Data0==0)&&(PT2272_Data1==1)&&(PT2272_Data2==1)&&(PT2272_Data3==0))
					 {SignalControl_Flag2=0;
					 SignalControl_Flag1=1;
					 }

			else if((PT2272_Data0==1)&&(PT2272_Data1==1)&&(PT2272_Data2==0)&&(PT2272_Data3==0))
					 {SignalControl_Flag2=0;
					 SignalControl_Flag1=1;
					 }

			else if((PT2272_Data0==0)&&(PT2272_Data1==1)&&(PT2272_Data2==0)&&(PT2272_Data3==0))
					 {SignalControl_Flag2=0;
					 SignalControl_Flag1=1;
					 }

			else if((PT2272_Data0==0)&&(PT2272_Data1==0)&&(PT2272_Data2==0)&&(PT2272_Data3==0))
					 {SignalControl_Flag2=0;
					 SignalControl_Flag1=1;
					 }
	
	Check_ForOnce=1;}
	/****************************************************************************************************/	

			if		 ((SignalControl_Flag1==0)&&((PT2272_Data0==1)&&(PT2272_Data1==0)&&(PT2272_Data2==0)&&(PT2272_Data3==0)))
					 {if(Motor_StepCounter<=0)SignalControl_Flag1=1;
					  else SignalControl_Flag1=0;
					 Motor_CompSlowlyUp();
					 Motor_StepCounter--;
					 
					 }

			else if((SignalControl_Flag1==0)&&((PT2272_Data0==0)&&(PT2272_Data1==0)&&(PT2272_Data2==1)&&(PT2272_Data3==0)))
					 {if(Motor_StepCounter>=45)SignalControl_Flag1=1;
					  else SignalControl_Flag1=0;
					 Motor_CompSlowlyDown();
					 Motor_StepCounter++;
					 }

		 	else if((SignalControl_Flag2==0)&&((PT2272_Data0==1)&&(PT2272_Data1==1)&&(PT2272_Data2==1)&&(PT2272_Data3==0)))
					 {Motor_OpenUp();
					 SignalControl_Flag2=1;
					 }

			else if((SignalControl_Flag2==0)&&((PT2272_Data0==0)&&(PT2272_Data1==1)&&(PT2272_Data2==1)&&(PT2272_Data3==0)))
					 {Motor_CloseDown();
					 SignalControl_Flag2=1;
					 }

			else if((SignalControl_Flag2==0)&&((PT2272_Data0==1)&&(PT2272_Data1==1)&&(PT2272_Data2==0)&&(PT2272_Data3==0)))
					 {Motor_CompSlowlyUp();
					 Motor_StepCounter--;
					 SignalControl_Flag2=1;
					 }

			else if((SignalControl_Flag2==0)&&((PT2272_Data0==0)&&(PT2272_Data1==1)&&(PT2272_Data2==0)&&(PT2272_Data3==0)))
					 {Motor_CompSlowlyDown();
					 Motor_StepCounter++;
					 SignalControl_Flag2=1;
					 }

if(P3_2==0){Speaker=0;}
else if(P3_2==1){Speaker=1;}


	delay1(100);
	if(SelfControl_Flag==0)break;
	
	}

}


void SelfControl_int(void) interrupt 2
{
	delay1(100);
	Check_ForOnce=0;
	if((PT2272_Data0==0)&&(PT2272_Data1==0)&&(PT2272_Data2==0)&&(PT2272_Data3==1))
			{SelfControl_Flag=0;}
	else if((PT2272_Data0==0)&&(PT2272_Data1==0)&&(PT2272_Data2==1)&&(PT2272_Data3==1))
			{SelfControl_Flag=1;}
			
			/*	
		INT1_Counter++;
		if(INT1_Counter==5)INT1_Counter=1;	
	}
if((INT1_Counter%2)==1){SelfControl_Flag=1;}
else SelfControl_Flag=0;
*/
}

/***************************************以下加入电机程序*******************************************/

void Motor_CloseDown(void){
turn=1;
while(Motor_StepCounter<=45){
turn=1;
	Motor_CompSlowlyDown();
	Motor_StepCounter++;

if	   (Motor_StepCounter>=45){LED6_=0;LED7_=1;}
else if(Motor_StepCounter<=0){LED6_=1;LED7_=0;}
else   {LED6_=1;LED7_=1;}

}

}


/*****************************************************************************************************/

void Motor_OpenUp(void){
turn=0;
while(Motor_StepCounter>=0){
turn=0;
	Motor_CompSlowlyUp();
	Motor_StepCounter--;

if	   (Motor_StepCounter>=45){LED6_=0;LED7_=1;}
else if(Motor_StepCounter<=0){LED6_=1;LED7_=0;}
else   {LED6_=1;LED7_=1;}

}

}


/*****************************************************************************************************/

void Motor_CompSlowlyDown(void){
turn=1;
spcount=0;
    speedlevel=2;
	 for(i=0;i<=31;i++)
	 	for(j=0;j<=15;j++){HadBeenDone=0;TH0=0xFE;TL0=0x0c;TR0=1;delay(2);
		if(HadBeenDone==1){
		if(spcount<=0){spcount=speedlevel;
		gorun();}}}
		TR0=0;
}


/**************************************************************************************************/

void Motor_CompSlowlyUp(void){
turn=0;
spcount=0;
    speedlevel=2;
	 for(i=0;i<=31;i++)
	 	for(j=0;j<=15;j++){HadBeenDone=0;TH0=0xFE;TL0=0x0c;TR0=1;delay(2);
		if(HadBeenDone==1){
		if(spcount<=0){spcount=speedlevel;
		gorun();}}}
		TR0=0;
}




/*************************************************************************************/

/*************************定时器1的中断**********************
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

*/
/****************定时器0中断处理*****************************/ 
void timeint(void) interrupt 1 
{ 
TH0=0xFE;
TL0=0x0C; //设定时每隔0.5ms中断一次


count++;
if(HadBeenDone==0){HadBeenDone=1;
spcount--;
}

}

void delay(unsigned int endcount)
{
count=0;
do{}while(count<endcount);
}

void gorun()
{
if (stop_flag==1)
{
    P1_0 = 0;
    P1_1 = 0;
    P1_2 = 0;
    P1_3 = 0;
    return;
}

switch(step_index)
{
case 0: //0
    P1_0 = 1;
    P1_1 = 0;
    P1_2 = 0;
    P1_3 = 0;
    break;
case 1: //0、1
    P1_0 = 1;
    P1_1 = 1;
    P1_2 = 0;
    P1_3 = 0;
    break;
case 2: //1
    P1_0 = 0;
    P1_1 = 1;
    P1_2 = 0;
    P1_3 = 0;
    break;
case 3: //1、2
    P1_0 = 0;
    P1_1 = 1;
    P1_2 = 1;
    P1_3 = 0;
    break;
case 4: //2
    P1_0 = 0;
    P1_1 = 0;
    P1_2 = 1;
    P1_3 = 0;
    break;
case 5: //2、3
    P1_0 = 0;
    P1_1 = 0;
    P1_2 = 1;
    P1_3 = 1;
    break;
case 6: //3
    P1_0 = 0;
    P1_1 = 0;
    P1_2 = 0;
    P1_3 = 1;
    break;
case 7: //3、0
    P1_0 = 1;
    P1_1 = 0;
    P1_2 = 0;
    P1_3 = 1;
}

if (turn==0)
{
    step_index++;
    if (step_index>7)
      step_index=0;
}
else
{
    step_index--;
    if (step_index<0)
      step_index=7;
}
    
}


