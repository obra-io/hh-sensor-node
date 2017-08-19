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

	while (1)
	{
		if (is_ms_set())
		{
			ms_loop();
		}
	}
}




