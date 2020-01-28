//--------------------------------  WifiӲ���豸��������
#include "stm32f1xx_hal.h"

typedef struct
{
	uint8_t Name[64];
	uint8_t IP[20];
	uint16_t Port;
	uint8_t C_Name[32];
	uint8_t C_Key[32];
	uint8_t Mode;
	uint16_t T;
	uint8_t AHT15_EN;		//AHT15
	uint8_t VOC_EN;			//AGS01DB
	uint8_t PM2_5_EN;		//�ͺŴ���
	uint8_t Light_EN;		//����
	uint8_t Noice_EN;		//����
	uint8_t Refresh_Set_Control;  // wifiˢ������ͳһ���ư�ť
	
	uint8_t FirstTimeSet_Or_Not;  // ֵ=0x55ʱ������״�����
}SSS;

extern SSS DeviceSet;	
#define SSS_Len sizeof(SSS)
#define Flash_Add  0x0800C000 

extern void Init_FirstTime(void);
extern void Init_SecondTime(void);
extern void ReadEEPROM(void);    //��Device �洢��Flash�е����ݶ���
extern void Refresh_Set(void);

//------------------------------------ ���ഫ�������ݻ�ȡ -----------------------------------
#define ADDR_AGS01DB_Write 0x22
#define ADDR_AGS01DB_Read  0x23

typedef struct
{
	uint16_t Temperature;
	uint16_t Wet;
	uint16_t VOC;
	uint16_t PM2_5;
	uint16_t PM10;
	uint16_t Light;
	uint16_t Noice;
}DDD;

extern DDD SensorData;

extern void Get_AHT15_Data(void);

extern void Get_AGS01DB_Data(void);
