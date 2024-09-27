#ifndef _UART_COMM_H_
#define _UART_COMM_H_

#include <stdint.h>
#include "stm32f1xx_hal.h"

#define UART_INFINITE_EN_CNT (0xFFFF)

void uart_comm_receive_data_IT(uint8_t data8bits);
HAL_StatusTypeDef uart_comm_enable_RX_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
#endif
