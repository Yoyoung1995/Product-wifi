/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2020 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     
#include "usart.h"
#include <string.h>
#include <stdlib.h>
#include "UserFunc.h"
#include "stm32f1xx_hal.h"
#include "iwdg.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId Task_GetSensorsDataHandle;
osThreadId Task_ReportDataHandle;
osThreadId Task_DeviceSettingHandle;
osThreadId Task_NetworkLinkHandle;
osThreadId Task_RXProductProtocolAnalyseHandle;
osTimerId myTimer1sHandle;
osSemaphoreId BinarySem_Task_ReportDataHandle;
osSemaphoreId BinarySem_Task_DeviceSettingHandle;
osSemaphoreId BinarySem_Task_NetworkLinkHandle;
osSemaphoreId BinarySem_Task_RXProductProtocolAnalyseHandle;

/* USER CODE BEGIN Variables */
uint16_t softTimerCount = 0;    //RTOS软件定时器  单位：秒
/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void Func_GetSensorsData(void const * argument);
void Func_ReportData(void const * argument);
void Func_DeviceSetting(void const * argument);
void Func_NetworkLink(void const * argument);
void Func_RXProductProtocolAnalyse(void const * argument);
void myTimer1s_Callback(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of BinarySem_Task_ReportData */
  osSemaphoreDef(BinarySem_Task_ReportData);
  BinarySem_Task_ReportDataHandle = osSemaphoreCreate(osSemaphore(BinarySem_Task_ReportData), 1);

  /* definition and creation of BinarySem_Task_DeviceSetting */
  osSemaphoreDef(BinarySem_Task_DeviceSetting);
  BinarySem_Task_DeviceSettingHandle = osSemaphoreCreate(osSemaphore(BinarySem_Task_DeviceSetting), 1);

  /* definition and creation of BinarySem_Task_NetworkLink */
  osSemaphoreDef(BinarySem_Task_NetworkLink);
  BinarySem_Task_NetworkLinkHandle = osSemaphoreCreate(osSemaphore(BinarySem_Task_NetworkLink), 1);

  /* definition and creation of BinarySem_Task_RXProductProtocolAnalyse */
  osSemaphoreDef(BinarySem_Task_RXProductProtocolAnalyse);
  BinarySem_Task_RXProductProtocolAnalyseHandle = osSemaphoreCreate(osSemaphore(BinarySem_Task_RXProductProtocolAnalyse), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of myTimer1s */
  osTimerDef(myTimer1s, myTimer1s_Callback);
  myTimer1sHandle = osTimerCreate(osTimer(myTimer1s), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
	osTimerStart(myTimer1sHandle,1000);
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of Task_GetSensorsData */
  osThreadDef(Task_GetSensorsData, Func_GetSensorsData, osPriorityNormal, 0, 128);
  Task_GetSensorsDataHandle = osThreadCreate(osThread(Task_GetSensorsData), NULL);

  /* definition and creation of Task_ReportData */
  osThreadDef(Task_ReportData, Func_ReportData, osPriorityNormal, 0, 128);
  Task_ReportDataHandle = osThreadCreate(osThread(Task_ReportData), NULL);

  /* definition and creation of Task_DeviceSetting */
  osThreadDef(Task_DeviceSetting, Func_DeviceSetting, osPriorityNormal, 0, 128);
  Task_DeviceSettingHandle = osThreadCreate(osThread(Task_DeviceSetting), NULL);

  /* definition and creation of Task_NetworkLink */
  osThreadDef(Task_NetworkLink, Func_NetworkLink, osPriorityNormal, 0, 128);
  Task_NetworkLinkHandle = osThreadCreate(osThread(Task_NetworkLink), NULL);

  /* definition and creation of Task_RXProductProtocolAnalyse */
  osThreadDef(Task_RXProductProtocolAnalyse, Func_RXProductProtocolAnalyse, osPriorityNormal, 0, 128);
  Task_RXProductProtocolAnalyseHandle = osThreadCreate(osThread(Task_RXProductProtocolAnalyse), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* Func_GetSensorsData function */
void Func_GetSensorsData(void const * argument)
{

  /* USER CODE BEGIN Func_GetSensorsData */
	AHT15_Reset();
  /* Infinite loop */
  for(;;)
  {
		if( DeviceSet.AHT15_EN == 1)
			Get_AHT15_Data(); 		//获取温湿度
		if( DeviceSet.VOC_EN ==1 )
			Get_AGS01DB_Data();		//获取VOC
		if(	DeviceSet.PM2_5_EN == 1 )
			;
		if( DeviceSet.Light_EN == 1 )
			;
		if( DeviceSet.Noice_EN == 1 )
			;
		
    osDelay(2000);				//AGS01DB传感器采样周期最长  为>=2S
  }
  /* USER CODE END Func_GetSensorsData */
}

/* Func_ReportData function */
void Func_ReportData(void const * argument)
{
  /* USER CODE BEGIN Func_ReportData */
	char * p = NULL;
	char * p2 = NULL;
	char buf[10]={0};		//itoa buf
	
	osDelay(1);
	osSemaphoreWait(BinarySem_Task_ReportDataHandle,osWaitForever);// 先清空信号量，信号量创建(cubemx)生成时为满	 虽然该信号量可能不会用到
  osDelay(12000);			//给点时间初始化 12S
	/* Infinite loop */
  for(;;)
  {
		osSemaphoreWait(BinarySem_Task_ReportDataHandle,osWaitForever);	//控制1秒仅进入该任务一次
		
		//定期检测TCP远程服务器链接情况 
		if( softTimerCount%(10*DeviceSet.T) == 0 )
		{
			snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPSTATUS\r\n" );
			HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
			osDelay(2000);		//时间未确定
			p = (char *)strstr((char *)&UsartType.RX_pData[0],"\"TCP\"");		// p+7 : IP地址首字节地址
			p2 = strchr(p+7,',');			// p2+1 : IP端口字符串首字节地址	
			snprintf(buf,10,"%d",DeviceSet.Port);	 //itoa	
			if( ( p == NULL ) || (0!= strncmp(p+7,(char *)DeviceSet.IP,strlen((char *)DeviceSet.IP))) || (0!= strncmp(p2+1,buf,strlen(buf)) )	)  //未连接上
			{
				p = NULL;		
				p2 = NULL;
				memset(buf,0,10);		//清零初始化，防意外
				//远程服务器链接状态 刷新
				DeviceSet.NetWork_Status = 2;
				//连接TCP远程服务器
				snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPSTART=2,\"TCP\",\"%s\",%hd\r\n",DeviceSet.IP,DeviceSet.Port );
				HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
				osDelay(5000);	
			}
			else	DeviceSet.NetWork_Status = 1;		//远程服务器链接状态 刷新 (连接上)
		}
				
		//定期自动上报数据 		数据帧形式
		if(  softTimerCount%DeviceSet.T == 0 )
		{
			if( (DeviceSet.Mode == 1)&&(DeviceSet.NetWork_Status == 1) )	 //网络连接正常 且开启自动上报
			{
				snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPSEND=2,%d\r\n",DDD_Len+4 );
				HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
				osDelay(200);	
				UsartTx[0] = 0xA5;
				UsartTx[1] = 0x5A;
				UsartTx[2] = DDD_Len+1;
				UsartTx[3] = 0x02;
				memcpy( &UsartTx[4],(uint8_t *)&SensorData,DDD_Len );
				HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,DDD_Len+4 );	
				osDelay(200);	//给wifi模块发送一点点时间
				//远程服务器端接收到数据帧，解析, 再将该数据转换成DDD结构体，方可读出数据
				;
			}
		}
		
		//定期上报Wifi设备配置信息  数据帧形式
		if(  softTimerCount%(3*DeviceSet.T) == 0 )
		{
			if( DeviceSet.NetWork_Status == 1)		//网络连接正常
			{
				snprintf((char *)UsartTx,sizeof(UsartTx),"AT+CIPSEND=2,%d\r\n",SSS_Len+4 );
				HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx) );	
				osDelay(200);	
				UsartTx[0] = 0xA5;
				UsartTx[1] = 0x5A;
				UsartTx[2] = SSS_Len+1;
				UsartTx[3] = 0x03;
				memcpy( &UsartTx[4],(uint8_t *)&DeviceSet,SSS_Len );
				HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,SSS_Len+4 );	
				osDelay(200);	//给wifi模块发送一点点时间
				//远程服务器端接收到数据帧，解析, 再将该数据转换成SSS结构体，方可读出数据
				;
			}
		}
		
    osDelay(1);
  }
  /* USER CODE END Func_ReportData */
}

