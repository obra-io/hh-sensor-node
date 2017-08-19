#include "stm32f0xx.h"
#include "stm32f0xx_hal_tim.h"
#include "stm32f0xx_hal_can.h"
#include "stm32f0xx_hal_rcc.h"


static TIM_HandleTypeDef handle;
static CAN_InitTypeDef can_handle;


static void init_timer();
static void init_can();
static void init_clk();




void init_hw(void)
{
	init_clk();
	init_timer();
	init_can();

}


void init_clk(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;


	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = 16;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
	RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;


	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

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

void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&handle);
}





 void init_can(void)
 {
	 //__HAL_RCC_CAN_CLK_ENABLE();
	 //HAL_CAN_Init(can_handle);

	 can_handle.Prescaler = 0;
	 can_handle.Mode = HAL_CAN_STATE_READY;
	 can_handle.SJW = 3;
	 can_handle.BS1 = 3;
	 can_handle.BS2 = 3;
	 can_handle.TTCM = DISABLE;
	 can_handle.ABOM = DISABLE;
	 can_handle.AWUM = DISABLE;
	 can_handle.NART = DISABLE;
	 can_handle.RFLM = DISABLE;
	 can_handle.TXFP = DISABLE;
 }

