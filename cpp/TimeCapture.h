/*
 * Time�apture.h
 *
 *  Created on: 5 ���. 2019 �.
 *      Author: yura
 */
#pragma once
#include "stdint.h"
#include "stm32f7xx.h"

/*
 * ������������ ����� ������� ��� �������� ������� �� 208 MHz - 20 ���
 * */
typedef uint32_t _time;

class TimeCapture {
public:
	TimeCapture();
	virtual ~TimeCapture();

	// ������ �������� ��� ���������
	void start_capture();

	// ��������� �������� ��� ���������
	// ������ ��������� ����� � ������� ������� ������
	_time stop_capture();

	// ������ ��������� ����� �� ���������� ������� �� ���������
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
