/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32c8t6_bluepill_board.h"
#include "user_include.h"
#include "uart_comm.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define WELCOME_MSG "Welcome to the Nucleo management console\r\n"
#define MAIN_MENU   "Select the option you are interested in:\r\n\t1. Toggle LD2 LED\r\n\t2. Read USER BUTTON status\r\n\t3. Clear screen and print this message "
#define PROMPT "\r\n> "
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define EDK_TEST (2)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1; //PIN9,10
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
__IO ITStatus UartReady = SET;
char readBuf[1];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void edk_main_test(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int8_t readUserInput(void){
  int8_t retVal = -1;
  if(UartReady == SET){
    UartReady = RESET;
      // if(HAL_OK == HAL_UART_Receive_IT(&huart1, (uint8_t *)readBuf, 1)){
      //   retVal = atoi(readBuf);
      // }    
    HAL_UART_Receive_IT(&huart1, (uint8_t*)readBuf, 1);
    retVal = atoi(readBuf);         
  }
  return retVal;
}

HAL_StatusTypeDef UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t len){
  if(HAL_UART_Transmit_IT(huart, pData, len) != HAL_OK){
    if(RingBuffer_Write(&uart1_txBuf, pData, len) != RING_BUFFER_OK){
      return 0;
    }
  }
  return 1;
}

HAL_StatusTypeDef UART_Transmit2(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t len){
  if(HAL_UART_Transmit_IT(huart, pData, len) != HAL_OK){
    if(RingBuffer_Write(&uart1_txBuf, pData, len) != RING_BUFFER_OK){
      return 0;
    }
  }
  return 1;
}

uint8_t processUserInput(int8_t opt) {
  char msg[30];

  if(!(opt >=1 && opt <= 3))
    return 0;

  sprintf(msg, "%d", opt);
  UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg));

  switch(opt) {
  case 1:
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    break;
  case 2:
    sprintf(msg, "\r\nstatus: %s", HAL_GPIO_ReadPin(LED_GPIO_Port, LED_Pin) == GPIO_PIN_RESET ? "PRESSED" : "RELEASED");
    UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg));
    break;
  case 3:
    return 2;
  };

  UART_Transmit(&huart1, (uint8_t*)PROMPT, strlen(PROMPT));
  return 1;
}

void printWelcomeMessage(void) {
  
#if 1

  const char *test = "test\n";
  for (uint8_t i = 0; i < 1; i++) {  
    UART_Transmit(&huart1, (uint8_t*)test, strlen(test));
    while (HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX || HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX_RX);
  }
#else
  char *strings[] = {"\033[0;0H", "\033[2J", WELCOME_MSG, MAIN_MENU, PROMPT};
  
  for (uint8_t i = 0; i < 5; i++) {
    UART_Transmit(&huart1, (uint8_t*)strings[i], strlen(strings[i]));
    while (HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX || HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX_RX);
  }
#endif 
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle) {
 /* Set transmission flag: transfer complete*/
 UartReady = SET;
}


void edk_main(void)
{
  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
  uart_comm_command();


/* usb virtual com port test*/

  uint8_t data[] = "edk usb cdc test\n";
  char cdcbuf[40];
  uint16_t count = 0;

  while(1)
  {
    sprintf(cdcbuf, "Count: %d\n", count++);
    while(CDC_Transmit_FS((uint8_t *)cdcbuf, strlen(cdcbuf)) == USBD_BUSY)
    {
      ;      
    }

    HAL_Delay(1000);
  }

}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint8_t opt = 0;
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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  //MX_USART1_UART_Init();
  //MX_USART2_UART_Init();
  /* Enable USART2 interrupt */
  HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);

  RingBuffer_Init(&uart1_txBuf);
  RingBuffer_Init(&uart1_rxBuf);

  uart_comm_enable_RX_IT(&huart1, (uint8_t *)&uart1_rxBuf.buf, UART_INFINITE_EN_CNT);
  /* USER CODE END 2 */  

  /* USER CODE BEGIN WHILE */
  while (1)
  {
    edk_main();
  }
  /* USER CODE END WHILE */
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
  GPIO_InitTypeDef GPIO_InitStruct;
  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);
  // HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
