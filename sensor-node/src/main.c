/**
  ******************************************************************************
  * @file    main.c
  * @author  Prem
  * @version V1.0
  * @date    20-May-2017
  * @brief   main function for task scheduling
  ******************************************************************************
*/

#include "stm32f0xx.h"
#include "stm32f0xx_hal_rcc.h"
#include "stm32f0xx_hal_tim.h"

#include "mcu.h"
/* peripheral files ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static 			uint8_t 			scheduler=0;
static volatile uint8_t  			timer_flag;
static 			TIM_HandleTypeDef 	handle;
static 			uint32_t 			count=0;


/* Private function prototypes -----------------------------------------------*/
void init_timer(void);


int main(void)
{
	init_hw();	//Initialize hardware
	init_timer();	// Initialize timer

	//enable interrupts
	HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
	for(;;)
	{
		if(scheduler==3)
			scheduler=0;
		switch(scheduler)
		{
		case 1:
			can_transmit();
			break;
		case 2:
			break;
		default:
			if(timer_flag ==1)
			{
				timer_flag =0;
				scheduler++;
			}
		}
	}
}

void init_timer(void)
{
	HAL_StatusTypeDef hal_status;

		__HAL_RCC_TIM2_CLK_ENABLE();

		handle.Instance = TIM2;
		handle.Init.Prescaler = 10;
		handle.Init.Period = 4800;
		handle.Init.RepetitionCounter = 0;
		handle.Init.CounterMode = TIM_COUNTERMODE_UP;
		handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

		hal_status = HAL_TIM_Base_Init(&handle);

		if (hal_status == HAL_OK)
		{
			NVIC_EnableIRQ(TIM2_IRQn);

			HAL_TIM_Base_Start_IT(&handle);
		}
}


void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&handle);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	timer_flag = 1;
}
