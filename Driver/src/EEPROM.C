#include "IAP15W4K61S4.h"
#include <EEPROM.h>
#include <intrins.h>
#include "nrf24l0102.h"
#include <MPU6050.H>
#include "func.h"

#define CMD_IDLE    0            
#define CMD_READ    1              
#define CMD_PROGRAM 2               
#define CMD_ERASE   3              

#define ENABLE_IAP 0x82           //if SYSCLK<20MHz
//extern char g_fOffsetx,g_fOffsety;            
extern int g_x,g_y,g_z;
//extern double Angle,Angley;
void IapIdle()
{
    IAP_CONTR = 0;                 
    IAP_CMD = 0;                    
    IAP_TRIG = 0;                  
    IAP_ADDRH = 0x80;              
    IAP_ADDRL = 0;
}
unsigned char IapReadByte(unsigned int addr) //��ȡ1�ֽ�
{
    unsigned char dat;                  
    IAP_CONTR = ENABLE_IAP;       
    IAP_CMD = CMD_READ;           
    IAP_ADDRL = addr;             
    IAP_ADDRH = addr >> 8;          
    IAP_TRIG = 0x5a;             
    IAP_TRIG = 0xa5;             
    _nop_();                      
    dat = IAP_DATA;
    IapIdle();	
    return dat;                   
}

void IapProgramByte(unsigned int addr, unsigned char dat) //д��1�ֽ�
{
    IAP_CONTR = ENABLE_IAP;        
    IAP_CMD = CMD_PROGRAM;        
    IAP_ADDRL = addr;             
    IAP_ADDRH = addr >> 8;         
    IAP_DATA = dat;                
    IAP_TRIG = 0x5a;              
    IAP_TRIG = 0xa5;                
    _nop_();    
    IapIdle();	
}

void IapEraseSector(unsigned int addr) //��������
{
    IAP_CONTR = ENABLE_IAP;        
    IAP_CMD = CMD_ERASE;          
    IAP_ADDRL = addr;             
    IAP_ADDRH = addr >> 8;         
    IAP_TRIG = 0x5a;              
    IAP_TRIG = 0xa5;               
    _nop_(); 
    IapIdle();	
}
void IAP_Gyro()   //������У׼
{
	IapEraseSector(0xE200); //��������
	g_fGyroYOffset = DataSynthesis(GYRO_YOUT_H);  //��ȡ����������
	g_fGyroXOffset = DataSynthesis(GYRO_XOUT_H);
	g_fGyroZOffset = DataSynthesis(GYRO_ZOUT_H);
	IapProgramByte(0xE200,g_fGyroYOffset>>8);  //д��������EEPROM
	IapProgramByte(0xE201,g_fGyroYOffset); 
	IapProgramByte(0xE202,g_fGyroXOffset>>8);     //���������ݴ����114����
	IapProgramByte(0xE203,g_fGyroXOffset);
	IapProgramByte(0xE204,g_fGyroZOffset>>8);
	IapProgramByte(0xE205,g_fGyroZOffset);
	IapProgramByte(0xE206,1);
}
void IAP_Angle()
{
	IapEraseSector(0xE400);           //���ٶȼ����ݴ����115����
	//a_x=(char)Angle;
	//a_y=(char)Angley;	 
	IapProgramByte(0xE400,1);
	IapProgramByte(0xE401,g_fOffsetx);
	IapProgramByte(0xE402,g_fOffsety);
}
void IAPRead()
{
	unsigned char i;
	i=IapReadByte(0xE206);
	if(i==1)      //�ж������Ƿ��Ѿ����¹������¹��Ŷ�ȡ����
	{
	g_y=IapReadByte(0xE200)<<8|IapReadByte(0xE201);
	g_x=IapReadByte(0xE202)<<8|IapReadByte(0xE203);
	g_z=IapReadByte(0xE204)<<8|IapReadByte(0xE205);
	}
	i=IapReadByte(0xE400);
	if(i==1)      //�ж������Ƿ��Ѿ����¹������¹��Ŷ�ȡ����
	{
	g_fOffsetx=IapReadByte(0xE401);
	g_fOffsety=IapReadByte(0xE402);
	}
}
