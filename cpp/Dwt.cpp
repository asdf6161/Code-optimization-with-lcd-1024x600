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

void Dwt::repack_to_uint32(bType * arr, uint16_t size)
{
	if (size % 2 == 1) {
		 size = size - 1;
	}
	uint16_t j = 0;
	for (uint16_t i = 0; i < size; i+=4)
	{
		/*
		 * y(0)_high = x[n+2]
		 * y(0)_low  = x[n]
		 * y(1)_high = x[n+3]
		 * y(1)_low  = x[n+1]
		 * */
		this->signal[j++] = __PKHBT(arr[i], arr[i + 2], 16);
		this->signal[j++] = __PKHBT(arr[i+1], arr[i + 3], 16);
	}
}

void Dwt::repack_to_uint32(combType * arr, uint16_t size)
{
	uint16_t j = 0;
	combType tmp;
	for (uint16_t i = 0; i < size; i+=2)
	{
		tmp = __PKHTB(arr[i + 1], arr[i], 16);
		arr[j++] = __PKHBT(arr[i], arr[i + 1], 16);
		arr[j++] = tmp;
	}
}

bType* Dwt::DirectTransform(bType * arr, uint16_t size, uint16_t *out_size, uint8_t N)
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

	// Convert to even size
	// Todo - how to quick solve
	if (size % 2 == 1) {
		*out_size = size - 1;
	}
	else {
		*out_size = size;
	}
	uint16_t half_size = *out_size >> 1;

	bType *detail = new bType[half_size];
	bType *approximation = new bType[half_size];

	uint16_t j = 0;
	uint16_t k = half_size;

	uint16_t i = 0;
	for (i; i < *out_size - 1; i+=2)
	{
		// Todo - optimization for conveyor
		// detail coeff
		detail[j] = (arr[i] - arr[i + 1]) >> 1;					// h(n) = (x(n) - x(n - 1)) / 2

		// approximation coefficients
		approximation[j++] = (arr[i] + arr[i + 1]) >> 1;		// g(n) = (x(n) + x(n - 1)) / 2
	}
	while (i < *out_size - 1)
	{
		detail[j] = (arr[i] - arr[i + 1]) >> 1;					// h(n) = (x(n) - x(n - 1)) / 2
		approximation[j++] = (arr[i] + arr[i + 1]) >> 1;		// g(n) = (x(n) + x(n - 1)) / 2
		i++;
	}

	uint16_t out_s;
	bType* app_res = Dwt::DirectTransform(approximation, half_size, &out_s, N-1);

	bType *ret = new bType[*out_size];
	std::memmove(ret, detail, sizeof(bType) * half_size);
	std::memmove(ret + half_size, app_res, sizeof(bType) * half_size);

	delete[] detail;
	delete[] approximation;
	delete[] app_res;
	return ret;
}

