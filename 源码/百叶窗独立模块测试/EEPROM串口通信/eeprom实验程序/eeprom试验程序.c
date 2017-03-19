#include < reg51.h >
#include < intrins.h >

#define uchar unsigned char
#define uint  unsigned int

sbit DQ = P1^4 ;  //����DS18B20�˿�DQ
sbit BEEP=P1^3  ; //������������

bit presence  ;

sbit LCD_RS = P2^2 ;
sbit LCD_RW = P2^3 ;
sbit LCD_EN = P2^4 ;

uchar code  cdis1[ ] = {" welcome to JLDZ "} ;//���ֵ�����д
uchar code  cdis2[ ] = {" WENDU:    .  C "} ;
uchar code  cdis3[ ] = {" DS18B20  ERR0R "} ;
uchar code  cdis4[ ] = {"  PLEASE CHECK  "} ;

unsigned char data  temp_data[2] = {0x00,0x00} ;
unsigned char data  display[5] =   {0x00,0x00,0x00,0x00,0x00} ;
unsigned char code  ditab[16] =    {0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x04,
                                                        0x05,0x06,0x06,0x07,0x08,0x08,0x09,0x09} ;///*С��λ����������*/
//
void beep() ;
unsigned char code  mytab[8] = {0x0C,0x12,0x12,0x0C,0x00,0x00,0x00,0x00} ;

#define delayNOP() ; {_nop_() ;_nop_() ;_nop_() ;_nop_() ;} ;

/*************************************************************************************/
/********************************eeprom���벿��һ*************************************/

uchar code word[]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x20};
uchar dispbuf[8],h=12,m,s,counter;

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

/***********************************eeprom����ĵ�������******************************/
/*************************************************************************************/
/*************************************************************************************/
#define ENABLE_ISP 0x80 //EEPROM�����������ã�ϵͳ����ʱ��<40MHz ʱ����IAP_CONTR �Ĵ������ô�ֵ

sfr IAP_DATA    = 0xe2;
sfr IAP_ADDRH   = 0xe3;
sfr IAP_ADDRL   = 0xe4;
sfr IAP_CMD     = 0xe5;
sfr IAP_TRIG    = 0xe6;
sfr IAP_CONTR   = 0xe7;
//����Flash �����ȴ�ʱ�估����IAP/ISP/EEPROM �����ĳ���

union union_temp16{unsigned int un_temp16;unsigned char un_temp8[2];}my_unTemp16;

//unsigned char temp[20]="";

unsigned char EEPROM_read(unsigned int add);   //��һ�ֽڣ�����ǰ���IAP ����
void EEPROM_write(unsigned int, unsigned char ch); //�ֽڱ�̣�����ǰ���IAP ����
void EEPROM_erase(unsigned int add);            //��������
void EEPROM_IAP_Disable();                       //�ر�IAP ����

void save_data_to_EEPROM(unsigned char num);

void send(unsigned char str[]);
void lcd_pos(uchar pos);
/************************************************    EEPROM��������   **************************************************/

unsigned char EEPROM_read(unsigned int add)	 //��һ�ֽڣ�����ǰ���IAP ���ܣ����:DPTR = �ֽڵ�ַ������:A = �����ֽ�
{
    IAP_DATA = 0x00;
    IAP_CMD = 0x01;                 //IAP/ISP/EEPROM �ֽڶ�����
    my_unTemp16.un_temp16 = add;
    IAP_ADDRH = my_unTemp16.un_temp8[0];    //����Ŀ�굥Ԫ��ַ�ĸ�8 λ��ַ
    IAP_ADDRL = my_unTemp16.un_temp8[1];    //����Ŀ�굥Ԫ��ַ�ĵ�8 λ��ַ
    //EA = 0;
    IAP_TRIG = 0x46;   //   46???   ���� 5Ah,����A5h ��ISP/IAP �����Ĵ���,ÿ�ζ������
    IAP_TRIG = 0xB9;   //   B9???   ����A5h ��ISP/IAP ����������������
    _nop_();
    //EA = 1;
    return (IAP_DATA);
}


void EEPROM_write(unsigned int add, unsigned char ch) //�ֽڱ�̣�����ǰ���IAP ���ܣ����:DPTR = �ֽڵ�ַ, A= �����ֽڵ�����
{
    IAP_CMD = 0x02;                 //IAP/ISP/EEPROM �ֽڱ������
    my_unTemp16.un_temp16 = add;
   	IAP_ADDRH = my_unTemp16.un_temp8[0];    //����Ŀ�굥Ԫ��ַ�ĸ�8 λ��ַ
   	IAP_ADDRL = my_unTemp16.un_temp8[1];    //����Ŀ�굥Ԫ��ַ�ĵ�8 λ��ַ
   	IAP_DATA = ch;                  //Ҫ��̵��������ͽ�IAP_DATA �Ĵ���
   	//EA = 0;
   	IAP_TRIG = 0x46;   //   46???   ���� 5Ah,����A5h ��ISP/IAP �����Ĵ���,ÿ�ζ������
   	IAP_TRIG = 0xB9;   //   B9???   ����A5h ��ISP/IAP ����������������
    _nop_();
   	//EA = 1;
}


