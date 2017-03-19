#include <reg51.h> 
#define uchar unsigned char

sbit P1_0=P1^0;
sbit P1_1=P1^1;
sbit P1_2=P1^2;
sbit P1_3=P1^3;

static unsigned int count; //����
static int step_index; //������������ֵΪ0��7

//static bit turn; //�������ת������
static bit stop_flag; //�������ֹͣ��־
static int speedlevel; //�������ת�ٲ�������ֵԽ���ٶ�Խ������СֵΪ1���ٶ����
static int spcount;    //�������ת�ٲ�������
void delay(unsigned int endcount); //��ʱ��������ʱΪendcount*0.5����



void Motor_OpenUp(void);  //����������Ʋ�������
int Motor_StepCounter=256;
char HadBeenDone;






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

EA = 1;            //����CPU�ж� 
TMOD = 0x11; //�趨ʱ��0��1Ϊ16λģʽ1 
ET0 = 1;           //��ʱ��0�ж����� 

TH0 = 0xFE;
TL0 = 0x0C;   //�趨ʱÿ��0.5ms�ж�һ��




do{Motor_OpenUp();
TR0=1;   
delay(1000);
TR0=0; 
}while(1);

} 

void Motor_OpenUp(void){

	uchar p,j=0;
	step_index=0;
	spcount=0;
	speedlevel=2;
	if(stop_flag==1)stop_flag=0;
	while(Motor_StepCounter>=31){
	for(p=0;p<=7;p++){
	HadBeenDone=0;
	TH0=0xFE;
	TL0=0x0C;
	TR0=1;
	delay(1);
	if(HadBeenDone==1){
		if(spcount<=0){
		spcount=speedlevel;
switch(step_index)
{
case 0: //0
    P1_0 = 1;
    P1_1 = 0;
    P1_2 = 0;
    P1_3 = 0;
    break;
case 1: //0��1
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
case 3: //1��2
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
case 5: //2��3
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
case 7: //3��0
    P1_0 = 1;
    P1_1 = 0;
    P1_2 = 0;
    P1_3 = 1;
}

step_index++;
if(step_index>7)step_index=0;
		
		}
	
	}
	
	}
j++;	

if(j>=64){speedlevel=1;Motor_StepCounter--;}

	
	}
TR0=0;
	}










/*
void Motor_CompSlowlyDown(void){

char i,j;
speedlevel=2;
spcount=0;
step_index=0;
for(i=0;i<=7;i++){
	for(j=0;j<=15;j++){
		HadBeenDone=0;
		TH0=0xFE;
		TL0=0x0C;
		TR0=1;
		delay(1);
		if(HadBeenDone==1){
			if(spcount<=0){
			spcount=speedlevel;
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
case 1: //0��1
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
case 3: //1��2
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
case 5: //2��3
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
case 7: //3��0
    P1_0 = 1;
    P1_1 = 0;
    P1_2 = 0;
    P1_3 = 1;
}


    step_index--;
    if (step_index<0)
      step_index=7;



			
			}
		}

	
	}
Motor_StepCounter++;
}
TR0=0;
}

void Motor_CompSlowlyUp(void){

char i,j;
speedlevel=2;
spcount=0;
step_index=0;
for(i=0;i<=7;i++){
	for(j=0;j<=15;j++){
		HadBeenDone=0;
		TH0=0xFE;
		TL0=0x0C;
		TR0=1;
		delay(1);
		if(HadBeenDone==1){
			if(spcount<=0){
				spcount=speedlevel;
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
case 1: //0��1
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
case 3: //1��2
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
case 5: //2��3
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
case 7: //3��0
    P1_0 = 1;
    P1_1 = 0;
    P1_2 = 0;
    P1_3 = 1;
}

step_index++;
if(step_index>7)step_index=0;
			}
		
		}
	}
Motor_StepCounter--;
}
TR0=0;
}

*/








//��ʱ��0�жϴ��� 
void timeint(void) interrupt 1 
{ 
TH0=0xFE;
TL0=0x0C; //�趨ʱÿ��0.5ms�ж�һ��

count++;
if(HadBeenDone==0){
HadBeenDone=1;
spcount--;
   }

}

void delay(unsigned int endcount)
{
count=0;
do{}while(count<endcount);
}
