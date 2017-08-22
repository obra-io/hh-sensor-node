/**
  ******************************************************************************
  * @file    mcu.c - commit to head
  * @author  Craig, Prem
  * @version V1.0
  * @date    20-May-2017
  * @brief   main function for task scheduling
  ******************************************************************************
*/


#include <stdbool.h>

#include "stm32f0xx.h"
#include "stm32f0xx_hal_tim.h"
#include "stm32f0xx_hal_can.h"
#include "stm32f0xx_hal_rcc.h"
#include "mcu.h"
#include <string.h>
/* peripheral files ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

CAN_HandleTypeDef 				can_handle;
static RCC_OscInitTypeDef 		osc;
static RCC_ClkInitTypeDef 		clk;
static CanTxMsgTypeDef 			can_out;
static CanTxMsgTypeDef 			TxMessage;
static CanRxMsgTypeDef 			RxMessage;
static volatile uint8_t 		tx_done;


/* Private function prototypes -----------------------------------------------*/


void 	send_can_msg(uint16_t adc_8,uint8_t num_of_bytes,uint8_t can_id);


static volatile uint8_t can_rcv_flag;

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
	init_can();
	init_can_message();
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

	/**Configure the Systick interrupt time*/
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/**Configure the Systick*/
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
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
//	HAL_CAN_Receive_IT(&can_handle,0);
//	HAL_CAN_Transmit_IT(&can_handle);
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



void CEC_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&can_handle);
}


/*
void CAN1_RX1_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&can_handle);
}

void CAN1_TX_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&can_handle);
}*/


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


void process_can_msg(struct can_msg *msg){
	msg->ready = 0;
	// create another file can protocol
	// build a giant table for all the data that comes on the can bus
	// use the id of the can message to figure which table data to update
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



void send_can_msg(uint16_t adc_8,uint8_t num_of_bytes,uint8_t can_id)
{
	uint8_t buff [12];
	buff[0] = can_id;
	buff[1] = 0x0;
	uint16_t local_adc=adc_8;
	buff[2] = ((local_adc >>8)& 0xff);
	buff[3] = ((local_adc >>0)& 0xff);
//	can_handle.pTxMsg->StdId = 0x055; // first byte line up with the id to make 11 bits
//	can_handle.pTxMsg->RTR = CAN_RTR_DATA;
//	can_handle.pTxMsg->IDE = buff[0] | ((buff[1] & 0x07)<<8U);
//	can_handle.pTxMsg->DLC = num_of_bytes;
	can_handle.pTxMsg->IDE = CAN_ID_STD;
	can_handle.pTxMsg->RTR = CAN_RTR_DATA;
	can_handle.pTxMsg->DLC = 8;
	can_handle.pTxMsg->StdId = 0x11;
	can_handle.pTxMsg->ExtId = 0x0;
	memcpy(can_handle.pTxMsg->Data ,&buff[0],8);
	HAL_CAN_Transmit_IT(&can_handle);
	//memcpy(can_handle.pRxMsg->Data ,&buff[4],8);
	//memcpy(can_handle.pRxMsg->Data,&buff[4],8);

}
/*
void get_msg_data(uint32_t msg_id, uint8_t buff [12])
{

}
*/

void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* hcan)
{
	tx_done = 1; // if I write my own ISR fucntion then I need to clear the interrupt

}
