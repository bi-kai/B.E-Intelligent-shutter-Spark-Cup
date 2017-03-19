#include <reg51.h>
 
sbit P1_0=P1^0;
sbit P1_1=P1^1;
sbit P1_2=P1^2;
sbit P1_3=P1^3;

static unsigned int count; //计数
static int step_index; //步进索引数，值为0－7

char i,j,k;
char HadBeenDone;

static bit turn; //步进电机转动方向
static bit stop_flag; //步进电机停止标志
static int speedlevel; //步进电机转速参数，数值越大速度越慢，最小值为1，速度最快
static int spcount;    //步进电机转速参数计数
void delay(unsigned int endcount); //延时函数，延时为endcount*0.5毫秒
void gorun();          //步进电机控制步进函数

void Motor_CompSlowlyDown(void);
void Motor_OpenUp(void);
char Motor_StepCounter=90;

void main(void) 
{
count = 0;
step_index = 0;
spcount = 0;
stop_flag = 0;

P1_0 = 0;
P1_1 = 0;
P1_2 = 0;
P1_3 = 0;

EA = 1;            //允许CPU中断 
TMOD = 0x11; //设定时器0和1为16位模式1 
ET0 = 1;           //定时器0中断允许 

TH0 = 0xFE;
TL0 = 0x0C;   //设定时每隔0.5ms中断一次
//TR0 = 1;         //开始计数

turn = 0;

do{
Motor_OpenUp();
TR0=1;
delay(10000);
TR0=0;

for(k=0;k<=90;k++){
Motor_CompSlowlyDown();
}
TR0=1;
delay(10000);
TR0=0;
}while(1);

} 

void Motor_OpenUp(void){
turn=0;
while(Motor_StepCounter>=0){
turn=0;
	Motor_CompSlowlyDown();
	Motor_StepCounter--;
}

}

void Motor_CompSlowlyDown(void){
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