void EEPROM_erase(unsigned int add)//��������, ���:DPTR = ������ַ
{
	IAP_CMD = 0x03;                 //IAP/ISP/EEPROM ������������
    my_unTemp16.un_temp16 = add;
    IAP_ADDRH = my_unTemp16.un_temp8[0];    //����Ŀ�굥Ԫ��ַ�ĸ�8 λ��ַ
    IAP_ADDRL = my_unTemp16.un_temp8[1];    //����Ŀ�굥Ԫ��ַ�ĵ�8 λ��ַ

    //EA = 0;
    IAP_TRIG = 0x46;   //   46???   ���� 5Ah,����A5h ��ISP/IAP �����Ĵ���,ÿ�ζ������
    IAP_TRIG = 0xB9;   //   B9???   ����A5h ��ISP/IAP ����������������
    _nop_();
    //EA = 1;
}

void EEPROM_IAP_Disable()
{
    //�ر�IAP ����, ����ص����⹦�ܼĴ���,ʹCPU ���ڰ�ȫ״̬
    //һ��������IAP �������֮����ر�IAP ����,����Ҫÿ�ζ���
	IAP_CONTR = 0;      //�ر�IAP ����
	IAP_CMD   = 0;      //������Ĵ���,ʹ����Ĵ���������,�˾�ɲ���
	IAP_TRIG = 0;      //��������Ĵ���,ʹ������Ĵ����޴���,�˾�ɲ���
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

/**********************************�Լ�����ĺ���*********************************************/
uint eeprom_address=0x2000;
uchar Model4_WTime[8];
uchar Model4_WTem[8];
uchar Model4_RTime[9];
uchar Model4_RTem[9];
uchar Model4_rkey;
uchar sp=0;
long k=0;				//��������������
uint Model4_EEPROM_Address=0x2000;
uchar Model4_rkey;
bit Model4_SendFlag=0;		//Ϊ0�����ͣ�Ϊ1�������ͣ���־λ��
uchar Model4_Key=0;			//ģʽ�ļ���ɨ��
/*******************************/
sbit Bu_SetZero=P1^7;			//ģʽ�ĸ��ã����·���
sbit Bu_MoveUp=P3^6;				//ģʽ�ĸ��ã���ʼ������Ϣ
sbit Bu_MoveDown=P3^7;			//ģʽ�ĸ��ã�ֹͣ������Ϣ
/**************************��������************************/
void INIT_NewTemp(void);
void send_data_from_EEPROM(void);
uchar Model4_ChangeForSave(uchar ucfs);
void send_data_from_EEPROM(void);
uchar m4_n;	//ģʽ������ʾ����
/*****************************/
void delay1(int ms);
void lcd_wdat(uchar dat);
void lcd_pos(uchar pos);
uchar Menu_INT1=0;
uchar BK_count; //����ר��
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
	IAP_CONTR = ENABLE_ISP;         //��IAP ����, ����Flash �����ȴ�ʱ��

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
	
	EEPROM_IAP_Disable(); //�ر�IAP ����, ����ص����⹦�ܼĴ���,ʹCPU ���ڰ�ȫ״̬
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
	Model4_WTime[2]='\x3A';	//ð��
	Model4_WTime[3]=Model4_ChangeForSave(dispbuf[4]);
	Model4_WTime[4]=Model4_ChangeForSave(dispbuf[3]);
	Model4_WTime[5]='\x3A';	//ð��
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
	
		IAP_CONTR = ENABLE_ISP;         //��IAP ����, ����Flash �����ȴ�ʱ��
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

/****************************ģʽ�İ���ɨ��*****************************************/
uchar Model4_KeyScan(void){

Bu_SetZero=1;			//ģʽ�ĸ��ã����·���
Bu_MoveUp=1;				//ģʽ�ĸ��ã���ʼ������Ϣ
Bu_MoveDown=1;			//ģʽ�ĸ��ã�ֹͣ������Ϣ
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
void Self_BK_Define4(void){    //����ģʽ�ģ���ʾ��������

	uchar  m ;
    lcd_pos(0) ;                //������ʾλ��Ϊ��һ�еĵ�1���ַ�
     m = 0 ;
    while(cdis7[m]!= '\0')
     {                         //��ʾ�ַ�
       lcd_wdat(cdis7[m]) ;
       m++ ;
     }

    lcd_pos(0x40) ;             //������ʾλ��Ϊ�ڶ��е�1���ַ�
     m = 0 ;
    while(Function12[m] != '\0')
     {
       lcd_wdat(Function12[m]) ;      //��ʾ�ַ�
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
/***********************************eeprom���벿��һ����*******************************/
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
/*���LCDæ״̬                                                   */
/*lcd_busyΪ1ʱ��æ���ȴ���lcd-busyΪ0ʱ,�У���дָ�������ݡ�   */
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
void  Ok_Menu ()
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
/********************************************************************************************************/
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
/* ������     */
/************************************/
void main()
 {
     Ok_Menu () ;
	  init();
/**********************************����ĵڶ�����*************************************/
/****************************************************************************************/
SCON=0x50;//���ڷ�ʽ1���������
   TMOD=0x20;//��ʱ��1����ʱ��ʽ2
//   TCON=0x40;//�趨ʱ��1��ʼ����
	ET1=1;
   TR1=1;
   TH1=0xE8;//11.0592MHz, 1200������
   TL1=0xE8;
   TI=1;
   TR1=1;
	IAP_CONTR = ENABLE_ISP; 
	EEPROM_erase(0x2000);
EEPROM_IAP_Disable();
/************����ĵڶ����ֽ���**********************************************************/
while(1){ 
if(((Menu_INT1)%2)==0){	MainModel1();}
else 	MainModel4();
}

}
/*�ⲿ�ж�*/
void int1_Menu_INT1() interrupt 2  
	{
		Menu_INT1++;
		
		if(Menu_INT1==8)Menu_INT1=0;
	
	}
