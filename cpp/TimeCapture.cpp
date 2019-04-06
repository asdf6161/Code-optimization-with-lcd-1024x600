/*
 * TimeCapture.cpp
 *
 *  Created on: 5 апр. 2019 г.
 *      Author: yura
 */

#include <TimeCapture.h>


volatile uint32_t *DWT_LAR = (volatile uint32_t *)(0xE0001FB0);
volatile uint32_t *DWT_LSR = (volatile uint32_t *)(0xE0001FB4);

/*void start_cap(){
	SET_BIT(DWT->CTRL, DWT_CTRL_CYCCNTENA_Msk);
}

_time stop_cap(){
	CLEAR_BIT(DWT->CTRL, DWT_CTRL_CYCCNTENA_Msk);
	return DWT->CYCCNT;
}*/

TimeCapture::TimeCapture() {
	this->time_passed = 0;
	this->timeend = 0;
	this->timestart = 0;
	this->__allow_core_debug();
	this->dwt_access_enable(true);
	this->reset_cnt();
}

TimeCapture::~TimeCapture() {
	// TODO Auto-generated destructor stub
	this->__disallow_core_debug();
}


void TimeCapture::start_capture(){
	this->timestart = DWT->CYCCNT;
	this->__enable_counter();
}

_time TimeCapture::stop_capture(){
	this->timeend = DWT->CYCCNT;
	this->__disable_counter();
	return this->timeend;
}

_time TimeCapture::get_passed_time(){
	this->time_passed = this->timeend - this->timestart;
	return this->time_passed;
}

void TimeCapture::reset_cnt(){
	DWT->CYCCNT = 0;
}

void TimeCapture::__enable_counter(){
	SET_BIT(DWT->CTRL, DWT_CTRL_CYCCNTENA_Msk);
}

void TimeCapture::__disable_counter(){
	CLEAR_BIT(DWT->CTRL, DWT_CTRL_CYCCNTENA_Msk);
}

void TimeCapture::__allow_core_debug(){
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
}

void TimeCapture::__disallow_core_debug(){
	CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;
}

void TimeCapture::delay_us(uint32_t us)
{
	DWT->CYCCNT = 0;
	uint32_t t0 = DWT->CYCCNT;
	uint32_t us_count_tic = us * (SystemCoreClock/1000000);
	while ((DWT->CYCCNT - t0) < us_count_tic);
	DWT->CYCCNT = 0;
}

// Not defined in CMSIS 4.00 headers - check if defined
// to allow for possible correction in later versions
#if !defined DWT_LSR_Present_Msk
    #define DWT_LSR_Present_Msk ITM_LSR_Present_Msk
#endif
#if !defined DWT_LSR_Access_Msk
    #define DWT_LSR_Access_Msk ITM_LSR_Access_Msk
#endif
#define DWT_LAR_KEY 0xC5ACCE55

void TimeCapture::dwt_access_enable( unsigned ena )
{
    uint32_t lsr = *DWT_LSR;

    if ( (lsr & DWT_LSR_Present_Msk) != 0 )
    {
        if( ena )
        {
            if ((lsr & DWT_LSR_Access_Msk) != 0) //locked: access need unlock
            {
            	*DWT_LAR = DWT_LAR_KEY;
            }
        }
        else
        {
            if ((lsr & DWT_LSR_Access_Msk) == 0) //unlocked
            {
            	*DWT_LAR = 0;
            }
        }
    }
}
