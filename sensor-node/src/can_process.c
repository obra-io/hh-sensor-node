///**
//  ******************************************************************************
//  * @file    can process.c
//  * @author  Prem
//  * @version V1.0
//  * @date    20-May-2017
//  * @brief   task for scheduling can messages
//  ******************************************************************************
//*/
//#include "stm32f0xx.h"
///* peripheral files ---------------------------------------------------------*/
///* Private variables ---------------------------------------------------------*/
//#include "can.h"
//#include "mcu.h"
//
////static volatile uint8_t tx_done;
//
//void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* hcan)
//{
//	//tx_done = 1; // if I write my own ISR fucntion then I need to clear the interrupt
//
//}
//
////static uint32_t get_next_tx_msg_id(void);
////
////
////
////static uint32_t get_next_tx_msg_id(void)
////{
////	uint32_t msg_id =0;
////	static uint8_t count; // implicitily decleared to zero
////	if(count++ > 10000)
////	{
////		msg_id =1;
////		count =0;
////	}
////	return msg_id;
////}
////
////static void process_tx(void)
////{
////	if(tx_in_progress && tx_done )
////	{
////		tx_done = 0;
////		tx_in_progress = 0;
////	}
////	else
////	{
////		uint32_t tx_msg_id;
////
////		tx_msg_id = get_next_tx_msg_id();
////		if(tx_msg_id) // round robin scheduler
////			{
////				send_can_msg(tx_msg_id);
////			}
////	}
////}
////
////static void process_rx(void)
////{
////	if (is_can_msg_in_staging())
////	{
////		process_can_msg();
////	}
////}
////
////void can_process(void)
////{
////	process_rx();
////	process_tx();
////}
//
//
//
