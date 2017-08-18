#include "stm32f0xx.h"
#include "stm32f0xx_hal_tim.h"
#include "stm32f0xx_hal_can.h"
#include "stm32f0xx_hal_rcc.h"


static TIM_HandleTypeDef handle;
static CAN_InitTypeDef can_handle;
static RCC_OscInitTypeDef osc;
static RCC_ClkInitTypeDef clk;

static void init_timer();
static void init_can();
static void init_clk();




void init_hw(void)
{
	init_timer();
	init_can();
	init_clk();
}


void init_clk(void)
{
	osc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	osc.HSEState = RCC_HSE_ON;

	clk.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
	clk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;//RCC_CFGR_PLLSRC_HSE_PREDIV.....RCC_SYSCLKSOURCE_HSE
	clk.AHBCLKDivider = RCC_CFGR_HPRE_DIV2; //<- can be used to divide sys clock
	clk.APB1CLKDivider = RCC_HCLK_DIV2;
	__HAL_RCC_PLL_ENABLE();
	//HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_2);

	uint32_t bob = HAL_RCC_GetSysClockFreq();
	uint32_t fred = HSE_VALUE;
	bob++;
	fred++;
}



void init_timer(void)
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

