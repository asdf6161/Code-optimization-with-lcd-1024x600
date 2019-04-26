/*
 * TCD1304DG.h
 *
 *  Created on: 24 апр. 2019 г.
 *      Author: yura
 */

#ifndef TCD1304DG_H_
#define TCD1304DG_H_

#include "main.h"
#include "adc.h"
#include "tim.h"

#define mclk 	TIM6	// if change rewrite __rcc_init();

void TCD_start_mclk();
void TCD_stop_mclk();
void TCD_toggle_mclk();
void TCD_start();

#endif /* TCD1304DG_H_ */
