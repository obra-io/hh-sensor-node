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
/* peripheral files ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static 			uint8_t tx_in_progress;
static volatile uint8_t tx_done;

/* Private function prototypes -----------------------------------------------*/

static void 	send_can_msg(void);
static uint32_t get_next_tx_msg_id(void);



static void send_can_msg(uint32_t msg_id)
{
	uint8_t buff [12];
	get_msg_data(msg_id, buff);		// fill the first 4 bytes woith can ID last 8 with data
	can_handle.pTxMsg->StdId = buff[0] | ((buff[1] & 0x07)<<8U);  // first byte line up with the id to make 11 bits we need another 3 bits of the second byte
	// U indicated its unsigned
	can_handle.pTxMsg->RTR = CAN_RTR_DATA;		// look at this
	can_handle.pTxMsg->IDE = CAN_ID_STD;			//
	can_handle.pTxMsg->DLC = 8;
	memcpy(can_handle->pRxMsg->Data,&buff[4],8);
	HAL_CAN_Transmit_IT(&can_handle);
}
void HAL_CAN_TxCpltCallback()
{
	tx_done = 1; // if I write my own ISR fucntion then I need to clear the interrupt
}

static uint32_t get_next_tx_msg_id(void)
{
	uint32_t msg_id =0;
	static uint8_t count; // implicitily decleared to zero
	if(count++ > 10000)
	{
		msg_id =1;
		count =0;
	}
	return msg_id;
}

static void process_tx(void)
{
	if(tx_in_progress && tx_done )
	{
		tx_done = 0;
		tx_in_progress = 0;
	}
	else
	{
		uint32_t tx_msg_id;

		tx_msg_id = get_next_tx_msg_id();
		if(tx_msg_id) // round robin scheduler
			{
				send_can_msg(tx_msg_id);
			}
	}
}

static void process_rx(void)
{
	if (is_can_msg_in_staging())
	{
		process_can_msg();
	}
}

void can_process(void)
{
	process_rx();
	process_tx();
}



