#ifndef _UART_COMM_H_
#define _UART_COMM_H_

#include <stdint.h>
#include "stm32f1xx_hal.h"

#define ARGV_MAX (10)
#define UART_INFINITE_EN_CNT (0xFFFF)

#define RX_SIZE (256)

typedef struct _uart_rx_data {
    uint8_t head;
    uint8_t tail;
    uint16_t cnt;
    uint8_t rx[RX_SIZE];
} uart_rx_comm_t;

//typedef struct _uart_tx_data {

//} uart_tx_comm_t;

void uart_comm_receive_data_IT(uint8_t data8bits);
HAL_StatusTypeDef uart_comm_enable_RX_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
void uart_comm_rx_init(uart_rx_comm_t *rx_type);
void uart_comm_command(void);
#endif
