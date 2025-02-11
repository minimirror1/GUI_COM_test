/*
 * main.cpp
 *
 *  Created on: Nov 14, 2024
 *      Author: minim
 */


#include "main.h"
#include "main_cpp.h"
#include "UART_Class.h"
#include "cpp_tick.h"

#include "com_protocol_class.h"
#include "STM32SerialImpl.h"
#include "STM32TickImpl.h"
#include <stdio.h>


#include "custom_protocol.h"

extern UART_HandleTypeDef huart1;

STM32SerialImpl serial(&huart1, USART1_IRQn);
Tick delay1;

Custom_Protocol *dbgProtocol;

int main_cpp(void)
{


	serial.init();
	serial.init_txLed(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
	serial.init_rxLed(LD7_GPIO_Port, LD7_Pin, GPIO_PIN_SET);

	STM32TickImpl tick;
	Custom_Protocol protocol(&serial, &tick);

	dbgProtocol = &protocol;


//	serial1.init(&huart1, USART1_IRQn);
//	serial1.init_txLed(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
//	serial1.init_rxLed(LD7_GPIO_Port, LD7_Pin, GPIO_PIN_SET);

	while(1){

		serial.loop();
		if(delay1.delay(1000)){
			//serial1.write("HELLO");
			//protocol.sendPing(10);
		}
		protocol.processReceivedData();
	}
}


/* HAL Driver Callback */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	serial.TxCpltCallback(huart);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	serial.RxCpltCallback(huart);
}
