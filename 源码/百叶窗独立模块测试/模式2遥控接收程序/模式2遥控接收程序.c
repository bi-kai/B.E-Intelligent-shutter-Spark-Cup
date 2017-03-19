#include < reg51.h >
#include < intrins.h >

#define uchar unsigned char
#define uint  unsigned int

sbit PT2272_Data0=P2^0;
sbit PT2272_Data1=P2^1;
sbit PT2272_Data2=P2^2;
sbit PT2272_Data3=P2^3;


bit SignalControl_Flag1=1;		//连动标志位
bit SignalControl_Flag2=1;		//仅允许调用一次电机程序标志位
bit SelfControl_Flag=1;     	//自动控制标志位
bit Check_ForOnce=1;		//每次按键中断后只允许端口扫描一次

char INT1_Counter=0;
//char Motor_StepCounter=0;

void Motor_CompSlowlyUp(void);
void Motor_CompSlowlyDown(void);
void MainModel2(void);
void Motor_OpenUp(void);
void Motor_CloseDown(void);
int Motor_StepCounter=180;


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




 /**************************************************************/


void main(void){

/*************加入电机2******************/

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
TMOD = 0x11; //设定时器0和1为16位模式1 
ET0 = 1;           //定时器0中断允许 

TH0 = 0xFE;
TL0 = 0x0C;   //设定时每隔0.5ms中断一次

/***************************************/


	PT2272_Data0=1;
	PT2272_Data1=1;
	PT2272_Data2=1;
	PT2272_Data3=1;

while(1){
//if(SelfControl_Flag==0){  }
//else 
//if(SelfControl_Flag==1){

	MainModel2();
//}

}
}


void MainModel2(void){

	PT2272_Data0=1;
	PT2272_Data1=1;
	PT2272_Data2=1;
	PT2272_Data3=1;

	while(1){
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
					 {if(Motor_StepCounter>=180)SignalControl_Flag1=1;
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


	delay1(100);
	if(SelfControl_Flag==0)break;
	
	}

}


void SelfControl_int(void) interrupt 2
{
	delay1(80);
	Check_ForOnce=0;
	if((PT2272_Data0==0)&&(PT2272_Data1==0)&&(PT2272_Data2==0)&&(PT2272_Data3==1)){
	
		INT1_Counter++;
		if(INT1_Counter==5)INT1_Counter=1;	
	}
if((INT1_Counter%2)==1){SelfControl_Flag=1;}
else SelfControl_Flag=0;

}

/***************************************以下加入电机程序*******************************************/

void Motor_CloseDown(void){
turn=1;
while(Motor_StepCounter<=180){
turn=1;
	Motor_CompSlowlyDown();
	Motor_StepCounter++;
}

}


/*****************************************************************************************************/

void Motor_OpenUp(void){
turn=0;
while(Motor_StepCounter>=0){
turn=0;
	Motor_CompSlowlyUp();
	Motor_StepCounter--;
}

}


/*****************************************************************************************************/

void Motor_CompSlowlyDown(void){
turn=1;
spcount=0;
    speedlevel = 2;
	 for(i=0;i<=7;i++)
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
    speedlevel = 2;
	 for(i=0;i<=7;i++)
	 	for(j=0;j<=15;j++){HadBeenDone=0;TH0=0xFE;TL0=0x0c;TR0=1;delay(2);
		if(HadBeenDone==1){
		if(spcount<=0){spcount=speedlevel;
		gorun();}}}
		TR0=0;
}

//定时器0中断处理 
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




























