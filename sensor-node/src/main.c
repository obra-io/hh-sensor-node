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
#include "stm32f0xx_hal_can.h"
#include "stm32f0xx_hal_adc.h"

#include "mcu.h"

/* peripheral files ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static volatile	uint8_t 			scheduler;
static volatile uint8_t  			timer_flag;
static 			TIM_HandleTypeDef 	handle;
static 			ADC_HandleTypeDef 	g_AdcHandle;
static 			uint16_t			g_ADCValue;

/* Private function prototypes -----------------------------------------------*/
static void init_timer(void);
static void adc_channel_init(void);

int main(void)
{
	init_hw();	//Initialize hardware
	init_timer();	// Initialize timer
	adc_channel_init();
	//enable interrupts
	HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
	HAL_NVIC_SetPriority(CEC_CAN_IRQn,1,1);
	HAL_ADC_Start(&g_AdcHandle);
	for(;;)
	{
		switch(scheduler)
		{
		case 1:
			timer_flag =0;
			if (HAL_ADC_PollForConversion(&g_AdcHandle, 10) == HAL_OK)
				{
					g_ADCValue = HAL_ADC_GetValue(&g_AdcHandle);
				}
			break;
		case 3:
			timer_flag=0;
			send_can_msg(g_ADCValue,2,0x023);
			break;
		default:
			break;
		}
	}
}
//initialize timer
void init_timer(void)
{
	HAL_StatusTypeDef hal_status;

		__HAL_RCC_TIM2_CLK_ENABLE();

		handle.Instance = TIM2;
		handle.Init.Prescaler = 100;
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
//handler for timer interrupt
void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&handle);
}
//timer call back
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	timer_flag = 1;
	if(scheduler<4)
		{
			scheduler++;
		}
	else
		{
			scheduler=0;
		}
}

//initialize ADC
static void adc_channel_init(void)
{
	__HAL_RCC_ADC1_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef gpioInit;

	gpioInit.Pin = GPIO_PIN_1;
	gpioInit.Mode = GPIO_MODE_ANALOG;
	gpioInit.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &gpioInit);

	ADC_ChannelConfTypeDef adcChannel;

	g_AdcHandle.Instance = ADC1;
	g_AdcHandle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
	g_AdcHandle.Init.Resolution = ADC_RESOLUTION_12B;
	g_AdcHandle.Init.ScanConvMode = DISABLE;
	g_AdcHandle.Init.ContinuousConvMode = ENABLE;
	g_AdcHandle.Init.DiscontinuousConvMode = DISABLE;
	g_AdcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	g_AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	g_AdcHandle.Init.DMAContinuousRequests = DISABLE;
	g_AdcHandle.Init.EOCSelection = ENABLE;

	adcChannel.Channel = ADC_CHANNEL_0;
	adcChannel.Rank = 1;
	adcChannel.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	HAL_ADC_Init(&g_AdcHandle);
	HAL_ADC_ConfigChannel(&g_AdcHandle, &adcChannel);

}
