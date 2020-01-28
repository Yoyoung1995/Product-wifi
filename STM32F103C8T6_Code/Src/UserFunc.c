/****************   用户功能函数存储C文件 —— John ************/
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


//--------------------------------  Wifi硬件设备配置任务 函数仓库
SSS DeviceSet={0};	//用户配置参数

//初始化值，是每一项参数都必须要有的，所有相关值都需要初始化，包括Flash
void Init_FirstTime(void)
{
		//初始化值
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
		
	
		//1.重启模块   不含字符串结束符
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+RST\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );
		osDelay(2000);
		//2.设置模块
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CWMODE=3\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
		osDelay(2500);
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+RST\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );
		osDelay(2000);		
		//3.配置AP参数
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CWSAP=\"Sensor03\",\"communication\",1,3\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );		
		osDelay(2000);		//延时未确定
		//注意事项测试
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+RST\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );
		osDelay(2000);
		//4.设置AP下的静态IP
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPAP=\"192.168.1.1\",\"192.168.1.1\",\"255.255.255.0\"\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
		osDelay(2000);		//延时未确定
		//5.开启多连接模式
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPMUX=1\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
		osDelay(1000);
		//6.创建服务器
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPSERVER=1,8080\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
		osDelay(2000);		//延时未确定		
		//7.设置服务器超时时间
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPSTO=3600\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
		osDelay(2000);		//延时未确定
		//(初始化结束)  --- 测试通过标准，手机能搜到并连上wifi，能访问wifi服务器。 改名，再次测试
		//测试通过，但需要几分钟后才能连上wifi
}

void Init_SecondTime(void)
{
		//0.重启模块   不含字符串结束符
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+RST\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );
		osDelay(2000);		
		//1.开启多连接模式
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPMUX=1\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );
		osDelay(1000);
		//2.创建服务器
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPSERVER=1,8080\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );
		osDelay(2000);		//延时未确定
		//3.设置服务器超时时间
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPSTO=3600\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
		osDelay(2000);		//延时未确定	
		//提取EEPROM - 用户配置参数 提取至DeviceSet
		//ReadEEPROM();  前面初始化时已提取过一次，无需再次提取
		//4.建立TCP连接
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPSTART=2,\"TCP\",\"%s\",%hd\r\n",DeviceSet.IP,DeviceSet.Port );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
		osDelay(5000);			
}	



//Flash 写入一个char数组 (一次写入两个字节)   len 整型数组的长度 = (数组实际长度+1)/2
uint32_t Flash_Write(uint16_t * myadd,uint8_t len)
{
	uint32_t Robot_Num_Flash_Add = Flash_Add;
	FLASH_EraseInitTypeDef My_Flash;
	uint32_t PageError = 0; //如果出错，返回出错的地址
	uint16_t Write_Flash_Data = 0;
	uint8_t i=0;
	
	HAL_FLASH_Unlock(); 	//解锁Flash
	My_Flash.TypeErase = FLASH_TYPEERASE_PAGES; //擦除页
	My_Flash.PageAddress = Robot_Num_Flash_Add;
	My_Flash.NbPages =1;   //要擦除的页数
	HAL_FLASHEx_Erase(&My_Flash,&PageError);	//调用擦除函数
	
	for(i=0;i<len;i++)
	{
		Write_Flash_Data = *(myadd+i);
		Robot_Num_Flash_Add = (Robot_Num_Flash_Add + 2);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,Robot_Num_Flash_Add,Write_Flash_Data); //对flash进行烧写，1次两个字节
	}
	
	HAL_FLASH_Lock(); //锁住Flash
	return PageError;
}


//Flash  读出一个 char数组 (一次读出2个字节)  len 整型数组的长度 = (要读出 数组实际长度+1)/2
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

