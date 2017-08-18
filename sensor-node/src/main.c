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
#include "init_hw.h"



int main(void)
{
	init_hw();

	static int i;

	for(;;)
	{
		i++;
	}
}



