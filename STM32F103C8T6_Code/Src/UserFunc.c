/****************   �û����ܺ����洢C�ļ� ���� John ************/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "usart.h"
#include <string.h>
#include "UserFunc.h"

void ReadEEPROM(void);
void RecordInEEPROM(void);

extern I2C_HandleTypeDef hi2c1;
uint8_t AGS01DB_Calc_CRC8(uint8_t *data, uint8_t Num);


//--------------------------------  WifiӲ���豸�������� �����ֿ�
SSS DeviceSet={0};	//�û����ò���

//��ʼ��ֵ����ÿһ�����������Ҫ�еģ��������ֵ����Ҫ��ʼ��������Flash
void Init_FirstTime(void)
{
		//��ʼ��ֵ
		strncpy( (char *)DeviceSet.Name,"Sensor",sizeof(DeviceSet.Name) );
		strncpy( (char *)DeviceSet.IP,"192.168.0.1",sizeof(DeviceSet.IP) );		
		DeviceSet.Port = 9999;
		strncpy( (char *)DeviceSet.C_Name,"LuYouQi",sizeof(DeviceSet.C_Name) );
		strncpy( (char *)DeviceSet.C_Key,"123456",sizeof(DeviceSet.C_Key) );		
		DeviceSet.Mode = 2;
		DeviceSet.T    = 5;
		DeviceSet.AHT15_EN = 2;
		DeviceSet.VOC_EN =2;
		DeviceSet.PM2_5_EN =2;
		DeviceSet.Light_EN =2;
		DeviceSet.Noice_EN =2;
		DeviceSet.Refresh_Set_Control =2;
		RecordInEEPROM();
		
	
		//1.����ģ��   �����ַ���������
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+RST\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );
		osDelay(2000);
		//2.����ģ��
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CWMODE=3\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
		osDelay(2500);
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+RST\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );
		osDelay(2000);		
		//3.����AP����
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CWSAP=\"Sensor03\",\"communication\",1,3\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );		
		osDelay(2000);		//��ʱδȷ��
		//ע���������
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+RST\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );
		osDelay(2000);
		//4.����AP�µľ�̬IP
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPAP=\"192.168.1.1\",\"192.168.1.1\",\"255.255.255.0\"\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
		osDelay(2000);		//��ʱδȷ��
		//5.����������ģʽ
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPMUX=1\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
		osDelay(1000);
		//6.����������
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPSERVER=1,8080\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
		osDelay(2000);		//��ʱδȷ��		
		//7.���÷�������ʱʱ��
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPSTO=3600\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
		osDelay(2000);		//��ʱδȷ��
		//(��ʼ������)  --- ����ͨ����׼���ֻ����ѵ�������wifi���ܷ���wifi�������� �������ٴβ���
		//����ͨ��������Ҫ�����Ӻ��������wifi
}

void Init_SecondTime(void)
{
		//0.����ģ��   �����ַ���������
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+RST\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );
		osDelay(2000);		
		//1.����������ģʽ
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPMUX=1\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );
		osDelay(1000);
		//2.����������
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPSERVER=1,8080\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );
		osDelay(2000);		//��ʱδȷ��
		//3.���÷�������ʱʱ��
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPSTO=3600\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
		osDelay(2000);		//��ʱδȷ��	
		//��ȡEEPROM - �û����ò��� ��ȡ��DeviceSet
		//ReadEEPROM();  ǰ���ʼ��ʱ����ȡ��һ�Σ������ٴ���ȡ
		//4.����TCP����
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPSTART=2,\"TCP\",\"%s\",%hd\r\n",DeviceSet.IP,DeviceSet.Port );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
		osDelay(5000);			
}	



//Flash д��һ��char���� (һ��д�������ֽ�)   len ��������ĳ��� = (����ʵ�ʳ���+1)/2
uint32_t Flash_Write(uint16_t * myadd,uint8_t len)
{
	uint32_t Robot_Num_Flash_Add = Flash_Add;
	FLASH_EraseInitTypeDef My_Flash;
	uint32_t PageError = 0; //����������س���ĵ�ַ
	uint16_t Write_Flash_Data = 0;
	uint8_t i=0;
	
	HAL_FLASH_Unlock(); 	//����Flash
	My_Flash.TypeErase = FLASH_TYPEERASE_PAGES; //����ҳ
	My_Flash.PageAddress = Robot_Num_Flash_Add;
	My_Flash.NbPages =1;   //Ҫ������ҳ��
	HAL_FLASHEx_Erase(&My_Flash,&PageError);	//���ò�������
	
	for(i=0;i<len;i++)
	{
		Write_Flash_Data = *(myadd+i);
		Robot_Num_Flash_Add = (Robot_Num_Flash_Add + 2);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,Robot_Num_Flash_Add,Write_Flash_Data); //��flash������д��1�������ֽ�
	}
	
	HAL_FLASH_Lock(); //��סFlash
	return PageError;
}


//Flash  ����һ�� char���� (һ�ζ���2���ֽ�)  len ��������ĳ��� = (Ҫ���� ����ʵ�ʳ���+1)/2
void Flash_Read(uint16_t * dest,uint8_t len, uint32_t Add )
{
	uint32_t Robot_Num_Flash_Add = Add;
	uint8_t i = 0;
	
	for(i=0;i<len;i++)
	{
		Robot_Num_Flash_Add = (Robot_Num_Flash_Add + 2);
		*(dest + i) = *(__IO uint16_t *)(Robot_Num_Flash_Add);
	}
}