void Dwt::DirectTransform(uint16_t size, uint16_t *out_size, uint8_t N)
{
#ifdef TIME_ANALIS
	// 0 замер времени это замер времени обработки условия выхода из прерывания
	cap->reset_cnt();
	cap->start_capture();
#endif
	if (depth != 0){
		arr = approx;
	} else {
		arr = signal;
	}

	if ((N - depth++) == 0) {
		std::memmove(dwt + res_index, approx, sizeof(combType) * size); // test производительности
		depth = 0;
		res_index = 0;
		return;
	}

	if (size <= 1) {
		std::memmove(dwt + res_index, approx, sizeof(combType) * size); // test производительности
		depth = 0;
		res_index = 0;
		return;
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
	uint16_t i = 0;
	combType tmp1 = 0;
	combType tmp2 = 0;
	this->repack_to_uint32(approx, approx_index);
	this->approx_index = 0;
#ifdef TIME_ANALIS  // 1
	code_analis[1] = cap->stop_capture();
#endif

#ifdef TIME_ANALIS
	// 2 - время выполнения операции свертки по откликам g и h
	cap->reset_cnt();
	cap->start_capture();
#endif
	for (i = 0; i < size; i+=2)
	{
		tmp1 = __SHSUB16(arr[i], arr[i + 1]);
		tmp2 = __SHADD16(arr[i], arr[i + 1]);
//		tmp1 = __SSUB16(arr[i], arr[i + 1]);
//		tmp2 = __SADD16(arr[i], arr[i + 1]);
		this->dwt[this->res_index++] = tmp1;
		this->approx[this->approx_index++] = tmp2;
	}
#ifdef TIME_ANALIS  // 2
	code_analis[2] = cap->stop_capture();
#endif

#ifdef TIME_ANALIS
	// 3 - Анализ времени копирования кэффициентов в результирующий массив
	cap->reset_cnt();
	cap->start_capture();
	this->depth = N;
	Dwt::DirectTransform(size >> 1, out_size, N);
#else
	uint16_t out_s;
	Dwt::DirectTransform(size >> 1, &out_s, N);
#endif

#ifdef TIME_ANALIS  // 3
	code_analis[3] = cap->stop_capture();
#endif

	return;
}

void Dwt::DirectTransform_2(uint32_t *arr, uint16_t size, uint16_t *out_size, uint8_t N)
{
#ifdef TIME_ANALIS
	// 0 замер времени это замер времени обработки условия выхода из прерывания
	cap->reset_cnt();
	cap->start_capture();
#endif
	/*if (depth != 0){
		arr = approx;
	} else {
		arr = signal;
	}*/

	if ((N - depth++) == 0) {
		std::memmove(dwt + res_index, approx, sizeof(combType) * size); // test производительности
		depth = 0;
		res_index = 0;
		return;
	}

	if (size <= 1) {
		std::memmove(dwt + res_index, approx, sizeof(combType) * size); // test производительности
		depth = 0;
		res_index = 0;
		return;
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
	uint16_t i = 0;
	combType tmp1 = 0;
	combType tmp2 = 0;
	this->repack_to_uint32(approx, approx_index);
	this->approx_index = 0;
#ifdef TIME_ANALIS  // 1
	code_analis[1] = cap->stop_capture();
#endif

#ifdef TIME_ANALIS
	// 2 - время выполнения операции свертки по откликам g и h
	cap->reset_cnt();
	cap->start_capture();
#endif
	for (i = 0; i < size; i+=1)
	{
		tmp1 = __SHSAX(arr[i],arr[i]);
		tmp1 = __SHSUB16(arr[i], arr[i + 1]);
		tmp2 = __SHADD16(arr[i], arr[i + 1]);
//		tmp1 = __SSUB16(arr[i], arr[i + 1]);
//		tmp2 = __SADD16(arr[i], arr[i + 1]);
		this->dwt[this->res_index++] = tmp1;
		this->approx[this->approx_index++] = tmp2;
	}
#ifdef TIME_ANALIS  // 2
	code_analis[2] = cap->stop_capture();
#endif

#ifdef TIME_ANALIS
	// 3 - Анализ времени копирования кэффициентов в результирующий массив
	cap->reset_cnt();
	cap->start_capture();
	this->depth = N;
	Dwt::DirectTransform(size >> 1, out_size, N);
#else
	uint16_t out_s;
	Dwt::DirectTransform(size >> 1, &out_s, N);
#endif

#ifdef TIME_ANALIS  // 3
	code_analis[3] = cap->stop_capture();
#endif

	return;
}

/*
 * Данный метод работает за 197584 тактов
 * */
bType * Dwt::InverseTransform(bType * inarr, uint16_t size, uint8_t N)
{
	if (N == 0) {
		bType *ret = new bType[size];
		std::memmove(ret, inarr, size * sizeof(bType));
		return ret;
	}

	if (size <= 1) {
		bType *ret = new bType[size];
		std::memmove(ret, inarr, size * sizeof(bType));
		return ret;
	}

	uint16_t half_size = size >> 1;
	bType *ret = new bType[size];
	bType SecondPart[half_size];

	std::memmove(SecondPart, inarr + half_size, sizeof(bType) * half_size);

	bType *inv_res = Dwt::InverseTransform(SecondPart, half_size, N - 1);

	uint16_t j = 0;
	for (uint16_t i = 0; i < half_size; i++)
	{
		ret[j++] = inv_res[i] + inarr[i];
		ret[j++] = inv_res[i] - inarr[i];
	}

	delete[] inv_res;
	return ret;
}

/*  https://www.keil.com/pack/doc/cmsis/Core/html/group__intrinsic__SIMD__gr.html#gad0bf46373a1c05aabf64517e84be5984
 *  __QSUB16 - вычистанием с насыщением
	res[15:0]  = val1[15:0]  - val2[15:0]
	res[31:16] = val1[31:16] - val2[31:16]

	__QADD16 - сложение с насыщением
	res[15:0]  = val1[15:0]  + val2[15:0]
	res[31:16] = val1[31:16] + val2[31:16]

	__SADD16 - сложение, с установкой флагов

	// Приоритетная попробовать передать одно и тотже регистр
	__SASX -
	res[15:0]  = val1[15:0]  - val2[31:16]
	res[31:16] = val1[31:16] + val2[15:0]
 * */
void Dwt::InverseTransform(uint16_t from, uint16_t size, uint8_t N)
{
	if ((N - depth++) == 0) {
		return;
	}

	if (from == size) {
		return;
	}

	uint16_t md = (from + size) >> 1;
	Dwt::InverseTransform(md, size, N);

	combType tmp1 = 0;
	combType tmp2 = 0;
	uint16_t j = md;
	for (uint16_t i = from; i < md; i++)
	{
		tmp1 = __SADD16(dwt[j], dwt[i]);
		tmp2 = __SSUB16(dwt[j++], dwt[i]);
		idwt[res_index++] = tmp1;
		idwt[res_index++] = tmp2;

//		cwt[j++] = dwt[md] + dwt[i];
//		cwt[j++] = dwt[md++] - dwt[i];
	}
	return;
}
