/*
 * Dwt.cpp
 *
 *  Created on: 5 апр. 2019 г.
 *      Author: yura
 */

#include <Dwt.h>

#ifdef TIME_ANALIS
extern uint32_t code_analis[10];
TimeCapture *cap = new TimeCapture();
#endif

Dwt::Dwt()
{

}


Dwt::~Dwt()
{
}

combType * Dwt::repack_to_uint32(bType * arr, uint16_t size)
{
	if (size % 2 == 1) {
		 size = size - 1;
	}

	/*combType *new_arr = new combType[size];
	uint16_t j = 0;
	for (uint16_t i = 0; i < size; i++)
	{
		new_arr[j] = arr[i] << 16;
		new_arr[j++] |= arr[i];
	}*/

	//for __SHADD16
	combType *new_arr = new combType[size / 2];
	uint16_t j = 0;
	for (uint16_t i = 0; i < size; i+=4)
	{
		new_arr[j]		= arr[i]	 << 16;
		new_arr[j++]	|= arr[i + 2];
		new_arr[j]		= arr[i + 1] << 16;
		new_arr[j++]	|= arr[i + 3];
	}

	return new_arr;
}

bType* Dwt::DirectTransform(bType * arr, uint16_t size, uint16_t *out_size, uint8_t N)
{
#ifdef TIME_ANALIS
	// 0 замер времени это замер времени обработки условия выхода из прерывания
	cap->reset_cnt();
	cap->start_capture();
#endif
	if (N == 0) {
		bType *ret = new bType[size];
		std::memmove(ret, arr, size * sizeof(bType));
		return ret;
	}

	if (size <= 1) {
		bType *ret = new bType[size];
		std::memmove(ret, arr, size * sizeof(bType));
		return ret;
	}
#ifdef TIME_ANALIS  // 0
	code_analis[0] = cap->stop_capture();
#endif

	// Convert to even size
	// Todo - how to quick solve
	if (size % 2 == 1) {
		*out_size = size - 1;
	}
	else {
		*out_size = size;
	}
#ifdef TIME_ANALIS
	// 1 - Выделение памяти для детализирующих и аппроксимирующих коэффициентов
	cap->reset_cnt();
	cap->start_capture();
#endif
	uint16_t half_size = *out_size >> 1;

	bType *detail = new bType[half_size];
	bType *approximation = new bType[half_size];

	uint16_t j = 0;

	uint16_t i = 0;
#ifdef TIME_ANALIS  // 1
	code_analis[1] = cap->stop_capture();
#endif

#ifdef TIME_ANALIS
	// 2 - время выполнения операции свертки по откликам g и h
	cap->reset_cnt();
	cap->start_capture();
#endif
	for (i = 0; i < *out_size - 1; i+=2)
	{
		// Todo - optimization for conveyor
		// detail coeff
//		detail[j] = (arr[i] - arr[i + 1]) >> 1;					// h(n) = (x(n) - x(n - 1)) / 2

//		bType res1 = (20 - 10) >> 1;
//		int32_t res2;
		__ASM volatile ("shsub16 %0, %1, %2" : "=r" (detail[j]) : "r" (arr[i]), "r" (arr[i + 1]) );
		__ASM volatile ("shadd16 %0, %1, %2" : "=r" (approximation[j++]) : "r" (arr[i]), "r" (arr[i + 1]) );
		int32_t res3 = __SSAX((20 << 16) | (2), (8 << 16) | 4);
		int16_t low = res3 & 0xFFFF;
		int16_t high = res3 >> 16;
		asm("nop");

		// approximation coefficients
//		approximation[j++] = (arr[i] + arr[i + 1]) >> 1;		// g(n) = (x(n) + x(n - 1)) / 2
//		detail[j] = __SHSUB16(arr[i], arr[i + 1]);
//		approximation[j++] = __SHADD16()(arr[i], arr[i + 1]);
	}
	while (i < *out_size - 1)
	{
		detail[j] = (arr[i] - arr[i + 1]) >> 1;					// h(n) = (x(n) - x(n - 1)) / 2
		approximation[j++] = (arr[i] + arr[i + 1]) >> 1;		// g(n) = (x(n) + x(n - 1)) / 2
		i++;
	}
#ifdef TIME_ANALIS  // 2
	code_analis[2] = cap->stop_capture();
#endif

#ifdef TIME_ANALIS
	// 3 - Анализ времени копирования кэффициентов в результирующий массив
	cap->reset_cnt();
	cap->start_capture();

	bType* app_res =  new bType[size];
	std::memmove(app_res, approximation, size * sizeof(bType));
#else
	uint16_t out_s;
	bType* app_res = Dwt::DirectTransform(approximation, half_size, &out_s, N-1);
#endif

	bType *ret = new bType[*out_size];
	std::memmove(ret, detail, sizeof(bType) * half_size);
	std::memmove(ret + half_size, app_res, sizeof(bType) * half_size);

	delete[] detail;
	delete[] approximation;
	delete[] app_res;
#ifdef TIME_ANALIS  // 3
	code_analis[3] = cap->stop_capture();
#endif
	return ret;
}



