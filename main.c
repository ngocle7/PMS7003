
#include "main.h"

#include "stdio.h"

uint8_t pms7003_Buffer[32];						//Array store 32 bytes of data to be received from the PMS7003 sensor
uint8_t pms7003_send_buffer[7]={0x42,0x4d,0};	//Array of data to be used to send commands to the sensor
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;


void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);

void print_PMS7003(void);
void write_PMS7003(char *cmd);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

int main(void)
{

  HAL_Init();

  SystemClock_Config();


  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();

  if(HAL_UART_Receive_DMA(&huart1,pms7003_Buffer,32)!=HAL_OK)
   {
 	  //Execution fail
   }

  while (1)
  {

  }

}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};


  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}


static void MX_USART1_UART_Init(void)
{


  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
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
  void print_PMS7003(void)
  {
  	/*
  	combine_value : Variables to be used to combine data divided by 8 bits into one
  	check_byte_receive :  Variables to use to combine the check codes (30,31) received from the sensor into one
  	check_byte_calculate : The data received from the sensor is summed up to 0-29 and compared with the check code
  	*/
  	uint16_t combine_value, check_byte_receive, check_byte_calculate=0;

  	check_byte_receive=pms7003_Buffer[30]<<8|pms7003_Buffer[31];
  	for(uint8_t i=0;i<30;i++)
  	{
  		check_byte_calculate+=pms7003_Buffer[i];

  	}

      	//Output only when the check code received from the sensor (30,31) and the sum of the received data from 0 to 29 match
  	if(check_byte_receive==check_byte_calculate)
  	{
  		printf("PM1.0 : %d	",(combine_value=(pms7003_Buffer[10]<<8)|pms7003_Buffer[11]));
  		printf("PM2.5 : %d	",(combine_value=(pms7003_Buffer[12]<<8)|pms7003_Buffer[13]));
  		printf("PM10 : %d	",(combine_value=(pms7003_Buffer[14]<<8)|pms7003_Buffer[15]));
  		printf("0.3um : %d	",(combine_value=(pms7003_Buffer[16]<<8)|pms7003_Buffer[17]));
  		printf("0.5um : %d	",(combine_value=(pms7003_Buffer[18]<<8)|pms7003_Buffer[19]));
  		printf("1.0um : %d	",(combine_value=(pms7003_Buffer[20]<<8)|pms7003_Buffer[21]));
  		printf("2.5um : %d	",(combine_value=(pms7003_Buffer[22]<<8)|pms7003_Buffer[23]));
  		printf("5.0um : %d	",(combine_value=(pms7003_Buffer[24]<<8)|pms7003_Buffer[25]));
  		printf("10.0um : %d\n",(combine_value=(pms7003_Buffer[26]<<8)|pms7003_Buffer[27]));
  	}
  	else	//The check code does not match
  	{
  	}
  }


}


static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
  /* DMA2_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

}


static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)		//UART is UART1 or other?
	{
		print_PMS7003();
	}
}

void Error_Handler(void)
{

  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{

}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
