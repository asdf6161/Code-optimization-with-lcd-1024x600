/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 ** This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * COPYRIGHT(c) 2019 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "dma2d.h"
#include "fatfs.h"
#include "ltdc.h"
#include "sdmmc.h"
#include "gpio.h"
#include "fmc.h"
#include "usart.h"
#include "tim.h"
#include "adc.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
extern "C" {
#include "stm32746g_sdram.h"
#include "stm32746g_LCD.h"
#include "lcd_log.h"
#include "stm32f7xx_hal_ltdc.h"
#include "TCD1304DG.h"
}
#include "VideoData.h"
#include "Dwt.h"
#include "TimeCapture.h"
//#include <vector>
//using std::vector;
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
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
uint32_t code_analis[10];
tcd_type tcd_res[tcd_buffer_size];
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static vector<float> DirectTransformVector(vector<float> SourceList, int N)
				{
	if (SourceList.size() == 1 || N == 0)
		return SourceList;

	vector<float> RetVal;
	vector<float> TmpArr;

	for (int j = 0; j < SourceList.size() - 1; j+=2)
	{
		RetVal.push_back((const float)((SourceList[j] - SourceList[j + 1]) / 2.0));
		TmpArr.push_back((const float)((SourceList[j] + SourceList[j + 1]) / 2.0));
	}

	vector<float> res = DirectTransformVector(TmpArr, N-1);
	//	vector<float>::iterator it = res.iterator;
	RetVal.insert(RetVal.begin(), res.begin(), res.end());

	return RetVal;
				}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */
	uint8_t lcd_line = 4;

	uint16_t fisrt = (9 << 8) | (8);
	uint16_t secound = (7 << 8) | (6);
	uint32_t r = __PKHBT(fisrt,secound, 16);  // 6 8
	uint32_t r2 = __PKHTB(secound,fisrt, 16);  // 7 9

	//	res_arr = new combType[1000];
	/* USER CODE END 1 */

	/* MPU Configuration--------------------------------------------------------*/
	MPU_Config();

	/* Enable I-Cache---------------------------------------------------------*/
	SCB_EnableICache();

	/* Enable D-Cache---------------------------------------------------------*/
	SCB_EnableDCache();

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
	MX_DMA_Init();
	MX_DMA2D_Init();
	MX_FMC_Init();
	MX_LTDC_Init();
	MX_SDMMC1_SD_Init();
	MX_FATFS_Init();
	MX_USART1_UART_Init();
	MX_TIM8_Init();
	MX_TIM9_Init();
	MX_ADC1_Init();
	/* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start(&htim8);
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_1);
	/* Initialize the SDRAM */
	BSP_SDRAM_Init();

	/* Initialize the LCD */
	BSP_LCD_Init();

	/* Clear the LCD Background layer */
	BSP_LCD_SetLayerVisible(1,DISABLE);

	/* Set LCD Foreground Layer  */
	BSP_LCD_SelectLayer(1);
	LCD_LOG_Init();
	BSP_LCD_SetFont(&LCD_DEFAULT_FONT);

	BSP_LCD_SelectLayer(0);

	//	BSP_LCD_Clear(LCD_COLOR_BLACK);
	BSP_LCD_SetFont(&LCD_DEFAULT_FONT);

	/* Initialize LCD Log module */
	LCD_LOG_Init();

	/* Show Header and Footer texts */
	LCD_LOG_SetHeader((uint8_t *)"Dissertation on STM32F746IGT");
	BSP_LCD_SelectLayer(0);
	BSP_LCD_SetFont(&Font20);
