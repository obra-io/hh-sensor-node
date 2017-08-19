#include <stdbool.h>

#include "stm32f0xx.h"
#include "stm32f0xx_hal_tim.h"
#include "stm32f0xx_hal_can.h"
#include "stm32f0xx_hal_rcc.h"


static TIM_HandleTypeDef handle;
CAN_HandleTypeDef can_handle;
static RCC_OscInitTypeDef osc;
static RCC_ClkInitTypeDef clk;
static CanTxMsgTypeDef can_out;
static bool ms_flag = 0;


static void init_timer();
static void init_can();
static void init_clk();
static void init_can_message();




void init_hw(void)
{
	init_clk();
	init_timer();
	init_can();
	init_can_message();
	int i = 0;
	i++;
}


void init_clk(void)
{


	osc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	osc.HSIState = RCC_HSI_ON;
	osc.HSICalibrationValue = 16;
	osc.PLL.PLLState = RCC_PLL_ON;
	osc.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	osc.PLL.PLLMUL = RCC_PLL_MUL6;
	osc.PLL.PREDIV = RCC_PREDIV_DIV1;


	clk.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
	clk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	clk.APB1CLKDivider = RCC_HCLK_DIV1;

	/**Configure the Systick interrupt time
	*/
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/**Configure the Systick
	*/
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

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




 void init_can(void)
 {
	 //__HAL_RCC_CAN_CLK_ENABLE();
	 //HAL_CAN_Init(can_handle);
	 can_handle.Instance = CAN;
	 can_handle.Init.Prescaler = 12;
	 can_handle.Init.Mode = CAN_MODE_NORMAL;
	 can_handle.Init.SJW = CAN_SJW_1TQ;
	 can_handle.Init.BS1 = CAN_BS1_13TQ;
	 can_handle.Init.BS2 = CAN_BS2_2TQ;
	 can_handle.Init.TTCM = DISABLE;
	 can_handle.Init.ABOM = DISABLE;
	 can_handle.Init.AWUM = DISABLE;
	 can_handle.Init.NART = DISABLE;
	 can_handle.Init.RFLM = DISABLE;
	 can_handle.Init.TXFP = DISABLE;

	 HAL_CAN_Init(&can_handle);



 }



void init_can_message(void)
{
	can_out.StdId = 0x33;
	can_out.ExtId = 0;
	can_out.IDE = CAN_ID_STD;
	can_out.RTR = CAN_RTR_DATA;
	can_out.DLC = 8;
	can_out.Data[7] = 5;

}


void can_transmit(void)
{
	HAL_CAN_Transmit_IT(&can_handle);
}


bool is_ms_set(void)
{
	bool set = false;

	if (ms_flag)
	{
		set = true;
		ms_flag = false;
	}

	return set;
}


void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&handle);
	ms_flag = true;
}



