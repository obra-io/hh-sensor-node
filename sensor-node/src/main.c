/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f0xx.h"
#include "stm32f0xx_hal_rcc.h"
#include "stm32f0xx_hal_tim.h"

#include "mcu.h"

static uint8_t ten_ms_loop = 0;

static volatile 	uint8_t  			  timer_flag;

static uint32_t count=0;

static void ms_loop(void)
{
	if(ten_ms_loop<9)
	{
		ten_ms_loop++;
	}
	else
	{
		ten_ms_loop = 0;
		can_transmit();
	}
}

int main(void)
{
	init_hw();
	for(;;)
	{
		if(timer_flag == 1)
		{
			timer_flag=0;
			run_1ms_function();
		}
		else
		{
			if (count++ == 2000000)
			{

				count = 0;
			}
			run_fast_function();
			// call can process pending function
		}
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	timer_flag = 1;

}



