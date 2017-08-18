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
			

static TIM_HandleTypeDef handle;

int main(void)
{
	HAL_StatusTypeDef hal_status;

	__HAL_RCC_TIM2_CLK_ENABLE();

	handle.Instance = TIM2;
	handle.Init.Prescaler = 1000;
	handle.Init.Period = 1000;
	handle.Init.RepetitionCounter = 0;
	handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	hal_status = HAL_TIM_Base_Init(&handle);

	if (hal_status == HAL_OK)
	{
		NVIC_EnableIRQ(TIM2_IRQn);

		HAL_TIM_Base_Start_IT(&handle);
	}

	static int i;

	for(;;)
	{
		i++;
	}
}

void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&handle);
}

void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&handle);
}
