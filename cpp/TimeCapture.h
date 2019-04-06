/*
 * TimeСapture.h
 *
 *  Created on: 5 апр. 2019 г.
 *      Author: yura
 */
#pragma once
#include "stdint.h"
#include "stm32f7xx.h"

/*
 * Максимальное время захвата при тактовой частоте ЦП 208 MHz - 20 сек
 * */
typedef uint32_t _time;

class TimeCapture {
public:
	TimeCapture();
	virtual ~TimeCapture();

	// Запуск счетчика без обнуления
	void start_capture();

	// Остановка счетчика без обнуления
	// Вернет прошедшее время с момента первого ресета
	_time stop_capture();

	// Вернет прошедшее время от последнего запуска до остановки
	_time get_passed_time();
	void reset_cnt();

	static void delay_us(uint32_t us);
	void dwt_access_enable( unsigned ena );

private:
	void __enable_counter();
	void __disable_counter();
	void __allow_core_debug();
	void __disallow_core_debug();

private:
	_time timestart;
	_time timeend;
	_time time_passed;
};
