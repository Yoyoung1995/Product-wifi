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
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Func_GetSensorsData */
}

/* Func_ReportData function */
void Func_ReportData(void const * argument)
{
  /* USER CODE BEGIN Func_ReportData */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Func_ReportData */
}

/* Func_DeviceSetting function */
void Func_DeviceSetting(void const * argument)
{
  /* USER CODE BEGIN Func_DeviceSetting */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
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
    osDelay(1);
  }
  /* USER CODE END Func_NetworkLink */
}

/* Func_RXProductProtocolAnalyse function */
void Func_RXProductProtocolAnalyse(void const * argument)
{
  /* USER CODE BEGIN Func_RXProductProtocolAnalyse */
  /* Infinite loop */
  for(;;)
  {
		osSemaphoreWait(BinarySem_Task_RXProductProtocolAnalyseHandle,osWaitForever);
//		snprintf((char *)UsartTx,sizeof(UsartTx),"������%s",UsartType.RX_pData );
//		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)UsartTx,strlen((char *)UsartTx)+1 );
		//
    osDelay(1);
  }
  /* USER CODE END Func_RXProductProtocolAnalyse */
}

/* myTimer1s_Callback function */
void myTimer1s_Callback(void const * argument)
{
  /* USER CODE BEGIN myTimer1s_Callback */
  
  /* USER CODE END myTimer1s_Callback */
}

/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
