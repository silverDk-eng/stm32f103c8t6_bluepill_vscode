
#include "uart_comm.h"

HAL_StatusTypeDef uart_comm_enable_RX_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    return HAL_UART_Receive_IT(huart, pData, UART_INFINITE_EN_CNT);
}

void uart_comm_receive_data_IT(uint8_t data8bits)
{

}

// HAL_StatusTypeDef UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t len){
//   if(HAL_UART_Transmit_IT(huart, pData, len) != HAL_OK){
//     if(RingBuffer_Write(&uart1_txBuf, pData, len) != RING_BUFFER_OK){
//       return 0;
//     }
//   }
//   return 1;
// }
