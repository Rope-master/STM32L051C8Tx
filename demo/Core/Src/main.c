/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "iwdg.h"
#include "usart.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "W25QXX.h"
#include "string.h"

#include <stdlib.h> 
#include <stdio.h> 
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t WriteAdress = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int HexToAscii(unsigned char *pHexStr,unsigned char *pAscStr,int Len)
{
	char Nibble[2];
//	unsigned char Buffer[33];
	int i = 0;
	int j = 0;

	for(i=0;i<Len;i++)
	{
		Nibble[0]=pHexStr[i] >> 4 & 0X0F;
		Nibble[1]=pHexStr[i] & 0x0F;
		for(j=0;j<2;j++)
		{
			if(Nibble[j]<10)
			{
				Nibble[j]=Nibble[j]+0x30;
			}
			else if(Nibble[j]<16)
			{
				Nibble[j]=Nibble[j]-10+'A';
			}
			else
			{
				return 0;
			}
		}
		memcpy(pAscStr+i*2,Nibble,2);
	}
	pAscStr[2*Len]=0x00;
//	memcpy(pAscStr,Buffer,2*Len);
//	pAscStr[2*Len]=0x00;
	return 1;
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint8_t wData[0x100];
  uint8_t rData[0x100];
  uint32_t i;
  uint8_t ID[2];
  
  uint16_t ADC_Value;
  float ADC_TrueValue;
  uint8_t  ADC_Str[10];
  
  uint8_t  md5_hex[16];
  uint8_t  md5_Str[33];
  
  RTC_DateTypeDef GetData;  //获取日期结构体

  RTC_TimeTypeDef GetTime;   //获取时间结构体
  
  RTC_DateTypeDef SetData;  //设置日期结构体

  RTC_TimeTypeDef SetTime;   //设置时间结构体
  
  uint8_t rtc_count = 0;

  
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  MX_LPUART1_UART_Init();
  MX_USART1_UART_Init();
//  MX_IWDG_Init();
  MX_ADC_Init();
  MX_TIM2_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
//  printf("123");
//  while(1);


  HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);    //AD校准

 if(HAL_GPIO_ReadPin(SW_DET_GPIO_Port,SW_DET_Pin) == GPIO_PIN_SET)
  {
//    POWER_GPIO_Init();
    HAL_GPIO_WritePin(POWER_ON_GPIO_Port, POWER_ON_Pin, GPIO_PIN_SET);
  }
  HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
  
  
  for(uint8_t i = 0; i < 16; i++)
  {
    md5_hex[i] = '0' + i;
  }
  
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      HAL_Delay(1000);
    
     HAL_ADC_Start(&hadc);     //启动ADC转换
     HAL_ADC_PollForConversion(&hadc, 50);   //等待转换完成，50为最大等待时间，单位为ms
     
     
     if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc), HAL_ADC_STATE_REG_EOC))
     {
      ADC_Value = HAL_ADC_GetValue(&hadc);   //获取AD值

      printf("ADC1 Reading : %d \r\n",ADC_Value);
       
       ADC_TrueValue = ADC_Value*3.3f/4096*153;
      printf("PA3 True Voltage value : %.4f \r\n",ADC_TrueValue);
      printf("Z小旋测试\r\n");
       ADC_Value = (uint16_t)ADC_TrueValue;
       printf("ADC1 Readings : %d \r\n",ADC_Value);

       ADC_Str[0] = ADC_Value/100 + '0';
//       if(ADC_Str[0] == '0')
//       {}
       ADC_Str[1] = (ADC_Value/10)%10 + '0';
       ADC_Str[2] = ADC_Value%10 + '0';
       ADC_Str[3] = '\0';
       
       printf("ADC1 Readings : %s \r\n",ADC_Str);
       HexToAscii(md5_hex, md5_Str, 16);
       printf("ADC1 Readings : %s \r\n",md5_Str);

      }
      HAL_ADC_Stop(&hadc);
      
      /* Get the RTC current Time */
      HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
      /* Get the RTC current Date */
      HAL_RTC_GetDate(&hrtc, &GetData, RTC_FORMAT_BIN);

      /* Display date Format : yy/mm/dd */
      printf("%02d/%02d/%02d\r\n",2000 + GetData.Year, GetData.Month, GetData.Date);
      /* Display time Format : hh:mm:ss */
      printf("%02d:%02d:%02d\r\n",GetTime.Hours, GetTime.Minutes, GetTime.Seconds);

      printf("\r\n");

      HAL_Delay(1000);

      rtc_count++;
      if(rtc_count > 10)
      {
        rtc_count = 0;
          SetTime.Hours = 10;
          SetTime.Minutes = 30;
          SetTime.Seconds = 0;

          if (HAL_RTC_SetTime(&hrtc, &SetTime, RTC_FORMAT_BIN) != HAL_OK)
          {
            Error_Handler();
          }
          SetData.WeekDay = RTC_WEEKDAY_MONDAY;
          SetData.Month = RTC_MONTH_OCTOBER;
          SetData.Date = 19;
          SetData.Year = 20;

          if (HAL_RTC_SetDate(&hrtc, &SetData, RTC_FORMAT_BIN) != HAL_OK)
          {
            Error_Handler();
          }
      }
      
     
//    HAL_Delay(1000);
//    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
//    HAL_Delay(1000);
//    HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_1);
    
//    HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
    
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_LPUART1|RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