combType* Dwt::DirectTransform(combType* arr, uint16_t size, uint16_t *out_size, uint8_t N)
{
#ifdef TIME_ANALIS
	// 0 замер времени это замер времени обработки условия выхода из прерывания
	cap->reset_cnt();
	cap->start_capture();
#endif
	if (N == 0) {
		combType *ret = new combType[size];
		std::memmove(ret, arr, size * sizeof(combType));
		return ret;
	}

	if (size <= 1) {
		combType *ret = new combType[size];
		std::memmove(ret, arr, size * sizeof(combType));
		return ret;
	}
#ifdef TIME_ANALIS  // 0
	code_analis[0] = cap->stop_capture();
#endif

	// Convert to even size
	// Todo - how to quick solve
	if (size % 2 == 1) {
		*out_size = size - 1;
	}
	else {
		*out_size = size;
	}
#ifdef TIME_ANALIS
	// 1 - Выделение памяти для детализирующих и аппроксимирующих коэффициентов
	cap->reset_cnt();
	cap->start_capture();
#endif
	uint16_t half_size = *out_size >> 1;

//	bType *detail = new bType[half_size];
//	bType *approximation = new bType[half_size];
	combType *detail = new combType[size/2];
	combType *approximation = new combType[size/2];
	combType detail2[size/2];

	uint16_t i = 0;
#ifdef TIME_ANALIS  // 1
	code_analis[1] = cap->stop_capture();
#endif

#ifdef TIME_ANALIS
	// 2 - время выполнения операции свертки по откликам g и h
	cap->reset_cnt();
	cap->start_capture();
#endif
	uint16_t j = 0;
	for (i = 0; i < size; i+=8)
	{
		/*cap->reset_cnt();
		SET_BIT(DWT->CTRL, DWT_CTRL_CYCCNTENA_Msk);
		detail[j] = __SHSUB16(detail[i], detail[i + 1]);
		detail[j++] = __SHADD16(detail[i], detail[i + 1]);

		detail[j] = __SHSUB16(detail[i + 2], detail[i + 3]);
		detail[j++] = __SHADD16(detail[i + 2], detail[i + 3]);

		detail[j] = __SHSUB16(detail[i + 4], detail[i + 5]);
		detail[j++] = __SHADD16(detail[i + 4], detail[i + 5]);

		detail[j] = __SHSUB16(detail[i + 6], detail[i + 7]);
		detail[j++] = __SHADD16(detail[i + 6], detail[i + 7]);
		CLEAR_BIT(DWT->CTRL, DWT_CTRL_CYCCNTENA_Msk);
		uint32_t res = 	DWT->CYCCNT;
		asm("nop");*/
		detail[j] = __SHSUB16(arr[i], arr[i + 1]);
//		 __ASM volatile ("shsub16 %0, %1, %2" : "=r" (detail[j]) : "r" (arr[i]), "r" (arr[i + 1]) );
		approximation[j++] = __SHADD16(arr[i], arr[i + 1]);
//		 __ASM volatile ("shadd16 %0, %1, %2" : "=r" (approximation[j++]) : "r" (arr[i]), "r" (arr[i + 1]) );
	}
#ifdef TIME_ANALIS  // 2
	code_analis[2] = cap->stop_capture();
#endif

#ifdef TIME_ANALIS
	// 3 - Анализ времени копирования кэффициентов в результирующий массив
	cap->reset_cnt();
	cap->start_capture();

	combType* app_res =  new combType[size];
	std::memmove(app_res, approximation, size * sizeof(combType));
#else
	uint16_t out_s;
	combType* app_res = Dwt::DirectTransform(approximation, half_size, &out_s, N-1);
#endif

	combType *ret = new combType[*out_size];
	std::memmove(ret, detail, sizeof(combType) * half_size);
	std::memmove(ret + half_size, app_res, sizeof(combType) * half_size);

	delete[] detail;
	delete[] approximation;
	delete[] app_res;
#ifdef TIME_ANALIS  // 3
	code_analis[3] = cap->stop_capture();
#endif

	return ret;
}


bType * Dwt::InverseTransform(bType * arr, uint16_t size, uint8_t N)
{
	if (N == 0) {
		bType *ret = new bType[size];
		std::memmove(ret, arr, size * sizeof(bType));
		return ret;
	}

	if (size <= 1) {
		bType *ret = new bType[size];
		std::memmove(ret, arr, size * sizeof(bType));
		return ret;
	}

	uint16_t half_size = size >> 1;
	bType *ret = new bType[size];
	bType *SecondPart = new bType[half_size];

	std::memmove(SecondPart, arr + half_size, sizeof(bType) * half_size);

	bType *inv_res = Dwt::InverseTransform(SecondPart, half_size, N - 1);

	uint16_t j = 0;
	for (uint16_t i = 0; i < half_size; i++)
	{
		ret[j++] = inv_res[i] + arr[i];
		ret[j++] = inv_res[i] - arr[i];
	}

	delete[] SecondPart;
	delete[] inv_res;
	return ret;
}
