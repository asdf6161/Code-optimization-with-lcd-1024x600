/*
 * TCD1304DG.cpp
 *
 *  Created on: 24 апр. 2019 г.
 *      Author: yura
 */

#include "TCD1304DG.h"

extern tcd_type tcd_res[tcd_buffer_size];

void TCD_start(){
	HAL_ADC_Stop(&hadc1);
	HAL_ADC_Stop_DMA(&hadc1);
	HAL_GPIO_WritePin(ICG_GPIO_Port, ICG_Pin, GPIO_PIN_SET);
	for (uint16_t i = 0; i < 400; ++i) {}  // delay
	HAL_GPIO_WritePin(ICG_GPIO_Port, ICG_Pin, GPIO_PIN_RESET);
	HAL_ADC_Start(&hadc1);
	HAL_ADC_Start_DMA(&hadc1, (tcd_res), tcd_buffer_size);
}
