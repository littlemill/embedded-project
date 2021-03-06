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
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
// #include "defines.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

//#define DEV //
#include "mydefines.h"

#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_i2c.h"
#include "tm_stm32_mpu6050.h"

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
I2C_HandleTypeDef hi2c1;

I2S_HandleTypeDef hi2s3;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

HCD_HandleTypeDef hhcd_USB_OTG_FS;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2S3_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_HCD_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
TM_MPU6050_t MPU6050_Sensor;
char buffer[512];
char readBuffer[10];
int gyroConn = 0;
// ========================= BUZZER =============================== //

// connect pin to "S" and GND to "-"

void setBuzzer(int state) {
	HAL_GPIO_WritePin(BUZZER_OUT_PIN, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// ========================== I2C connect ========================= //

int gyroConnected() {
	return TM_I2C_IsDeviceConnected(MPU6050_I2C, 0xD0) == TM_I2C_Result_Ok;
}

void gyroConnect(int maxTry) {
	int tries = 0;
	while (TM_MPU6050_Init(&MPU6050_Sensor, TM_MPU6050_Device_0,
			TM_MPU6050_Accelerometer_8G, TM_MPU6050_Gyroscope_250s)
			!= TM_MPU6050_Result_Ok && tries < maxTry) {
		HAL_Delay(10);
		tries++;
	}
}

void uartPrintf(UART_HandleTypeDef *handle, const char *format, ...) {
	va_list args;
	va_start(args, format);
	int n = sprintf(buffer, format, args);
//	HAL_UART_Transmit(handle, buffer, n, 1000);
	HAL_UART_Transmit(&huart2, buffer, n, 1000);
	HAL_UART_Transmit(&huart3, buffer, n, 1000);
	va_end(args);
}

#include "logic.h"

int enableFallAlarm = 1, enableOutAlarm = 1;
int fallen = 0, outOfRange = 0;
int loopCount = 0;
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
  MX_I2C1_Init();
  MX_I2S3_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_HCD_Init();
  /* USER CODE BEGIN 2 */
  	float accHistory[5] = {0}, gyroHistory[5] = {0}, _accSum = 0, _gyroSum = 0;
  	int _aIdx = 0, _gIdx = 0;
	float ax, ay, az, aMag, gx, gy, gz, gMag, accMult, gyroMult, length;
	int outAlarmOn = 0, fallAlarmOn = 0;
	int fallingNow = 0, gyroConn, n;
	uartPrintf(NODEMCU_HANDLE, "t");
	gyroConnect(1000);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		HAL_Delay(10);
		loopCount = (loopCount+1)%100;
		if (loopCount == 0) {
			uartPrintf(NODEMCU_HANDLE, "t");
		}

		if (!(gyroConn = gyroConnected())) {
			gyroConnect(100);
			HAL_Delay(500);
		}

		uartPrintf(NODEMCU_HANDLE, gyroConnected() ? "e": "E");

		if (gyroConn) { // why did connected always return 0 ?
			TM_MPU6050_ReadAll(&MPU6050_Sensor);
			accMult = (float) 1 / ((1 << 16) / 16) * 9.81; // +-8G
			gyroMult = (float) 1 / ((1 << 16) / 500); // +-250
//			accMult = (float) 1 / ((1 << 16) / 5) * 9.81; // +-8G
//			gyroMult = (float) 1 / ((1 << 16) / 20); // +-250

			ax = MPU6050_Sensor.Accelerometer_X * accMult;
			ay = MPU6050_Sensor.Accelerometer_Y * accMult;
			az = MPU6050_Sensor.Accelerometer_Z * accMult;
			aMag = ax * ax + ay * ay + az * az;

			gx = MPU6050_Sensor.Gyroscope_X * gyroMult;
			gy = MPU6050_Sensor.Gyroscope_Y * gyroMult;
			gz = MPU6050_Sensor.Gyroscope_Z * gyroMult;
			gMag = gx * gx + gy * gy + gz * gz;

			if(aMag == 0 && gMag==0){
				HAL_GPIO_WritePin(GYRO_STATUS_LED_PIN, GPIO_PIN_SET);
			}else{
				HAL_GPIO_WritePin(GYRO_STATUS_LED_PIN, GPIO_PIN_RESET);
			}

			/* Send the data to serial buffer */
#ifdef DEV
//		 n =
//					sprintf(buffer,
//							"A (%5.2f %5.2f %5.2f = %7.2f) G (%7.2f %7.2f %7.2f = %.2f)\r\n",
//							ax, ay, az, aMag, gx, gy, gz, gMag);
//			//	uartPrintf(RED_BOARD_HANDLE, "A (%5.2f %5.2f %5.2f = %7.2f) G (%7.2f %7.2f %7.2f = %.2f)\r\n", ax, ay, az, aMag, gx, gy, gz, gMag);
//			HAL_UART_Transmit(RED_BOARD_HANDLE, buffer, n, 100);
#endif

			_accSum -= accHistory[_aIdx];
			_gyroSum -= gyroHistory[_gIdx];
			_accSum += (accHistory[_aIdx] = aMag);
			_gyroSum += (gyroHistory[_gIdx] = gMag);

			if (_accSum/5 > 70 && _accSum/5<500 && _gyroSum/5 > 36000 && _gyroSum/5 < 49000) {

				if(_accSum/5 > 90 && _accSum/5<190 && _gyroSum/5 > 40000 ){
					//run
				}else{
					n =
										sprintf(buffer,
												"------------- A/5 (%.2f) G/5 (%.2f)\r\n",
												_accSum/5, _gyroSum/5);
					HAL_UART_Transmit(RED_BOARD_HANDLE, buffer, n, 100);
					fallen = 1;
					for(int i=0;i<5;i++){
						_accSum=0;
						_gyroSum=0;
						accHistory[i]=0;
						gyroHistory[i]=0;
					}
				}

			}
			n =
								sprintf(buffer,
										"------------- A/5 (%.2f) G/5 (%.2f)\r\n",
										_accSum/5, _gyroSum/5);
			HAL_UART_Transmit(RED_BOARD_HANDLE, buffer, n, 100);


			if (accHistory[_aIdx] > 300 || gyroHistory[_gIdx] > 20000) {
				HAL_GPIO_WritePin(FALL_STATUS_LED_PIN, GPIO_PIN_SET);
			} else {
				HAL_GPIO_WritePin(FALL_STATUS_LED_PIN, GPIO_PIN_RESET);
			}
			_aIdx++;
			_aIdx %= 5;
			_gIdx++;
			_gIdx %= 5;

		}

		// listen command
//		length = HAL_UART_Receive(&huart2, readBuffer, 1, 10);
		length = readMessage(NODEMCU_HANDLE, readBuffer, 1);
		if (length > 0) {
			uartPrintf(RED_BOARD_HANDLE, "read:%c\n" ,readBuffer[0]);
			switch (readBuffer[0]) {
			case 'R':
				fallen = 0;
				break;
			case 'r':
				outOfRange = 0;
				break;
			case 'F':
				enableFallAlarm = 1;
				break;
			case 'f':
				enableFallAlarm = 0;
				break;
			case 'O':
				enableOutAlarm = 1;
				break;
			case 'o':
				enableOutAlarm = 0;
				break;
			case 'X':
				outOfRange = 1;
				break;
			case 'x':
				outOfRange = 0;
				break;
			}
			enableOutAlarm == 1?
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET):
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(COMMAND_RECV_LED_PIN, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(COMMAND_RECV_LED_PIN, GPIO_PIN_RESET);
		}

		// alarm logic
		outAlarmOn = outOfRange && enableOutAlarm;
		fallAlarmOn = fallen && enableFallAlarm;
		if (outAlarmOn || fallAlarmOn) {
			setBuzzer(1);
		} else {
			setBuzzer(0);
		}

		HAL_GPIO_WritePin(OUT_ENABLE_LED_PIN,
				enableOutAlarm ? GPIO_PIN_SET : GPIO_PIN_RESET);
		HAL_GPIO_WritePin(FALL_ENABLE_LED_PIN,
				enableFallAlarm ? GPIO_PIN_SET : GPIO_PIN_RESET);
		HAL_GPIO_WritePin(OUT_STATUS_LED_PIN,
				outOfRange ? GPIO_PIN_SET : GPIO_PIN_RESET);
		HAL_GPIO_WritePin(FALL_STATUS_LED_PIN,
				fallen ? GPIO_PIN_SET : GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GYRO_STATUS_LED_PIN, gyroConn ? GPIO_PIN_RESET : GPIO_PIN_SET);

#ifdef DEV
		if (fallAlarmOn) {
			uartPrintf(RED_BOARD_HANDLE, "F");
		} else if (fallingNow) {
			uartPrintf(RED_BOARD_HANDLE, "-");
		} else {
			uartPrintf(RED_BOARD_HANDLE, ".");
		}

#else
//	if (fallAlarmOn){
//		uartPrintf(NODEMCU_HANDLE, "F");
//	}
//	else {
//		uartPrintf(NODEMCU_HANDLE, "f");
//	}
#endif

	if (fallAlarmOn){
		uartPrintf(NODEMCU_HANDLE, "F");
	}
	else {
		uartPrintf(NODEMCU_HANDLE, "f");
	}

	if(enableFallAlarm) {
		uartPrintf(NODEMCU_HANDLE, "S");
	}else if(!enableFallAlarm){
		uartPrintf(NODEMCU_HANDLE, "s");
	}
	if(enableOutAlarm) {
			uartPrintf(NODEMCU_HANDLE, "U");
		}else if(!enableOutAlarm){
			uartPrintf(NODEMCU_HANDLE, "u");
	}

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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
  PeriphClkInitStruct.PLLI2S.PLLI2SN = 192;
  PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2S3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2S3_Init(void)
{

  /* USER CODE BEGIN I2S3_Init 0 */

  /* USER CODE END I2S3_Init 0 */

  /* USER CODE BEGIN I2S3_Init 1 */

  /* USER CODE END I2S3_Init 1 */
  hi2s3.Instance = SPI3;
  hi2s3.Init.Mode = I2S_MODE_MASTER_TX;
  hi2s3.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s3.Init.DataFormat = I2S_DATAFORMAT_16B;
  hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
  hi2s3.Init.AudioFreq = I2S_AUDIOFREQ_96K;
  hi2s3.Init.CPOL = I2S_CPOL_LOW;
  hi2s3.Init.ClockSource = I2S_CLOCK_PLL;
  hi2s3.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
  if (HAL_I2S_Init(&hi2s3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2S3_Init 2 */

  /* USER CODE END I2S3_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_HCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  hhcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hhcd_USB_OTG_FS.Init.Host_channels = 8;
  hhcd_USB_OTG_FS.Init.speed = HCD_SPEED_FULL;
  hhcd_USB_OTG_FS.Init.dma_enable = DISABLE;
  hhcd_USB_OTG_FS.Init.phy_itface = HCD_PHY_EMBEDDED;
  hhcd_USB_OTG_FS.Init.Sof_enable = DISABLE;
  if (HAL_HCD_Init(&hhcd_USB_OTG_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_I2C_SPI_GPIO_Port, CS_I2C_SPI_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OTG_FS_PowerSwitchOn_GPIO_Port, OTG_FS_PowerSwitchOn_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|LD4_Pin 
                          |LD3_Pin|LD5_Pin|LD6_Pin|Audio_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : CS_I2C_SPI_Pin */
  GPIO_InitStruct.Pin = CS_I2C_SPI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CS_I2C_SPI_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = OTG_FS_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OTG_FS_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PDM_OUT_Pin */
  GPIO_InitStruct.Pin = PDM_OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(PDM_OUT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BOOT1_Pin */
  GPIO_InitStruct.Pin = BOOT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CLK_IN_Pin */
  GPIO_InitStruct.Pin = CLK_IN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(CLK_IN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PD9 PD10 PD11 LD4_Pin 
                           LD3_Pin LD5_Pin LD6_Pin Audio_RST_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|LD4_Pin 
                          |LD3_Pin|LD5_Pin|LD6_Pin|Audio_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_OverCurrent_Pin */
  GPIO_InitStruct.Pin = OTG_FS_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(OTG_FS_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : MEMS_INT2_Pin */
  GPIO_InitStruct.Pin = MEMS_INT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MEMS_INT2_GPIO_Port, &GPIO_InitStruct);

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
