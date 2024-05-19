#include "user_include.h"

uint8_t txData;

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
  if(RingBuffer_GetDataLength(&uart1_txBuf) > 0) {
    RingBuffer_Read(&uart1_txBuf, &txData, 1);
    HAL_UART_Transmit_IT(huart, &txData, 1);
  }
}