//录入EEPROM   从Flash 48K处开始 0x0800 C000  
void RecordInEEPROM(void)
{
	uint8_t buf[SSS_Len]={0};
	
	memcpy(buf,(char *)&DeviceSet,SSS_Len);  //结构体-->内存数组
	
	//将buff内存数组 写入Flash 
	Flash_Write( (uint16_t *)buf,(SSS_Len+1)/2 );
}

//提取EEPROM  Flash_Add处地址的FLash
void ReadEEPROM(void)
{
	uint8_t buf[SSS_Len+10]={0}; //预留点空间
	
	// 读出flash 存入buff数组
	Flash_Read( (uint16_t *)buf,(SSS_Len+1)/2, Flash_Add);
	
	memcpy((char *)&DeviceSet,buf,SSS_Len);
}

//刷新wifi配置 并 配置参数录入Flash
void Refresh_Set(void)
{
		DeviceSet.FirstTimeSet_Or_Not = 0x55;
	
		//录入EEPROM
		vTaskSuspendAll(); //关调度
		RecordInEEPROM();
		xTaskResumeAll ();//开调度
	
	 	//更改AP参数设备名
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CWSAP=\"%s\",\"communication\",1,3\r\n",DeviceSet.Name );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );		
		osDelay(2000);		//延时未确定		
		//执行加入wifi路由器
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CWJAP=\"%s\",\"%s\"\r\n",DeviceSet.C_Name,DeviceSet.C_Key );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
		osDelay(18000);	
				//暂缺 确认是否真正接上
		//TCP/IP网络连接
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPSTART=2,\"TCP\",\"%s\",%hd\r\n",DeviceSet.IP,DeviceSet.Port );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
		osDelay(5000);	
	
		//检查连接状态并反馈
		snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPSTATUS\r\n" );
		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
		osDelay(200);
		//!!!!不能这么判断TCP连接是否成功
		if( ( UsartType.RX_pData[UsartType.RX_Size-2]=='O' ) && ( UsartType.RX_pData[UsartType.RX_Size-1]=='K' ) ) //连接成功
		{
			//反馈手机
			snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPSEND=0,7\r\n" );  // "ok"3字节+数据帧结构4字节
			HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
			osDelay(100);
			UsartTx[0] = 0xA5;
			UsartTx[1] = 0x5A;
			UsartTx[2] = 4;
			UsartTx[3] = 0x04;
			strncpy((char *)&UsartTx[4],"ok",3 );
			HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx)+1 ); //发送字符串，含字符串结束符
		}
		else
		{
			//反馈手机
			snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPSEND=0,10\r\n" );  // "false"6字节+数据帧结构4字节
			HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
			osDelay(100);
			UsartTx[0] = 0xA5;
			UsartTx[1] = 0x5A;
			UsartTx[2] = 4;
			UsartTx[3] = 0x04;
			strncpy((char *)&UsartTx[4],"false",6 );
			HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx)+1 ); //发送字符串，含字符串结束符		
		}
		;
}


//--------------------------------  Wifi网络连接任务
//(删去--保留不用)

//------------------------------------ 各类传感器数据获取 任务 ----------------------------
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
	vTaskSuspendAll(); //关调度
	HAL_I2C_Master_Transmit(&hi2c1,ADDR_AGS01DB_Write,&IIC_WriteBuf[0],2,0x100);

	HAL_Delay(5);	//STM32F1 硬件IIC Bug 修复
	
	HAL_I2C_Master_Receive(&hi2c1,ADDR_AGS01DB_Read,&IIC_ReadBuf[0],3,0x100);
	xTaskResumeAll ();//开调度
	//数据处理
	buf = AGS01DB_Calc_CRC8(&IIC_ReadBuf[0],2);
	if( IIC_ReadBuf[2]==buf )	//CRC校验
	{
		SensorData.VOC = IIC_ReadBuf[0];
		SensorData.VOC <<= 8;
		SensorData.VOC += IIC_ReadBuf[1];
		//VOC气体浓度 = SensorData.VOC * 0.1 PPM
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
