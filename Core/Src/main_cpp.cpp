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
#include <stdio.h>

Serial serial1;
extern UART_HandleTypeDef huart1;

Tick delay1;

int main_cpp(void)
{

	serial1.init(&huart1, USART1_IRQn);
	serial1.init_txLed(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
	serial1.init_rxLed(LD7_GPIO_Port, LD7_Pin, GPIO_PIN_SET);

	while(1){

		serial1.loop();
		if(delay1.delay(1000)){
			serial1.write("HELLO");
		}


	}
}


/* HAL Driver Callback */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	serial1.TxCpltCallback(huart);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	serial1.RxCpltCallback(huart);
}
