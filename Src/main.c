/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "DDS9954.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
	
//	void AD9954_RESET(void){
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET);
//		delay_us(100);
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET);
//	}
//	
//	void AD9954_SendByte(uint8_t data){
//		uint8_t i;
//		for(i=0;i<8;i++){
//			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET); //SCLK = 0;
//			if( data & 0x80)
//			{
//				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET); //SDIO = 1;
//			}
//			else
//			{
//				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET); //SDIO = 0;	
//			}
//			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET); //SCLK = 1;
//			data <<= 1;
//		}
//	}
//	
//	void UPDATE(void){
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET); //UPD = 1;
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET); //UPD = 0;
//	}
//	
//	void AD9954_Init(void){
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET); //SCLK = 0;
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET); //RES = 0;
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET); //UPD = 0;
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET); //PS0 = 0;
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET); //PS1 = 0;
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET); //CS = 0;
//		delay_us(1);
//		AD9954_SendByte(0x00);
//		AD9954_SendByte(0x00);
//		AD9954_SendByte(0x00);
//		AD9954_SendByte(0x00);
//		AD9954_SendByte(0x40);
//		AD9954_SendByte(0x01);
//		AD9954_SendByte(0x00);
//		AD9954_SendByte(0x00);
//		AD9954_SendByte(0x24);
//	}
//	
//	void AD9954_SetFre(float f){
//		uint32_t date;
////		f*=5;
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET); //CS = 0;
////		date=(uint32_t)(10.7374*f);
//		date=(uint32_t)(53.6715*f);
//		AD9954_SendByte(0x04);
//		AD9954_SendByte((uint8_t)(date>>24));
//		AD9954_SendByte((uint8_t)(date>>16));
//		AD9954_SendByte((uint8_t)(date>>8));
//		AD9954_SendByte((uint8_t)date);
//		UPDATE();
//	}
//	
//	void AD9954_LinearSweep(float f1,float f2){
//		uint32_t date;
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET); //SCLK = 0;
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET); //RES = 0;
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET); //UPD = 0;
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET); //PS0 = 0;
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET); //PS1 = 0;
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET); //CS = 0;
//		AD9954_SendByte(0x00);
//		AD9954_SendByte(0x00);
//		AD9954_SendByte(0x20);
//		AD9954_SendByte(0x00);
//		AD9954_SendByte(0x40);
//		AD9954_SendByte(0x01);
//		AD9954_SendByte(0x00);
//		AD9954_SendByte(0x00);
//		AD9954_SendByte(0x24);
//		date = 53.6715*f1;
//		AD9954_SendByte(0x04);
//		AD9954_SendByte((uint8_t)(date>>24));
//		AD9954_SendByte((uint8_t)(date>>16));
//		AD9954_SendByte((uint8_t)(date>>8));
//		AD9954_SendByte((uint8_t)date);
//		date = 53.6715*f2;
//		AD9954_SendByte(0x06);
//		AD9954_SendByte((uint8_t)(date>>24));
//		AD9954_SendByte((uint8_t)(date>>16));
//		AD9954_SendByte((uint8_t)(date>>8));
//		AD9954_SendByte((uint8_t)date);
//		AD9954_SendByte(0x07);
//		AD9954_SendByte(0x01);
//		AD9954_SendByte(0x00);
//		AD9954_SendByte(0x00);
//		AD9954_SendByte(0x1b);
//		AD9954_SendByte(0xf4);
//		AD9954_SendByte(0x08);
//		AD9954_SendByte(0x01);
//		AD9954_SendByte(0x00);
//		AD9954_SendByte(0x01);
//		AD9954_SendByte(0xa3);
//		AD9954_SendByte(0x6f);
//		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET); //CS = 1;
//		UPDATE();
//	}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  /* USER CODE BEGIN 2 */
	AD9954_RESET();
	AD9954_Init();
	AD9954_SETFRE((uint32_t)150000000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

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

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
