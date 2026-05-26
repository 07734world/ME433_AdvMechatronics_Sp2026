/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
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

COM_InitTypeDef BspCOMInit;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
//uint8_t buffer[1];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
/*
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}
*/
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*
//=====ClaudeAI: setup fgets (part 1 of 2)
// In your main header file or at the top of main.c
#define RXBUFFERSIZE 256
uint8_t aRxBuffer[RXBUFFERSIZE];
uint8_t rx_index = 0;

// Add this near the top of main.c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        // Store received character
        if (rx_index < RXBUFFERSIZE - 1)
        {
            rx_index++;
        }
        // Restart DMA reception
        HAL_UART_Receive_IT(&huart1, aRxBuffer + rx_index, 1);
    }
}

// Redirect printf and fgets to UART
int _write(int file, char *ptr, int len)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);
    return len;
}

int _read(int file, char *ptr, int len)
{
    while (rx_index == 0);  // Wait for data

    int i;
    for (i = 0; i < len && i < rx_index; i++)
    {
        ptr[i] = aRxBuffer[i];
    }

    rx_index = 0;  // Reset buffer
    return i;
}
//===
*/

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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /*
  //===ClaudeAI: setup fgets (part 2 of 2)
  // In main(), after HAL_Init() and system clock configuration
  HAL_UART_Receive_IT(&huart1, aRxBuffer, 1);
  //===
*/

  //HAL_UART_Receive(&huart1, buffer, 1, 0xFFFF);
  //HAL_UART_Transmit(&huart1, buffer, 1, 0xFFFF);

  //char txMessage[]="xkcd3\r\n";
  uint8_t rxBuffer[50];
