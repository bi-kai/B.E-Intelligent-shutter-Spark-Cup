#include<reg51.h>
#include<intrins.h>

#define uchar unsigned char


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


uchar KeyScan(void){

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

	PT2262_Data0=1;PT2262_Data1=1;PT2262_Data2=1;PT2262_Data3=1;
	PT2262_TE=1;
 //  Self_BK_Define3();
	Button_Command=KeyScan();
	if(Button_Command==0){PT2262_Data0=1;PT2262_Data1=1;PT2262_Data2=1;PT2262_Data3=1;
		
	lcd_pos(0x40);
	n=0;
	while(Function6[n]!='\0'){
	lcd_wdat(Function6[n]);
			n++;
			}
}	

else if(Button_Command==1){PT2262_Data0=1;PT2262_Data1=1;PT2262_Data2=1;PT2262_Data3=0;
		
	lcd_pos(0x40);
	n=0;
	while(Function1[n]!='\0'){
	lcd_wdat(Function1[n]);
			n++;
			}
}	

else if(Button_Command==2){PT2262_Data0=0;PT2262_Data1=1;PT2262_Data2=1;PT2262_Data3=0;
		
	lcd_pos(0x40);
	n=0;
	while(Function2[n]!='\0'){
	lcd_wdat(Function2[n]);
			n++;
			}
}	


else if(Button_Command==3){PT2262_Data0=1;PT2262_Data1=1;PT2262_Data2=0;PT2262_Data3=0;
		
	lcd_pos(0x40);
	n=0;
	while(Function3[n]!='\0'){
	lcd_wdat(Function3[n]);
			n++;
			}
}	


else if(Button_Command==4){PT2262_Data0=0;PT2262_Data1=1;PT2262_Data2=0;PT2262_Data3=0;
		
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

}//最后一行



















































































