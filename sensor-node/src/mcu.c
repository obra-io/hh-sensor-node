#include <stdbool.h>

#include "stm32f0xx.h"
#include "stm32f0xx_hal_tim.h"
#include "stm32f0xx_hal_can.h"
#include "stm32f0xx_hal_rcc.h"
#include "can.h"
#include <string.h>


static TIM_HandleTypeDef handle;
CAN_HandleTypeDef can_handle;
static RCC_OscInitTypeDef osc;
static RCC_ClkInitTypeDef clk;
static CanTxMsgTypeDef can_out;


static CanTxMsgTypeDef        TxMessage;
static CanRxMsgTypeDef        RxMessage;

static bool ms_flag = 0;

static volatile uint8_t can_rcv_flag;

static void init_timer();
static void init_can();
static void init_clk();
static void init_can_message();

struct can_msg{
	uint8_t ready;
	uint8_t data[8];
	uint32_t id;
};

struct can_msg pend_msgs[5];


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

		HAL_TIM_Base_Init(&handle);

	    HAL_TIM_Base_Start_IT(&handle);

		if (hal_status == HAL_OK)
		{
			NVIC_EnableIRQ(TIM2_IRQn);

			HAL_TIM_Base_Start_IT(&handle);
		}
}




 void init_can(void)
 {
	 __HAL_RCC_GPIOB_CLK_ENABLE();
	 __HAL_RCC_GPIOA_CLK_ENABLE();
	 __HAL_RCC_CAN1_CLK_ENABLE();


	 GPIO_InitTypeDef 	  can_gpio;

	can_gpio.Pin = GPIO_PIN_11; 				// can rx
	can_gpio.Mode = GPIO_MODE_AF_PP;
	can_gpio.Pull = GPIO_PULLUP;
	can_gpio.Speed = GPIO_SPEED_FREQ_HIGH;
	can_gpio.Alternate = GPIO_AF4_CAN;
	HAL_GPIO_Init(GPIOA, &can_gpio);

	can_gpio.Pin = GPIO_PIN_12;				// can tx
	can_gpio.Mode = GPIO_MODE_AF_PP;
	can_gpio.Pull = GPIO_PULLUP;
	can_gpio.Speed = GPIO_SPEED_FREQ_HIGH;
	can_gpio.Alternate = GPIO_AF4_CAN;
	HAL_GPIO_Init(GPIOA, &can_gpio);

	 can_handle.Instance = CAN;
	 can_handle.Init.Mode = CAN_MODE_NORMAL;
	 can_handle.Init.Prescaler = 12;
	 can_handle.Init.SJW = CAN_SJW_1TQ;
	 can_handle.Init.BS1 = CAN_BS1_13TQ;
	 can_handle.Init.BS2 = CAN_BS2_2TQ;
	 can_handle.Init.TTCM = DISABLE;
	 can_handle.Init.ABOM = DISABLE;
	 can_handle.Init.AWUM = DISABLE;
	 can_handle.Init.NART = DISABLE;
	 can_handle.Init.RFLM = DISABLE;
	 can_handle.Init.TXFP = DISABLE;
	 can_handle.pTxMsg = &TxMessage;
	 can_handle.pRxMsg = &RxMessage;
	 CAN_FilterConfTypeDef  		  sFilterConfig;
	 //CAN_Init(CAN1, &can_handle);
	 if(HAL_CAN_Init(&can_handle)!=HAL_OK)
		{
		 while(1);
		}

	 sFilterConfig.FilterNumber = 0;
	 sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	 sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	 sFilterConfig.FilterIdHigh = 0x0000;
	 sFilterConfig.FilterIdLow = 0x0000;
	 sFilterConfig.FilterMaskIdHigh = 0x0000;
	 sFilterConfig.FilterMaskIdLow = 0x0000;
	 sFilterConfig.FilterFIFOAssignment = 0;
	 sFilterConfig.FilterActivation = ENABLE;
	 sFilterConfig.BankNumber = 14;

	 if(HAL_CAN_ConfigFilter(&can_handle, &sFilterConfig) != HAL_OK)
	  {
		 while(1);
	  }

	 //## 1 Configure the CAN peripheral //
	 HAL_CAN_Init(&can_handle);
	 HAL_CAN_Receive_IT(&can_handle,0);
	 HAL_CAN_Transmit_IT(&can_handle);
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

void CAN1_RX0_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&can_handle);
}

void CAN1_RX1_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&can_handle);
}

void CAN1_TX_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&can_handle);
}

void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* hcan)
{

}
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{

}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
	for(int i = 0; i<5; i++)
	{
		if(!pend_msgs[i].ready)
		{
			memcpy(pend_msgs[i].data, can_handle.pRxMsg->Data, 8);
			pend_msgs[i].id 	= can_handle.pRxMsg->StdId;
			pend_msgs[i].ready = 1;
			break;
		}
	}
	can_rcv_flag = 1;
}

//processing interrupts
void can_loop(void)
{
	if(can_rcv_flag == 1)
	{
		can_rcv_flag = 0;
		for(int i = 0; i<5; i++)
		{
			if(pend_msgs[i].ready)
			{
				process_can_msg(&pend_msgs[i]);
			}
		}
	}
}

void process_can_msg(struct can_msg *msg){
	msg->ready = 0;
	// create another file can protocol
	// build a giant table for all the data that comes on the can bus
	// use the id of the can message to figure which table data to update
}


void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&handle);
}

