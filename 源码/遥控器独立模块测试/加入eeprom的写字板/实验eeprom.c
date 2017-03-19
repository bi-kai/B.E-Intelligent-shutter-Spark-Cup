//进入新编的写函数时将1到7的所有扇区全部擦除


void New_Temprature(void){

Model4_WTemprature[0]=display[2];
Model4_WTemprature[1]=display[1];
Model4_WTemprature[2]=display[0];

}




uchar Model4_WTemprature[3];
uint Model4_Eeprom_Counter=0x2200;

bit Model4_SendFlag=0;
uint Model4_SendData_Counter=0x2200;
uchar Model4_STemprature[3];


void Model4_Sve_Data(void){

	uchar M4_Wi;
	uint M4_Wj=0,disc=0;

	
	IAP_CONTR = ENABLE_ISP;         //打开IAP 功能, 设置Flash 操作等待时间 
	for(disc=0;disc<=6;disc++){

	EEPROM_erase(0x2000+(disc+1)*0x200);
	}
	EEPROM_IAP_Disable();
		
	if(((0x2fff-Model4_Eeprom_Counter)>=3)&&(Model4_Eeprom_Counter<=0x2fff)){
			New_Temprature();
			for(M4_Wi=0;M4_Wi<=2;M4_Wi++){
				EEPROM_write(Model4_Eeprom_Counter,Model4_WTemprature[M4_Wi]);
				M4_Wi++;
				M4_Wj++;
			}	
	}

else {M4_Wj=0;}

//if((M4_Wj%509)==1){disc=(0x2000+((M4_Wj/511)+2)*0x200+256);EEPROM_erase(disc);}

EEPROM_IAP_Disable(); //关闭IAP 功能, 清相关的特殊功能寄存器,使CPU 处于安全状态
}




void Model4_Send_Data(void){

	uchar M4_Si,M4_Sj;
	if(((Model4_SendFlag==0)&&((0x2fff-Model4_SendData_Counter)>=3)&&(Model4_SendData_Counter<0x2fff)){
		IAP_CONTR = ENABLE_ISP;         //打开IAP 功能, 设置Flash 操作等待时间
		for(M4_Si=0;M4_Si<=2;M4_Si++){
			Model4_STemprature[M4_Si]=EEPROM_read(Model4_SendData_Counter);
			Model4_SendData_Counter++;
			M4_Si++;		
		}
	send(Model4_STemprature);
	send("\x20");
	
	}

else if(((0x2fff-Model4_Eeprom_Counter)<3)&&(Model4_SendFlag==0)){
		Model4_SendFlag=1;send("Sanding Over");}

}