/* Func_DeviceSetting function */
void Func_DeviceSetting(void const * argument)
{
  /* USER CODE BEGIN Func_DeviceSetting */
	osDelay(1);
	osSemaphoreWait(BinarySem_Task_DeviceSettingHandle,osWaitForever);// 先清空信号量，信号量创建(cubemx)生成时为满
	
	ReadEEPROM();
	if(DeviceSet.FirstTimeSet_Or_Not == 0x55)
		Init_SecondTime();
	else
		Init_FirstTime();
  /* Infinite loop */
  for(;;)
  {
		osSemaphoreWait(BinarySem_Task_DeviceSettingHandle,osWaitForever);
		Refresh_Set();
    osDelay(100);
  }
  /* USER CODE END Func_DeviceSetting */
}

/* Func_NetworkLink function */
void Func_NetworkLink(void const * argument)
{
  /* USER CODE BEGIN Func_NetworkLink */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END Func_NetworkLink */
}

/* Func_RXProductProtocolAnalyse function */
void Func_RXProductProtocolAnalyse(void const * argument)
{
  /* USER CODE BEGIN Func_RXProductProtocolAnalyse */
	osDelay(1);
	osSemaphoreWait(BinarySem_Task_RXProductProtocolAnalyseHandle,osWaitForever);  //先清空信号量，刚创建时为满
  /* Infinite loop */
  for(;;)
  {
		osSemaphoreWait(BinarySem_Task_RXProductProtocolAnalyseHandle,osWaitForever);
		//1.解析RX数据，并刷新缓存
		if( (UsartType.RX_pData[0]==0xA5) && (UsartType.RX_pData[1]==0x5A) )
		{
			switch( UsartType.RX_pData[3])
			{
				case 0x03:{
											switch(UsartType.RX_pData[4])	//子判定字节
											{
												case 0x01: strncpy((char *)&DeviceSet.Name[0],(char *)&UsartType.RX_pData[5],strlen((char *)&UsartType.RX_pData[5])+1 );
																		break;
												case 0x02: strncpy((char *)&DeviceSet.IP[0],(char *)&UsartType.RX_pData[5],strlen((char *)&UsartType.RX_pData[5])+1 );
																		break;
												case 0x03: DeviceSet.Port = *(uint16_t *)(&UsartType.RX_pData[5]);		//stm32是小端模式 其它端发过来时要注意
																		break;
												case 0x04: strncpy((char *)&DeviceSet.C_Name[0],(char *)&UsartType.RX_pData[5],strlen((char *)&UsartType.RX_pData[5])+1 );
																		break;
												case 0x05: strncpy((char *)&DeviceSet.C_Key[0],(char *)&UsartType.RX_pData[5],strlen((char *)&UsartType.RX_pData[5])+1 );
																		break;
												case 0x06: DeviceSet.Mode = UsartType.RX_pData[5];
																		break;
												case 0x07: DeviceSet.T = *(uint16_t *)(&UsartType.RX_pData[5]);		//stm32是小端模式 其它端发过来时要注意
																		break;
												case 0x08: DeviceSet.AHT15_EN = UsartType.RX_pData[5];
																		break;
												case 0x09: DeviceSet.VOC_EN = UsartType.RX_pData[5];
																		break;
												case 0x0a: DeviceSet.PM2_5_EN = UsartType.RX_pData[5];
																		break;
												case 0x0b: DeviceSet.Light_EN = UsartType.RX_pData[5];
																		break;
												case 0x0c: DeviceSet.Noice_EN = UsartType.RX_pData[5];
																		break;
												case 0x0d: {
																		DeviceSet.Refresh_Set_Control = UsartType.RX_pData[5];
																		if( DeviceSet.Refresh_Set_Control == 1)
																			{ DeviceSet.Refresh_Set_Control = 2; 
																				osSemaphoreRelease(BinarySem_Task_DeviceSettingHandle); } 
																				
																	 }
												default:   break;
											}
											
									}
									break;
				default:
									break;
			}
		}

    osDelay(1);
  }
  /* USER CODE END Func_RXProductProtocolAnalyse */
}

/* myTimer1s_Callback function */
void myTimer1s_Callback(void const * argument)
{
  /* USER CODE BEGIN myTimer1s_Callback */
  softTimerCount++;
	osSemaphoreRelease(BinarySem_Task_ReportDataHandle);	//用信号量控制一秒仅进入那个函数一次
	
	HAL_IWDG_Refresh(&hiwdg);
  /* USER CODE END myTimer1s_Callback */
}

/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