//	HAL_ADC_Start_IT(&hadc1);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	VideoData *data = new VideoData();
	Dwt dwt = Dwt();

	while(true){
		TCD_start();
		HAL_Delay(50);
	}
	//	TimeCapture *cap = new TimeCapture();

	bType* line;
	bType* lineLow;

	combType *res2;
	uint16_t out_size = 0;
	TimeCapture cap1;
	uint32_t time = 0;
	if (data->openFile("Vid13.al")){
		std::string s = "";
		BSP_LCD_DisplayStringAtLine(lcd_line++, (unsigned char *)"File open ok.");
		line = data->getLine();

		/******************�� ���������**********************************/
		/****************************************************************/
		cap1.reset_cnt();
		cap1.start_capture();

		vector<float> lst;
		for (uint16_t i = 0; i < data->getPixelCnt() - 1; ++i) {
			lst.push_back(line[i]);
		}
		vector<float> res_lst = DirectTransformVector(lst, 3);
		//		vector<float> lst_res = Dwt::DirectTransformVector(lst);

		time = cap1.stop_capture();
		s = "Time for code from wiki: " + std::to_string(time);
		BSP_LCD_DisplayStringAtLine(lcd_line++, (unsigned char *)s.data());

		HAL_UART_Transmit(&huart1, (unsigned char *)"\r\nwiki=", 7, 1000);
		for (uint16_t i = 0; i < res_lst.size(); ++i) {
			s = std::to_string((uint16_t)res_lst.at(i)) + ",";
			HAL_UART_Transmit(&huart1, (unsigned char *)s.data(), s.size(), 1000);
		}
		/****************************************************************/
		/******************�� ���������**********************************/


		/****************������� �� ����������������*********************/
		/****************************************************************/
		cap1.reset_cnt();
		cap1.start_capture();

		bType *dwt_res = dwt.DirectTransform(line, data->getPixelCnt() - 1, &out_size, 3);

		time = cap1.stop_capture();
		s = "Time for not optimized DirectTransform: " + std::to_string(time);
		BSP_LCD_DisplayStringAtLine(lcd_line++, (unsigned char *)s.data());
		/****************************************************************/
		/****************������� �� ����������������*********************/


		/****************���������������� �������************************/
		/****************************************************************/
		cap1.reset_cnt();
		cap1.start_capture();

		dwt.repack_to_uint32(line, data->getPixelCnt());
		dwt.DirectTransform((data->getPixelCnt() - 1) / 2 , &out_size, 3);

		time = cap1.stop_capture();
		s = "Time for optimized DirectTransform: " + std::to_string(time);
		BSP_LCD_DisplayStringAtLine(lcd_line++, (unsigned char *)s.data());
		/****************************************************************/
		/****************���������������� �������************************/


		/************�������� �������������� �� ����������������*********/
		/****************************************************************/
		/*s = "";
		cap1.reset_cnt();
		cap1.start_capture();

		bType *arr = (bType*) realloc(dwt.dwt, (data->getPixelCnt() - 1) * sizeof(bType));
		bType *idwt = Dwt::InverseTransform(arr, (data->getPixelCnt() - 1), 3);

		time = cap1.stop_capture();
		s = "Time for full InverseTransform: " + std::to_string(time);
		BSP_LCD_DisplayStringAtLine(lcd_line++, (unsigned char *)s.data());*/
		/****************************************************************/
		/************�������� �������������� �� ����������������*********/


		/************�������� �������������� �� ����������������*********/
		/****************************************************************/

		/****************************************************************/
		/************�������� �������������� �� ����������������*********/

		// Display data
		/*HAL_UART_Transmit(&huart1, (unsigned char *)"\r\nidwt=", 7, 1000);
		for (uint16_t i = 0; i < data->getPixelCnt() - 1; ++i) {
			s = std::to_string(idwt[i]) + ",";
			HAL_UART_Transmit(&huart1, (unsigned char *)s.data(), s.size(), 1000);
		}*/

		HAL_UART_Transmit(&huart1, (unsigned char *)"\r\ndwt=", 6, 1000);
		s = "";
		for (uint16_t i = 0; i < out_size; ++i) {
			int16_t low = dwt.dwt[i];
			int16_t high = dwt.dwt[i] >> 16;
			s = std::to_string(low) + "," + std::to_string(high) + ",";
			HAL_UART_Transmit(&huart1, (unsigned char *)s.data(), s.size(), 1000);
		}

		for (uint16_t i = 0; i < out_size; ++i) {
			int16_t low = dwt.dwt[i];
			int16_t high = dwt.dwt[i] >> 16;
			uint16_t posx = (uint16_t)(i * (1024.0 / out_size / 2)) << 1;
			uint16_t posy = 480-10;
			if (low < 0){
				BSP_LCD_DrawVLine(posx, posy, abs(low));
			} else {
				BSP_LCD_DrawVLine(posx, posy - low, low);
			}
			if (high < 0){
				BSP_LCD_DrawVLine(posx + 1, posy, abs(high));
			} else {
				BSP_LCD_DrawVLine(posx + 1, posy - high, high);
			}
		}

		std::string str = "Check on exit method: " + std::to_string(code_analis[0]);
		BSP_LCD_DisplayStringAtLine(lcd_line++, (unsigned char *)str.data());

		str = "Memory allocation: " + std::to_string(code_analis[1]);
		BSP_LCD_DisplayStringAtLine(lcd_line++, (unsigned char *)str.data());

		str = "convolution operation: " + std::to_string(code_analis[2]);
		BSP_LCD_DisplayStringAtLine(lcd_line++, (unsigned char *)str.data());

		str = "coefficient copying time: " + std::to_string(code_analis[3]);
		BSP_LCD_DisplayStringAtLine(lcd_line++, (unsigned char *)str.data());

		str = "All time: " + std::to_string(code_analis[3] + code_analis[2] + code_analis[1] + code_analis[0]);
		BSP_LCD_DisplayStringAtLine(lcd_line++, (unsigned char *)str.data());

		str = "Dimensions per second: " + std::to_string(SystemCoreClock/(code_analis[3] + code_analis[2] + code_analis[1] + code_analis[0]));
		BSP_LCD_DisplayStringAtLine(lcd_line++, (unsigned char *)str.data());

		lineLow = data->getLowLine();

		data->closeFile();
		delete data;
	}
	else {
		BSP_LCD_DisplayStringAtLine(lcd_line++, (unsigned char *)"File open error! check filename...");
	}
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
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

	/**Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 400;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 8;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/**Activate the Over-Drive mode
	 */
	if (HAL_PWREx_EnableOverDrive() != HAL_OK)
	{
		Error_Handler();
	}
	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
	{
		Error_Handler();
	}
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC|RCC_PERIPHCLK_SDMMC1
			|RCC_PERIPHCLK_CLK48;
	PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
	PeriphClkInitStruct.PLLSAI.PLLSAIR = 3;
	PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
	PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV2;
	PeriphClkInitStruct.PLLSAIDivQ = 1;
	PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
	PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLL;
	PeriphClkInitStruct.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_CLK48;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void)
{
	MPU_Region_InitTypeDef MPU_InitStruct = {0};

	/* Disables the MPU */
	HAL_MPU_Disable();
	/**Initializes and configures the Region and the memory to be protected
	 */
	MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER0;
	MPU_InitStruct.BaseAddress = 0x20010000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_256KB;
	MPU_InitStruct.SubRegionDisable = 0x0;
	MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
	MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);
	/* Enables the MPU */
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

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
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