/*
  // Retarget _read for scanf and fgets
  int _read(int file, unsigned char *ptr, int len) {
      HAL_UART_Receive(&huart1, ptr, len, HAL_MAX_DELAY);
      return len;
  }*/



  /* USER CODE END 2 */

  /* Initialize leds */
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_BLUE);

  /* Initialize USER push-button, will be used to trigger an interrupt each time it's pressed.*/
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  /* Initialize COM1 port (115200, 8 bits (7-bit data + 1 stop bit), no parity */
  BspCOMInit.BaudRate   = 115200;
  BspCOMInit.WordLength = COM_WORDLENGTH_8B;
  BspCOMInit.StopBits   = COM_STOPBITS_1;
  BspCOMInit.Parity     = COM_PARITY_NONE;
  BspCOMInit.HwFlowCtl  = COM_HWCONTROL_NONE;
  if (BSP_COM_Init(COM1, &BspCOMInit) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  extern UART_HandleTypeDef hcom_uart[]; //setup for user input

  while (1)
  {
	  BSP_LED_Toggle(LED_GREEN);
	  HAL_Delay(500);



	  //======= RECEIVE
	  HAL_UART_Receive(&huart1, rxBuffer, sizeof(rxBuffer), 3000);
	 	 printf("[STM32 Rx]: ");
	 	  /*for(int i = 0; i<=sizeof(rxBuffer); i++){
	 	  	  	printf("%c", rxBuffer[i]);
	 	  	  }*/
	 	 printf("%s\r\n",rxBuffer);


/*
	 	//=== User Input/UART Transmit
	 		  unsigned char rx_byte;
	 		  if(HAL_UART_Receive(&hcom_uart[COM1], &rx_byte, 1, HAL_MAX_DELAY)==HAL_OK){
	 			 printf("[STM32 Tx]rx_byte ok: %c\r\n", rx_byte);
	 			 HAL_UART_Transmit(&huart1, &rx_byte, sizeof(rx_byte), 1000);
	 			 BSP_LED_Toggle(LED_BLUE);
	 		  }
	 		  if(rx_byte=='a'){
	 			  printf("I got rx_byte as 'a'!");
	 		  }
*/

	 /*
	 	 // HAL_Delay(5000);
	 	  uint8_t rxBuffer[100];
	 	  HAL_StatusTypeDef status;

	 	  // Wait up to 1000ms for 100 bytes
	 	  status = HAL_UART_Receive(&huart1, rxBuffer, 100, 5000);

	 	  if (status == HAL_OK) {
	 	      // Data received successfully
	 	      printf("Received: %s\r\n", (char*)rxBuffer);
	 	  } else if (status == HAL_TIMEOUT) {
	 	      printf("Timeout - no data received\r\n");
	 	  } else {
	 	      printf("Error receiving data\r\n");
	 	  }
	 */



		 //=== Echo RX input into TX
		 printf("[STM32 Tx]: %s\r\n", rxBuffer);
		 HAL_UART_Transmit(&huart1, rxBuffer, sizeof(rxBuffer), 1000);

/*
		///=== failed attempt at user input via fgets/scanf
		char txMessage[50];
	 	 //char txMessage[100]="I'm STM32! The Pico says:";
	 	 //strcat(txMessage, rxBuffer);
		 printf("[STM32 Tx]: \r\n");
		 //scanf("%c", &txMessage);
		 fgets(txMessage, sizeof(txMessage),stdin); //******* Doesn't take inputs???
		 HAL_Delay(2000);
		 printf("Sending: %s\r\n",txMessage);

		 BSP_LED_Toggle(LED_BLUE); //BSP_LED_Toggle(LED_BLUE);1[

		 HAL_UART_Transmit(&huart1, txMessage, sizeof(txMessage), 1000);
*/
	  /*
	  char input[5];
	  printf("Enter input: ");
	  HAL_UART_Receive(&huart1, (uint8_t *)input, sizeof(input) - 1, 3000);
	  printf("You entered: %s\r\n", input);
*/

	  //char txMessage='fun\r\n';
	  //printf("STM32 message to send [letters only]: \r\n");
	  //scanf(" %c", &txMessage);
	  //fgets(txMessage, sizeof(txMessage), stdin);
	  //char message="neato\r\n";


	 //strcpy(txMessage,charMessage);*/
	  //HAL_Delay(5000);



	  //printf("[STM32 Tx]: %s\r\n", txMessage);




	  //HAL_UART_Transmit_IT(&huart1, buffer_uart, sizeof(buffer_uart));
/*
	  for(int i = 0; i<=sizeof(txMessage); i++){
	  	printf("Tx: %c\r\n", txMessage[i]);
	  }*/


	  //sprintf(txMessage, "Hello from STM32 \r\n");
	  //(const uint8_t) txMessage;
	  //HAL_UART_Transmit(&huart1, buffer, 1, 0xFFFF);
	  //HAL_UART_Receive(&huart1, buffer, 1, 0xFFFF);
	  //HAL_UART_Transmit(&huart1, buffer, 1, 0xF);
	  //HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	  //HAL_UART_Transmit_IT(&huart1, buffer, 1);
	  //HAL_Delay(100);

	  //printf("testSTM32\r\n");



/*
	  int rxNumber;
	  char rxMessage[100];
	  char buffer[1000];
	  int index=0;
	  HAL_UART_Receive(&huart1, rxMessage, 100, 10);

	  for (int i=0; i < strlen(rxMessage);i++){
		  if(rxMessage[i]=='\n'){
			  // msg received
			  sscanf(buffer, "%d", &rxNumber);
	  	  }
		  else{
			  buffer[index]=rxMessage[i];
			  index++;
			  if (index==1000){
				  index=0;
			  }
		  }
	  }

*/

	  //sprintf(m, "helloIamSTM32-%d\r\n",number);
	  //HAL_UART_Transmit(&huart1, m, strlen(m), 100);

	 // HAL_Delay(1000);
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

  __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_0);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV4;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_DMADISABLEONERROR_INIT;
  huart1.AdvancedInit.DMADisableonRxError = UART_ADVFEATURE_DMA_DISABLEONRXERROR;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

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
#ifdef USE_FULL_ASSERT
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