//¼��EEPROM   ��Flash 48K����ʼ 0x0800 C000  
void RecordInEEPROM(void)
{
	uint8_t buf[SSS_Len]={0};
	
	memcpy(buf,(char *)&DeviceSet,SSS_Len);  //�ṹ��-->�ڴ�����
	
	//��buff�ڴ����� д��Flash 
	Flash_Write( (uint16_t *)buf,(SSS_Len+1)/2 );
}

//��ȡEEPROM  Flash_Add����ַ��FLash
void ReadEEPROM(void)
{
	uint8_t buf[SSS_Len+10]={0}; //Ԥ����ռ�
	
	// ����flash ����buff����
	Flash_Read( (uint16_t *)buf,(SSS_Len+1)/2, Flash_Add);
	
	memcpy((char *)&DeviceSet,buf,SSS_Len);
}

//ˢ��wifi���� �� ���ò���¼��Flash
void Refresh_Set(void)
{
		DeviceSet.FirstTimeSet_Or_Not = 0x55;
	
		//¼��EEPROM
		vTaskSuspendAll(); //�ص���
		RecordInEEPROM();
		xTaskResumeAll ();//������
	
	 	//����AP�����豸��
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CWSAP=\"%s\",\"communication\",1,3\r\n",DeviceSet.Name );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );		
		osDelay(2000);		//��ʱδȷ��		
		//ִ�м���wifi·����
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CWJAP=\"%s\",\"%s\"\r\n",DeviceSet.C_Name,DeviceSet.C_Key );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
		osDelay(18000);	
				//��ȱ ȷ���Ƿ���������
		//TCP/IP��������
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPSTART=2,\"TCP\",\"%s\",%hd\r\n",DeviceSet.IP,DeviceSet.Port );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
		osDelay(5000);	
	
		//�������״̬������
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPSTATUS\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
		osDelay(200);
		//!!!!������ô�ж�TCP�����Ƿ�ɹ�
		if( ( UsartType.RX_pData[UsartType.RX_Size-2]=='O' ) && ( UsartType.RX_pData[UsartType.RX_Size-1]=='K' ) ) //���ӳɹ�
		{
			//�����ֻ�
			snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPSEND=0,7\r\n" );  // "ok"3�ֽ�+����֡�ṹ4�ֽ�
			HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
			osDelay(100);
			UsartTx[0] = 0xA5;
			UsartTx[1] = 0x5A;
			UsartTx[2] = 4;
			UsartTx[3] = 0x04;
			strncpy((char *)&UsartTx[4],"ok",3 );
			HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx)+1 ); //�����ַ��������ַ���������
		}
		else
		{
			//�����ֻ�
			snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPSEND=0,10\r\n" );  // "false"6�ֽ�+����֡�ṹ4�ֽ�
			HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
			osDelay(100);
			UsartTx[0] = 0xA5;
			UsartTx[1] = 0x5A;
			UsartTx[2] = 4;
			UsartTx[3] = 0x04;
			strncpy((char *)&UsartTx[4],"false",6 );
			HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx)+1 ); //�����ַ��������ַ���������		
		}
		;
}


//--------------------------------  Wifi������������
//(ɾȥ--��������)

//------------------------------------ ���ഫ�������ݻ�ȡ ���� ----------------------------
DDD SensorData={0};

uint8_t IIC_WriteBuf[64]={0};
uint8_t IIC_ReadBuf[64]={0};

void Get_AHT15_Data(void)
{
	;
}

void Get_AGS01DB_Data(void)
{
	uint8_t buf=0;
	
	IIC_WriteBuf[0]=0x00;
	IIC_WriteBuf[1]=0x02;
	vTaskSuspendAll(); //�ص���
	HAL_I2C_Master_Transmit(&hi2c1,ADDR_AGS01DB_Write,&IIC_WriteBuf[0],2,0x100);

	HAL_Delay(5);	//STM32F1 Ӳ��IIC Bug �޸�
	
	HAL_I2C_Master_Receive(&hi2c1,ADDR_AGS01DB_Read,&IIC_ReadBuf[0],3,0x100);
	xTaskResumeAll ();//������
	//���ݴ���
	buf = AGS01DB_Calc_CRC8(&IIC_ReadBuf[0],2);
	if( IIC_ReadBuf[2]==buf )	//CRCУ��
	{
		SensorData.VOC = IIC_ReadBuf[0];
		SensorData.VOC <<= 8;
		SensorData.VOC += IIC_ReadBuf[1];
		//VOC����Ũ�� = SensorData.VOC * 0.1 PPM
	}
	
}

uint8_t AGS01DB_Calc_CRC8(uint8_t *data, uint8_t Num)
{
	uint8_t bit,byte,crc=0xFF;
	for(byte=0; byte<Num; byte++)
	{
		crc^=(data[byte]);
		for(bit=8;bit>0;--bit)
		{
			if(crc&0x80) crc=(crc<<1)^0x31;
			else crc=(crc<<1);
		}
	}
	return crc;
}
