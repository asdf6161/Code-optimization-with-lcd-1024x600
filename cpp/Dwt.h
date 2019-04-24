/*
 * Dwt.h
 *
 *  Created on: 5 апр. 2019 г.
 *      Author: yura
 */
#pragma once

//#define TIME_ANALIS

#include "main.h"
#include <cstring>
#include <vector>
#ifdef TIME_ANALIS
#include "TimeCapture.h"
#endif

#define BUFFER_SIZE 1000  // четное

using std::vector;

union bTypeUnion
{
	uint32_t all;
	uint16_t data[2];
};

class Dwt
{
public:
	Dwt();
	~Dwt();

	/* wevelet 'haar' algoritm Malla
	  Описание https://etu.ru/assets/files/nauka/dissertacii/2018/kalyakin/dissertaciya_kalyakin.pdf стр 25
	  О написание под cortex m4 - file:///C:/Users/sk120/Google%20%D0%94%D0%B8%D1%81%D0%BA/%D0%A3%D1%87%D0%B5%D0%B1%D0%B0/!%D0%94%D0%B8%D1%81%D0%B5%D1%80%D1%82%D0%B0%D1%86%D0%B8%D1%8F/%D0%9B%D0%B8%D1%82%D0%B5%D1%80%D0%B0%D1%82%D1%83%D1%80%D0%B0/%D0%92%D0%B5%D0%B9%D0%B2%D0%BB%D0%B5%D1%82%20%D1%81%D1%82%D1%80%208.pdf
	*/

	// *out_size - length output array
	// N - dwt level
	void DirectTransform(uint16_t size, uint16_t *out_size, uint8_t N);
	void DirectTransform_2(uint32_t *arr, uint16_t size, uint16_t *out_size, uint8_t N);
	bType * DirectTransform(bType * arr, uint16_t size, uint16_t *out_size, uint8_t N);

	// arr - input DirectTransform array
	// size - length of arr
	// N - dwt level
	static bType * InverseTransform(bType *arr, uint16_t size, uint8_t N);
	void InverseTransform(uint16_t from, uint16_t size, uint8_t N);


	void repack_to_uint32(bType * arr, uint16_t size);
	void repack_to_uint32(combType * arr, uint16_t size);

public:
	combType signal[BUFFER_SIZE];
	combType idwt[BUFFER_SIZE];
	combType dwt[BUFFER_SIZE];
	combType approx[BUFFER_SIZE / 2];
	uint16_t approx_index = 0;
	uint16_t res_index = 0;
	uint8_t depth = 0;
	combType *arr = nullptr;
};

